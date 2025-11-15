# 🔧 Troubleshooting Guide - Button Inputs

## NodeMCU Button Issues - SOLVED

If your buttons aren't registering on NodeMCU (ESP8266), here's what was fixed:

### Problem 1: GPIO16 Limitations ❌

**Issue:** GPIO16 (D0) on ESP8266 doesn't fully support `INPUT_PULLUP`
- GPIO16 has different hardware than other GPIOs
- Internal pullup is unreliable or absent
- Can cause buttons not to register

**Solution:** Use GPIO0 (D3) instead ✅

### Problem 2: Debounce Logic Bug ❌

**Issue:** The original button reading logic had a flaw
- It wasn't properly tracking state transitions
- Edge detection wasn't reliable
- Buttons might not register or trigger multiple times

**Solution:** Complete rewrite with proper state tracking ✅

## Updated Pin Configuration for NodeMCU

### ✅ RECOMMENDED (Current):
```
UP Button:     D5 (GPIO14) → GND
DOWN Button:   D6 (GPIO12) → GND
SELECT Button: D7 (GPIO13) → GND
BACK Button:   D3 (GPIO0) → GND   ← Changed from D0
```

### ❌ OLD (Had Issues):
```
BACK Button:   D0 (GPIO16) → GND   ← Problematic on ESP8266
```

## How to Update Your Code

### In Your Sketch:
```cpp
// OLD - Don't use this
ui = new UIEngine(14, 12, 13, 16);  // GPIO16 problematic

// NEW - Use this instead
ui = new UIEngine(14, 12, 13, 0);   // GPIO0 works reliably
```

## Testing Your Buttons

Add this debug code to verify buttons work:

```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize pins
    pinMode(14, INPUT_PULLUP);  // UP
    pinMode(12, INPUT_PULLUP);  // DOWN
    pinMode(13, INPUT_PULLUP);  // SELECT
    pinMode(0, INPUT_PULLUP);   // BACK
    
    Serial.println("Button Test - Press buttons to test");
}

void loop() {
    if (digitalRead(14) == LOW) Serial.println("UP pressed");
    if (digitalRead(12) == LOW) Serial.println("DOWN pressed");
    if (digitalRead(13) == LOW) Serial.println("SELECT pressed");
    if (digitalRead(0) == LOW) Serial.println("BACK pressed");
    delay(200);
}
```

## What Was Fixed in MintUi.h

### 1. Improved Debounce Logic
- Now properly tracks: raw reading → stable state → edge detection
- Prevents multiple triggers
- More reliable button press detection

### 2. ESP8266-Specific Handling
```cpp
#ifdef ESP8266
if (btnBack == 16) {
    pinMode(btnBack, INPUT);  // GPIO16 needs external pullup
} else {
    pinMode(btnBack, INPUT_PULLUP);
}
#endif
```

### 3. Better Initialization
- Longer stabilization delay (100ms)
- Proper state initialization
- More robust startup

## Still Having Issues?

### Check Your Wiring
1. Verify all buttons connect GPIO pin to GND
2. Check for loose connections
3. Make sure you're using the correct pin numbers

### Serial Monitor Debug
Add this to your loop to see button states:
```cpp
void loop() {
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 1000) {
        Serial.print("Buttons: UP=");
        Serial.print(digitalRead(14));
        Serial.print(" DN=");
        Serial.print(digitalRead(12));
        Serial.print(" SEL=");
        Serial.print(digitalRead(13));
        Serial.print(" BACK=");
        Serial.println(digitalRead(0));
        lastPrint = millis();
    }
    ui->update();
}
```

Expected output when not pressed: `1 1 1 1` (HIGH with pullup)
Expected output when pressed: `0 0 0 0` (LOW when button pressed)

### Alternative Pin Options for ESP8266

If you need different pins, these GPIOs work well with INPUT_PULLUP:
- ✅ GPIO0 (D3) - used for BACK
- ✅ GPIO2 (D4) - has LED but works
- ✅ GPIO12 (D6) - used for DOWN
- ✅ GPIO13 (D7) - used for SELECT
- ✅ GPIO14 (D5) - used for UP
- ✅ GPIO15 (D8) - can use but pulled down at boot
- ⚠️ GPIO16 (D0) - limited INPUT_PULLUP support

### Avoid These:
- ❌ GPIO1/GPIO3 - Serial TX/RX
- ❌ GPIO6-11 - Flash memory
- ⚠️ GPIO0 - Boot mode (pulled HIGH with button for normal boot)
- ⚠️ GPIO2 - Boot mode (must be HIGH at boot)
- ⚠️ GPIO15 - Boot mode (must be LOW at boot)

## Changes Applied

✅ Fixed debounce logic in `MintUi.h`
✅ Updated `MintUI_NodeMCU_Example.ino` to use GPIO0 instead of GPIO16
✅ Added ESP8266-specific GPIO handling
✅ Improved initialization with proper delays
✅ Better state tracking for button presses

Your buttons should now work reliably! 🎉

## Quick Reference

**Current GPIO Mapping (NodeMCU):**
```
UP     = GPIO14 = D5
DOWN   = GPIO12 = D6
SELECT = GPIO13 = D7
BACK   = GPIO0  = D3  (NEW!)
```

**Code to Initialize:**
```cpp
ui = new UIEngine(14, 12, 13, 0);
```
