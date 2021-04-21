import os
import zipfile
import re
import shutil
import sys

# copy a directory and all its content into a given ZipFile.
def copytree(src, dst: zipfile.ZipFile, arch_path: str = '', symlinks=False, ignore: list = []):
    for item in os.listdir(src):
        if ignore.count(item) == 0:
            s = os.path.join(src, item)
            new_s = os.path.join(arch_path, item)
            if os.path.isdir(s):
                copytree(s, dst, new_s, symlinks, ignore)
            else:
                dst.write(s, new_s)


# MAIN

usage: str = 'usage: create_package.py both|rhi|yak'
if len(sys.argv) == 1:
    print(usage)
    exit(0)

curr_dir: str = os.getcwd()
build_dir: str = os.path.join(curr_dir, 'build')
package_dir: str = os.path.join(curr_dir, 'Package_output')

if os.path.exists(package_dir):
    shutil.rmtree(package_dir)

os.mkdir(package_dir)

package_type = 'both'
if len(sys.argv) > 1:
    package_type = sys.argv[1]

if package_type == 'both' or package_type == 'rhi':
    # Parse file "version.h" to get major, minor, revision, build numbers
    version_file: str = os.path.join(curr_dir, "RhinoPRT", "version.h")

    pattern = re.compile(r'^#define (?P<version>\w+)\s+(?P<number>\d+)$')

    v_major = 0
    v_minor = 0
    v_revision = 0
    v_build = 0

    with open(version_file, 'r') as file:
        lines: list = file.readlines()
        for line in lines:
            result = pattern.match(line)
            if result:
                version = result.group('version')
                number = result.group('number')

                if version == 'VERSION_MAJOR':
                    v_major = number
                elif version == 'VERSION_MINOR':
                    v_minor = number
                elif version == 'VERSION_REVISION':
                    v_revision = number
                elif version == 'VERSION_BUILD':
                    v_build = number

    version_str: str = f'v{v_major}.{v_minor}.{v_revision}.{v_build}'

    with zipfile.ZipFile(os.path.join(package_dir, f'Puma_{version_str}.rhi'), 'w') as myZip:
        # Copy the folders bin and lib of Esri sdk
        copytree(build_dir, myZip)

if package_type == 'both' or package_type == 'yak':
    # yak package creation
    shutil.copyfile(os.path.join(curr_dir, 'manifest.yml'), os.path.join(build_dir, 'manifest.yml'))
    os.chdir(build_dir)
    os.system('cmd /c ""C:\Program Files\Rhino 7\System\Yak.exe" build"')

    # create rhino 7 yak package
    shutil.copyfile(os.path.join(build_dir, 'puma-0.6.0-rh6_34-any.yak'),
                    os.path.join(build_dir, 'puma-0.6.0-rh7-any.yak'))

    # move both yak to the package_output directory
    shutil.move('puma-0.6.0-rh6_34-any.yak', os.path.join(package_dir, 'puma-0.6.0-rh6_34-any.yak'))
    shutil.move('puma-0.6.0-rh7-any.yak', os.path.join(package_dir, 'puma-0.6.0-rh7-any.yak'))

    # cleaning
    os.remove('manifest.yml')
    os.chdir(curr_dir)
