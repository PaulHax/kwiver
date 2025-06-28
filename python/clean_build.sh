#!/bin/bash

set -e

# Check if we're at the KWIVER root directory
if [ ! -f "python/requirements_dev.txt" ]; then
  echo "Error: This script should be run from the KWIVER root directory (where python/requirements_dev.txt exists)"
  echo "Current directory: $(pwd)"
  echo "Please run this script from the KWIVER root directory"
  exit 1
fi

echo "Cleaning build artifacts..."
rm -rf dist/ _skbuild/ build/ *.egg-info/ kwiver_build.log  2>/dev/null || true
echo "Done."