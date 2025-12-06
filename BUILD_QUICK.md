# 5-Minute Build Guide for Pico Zero

**Goal:** Build and flash the Movi Pro gimbal controller to your Pico Zero

## Prerequisites Check

✅ You should have:
- `cmake` installed
- ARM compiler (`arm-none-eabi-gcc`)
- Pico SDK at `~/.pico-sdk/` or `~/pico-sdk/`
- Your Pico Zero connected via USB

## The 4-Step Process

### Step 1: Open Terminal and Navigate (30 seconds)

```bash
cd /workspaces/picocalc-movi
ls                    # Verify you see movi_*.c files
```

### Step 2: Create Build Folder (1 minute)

```bash
rm -rf build          # Clean old builds
mkdir build
cd build
```

### Step 3: Build the Code (3 minutes)

```bash
cmake ..
make -j4
```

When done, you'll see:
```
[100%] Built target picocalc-text-starter
```

### Step 4: Flash to Pico Zero (30 seconds)

**Option A: Drag-and-Drop (EASIEST)**
```
1. Hold BOOTSEL button on Pico Zero
2. Plug USB cable in (while holding BOOTSEL)
3. Release BOOTSEL
4. USB drive "RPI-RP2" appears on desktop
5. Drag: picocalc-text-starter.uf2 → RPI-RP2
6. Pico reboots automatically ✅ DONE
```

**Option B: Using picotool**
```bash
picotool load -fx picocalc-text-starter.uf2
```

**Option C: Manual**
```bash
cp picocalc-text-starter.uf2 /media/[your-rpi-drive]/
# Pico reboots automatically
```

## That's It! 🎉

Your Pico Zero is now running the gimbal controller.

## What You Should See

1. **PicoCalc display lights up** with gimbal controller screen
2. **Telemetry displays:**
   - Current mode
   - Battery voltage (default if no gimbal)
   - Gimbal angles
   - Lens positions

3. **D-pad works:**
   - Try pressing D-pad up/down/left/right
   - Nothing moves yet (no gimbal), but inputs register

## Testing Before Hardware

Try these:

```
F1 → Display should change mode (Rate → Position → Lens)
F2 → "Gimbal Kill: OFF" should toggle to "ON"
D-Pad → Nothing moves but system responds
```

If all work, your Pico Zero is ready for gimbal!

## Connecting Real Gimbal (When Ready)

See `WIRING.md` for:
- Pin connections (3 wires)
- Power sequence
- Troubleshooting

## If Build Fails

**Most common issues:**

**"cmake not found"**
```bash
# Install cmake
brew install cmake    # macOS
sudo apt install cmake  # Linux/Ubuntu
```

**"Pico SDK not found"**
```bash
# Set the SDK path
export PICO_SDK_PATH=~/.pico-sdk
cd build && cmake ..
```

**"arm-none-eabi-gcc not found"**
```bash
# Install ARM toolchain
brew install arm-none-eabi-gcc    # macOS
sudo apt install gcc-arm-none-eabi  # Linux
```

## Quick Rebuild

Next time (after code changes):

```bash
cd /workspaces/picocalc-movi/build
make -j4
picotool load -fx picocalc-text-starter.uf2
```

Or create a one-liner:
```bash
cd /workspaces/picocalc-movi/build && make -j4 && picotool load -fx picocalc-text-starter.uf2
```

## Files Reference

For detailed info, see:

| File | Info |
|------|------|
| `BUILD.md` | Complete build guide |
| `WIRING.md` | How to wire gimbal |
| `QUICKSTART.md` | Control guide |
| `MOVI_CONTROLLER.md` | Full API documentation |

## Next Steps

1. **Build** (follow 4 steps above)
2. **Flash** to Pico Zero
3. **Test** without gimbal (all inputs work?)
4. **Wire up** gimbal (see WIRING.md)
5. **Connect** gimbal and test real control

---

**Questions?**

- Build issues → Check BUILD.md
- Wiring questions → Check WIRING.md  
- Control questions → Check QUICKSTART.md
- API details → Check MOVI_CONTROLLER.md

**You're ready to build!** 🚀
