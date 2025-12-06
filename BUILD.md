# Building & Flashing to Pico Zero

## Prerequisites

You should have these tools available:
- `cmake` - Build system
- `arm-none-eabi-gcc` - ARM compiler
- `picotool` - Flashing utility (or UF2 bootloader method)
- Pico SDK installed (usually in `~/.pico-sdk/`)

## Step 1: Configure for Pico Zero

✅ **Already done!** The CMakeLists.txt has been updated:
```cmake
set(PICO_BOARD pico_zero CACHE STRING "Board type")
```

This tells the Pico SDK to use the Pico Zero board configuration.

## Step 2: Create Build Directory

```bash
cd /workspaces/picocalc-movi
rm -rf build                 # Clean if you have an old build
mkdir -p build
cd build
```

## Step 3: Configure the Build

```bash
cmake ..
```

This should output something like:
```
-- PICO_BOARD is set to pico_zero
-- Compiling for Raspberry Pi Pico Zero
-- ...
-- Build files have been written to: /workspaces/picocalc-movi/build
```

## Step 4: Compile

```bash
make -j4
```

The `-j4` uses 4 parallel jobs to speed up compilation. Adjust based on your CPU cores.

Expected output:
```
Scanning dependencies...
[  5%] Building C object ...
[ 10%] Linking C executable picocalc-text-starter.elf
[100%] Built target picocalc-text-starter
```

The final binary will be at:
```
build/picocalc-text-starter.uf2
```

## Step 5: Put Pico Zero into Bootloader Mode

**Method 1: Using BOOTSEL Button (Easiest)**
1. Disconnect Pico Zero from USB
2. Hold the **BOOTSEL button** (on top of board)
3. Plug USB cable in while holding BOOTSEL
4. Release BOOTSEL
5. A new USB drive should appear (`RPI-RP2`)

**Method 2: Using Picotool**
```bash
picotool reboot -a                # Force into bootloader mode
```

## Step 6: Flash the Binary

**Option A: Drag-and-Drop (Easiest)**
1. The Pico Zero appears as a USB drive
2. Drag `build/picocalc-text-starter.uf2` onto it
3. Pico automatically reboots and runs the code

**Option B: Using picotool**
```bash
picotool load -fx build/picocalc-text-starter.uf2
```

The `-f` forces upload, `-x` auto-runs after flashing.

**Option C: Manual with picotool**
```bash
cd build
picotool load picocalc-text-starter.uf2
picotool reboot
```

## Step 7: Verify It's Running

Once flashed:
1. Pico Zero should show the PicoCalc display
2. You should see the gimbal controller startup message
3. Display should show telemetry (initially default values without gimbal)
4. D-pad should respond (nothing happens without gimbal, but inputs work)

## Complete Build Script

Copy this into a file (`build.sh`) for quick rebuilding:

```bash
#!/bin/bash
set -e

cd /workspaces/picocalc-movi

# Clean old build (optional)
# rm -rf build

# Create build directory if needed
mkdir -p build
cd build

# Configure
echo "Configuring build for Pico Zero..."
cmake ..

# Compile
echo "Compiling..."
make -j4

# Show result
echo ""
echo "✅ Build successful!"
echo "Binary: $(pwd)/picocalc-text-starter.uf2"
echo ""
echo "Next steps:"
echo "1. Put Pico Zero in bootloader mode (hold BOOTSEL, plug in USB)"
echo "2. Drag .uf2 file to the RPI-RP2 drive"
echo "   OR use: picotool load -fx picocalc-text-starter.uf2"
```

Make it executable:
```bash
chmod +x build.sh
./build.sh
```

## Troubleshooting

### Build fails with "Pico SDK not found"

The SDK should be at: `~/.pico-sdk/`

If not, set it manually:
```bash
export PICO_SDK_PATH=~/pico/pico-sdk
cmake ..
```

### Compiler errors about missing headers

Make sure your Pico SDK is up to date (version 2.0+):
```bash
cd ~/.pico-sdk
git pull
```

### Picotool not found

Install it:
```bash
pip install picotool
# or
brew install picotool    # macOS
sudo apt install picotool  # Linux
```

### Pico doesn't appear as USB drive

- Try different USB cable (some are charge-only)
- Restart your computer
- Try BOOTSEL button again with gentle hold
- Check if BOOTSEL button is stuck

### Display shows nothing after flashing

The program is running but display isn't initialized. This could be:
- Display not powered
- I2C connection issue
- Display initialization code problem

Check the serial output (if available) for error messages.

## Quick Rebuild Loop

Once built once, for faster rebuilds:

```bash
cd /workspaces/picocalc-movi/build
make -j4                           # Just recompile
picotool load -fx *.uf2            # Flash (auto-finds .uf2)
```

Or if you make code changes:
```bash
cd /workspaces/picocalc-movi
./build.sh                         # One command does everything
```

## Testing Without Hardware

You can test the code without a Movi Pro gimbal:

1. Build and flash as above
2. Display will show telemetry (all default values)
3. D-pad will respond (but gimbal won't move, no gimbal)
4. Mode switching works (F1 key)
5. Gimbal kill toggles (F2 key)

This is perfect for verifying the UI and input mapping before connecting real hardware.

## Next: Connecting the Gimbal

Once running on Pico Zero, to connect a Movi Pro:

1. Wire Pico Zero UART0 to gimbal:
   - **Pico GP0 (TX)** → Gimbal RX
   - **Pico GP1 (RX)** → Gimbal TX
   - **GND** → GND (same ground)

2. Power sequence:
   - Power gimbal first
   - Then power Pico Zero
   - This ensures gimbal is ready

3. Watch display:
   - Battery voltage should appear
   - Gimbal quaternion should update
   - Lens positions should change

4. Control:
   - Use D-pad to move gimbal
   - F1 to switch modes
   - F2 to toggle gimbal kill (safety)

## Size Check (Pico Zero vs Pico)

Pico Zero is smaller and lower power than regular Pico, but the code is identical. The binary will work on both:

- **Pico Zero**: 2MB flash (plenty of space)
- **Regular Pico**: 2MB flash (plenty of space)
- Binary size: ~100-150KB (very small)

## Performance on Pico Zero

The gimbal controller loop runs at:
- **50Hz main loop** (20ms updates) - Pico Zero handles easily
- **Display ~1Hz** (less demanding)
- **UART 111111 baud** - No problem

Pico Zero has plenty of performance headroom for this application.

---

**Ready to build?** Run:
```bash
cd /workspaces/picocalc-movi/build
cmake ..
make -j4
```

Then flash using one of the methods above! 🚀
