# MintUI Examples

This folder contains example sketches for MintUI on different platforms.

## 📁 Folder Structure

Each example is in its own folder (required by Arduino IDE):

- **MintUI_ESP32_Example/** - Full-featured demo for ESP32
- **MintUI_NodeMCU_Example/** - NodeMCU (ESP8266) optimized demo

## 🚀 How to Use

### Important: Arduino Sketch Organization

Arduino requires each sketch to be in **its own folder** with a matching name. For example:
- `MintUI_ESP32_Example/MintUI_ESP32_Example.ino` ✅
- `MintUI_ESP32_Example/example.ino` ❌

### Installation Steps

1. **Copy the MintUi.h file** to your Arduino libraries folder or keep it in each example folder
2. **Open the example** you want:
   - For ESP32: Open `MintUI_ESP32_Example/MintUI_ESP32_Example.ino`
   - For NodeMCU: Open `MintUI_NodeMCU_Example/MintUI_NodeMCU_Example.ino`
3. **Install required libraries** via Arduino Library Manager:
   - Adafruit GFX Library
   - Adafruit SSD1306
4. **Select your board**:
   - ESP32: "ESP32 Dev Module"
   - NodeMCU: "NodeMCU 1.0 (ESP-12E Module)"
5. **Upload!**

## 📌 Placing MintUi.h

You have two options for the header file:

### Option 1: In Each Example Folder (Easier for beginners)
```
examples/
├── MintUI_ESP32_Example/
│   ├── MintUI_ESP32_Example.ino
│   └── MintUi.h
└── MintUI_NodeMCU_Example/
    ├── MintUI_NodeMCU_Example.ino
    └── MintUi.h
```

### Option 2: In Arduino Libraries (Better for multiple projects)
```
Arduino/
└── libraries/
    └── MintUI/
        ├── MintUi.h
        └── examples/
            ├── MintUI_ESP32_Example/
            └── MintUI_NodeMCU_Example/
```

## ⚠️ Common Errors

### "Redefinition" Error
**Problem:** Multiple .ino files in the same folder
```
error: redefinition of 'UIEngine* ui'
```

**Solution:** Keep only ONE .ino file per folder. Don't mix ESP32 and NodeMCU examples in the same directory!

### "Display initialization failed"
**Problem:** Wrong I2C address or wiring
**Solution:** 
- Check wiring (SDA/SCL)
- Try changing I2C address in MintUi.h from `0x3C` to `0x3D`
- Run an I2C scanner to find your display's address

## 🎮 Controls

Both examples use 4 buttons:
- **UP**: Navigate to previous widget
- **DOWN**: Navigate to next widget
- **SELECT**: Activate/click focused widget
- **BACK**: Return to previous window

## 📖 Features Demonstrated

- ✅ Multiple windows with navigation
- ✅ Buttons with callbacks
- ✅ Labels for text display
- ✅ Checkboxes with state management
- ✅ Smooth animations with CSS easing curves
- ✅ Window slide transitions
- ✅ Focus management
- ✅ Platform-specific features (ESP8266 stats)

## 🔧 Customization

Feel free to modify these examples to:
- Change button pins
- Add more widgets
- Create custom windows
- Implement your own UI logic
- Add WiFi functionality
- Connect to sensors

Refer to the main README.md in the root folder for API documentation.
