#!/bin/sh

set -e

readonly version="3.26.3"

case "$( uname -s )" in
    Linux)
        shatool="sha256sum"
        sha256sum="28d4d1d0db94b47d8dfd4f7dec969a3c747304f4a28ddd6fd340f553f2384dc2"
        platform="linux-x86_64"
        ;;
    Darwin)
        shatool="shasum -a 256"
        sha256sum="2b44cc892dc68b42123b9517c5d903690785b7ef489af26abf2fe3f3a6f2a112"
        platform="macos-universal"
        ;;
    *)
        echo "Unrecognized platform $( uname -s )"
        exit 1
        ;;
esac
readonly shatool
readonly sha256sum
readonly platform

readonly filename="cmake-$version-$platform"
readonly tarball="$filename.tar.gz"

cd .gitlab

echo "$sha256sum  $tarball" > cmake.sha256sum
curl -OL "https://github.com/Kitware/CMake/releases/download/v$version/$tarball"
$shatool --check cmake.sha256sum
tar xf "$tarball"
mv "$filename" cmake

if [ "$( uname -s )" = "Darwin" ]; then
    ln -s CMake.app/Contents/bin "cmake/bin"
fi
