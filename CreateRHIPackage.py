import os
import zipfile


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
curr_dir: str = os. getcwd()
esri_lib_dir: str = os.path.join(curr_dir, 'esri_sdk')
release_dir: str = os.path.join(curr_dir, 'x64', 'Release')
release_gh: str = os.path.join(curr_dir, 'x64', 'Release_gh')

if os.path.exists('RhinoPRT.rhi'):
    os.remove('RhinoPRT.rhi')

arch_release_dir: str = os.path.join('x64', 'Release')

with zipfile.ZipFile('RhinoPRT.rhi', 'w') as myZip:
    # Copy the folders bin and lib of Esri sdk
    copytree(esri_lib_dir, myZip, 'esri_sdk', ignore=['cmake', 'doc', 'include'])

    myZip.write(os.path.join(release_dir, 'codecs_rhino.dll'), os.path.join(arch_release_dir, 'codecs_rhino.dll'))
    myZip.write(os.path.join(release_dir, 'com.esri.prt.core.dll'), os.path.join(arch_release_dir, 'com.esri.prt.core'
                                                                                                   '.dll'))
    myZip.write(os.path.join(release_dir, 'glutess.dll'), os.path.join(arch_release_dir, 'glutess.dll'))
    myZip.write(os.path.join(release_dir, 'RhinoPRT.rhp'), os.path.join(arch_release_dir, 'RhinoPRT.rhp'))
    myZip.write(os.path.join(release_gh, 'RhinoPRT.dll'), os.path.join(arch_release_dir, 'RhinoPRT.dll'))
    myZip.write(os.path.join(release_gh, 'GrasshopperPRT.gha'), os.path.join(arch_release_dir, 'GrasshopperPRT.gha'))
