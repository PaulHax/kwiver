# Building kwiver
These instructions are subject to change as we enable more features in kwiver-v2.

## Prerequisites 
### Fletch 
Build fletch using an unmerged pull request and enable the features we use in this project.

```
fletch_dir="fletch"
mkdir -p "${fletch_dir}"
cd "${fletch_dir}"
fletch_git="fletch.git"
git clone https://github.com/Kitware/fletch "$fletch_git"
cd "$fletch_git"
git remote add chet https://github.com/chetnieter/fletch
git fetch chet
git checkout dev/update-pybind11-version
cd -
fletch_build="build"
mkdir -p "$fletch_build"
cd "$fletch_build"
cmake \
  -GNinja \
  -Dfletch_DOWNLOAD_DIR="$fletch_dir/downloads" \
  -C <kwiver-source>/.gitlab/ci/configure_fletch.cmake \
  ../fletch.git/

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
