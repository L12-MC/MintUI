# MINTUI

A comprehensive UI engine for ESP32 with SSD1306 OLED displays featuring smooth animations, multiple windows, and various widgets. Used by MINT Firmware

## Features

### Widgets
- **Buttons** - Interactive buttons with press animations and callbacks
- **Labels** - Text display with customizable size and centering
- **Checkboxes** - Toggle switches with animated checkmarks and state callbacks

### Window Management
- Stack-based navigation system
- Smooth slide-in/slide-out transitions
- Title bar for each window
- Back button support for navigation

### Animations
- easing curve implementation
- Smooth transitions for all UI interactions
- Button press animations
- Checkbox toggle animations
- Window slide animations

### Input Handling
- 4-button navigation (UP, DOWN, SELECT, BACK)
- Debounced button reading
- Focus management with visual feedback
- Keyboard-like navigation between focusable widgets

## Hardware Requirements

- **ESP32** development board OR **NodeMCU (ESP8266)**
- SSD1306 OLED display (128x64, I2C)
- 4 push buttons (with optional external pull-up resistors)

## Wiring

### For ESP32

**SSD1306 OLED Display (I2C):**
- VCC → 3.3V
- GND → GND
- SDA → GPIO21
- SCL → GPIO22

**Buttons (with internal pull-ups):**
- UP Button → GPIO12 → GND
- DOWN Button → GPIO14 → GND
- SELECT Button → GPIO27 → GND
- BACK Button → GPIO26 → GND

### For NodeMCU (ESP8266)

**SSD1306 OLED Display (I2C):**
- VCC → 3.3V
- GND → GND
- SDA → D2 (GPIO4)
- SCL → D1 (GPIO5)

**Buttons:**
- UP Button → D5 (GPIO14) → GND (internal pullup)
- DOWN Button → D6 (GPIO12) → GND (internal pullup)
- SELECT Button → D7 (GPIO13) → GND (internal pullup)
- BACK Button → D0 (GPIO16) → GND (internal pullup)

**NodeMCU Pin Reference:**
- D0 = GPIO16, D1 = GPIO5, D2 = GPIO4, D3 = GPIO0
- D4 = GPIO2, D5 = GPIO14, D6 = GPIO12, D7 = GPIO13, D8 = GPIO15

*Note: All buttons use internal pull-up resistors on both platforms.*

## Software Dependencies

Install these libraries through Arduino Library Manager:

1. **Adafruit GFX Library**
2. **Adafruit SSD1306**

## Installation

1. Copy `MintUi.h` to your Arduino sketch folder
2. For ESP32: Use `MintUi-EXAMPLE.ino`
3. For NodeMCU: Use `MintUI_NodeMCU_Example.ino`
4. Install required libraries (Adafruit GFX, Adafruit SSD1306)
5. Select your board (ESP32 Dev Module or NodeMCU 1.0)
6. Upload!

## Usage

### Basic Setup

**For ESP32:**
```cpp
#include "MintUi.h"

UIEngine* ui;

void setup() {
    // Initialize UI Engine (button pins: UP, DOWN, SELECT, BACK)
    ui = new UIEngine(12, 14, 27, 26);
    
    if (!ui->begin()) {
        Serial.println("Display initialization failed!");
        while(1);
    }
    
    // Create your first window
    Window* mainWindow = new Window("Main Menu");
    
    // Add widgets...
    
    ui->pushWindow(mainWindow);
}

void loop() {
    ui->update();
    delay(10);
}
```

**For NodeMCU (ESP8266):**
```cpp
#include "MintUi.h"

UIEngine* ui;

void setup() {
    // Initialize UI Engine with NodeMCU GPIO pins
    // UP(D5/14), DOWN(D6/12), SELECT(D7/13), BACK(D0/16)
    ui = new UIEngine(14, 12, 13, 16);
    
    if (!ui->begin()) {
        Serial.println("Display initialization failed!");
        while(1);
    }
    
    // Create your first window
    Window* mainWindow = new Window("Main Menu");
    
    // Add widgets...
    
    ui->pushWindow(mainWindow);
}

void loop() {
    ui->update();
    delay(10);
    yield(); // Important for ESP8266!
}
```

### Creating Widgets

#### Label
```cpp
// Label(x, y, text, textSize=1, centered=false, id="")
Label* label = new Label(10, 20, "Hello World", 1, false);
window->addWidget(label);

// Update label text
label->setText("New Text");
```

#### Button
```cpp
void onButtonClick() {
    Serial.println("Button clicked!");
}

// Button(x, y, width, height, text, callback, id)
Button* btn = new Button(10, 30, 50, 15, "Click Me", onButtonClick);
window->addWidget(btn);
```

