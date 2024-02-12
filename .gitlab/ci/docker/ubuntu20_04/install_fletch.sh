#!/usr/bin/sh

set -e

readonly fletch_repo="https://github.com/Kitware/fletch"
# snapshot of master branch on 2024.02.13
readonly fletch_commit="3f39a6966bb94cbd03cf70f1814865d530470973"

readonly fletch_root="$HOME/fletch"
readonly fletch_src="$fletch_root/src"
readonly fletch_build="$fletch_root/build"
readonly fletch_prefix="/opt/fletch"

git clone "$fletch_repo" "$fletch_src"
git -C "$fletch_src" checkout "$fletch_commit"

cmake \
  -GNinja \
  -B "$fletch_build" \
  "-Dfletch_BUILD_INSTALL_PREFIX=$fletch_prefix" \
  -Dfletch_BUILD_WITH_PYTHON=ON \
  -Dfletch_BUILD_WITH_CX11=ON \
  -Dfletch_ENABLE_Eigen=ON \
  -Dfletch_ENABLE_pybind11=ON \
  -Dpybind11_SELECT_VERSION="2.10.3" \
  -Dfletch_ENABLE_FFmpeg=ON \
  -Dfletch_ENABLE_GTest=ON \
  -Dfletch_ENABLE_OpenCV=ON \
  -Dfletch_ENABLE_PDAL=ON \
  -Dfletch_ENABLE_GDAL=ON \
  -Dfletch_ENABLE_PROJ=ON \
  -Dfletch_ENABLE_libjpeg-turbo=ON \
  -Dfletch_ENABLE_libgeotiff=ON \
  -Dfletch_ENABLE_libtiff=ON \
  -Dfletch_ENABLE_GEOS=ON \
  -Dfletch_ENABLE_SQLite3=ON \
  -Dfletch_ENABLE_x264=OFF \
  -Dfletch_ENABLE_x265=OFF \
  -S "$fletch_src"
cmake --build "$fletch_build"

# Clean up.
rm -rf "$fletch_root"
