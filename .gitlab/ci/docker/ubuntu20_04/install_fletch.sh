#!/usr/bin/sh

set -e

readonly fletch_repo="https://github.com/Kitware/fletch"
readonly fletch_commit="998bbc00e7ccb000334c6d6d21629de10b4b8b66" # snapshot at image creation time

readonly fletch_root="$HOME/fletch"
readonly fletch_src="$fletch_root/src"
readonly fletch_build="$fletch_root/build"
readonly fletch_prefix="/opt/fletch"

git clone "$fletch_repo" "$fletch_src"
git -C "$fletch_src" checkout "$fletch_commit"

git -C "$fletch_src" config user.name "kwiver Developers"
git -C "$fletch_src" config user.email "kwiver-developers@kitware.com"

# https://github.com/Kitware/fletch/pull/722
git -C "$fletch_src" fetch origin refs/pull/722/head
git -C "$fletch_src" merge --no-ff FETCH_HEAD

cmake \
  -GNinja \
  -B "$fletch_build" \
  "-Dfletch_BUILD_INSTALL_PREFIX=$fletch_prefix" \
  -Dfletch_BUILD_WITH_PYTHON=ON \
  -Dfletch_BUILD_WITH_CX11=ON \
  -Dfletch_ENABLE_Eigen=ON \
  -Dfletch_ENABLE_pybind11=ON \
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
