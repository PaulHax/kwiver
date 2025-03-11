#!/usr/bin/sh

set -e

readonly fletch_repo="https://github.com/Kitware/fletch"
# update to include new default packages used by kwiver
readonly fletch_commit="70f4e025067453cbf2f40565c05d80c6263d64c8"

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
fi

cmake \
  -B "$fletch_build" \
  "-Dfletch_BUILD_INSTALL_PREFIX=$fletch_prefix" \
  -C "$HOME/configure_fletch.cmake" \
  -S "$fletch_src"
cmake --build "$fletch_build" --parallel "$(nproc)"

# Clean up.
rm -rf "$fletch_root"