#### Checkbox
```cpp
void onCheckChange(bool checked) {
    Serial.println(checked ? "Checked" : "Unchecked");
}

// Checkbox(x, y, label, initialState, callback, id)
Checkbox* check = new Checkbox(10, 50, "Enable", false, onCheckChange);
window->addWidget(check);

// Get/set checkbox state
bool state = check->isChecked();
check->setChecked(true);
```

### Window Navigation

```cpp
// Create and push a new window
Window* settingsWindow = new Window("Settings");
// ... add widgets ...
ui->pushWindow(settingsWindow);

// Pop current window (go back)
ui->popWindow();

// Get current window
Window* current = ui->getCurrentWindow();

// Get widget by ID
Widget* widget = current->getWidgetById("myWidget");
```

### Custom Animations

```cpp
Animation myAnim;

void setup() {
    // start(startValue, endValue, durationMs)
    myAnim.start(0, 100, 500);
}

void loop() {
    float value = myAnim.getValue();
    bool running = myAnim.isRunning();
    // Use value for custom animations...
}
```

## API Reference

### UIEngine Class
- `UIEngine(btnUp, btnDown, btnSelect, btnBack)` - Constructor with button pins
- `bool begin()` - Initialize display and buttons
- `void pushWindow(Window* window)` - Navigate to new window
- `void popWindow()` - Go back to previous window
- `Window* getCurrentWindow()` - Get active window
- `void update()` - Main update loop (call in loop())

### Window Class
- `Window(title, maxWidgets=10)` - Create new window
- `void addWidget(Widget* widget)` - Add widget to window
- `Widget* getWidget(int index)` - Get widget by index
- `Widget* getWidgetById(String id)` - Get widget by ID
- `void focusNext()` - Move focus to next widget
- `void focusPrevious()` - Move focus to previous widget

### Widget Classes

All widgets inherit from `Widget` base class.

**Common Methods:**
- `void setVisible(bool visible)` - Show/hide widget
- `bool isVisible()` - Check visibility
- `String getId()` - Get widget ID

**Label:**
- `void setText(String text)` - Update label text
- `String getText()` - Get current text

**Button:**
- `void setText(String text)` - Update button text
- `void setCallback(void (*callback)())` - Set click callback

**Checkbox:**
- `bool isChecked()` - Get checked state
- `void setChecked(bool checked)` - Set checked state
- `void setLabel(String label)` - Update label text

### Animation Class
- `void start(float start, float end, unsigned long durationMs)` - Start animation
- `float getValue()` - Get current animated value
- `bool isRunning()` - Check if animating
- `void stop()` - Stop animation

## Customization

### Change Display Resolution
Edit in `UIEngine.h`:
```cpp
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
```

### Change Button Pins
Pass different pins to UIEngine constructor:
```cpp
UIEngine* ui = new UIEngine(upPin, downPin, selectPin, backPin);
```

### Modify Easing Curve
Edit the `EasingCurve::curve` array in `UIEngine.h` to use different animation curves.

### Add Custom Widgets
Inherit from `Widget` class and implement:
```cpp
class CustomWidget : public Widget {
public:
    CustomWidget(int x, int y, int w, int h) 
        : Widget(x, y, w, h) {}
    
    void draw(Adafruit_SSD1306& display) override {
        // Your drawing code here
    }
    
    bool canFocus() const override { return true; }
    void onClick() override {
        // Handle click
    }
};
```

## Example Screenshots

The example sketch demonstrates:
- Main menu with counter and multiple buttons
- Settings window with checkboxes
- About window with information
- Smooth animations on all interactions
- Window transitions with slide effects

## Performance Notes

- The animation system uses `millis()` for timing
- Display updates run at ~100Hz (10ms delay in loop)
- Button debouncing: 50ms
- Animation duration: 200-300ms for smooth feel
- Memory usage: ~2-3KB RAM depending on number of windows/widgets

## Troubleshooting

**Display not working:**
- Check I2C connections (SDA/SCL)
- Verify I2C address (0x3C is common, some use 0x3D)
- Try scanning I2C bus with I2C scanner sketch

**Buttons not responding:**
- Check button wiring (should connect GPIO to GND when pressed)
- Verify internal pull-ups are working
- Adjust debounce delay if needed

**Animations choppy:**
- Reduce display update frequency
- Simplify widget drawing code
- Check for long-running code in callbacks

## License

This project is provided as-is for educational and commercial use.

## Credits

- Adafruit SSD1306 library
- Adafruit GFX Library

## Contributing

Feel free to extend this UI engine with:
- More widget types (sliders, progress bars, etc.)
- Touch screen support
- Different display drivers
- Advanced layout managers
- Theme support

---

**Happy coding!**
