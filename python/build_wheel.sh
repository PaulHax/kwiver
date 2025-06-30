#!/bin/bash

set -e

# Check if we're at the KWIVER root directory (should have python/ subdirectory with requirements_dev.txt)
if [ ! -f "python/requirements_dev.txt" ]; then
  echo "Error: This script should be run from the KWIVER root directory (where python/requirements_dev.txt exists)"
  echo "Current directory: $(pwd)"
  echo "Please run this script from the KWIVER root directory"
  exit 1
fi

# Configure parallel jobs
PARALLEL_JOBS=${KWIVER_PARALLEL_JOBS:-$(nproc)}

# Clean build artifacts but preserve cache directories
rm -rf _skbuild/ build/ *.egg-info/ 2>/dev/null || true
rm -f dist/*.whl 2>/dev/null || true

# Create cache directories in dist
mkdir -p dist/sccache-cache
mkdir -p dist/pip-cache

echo "Starting Docker build..."
LOG_FILE="$(pwd)/kwiver_build.log"
echo "Build output will be logged to: $LOG_FILE"

docker run --rm \
  -v $(pwd):/kwiver \
  -v $(pwd)/dist/sccache-cache:/tmp/sccache \
  -v $(pwd)/dist/pip-cache:/tmp/pip \
  -w /kwiver \
  -e CMAKE_CONFIGURATION=wheel_linux38_x86_64_ceres_dbow2_ffmpeg_gdal_kpf_opencv_pdal_proj_python_uuid_vtk_vxl \
  -e SCCACHE_DIR=/tmp/sccache \
  -e PIP_CACHE_DIR=/tmp/pip \
  -e HOST_UID=$(id -u) \
  -e HOST_GID=$(id -g) \
  -e PARALLEL_JOBS=$PARALLEL_JOBS \
  kitware/kwiver:ci-python-20240405 \
  bash -c "
    export PYTHON_PREFIX=/opt/python/cp38-cp38
    export GIT_CLONE_PATH=\$(pwd)
    
    echo 'Setting up cache directories...'
    # Ensure cache directories exist and are writable by root
    mkdir -p /tmp/sccache /tmp/pip
    touch /tmp/sccache/.keep /tmp/pip/.keep
    chown -R root:root /tmp/sccache /tmp/pip
    
    echo 'Configuring Git for Docker environment...'
    git config --global --add safe.directory /kwiver
    
    echo 'Creating virtual environment...'
    \$PYTHON_PREFIX/bin/python -m venv build/ci-venv
    . build/ci-venv/bin/activate
    
    echo 'Installing Python dependencies...'
    pip install -U pip
    pip install -r ./python/requirements_dev.txt
    pip install scikit-build auditwheel pybind11[global]

    echo 'Setting up sccache...'
    # Use CI script but download to /tmp instead of .gitlab
    mkdir -p /tmp/sccache-bin
    cd /tmp/sccache-bin
    # Run the CI script but redirect to our temp location
    sed 's|cd \.gitlab|cd /tmp/sccache-bin|' /kwiver/.gitlab/ci/sccache.sh | sh
    export PATH=/tmp/sccache-bin:\$PATH
    cd /kwiver

    # Ensure sccache uses our mounted cache directory  
    export SCCACHE_DIR=/tmp/sccache
    sccache --start-server
    sccache --show-stats

    echo 'Building wheel (CI-style)...'
    # Ensure CMAKE_PREFIX_PATH is set for pybind11 and other fletch dependencies
    export CMAKE_PREFIX_PATH=/opt/fletch:\$CMAKE_PREFIX_PATH
    
    # Standard build without complex compiler wrapping
    python setup.py bdist_wheel -- -C \$GIT_CLONE_PATH/.gitlab/ci/configure_wheel.cmake -DCMAKE_PREFIX_PATH=/opt/fletch -Dpybind11_library=pybind11::module -- -j$PARALLEL_JOBS

    # Set library path to include fletch libraries for auditwheel
    export LD_LIBRARY_PATH=/opt/fletch/lib/:\$LD_LIBRARY_PATH
    
    # Only process the original linux wheel (not any pre-existing manylinux wheels)
    ORIGINAL_WHEEL=\$(ls dist/*linux_x86_64.whl | grep -v manylinux | head -1)
    if [ -n \"\$ORIGINAL_WHEEL\" ]; then
        auditwheel show \$ORIGINAL_WHEEL
        # Try auditwheel repair with --exclude to skip problematic GL libraries
        auditwheel repair \$ORIGINAL_WHEEL --exclude libGLdispatch-228223fa.so.0.0.0 --exclude libGLX_mesa.so.0 --exclude libGL.so.1 || {
            echo 'Auditwheel repair failed, but continuing with unrepaired wheel...'
            echo 'Wheel will work locally but may not be portable'
        }
    else
        echo 'No original linux wheel found to repair'
    fi
    
    # Move to final location (prefer repaired wheel if available)
    if [ -d wheelhouse ] && [ -n \"\$(ls -A wheelhouse 2>/dev/null)\" ]; then
        rm -rf dist-orig/ 2>/dev/null || true
        cp -v wheelhouse/*.whl dist/
        rm -rf wheelhouse/
        echo 'Using repaired manylinux wheel'
        ls -lh dist/
    else
        echo 'Using original wheel (auditwheel repair failed)'
    fi
    
    echo 'Build completed!'
    
    echo 'Final sccache statistics:'
    sccache --show-stats
    
    echo 'Fixing file ownership...'
    chown -R \$HOST_UID:\$HOST_GID dist/ _skbuild/ kwiver_build.log /tmp/sccache /tmp/pip 2>/dev/null || true
  " 2>&1 | tee "$LOG_FILE"

# Validate wheel was created
if ls dist/*.whl >/dev/null 2>&1; then
    echo "✓ Wheel(s) created successfully:"
    ls -lh dist/*.whl
else
    echo "✗ ERROR: No wheel files found!"
    exit 1
fi
