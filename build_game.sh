#!/bin/bash

# Output on error
set -e

# Delete build directory if it exists
if [ -d "build" ]; then
  rm -r build
fi

# Create build directory
mkdir build
cd build

cmake ..

make -j12
