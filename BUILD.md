# Building kwiver
These instructions are subject to change as we enable more features in kwiver-v2.

## Prerequisites
### Fletch
Build fletch using the main branch and enable the features we use in this project.
Change directory names if desired.

```
fletch_dir="fletch"
mkdir -p "${fletch_dir}"
cd "${fletch_dir}"
fletch_git="fletch.git"
git clone https://github.com/Kitware/fletch "$fletch_git"
fletch_build="build"
mkdir -p "$fletch_build"
cd "$fletch_build"
cmake \
  -GNinja \
  -Dfletch_DOWNLOAD_DIR="$fletch_dir/downloads" \
  -C <kwiver-source>/.gitlab/ci/docker/ubuntu22_04/configure_fletch.cmake \
  ../$fletch_git/

cmake --build .
```

### Python
Setup a python environment:
```
python -m venv env
source env/bin/activate
pip install -r python/requirements_dev.txt
```

## Build kwiver
Use the CI's common configuration to build using the defaults.
```
mkdir build
cmake -GNinja -Dfletch_DIR=<fletch build directory> -C ../kwiver/.gitlab/ci/configure_common.cmake ../kwiver
cmake --build .
```

## Run Tests
From the build directory:

```
source setup_KWIVER.sh
ctest
```
