#!/bin/bash
# Build script for Movi Pro Gimbal Controller on Pico Zero

set -e

WORKSPACE="/workspaces/picocalc-movi"

echo "=========================================="
echo "Building Movi Pro Gimbal Controller"
echo "Board: Pico Zero"
echo "=========================================="
echo ""

# Change to workspace
cd "$WORKSPACE"

# Check prerequisites
echo "Checking prerequisites..."
if ! command -v cmake &> /dev/null; then
    echo "❌ cmake not found. Install with: apt install cmake"
    exit 1
fi

if ! command -v arm-none-eabi-gcc &> /dev/null; then
    echo "❌ arm-none-eabi-gcc not found. Install with: apt install gcc-arm-none-eabi"
    exit 1
fi

echo "✅ Prerequisites OK"
echo ""

# Clean old build
echo "Cleaning old build..."
rm -rf build
mkdir -p build
cd build

# Configure
echo "Configuring CMake..."
if ! cmake ..; then
    echo "❌ CMake configuration failed"
    echo "Check that PICO_SDK_PATH is set correctly"
    exit 1
fi
echo "✅ CMake OK"
echo ""

# Compile
echo "Compiling (this may take a minute)..."
if ! make -j4; then
    echo "❌ Compilation failed"
    echo "Check error messages above"
    exit 1
fi
echo "✅ Compilation OK"
echo ""

# Check output
if [ -f picocalc-text-starter.uf2 ]; then
    SIZE=$(ls -lh picocalc-text-starter.uf2 | awk '{print $5}')
    echo "=========================================="
    echo "✅ BUILD SUCCESSFUL!"
    echo "=========================================="
    echo ""
    echo "Binary: $WORKSPACE/build/picocalc-text-starter.uf2"
    echo "Size: $SIZE"
    echo ""
    echo "Next steps:"
    echo "1. Hold BOOTSEL button on Pico Zero"
    echo "2. Plug in USB (while holding BOOTSEL)"
    echo "3. Drag picocalc-text-starter.uf2 to RPI-RP2 drive"
    echo ""
    echo "Or use:"
    echo "  picotool load -fx picocalc-text-starter.uf2"
    echo ""
else
    echo "❌ Build succeeded but .uf2 file not found"
    exit 1
fi
