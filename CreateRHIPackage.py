import os
import zipfile
import re
import shutil


# copy a directory and all its content into a given ZipFile.
def copytree(src, dst: zipfile.ZipFile, arch_path: str, symlinks=False, ignore: list = None):
    for item in os.listdir(src):
        if ignore.count(item) == 0:
            s = os.path.join(src, item)
            new_s = os.path.join(arch_path, item)
            if os.path.isdir(s):
                copytree(s, dst, new_s, symlinks, ignore)
            else:
                dst.write(s, new_s)


# MAIN

# path to the different folders to zip
curr_dir: str = os.getcwd()
esri_lib_dir: str = os.path.join(curr_dir, 'esri_sdk')
release_dir: str = os.path.join(curr_dir, 'x64', 'Release')
release_gh: str = os.path.join(curr_dir, 'x64', 'Release_gh')

if os.path.exists('Package_output'):
    shutil.rmtree('Package_output')

os.mkdir('Package_output')

arch_release_dir: str = os.path.join('x64', 'Release')

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
        result: re.Match = pattern.match(line)
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

with zipfile.ZipFile(f'Package_output/RhinoPRT_{version_str}.rhi', 'w') as myZip:
    # Copy the folders bin and lib of Esri sdk
    copytree(esri_lib_dir, myZip, 'esri_sdk', ignore=['cmake', 'doc', 'include'])

    myZip.write(os.path.join(release_dir, 'codecs_rhino.dll'), os.path.join(arch_release_dir, 'codecs_rhino.dll'))
    myZip.write(os.path.join(release_dir, 'com.esri.prt.core.dll'), os.path.join(arch_release_dir, 'com.esri.prt.core'
                                                                                                   '.dll'))
    myZip.write(os.path.join(release_dir, 'glutess.dll'), os.path.join(arch_release_dir, 'glutess.dll'))
    myZip.write(os.path.join(release_dir, 'RhinoPRT.rhp'), os.path.join(arch_release_dir, 'RhinoPRT.rhp'))
    myZip.write(os.path.join(release_gh, 'RhinoPRT.dll'), os.path.join(arch_release_dir, 'RhinoPRT.dll'))
    myZip.write(os.path.join(release_gh, 'GrasshopperPRT.gha'), os.path.join(arch_release_dir, 'GrasshopperPRT.gha'))
