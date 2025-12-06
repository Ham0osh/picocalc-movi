# Pico Zero Wiring Guide for Movi Pro Gimbal

## UART Connection Diagram

### Pico Zero Pinout
```
                    Pico Zero
    ┌─────────────────────────────┐
    │ USB                         │
    │ ▯▯▯▯▯▯▯▯▯                 │
    │                             │
    │ GP0 (TX) ═══════════════┐   │
    │ GP1 (RX) ═════════════╗ ┃   │
    │ GND ═════════════════╗╗┃   │
    │                      ║║║   │
    └──────────────────────║║║───┘
                           ║║║
                    ┌──────╨╨╨──────┐
                    │   Movi Pro    │
                    │   Gimbal      │
                    │               │
                    │ RX ◄──────────┤
                    │ TX ────────►  │
                    │ GND ◄───────  │
                    └───────────────┘
```

### Physical Wiring

**Three wires needed:**

| Pico Zero | Gimbal | Color (Suggested) |
|-----------|--------|-------------------|
| GP0 (Pin 1) | RX | Red (TX out) |
| GP1 (Pin 2) | TX | Yellow (RX in) |
| GND (Pin 3) | GND | Black (Ground) |

### Pico Zero Pin Layout

```
View from top with USB on left:

┌─ USB ─┐
│ ●1 ●2 │ ← GP0(TX), GP1(RX) 
│ ●3 ●4 │ ← GND, 3V3
│ ●5 ●6 │ ← Other pins...
└───────┘

Pin 1 = GP0 (UART TX)   ← Connect to Gimbal RX
Pin 2 = GP1 (UART RX)   ← Connect to Gimbal TX  
Pin 3 = GND             ← Connect to Gimbal GND
Pin 4 = 3V3             ← (Not needed for gimbal)
```

## Power Setup

⚠️ **IMPORTANT: Power Sequence**

1. **Power the Movi Pro gimbal FIRST**
   - Ensures gimbal is ready before Pico initializes

2. **Then connect/power Pico Zero**
   - Can be powered via USB or from PicoCalc system

3. **Shutdown sequence (reverse)**
   - Disconnect Pico first
   - Then power down gimbal

## Connection Checklist

Before powering on:

- [ ] GP0 → Gimbal RX (red wire)
- [ ] GP1 → Gimbal TX (yellow wire)
- [ ] GND → Gimbal GND (black wire)
- [ ] No crossed wires
- [ ] All connections soldered/crimped securely
- [ ] No shorts between pins
- [ ] TX and RX not reversed
- [ ] GND connected (essential!)

## Baud Rate Confirmation

The code uses **111111 baud** for gimbal communication.

This is set in `qx_uart.c`:
```c
#define GIMBAL_UART_BAUD 111111
```

Verify your Movi Pro gimbal is configured for this baud rate. This is the standard FreeFly API rate.

## Testing the Connection

### Without Gimbal (Safety Test)
1. Flash code to Pico Zero
2. Watch display for startup message
3. Test D-pad (no movement, just inputs)
4. Verify mode switching works
5. All inputs should work normally

### With Gimbal Connected
1. Flash code to Pico Zero
2. Power gimbal first
3. Connect UART wires (3 wires)
4. Power Pico Zero
5. Watch display:
   - Battery voltage should appear (not 0V)
   - Gimbal quaternion should show values
   - Lens positions should update
6. Test D-pad → gimbal should move
7. Test F1 → mode should change
8. Test F2 → gimbal kill should toggle

## If Connection Fails

**No telemetry appearing:**
1. Check wires:
   - GP0 → Gimbal RX (not crossed)
   - GP1 → Gimbal TX (not crossed)
   - GND properly connected
2. Power sequence:
   - Gimbal ON first
   - Pico Zero second
3. Baud rate:
   - Gimbal must be 111111 baud
   - Code is hardcoded to 111111
4. USB cable:
   - Some cables are power-only
   - Try different USB port/cable

**Gimbal appears but doesn't respond:**
1. Gimbal kill might be ON (F2 toggle)
2. Control mode might be wrong (F1 cycle)
3. Try F4 to clear any faults
4. Verify gimbal motor power

**Garbled display:**
1. This is UART communication issue
2. Check all three wires connected
3. Try different USB power source
4. Reset both devices

## Pico Zero Advantages

- **Smaller** - Fits in compact systems
- **Lower cost** - Cheaper than regular Pico
- **Adequate performance** - Handles gimbal control easily
- **Same code** - Works identically to regular Pico

## Pin Reference (Full Pico Zero)

If you need other pins:

```
GP0  = UART0 TX (Pin 1)  ← Gimbal RX
GP1  = UART0 RX (Pin 2)  ← Gimbal TX
GND  = Ground (Pin 3)    ← Gimbal GND
3V3  = 3.3V out (Pin 4)
GP2  = (available)
GP3  = (available)
GP4  = (available)
...
VBUS = USB Power
```

Most other pins available for expansion.

## Expansion Ideas

Once gimbal control is working:

1. **Add analog joystick** (instead of D-pad)
   - Use ADC pins (GP26, GP27, GP28)
   - More natural control

2. **Add more buttons** 
   - GPIO pins available for extra functions
   - Quick-save positions
   - Profile selection

3. **SD card logging**
   - Log gimbal movements
   - Record control sequences
   - Firmware updates

4. **Wireless remote**
   - Add Bluetooth module
   - Control from distance
   - Monitor telemetry remotely

---

**Diagram Summary:**
```
Pico Zero          Gimbal
GP0 ───────────► RX
GP1 ◄───────────  TX
GND ────────────  GND

Power: Gimbal first, then Pico
Baud: 111111
Test: Display should show battery voltage
```

Ready to connect! 🎥
