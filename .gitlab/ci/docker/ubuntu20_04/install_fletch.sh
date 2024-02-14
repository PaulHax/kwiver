#!/usr/bin/sh

set -e

readonly fletch_repo="https://github.com/Kitware/fletch"
# update for GDAL rpath merge
readonly fletch_commit="c310f3cee87448a31ffbbb8f24c4bdd828bae071"

readonly fletch_root="$HOME/fletch"
readonly fletch_src="$fletch_root/src"
readonly fletch_build="$fletch_root/build"
readonly fletch_prefix="/opt/fletch"

# check for existing dir - helpful for debugging build failures interacively
if [ ! -d "$fletch_src" ]; then
  git clone "$fletch_repo" "$fletch_src"
  git -C "$fletch_src" checkout "$fletch_commit"

  git -C "$fletch_src" config user.name "kwiver Developers"
  git -C "$fletch_src" config user.email "kwiver-developers@kitware.com"

  # Fix rpath for cmake projects
  # https://github.com/Kitware/fletch/pull/745
  git -C "$fletch_src" fetch origin refs/pull/745/head
  git -C "$fletch_src" merge --no-ff FETCH_HEAD -m "x"
fi

cmake \
  -B "$fletch_build" \
  "-Dfletch_BUILD_INSTALL_PREFIX=$fletch_prefix" \
  -C "$HOME/configure_fletch.cmake" \
  -S "$fletch_src"
cmake --build "$fletch_build" --parallel "$(nproc)"

# Clean up.
rm -rf "$fletch_root"
