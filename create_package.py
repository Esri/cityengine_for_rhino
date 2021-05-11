import argparse
import os
import zipfile
import re
import shutil


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


def parse_version_file(version_file: str) -> (int, int, int, int):
    # Parse file "version.h" to get major, minor, revision, build numbers
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
    return v_major, v_minor, v_revision, v_build


def clean_package_dir(package_dir: str):
    if os.path.exists(package_dir):
        shutil.rmtree(package_dir)
    os.mkdir(package_dir)


def build_rhi_package(build_dir: str, package_dir: str, v_major, v_minor, v_revision, v_build):
    version_str: str = f'v{v_major}.{v_minor}.{v_revision}.{v_build}'

    with zipfile.ZipFile(os.path.join(package_dir, f'Puma_{version_str}.rhi'), 'w') as myZip:
        # Copy the folders bin and lib of Esri sdk
        copytree(build_dir, myZip)


def build_yak_package(build_dir: str, curr_dir: str, package_dir: str, v_major, v_minor, v_revision):
    # yak package creation
    shutil.copyfile(os.path.join(curr_dir, 'manifest.yml'), os.path.join(build_dir, 'manifest.yml'))
    os.chdir(build_dir)
    os.system('cmd /c ""C:\Program Files\Rhino 7\System\Yak.exe" build"')

    # Get the file created: could change depending on Rhino/GH versions.
    yak_filename: str = ''
    for file in os.listdir(build_dir):
        if file.endswith('.yak'):
            yak_filename = file

    if len(yak_filename) == 0:
        raise IOError('Error: the yak package could not be created.')

    # create rhino 7 yak package
    shutil.copyfile(os.path.join(build_dir, yak_filename),
                    os.path.join(build_dir, f'puma-{v_major}.{v_minor}.{v_revision}-rh7-any.yak'))

    # move both yak to the package_output directory
    shutil.move(yak_filename,
                os.path.join(package_dir, yak_filename))
    shutil.move(f'puma-{v_major}.{v_minor}.{v_revision}-rh7-any.yak',
                os.path.join(package_dir, f'puma-{v_major}.{v_minor}.{v_revision}-rh7-any.yak'))

    # cleaning
    os.remove('manifest.yml')
    os.chdir(curr_dir)


def build(build_mode: str, build_dir: str, package_dir: str, current_dir: str, v_major, v_minor, v_revision, v_build):
    if build_mode == 'both' or build_mode == 'rhi':
        build_rhi_package(build_dir, package_dir, v_major, v_minor, v_revision, v_build)

    if build_mode == 'both' or build_mode == 'yak':
        build_yak_package(build_dir, current_dir, package_dir, v_major, v_minor, v_revision)


def parse_args():
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--build-module', default='both', choices=['both', 'rhi', 'yak'],
                            help='The package to build.')
    args = arg_parser.parse_args()
    return args


def main():
    curr_dir: str = os.getcwd()
    build_dir: str = os.path.join(curr_dir, 'build')
    package_dir: str = os.path.join(curr_dir, 'Package_output')
    args = parse_args()
    clean_package_dir(package_dir)
    version_file: str = os.path.join(curr_dir, "RhinoPRT", "version.h")
    (v_major, v_minor, v_revision, v_build) = parse_version_file(version_file)
    build(args.build_module, build_dir, package_dir, curr_dir, v_major, v_minor, v_revision, v_build)


if __name__ == "__main__":
    main()
