import argparse
import os
import zipfile
import re
import shutil
from pathlib import Path
import subprocess

PACKAGE_WHITELIST = ["PumaGrasshopper.gha", "PumaRhino.rhp", "com.esri.prt.core.dll", "glutess.dll",
                     "lib/PumaCodecs.dll", "lib/com.esri.prt.adaptors.dll", "lib/com.esri.prt.codecs.dll"]


def copy_to_zip(src_path: Path, relative_file_paths: list, dst: zipfile.ZipFile):
    for file_path in relative_file_paths:
        src_file = Path(src_path, file_path)
        dst.write(filename=src_file, arcname=file_path)


def copy_to_dir(src_path: Path, relative_file_paths: list, dst_path: Path):
    for rel_path in relative_file_paths:
        src_abs_path = Path(src_path, rel_path)
        if src_abs_path.is_file():
            dst_abs_path = Path(dst_path, rel_path)
            dst_abs_path.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(src_abs_path, dst_abs_path)


# Parse file "version.h" to get major, minor, revision, build numbers
def parse_version_file(version_file: Path) -> (int, int, int, int):
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


def clean_package_output(package_path: Path):
    if package_path.exists():
        shutil.rmtree(package_path)
    package_path.mkdir()


def build_rhi_package(build_dir: str, package_dir: str, v_major, v_minor, v_revision, v_build):
    version_str: str = f'v{v_major}.{v_minor}.{v_revision}.{v_build}'
    rhi_path = Path(package_dir, f'Puma_{version_str}.rhi')
    with zipfile.ZipFile(rhi_path, 'w') as myZip:
        copy_to_zip(Path(build_dir), PACKAGE_WHITELIST, myZip)


def build_yak_package(build_dir: str, package_dir: str, v_major, v_minor, v_revision):
    build_path = Path(build_dir)
    yak_temp_path = Path(package_dir, "yak_temp")
    copy_to_dir(build_path, PACKAGE_WHITELIST, yak_temp_path)
    copy_to_dir(build_path.parent, ['manifest.yml'], yak_temp_path)

    subprocess.run(["C:\\Program Files\\Rhino 7\\System\\Yak.exe", "build"], shell=True, check=True, cwd=yak_temp_path)

    # Get the file created: could change depending on Rhino/GH versions.
    yak_paths = list(yak_temp_path.glob("puma-*.yak"))
    if len(yak_paths) != 1:
        raise IOError('Error: the yak package could not be created.')
    yak_path = yak_paths[0]

    # create rhino 7 yak package
    yak_rh7_path = Path(yak_temp_path, f'puma-{v_major}.{v_minor}.{v_revision}-rh7-any.yak')
    shutil.copyfile(yak_path, yak_rh7_path)

    # move both yak to the package_output directory
    package_path = Path(package_dir)
    shutil.move(yak_path, Path(package_path, yak_path.name))
    shutil.move(yak_rh7_path, Path(package_path, yak_rh7_path.name))

    shutil.rmtree(yak_temp_path)


def build(build_mode: str, build_dir: Path, package_dir: Path, v_major, v_minor, v_revision, v_build):
    if build_mode == 'both' or build_mode == 'rhi':
        build_rhi_package(build_dir, package_dir, v_major, v_minor, v_revision, v_build)

    if build_mode == 'both' or build_mode == 'yak':
        build_yak_package(build_dir, package_dir, v_major, v_minor, v_revision)


def parse_args():
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument('--build-module', default='both', choices=['both', 'rhi', 'yak'],
                            help='The package to build.')
    args = arg_parser.parse_args()
    return args


def main():
    args = parse_args()

    root_path = Path(__file__).parent
    build_path = Path(root_path, 'build')
    package_path = Path(root_path, 'packages')
    clean_package_output(package_path)

    version_file = Path(root_path, "PumaRhino", "version.h")
    (v_major, v_minor, v_revision, v_build) = parse_version_file(version_file)

    build(args.build_module, build_path, package_path, v_major, v_minor, v_revision, v_build)


if __name__ == "__main__":
    main()
