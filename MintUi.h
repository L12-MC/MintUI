#ifndef MINT_UI_H
#define MINT_UI_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Animation easing curve based on CSS linear()
// linear(0, 0.221 2.5%, 0.421 5.2%, 0.592 8%, 0.733 10.9%, 0.852 14%, 
//        0.901 15.6%, 0.946 17.3%, 0.984 19%, 1.017 20.8%, 1.045 22.7%, 
//        1.068 24.7%, 1.089 27.2%, 1.102 29.9%, 1.109 32.8%, 1.109 36.1%, 
//        1.105 39.1%, 1.096 42.5%, 1.052 54.7%, 1.035 59.8%, 1.024 64.2%, 
//        1.015 68.6%, 1.007 74.3%, 1.002 80.7%, 1 87.9%, 1)

class EasingCurve {
private:
    static const int CURVE_POINTS = 26;
    
    struct Point {
        float progress;
        float value;
    };
    
    static const Point curve[CURVE_POINTS];
    
public:
    static float ease(float t) {
        if (t <= 0.0f) return 0.0f;
        if (t >= 1.0f) return 1.0f;
        
        // Linear interpolation between curve points
        for (int i = 0; i < CURVE_POINTS - 1; i++) {
            if (t >= curve[i].progress && t <= curve[i + 1].progress) {
                float localT = (t - curve[i].progress) / 
                              (curve[i + 1].progress - curve[i].progress);
                return curve[i].value + 
                       (curve[i + 1].value - curve[i].value) * localT;
            }
        }
        
        return 1.0f;
    }
};

// Define the curve points
const EasingCurve::Point EasingCurve::curve[EasingCurve::CURVE_POINTS] = {
    {0.000f, 0.000f},
    {0.025f, 0.221f},
    {0.052f, 0.421f},
    {0.080f, 0.592f},
    {0.109f, 0.733f},
    {0.140f, 0.852f},
    {0.156f, 0.901f},
    {0.173f, 0.946f},
    {0.190f, 0.984f},
    {0.208f, 1.017f},
    {0.227f, 1.045f},
    {0.247f, 1.068f},
    {0.272f, 1.089f},
    {0.299f, 1.102f},
    {0.328f, 1.109f},
    {0.361f, 1.109f},
    {0.391f, 1.105f},
    {0.425f, 1.096f},
    {0.547f, 1.052f},
    {0.598f, 1.035f},
    {0.642f, 1.024f},
    {0.686f, 1.015f},
    {0.743f, 1.007f},
    {0.807f, 1.002f},
    {0.879f, 1.000f},
    {1.000f, 1.000f}
};

// Animation class
class Animation {
private:
    unsigned long startTime;
    unsigned long duration;
    float startValue;
    float endValue;
    bool running;
    
public:
    Animation() : startTime(0), duration(0), startValue(0), 
                  endValue(0), running(false) {}
    
    void start(float start, float end, unsigned long durationMs) {
        startTime = millis();
        duration = durationMs;
        startValue = start;
        endValue = end;
        running = true;
    }
    
    float getValue() {
        if (!running) return endValue;
        
        unsigned long elapsed = millis() - startTime;
        if (elapsed >= duration) {
            running = false;
            return endValue;
        }
        
        float t = (float)elapsed / (float)duration;
        float easedT = EasingCurve::ease(t);
        return startValue + (endValue - startValue) * easedT;
    }
    
    bool isRunning() const { return running; }
    void stop() { running = false; }
};

// Forward declarations
class UIEngine;
class Window;

// Widget base class
class Widget {
protected:
    int x, y, width, height;
    bool visible;
    bool focused;
    String id;
    
public:
    Widget(int x, int y, int w, int h, const String& id = "") 
        : x(x), y(y), width(w), height(h), visible(true), 
          focused(false), id(id) {}
    
    virtual ~Widget() {}
    
    virtual void draw(Adafruit_SSD1306& display) = 0;
    virtual bool canFocus() const { return false; }
    virtual void onFocus() { focused = true; }
    virtual void onBlur() { focused = false; }
    virtual void onClick() {}
    
    bool isFocused() const { return focused; }
    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }
    
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    String getId() const { return id; }
    
    bool contains(int px, int py) const {
        return px >= x && px < x + width && 
               py >= y && py < y + height;
    }
};

// Label widget
class Label : public Widget {
private:
    String text;
    int textSize;
    bool centered;
    
public:
    Label(int x, int y, const String& text, int textSize = 1, 
          bool centered = false, const String& id = "")
        : Widget(x, y, text.length() * 6 * textSize, 8 * textSize, id),
          text(text), textSize(textSize), centered(centered) {}
    
    void draw(Adafruit_SSD1306& display) override {
        if (!visible) return;
        
        display.setTextSize(textSize);
        display.setTextColor(SSD1306_WHITE);
        
        int drawX = x;
        if (centered) {
            drawX = x - (text.length() * 6 * textSize) / 2;
        }
        
        display.setCursor(drawX, y);
        display.print(text);
    }
    
    void setText(const String& newText) {
        text = newText;
        width = text.length() * 6 * textSize;
    }
    
    String getText() const { return text; }
};

// Button widget
class Button : public Widget {
private:
    String text;
    bool pressed;
    Animation pressAnim;
    void (*callback)();
    
public:
    Button(int x, int y, int w, int h, const String& text, 
           void (*callback)() = nullptr, const String& id = "")
        : Widget(x, y, w, h, id), text(text), pressed(false), 
          callback(callback) {}
    
    void draw(Adafruit_SSD1306& display) override {
        if (!visible) return;
        
        float animValue = pressAnim.getValue();
        int offset = (int)animValue;
        
        // Draw button border
        if (focused) {
            display.fillRect(x - 1 + offset, y - 1 + offset, 
                           width + 2, height + 2, SSD1306_WHITE);
            display.fillRect(x + offset, y + offset, 
                           width, height, SSD1306_BLACK);
        } else {
            display.drawRect(x + offset, y + offset, 
                           width, height, SSD1306_WHITE);
        }
        
        // Draw text centered
        display.setTextSize(1);
        display.setTextColor(focused ? SSD1306_WHITE : SSD1306_WHITE);
        int textX = x + (width - text.length() * 6) / 2 + offset;
        int textY = y + (height - 8) / 2 + offset;
        display.setCursor(textX, textY);
        display.print(text);
    }
    
    bool canFocus() const override { return true; }
    
    void onClick() override {
        pressed = true;
        pressAnim.start(0, 2, 100);
        
        if (callback) {
            callback();
        }
    }
    
    void setText(const String& newText) { text = newText; }
    String getText() const { return text; }
    void setCallback(void (*cb)()) { callback = cb; }
};

// Checkbox widget
class Checkbox : public Widget {
private:
    String label;
    bool checked;
    Animation checkAnim;
    void (*onChange)(bool);
    
public:
    Checkbox(int x, int y, const String& label, bool checked = false,
             void (*onChange)(bool) = nullptr, const String& id = "")
        : Widget(x, y, 12 + (label.length() > 0 ? label.length() * 6 + 4 : 0), 
                 12, id),
          label(label), checked(checked), onChange(onChange) {}
    
    void draw(Adafruit_SSD1306& display) override {
        if (!visible) return;
        
        float animValue = checkAnim.getValue();
        
        // Draw checkbox border
        if (focused) {
            display.fillRect(x - 1, y - 1, 14, 14, SSD1306_WHITE);
            display.fillRect(x, y, 12, 12, SSD1306_BLACK);
        }
        display.drawRect(x, y, 12, 12, SSD1306_WHITE);
        
        // Draw checkmark with animation
        if (checked || checkAnim.isRunning()) {
            int size = (int)(8 * animValue);
            if (size > 0) {
                display.fillRect(x + 2, y + 2, size, size, SSD1306_WHITE);
            }
        }
        
        // Draw label
        if (label.length() > 0) {
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(x + 16, y + 2);
            display.print(label);
        }
    }
    
    bool canFocus() const override { return true; }
    
    void onClick() override {
        checked = !checked;
        checkAnim.start(checked ? 0.0f : 1.0f, 
                       checked ? 1.0f : 0.0f, 200);
        
        if (onChange) {
            onChange(checked);
        }
    }
    
    bool isChecked() const { return checked; }
    void setChecked(bool c) {
        if (c != checked) {
            checked = c;
            checkAnim.start(checked ? 0.0f : 1.0f, 
                          checked ? 1.0f : 0.0f, 200);
        }
    }
    
    void setLabel(const String& newLabel) { label = newLabel; }
    String getLabel() const { return label; }
};

// Window class
class Window {
private:
    String title;
    Widget** widgets;
    int widgetCount;
    int maxWidgets;
    int focusedIndex;
    Animation slideAnim;
    bool transitioning;
    int slideOffset;
    
public:
    Window(const String& title, int maxWidgets = 10)
        : title(title), widgetCount(0), maxWidgets(maxWidgets), 
          focusedIndex(-1), transitioning(false), slideOffset(0) {
        widgets = new Widget*[maxWidgets];
    }
    
    ~Window() {
        for (int i = 0; i < widgetCount; i++) {
            delete widgets[i];
        }
        delete[] widgets;
    }
    
    void addWidget(Widget* widget) {
        if (widgetCount < maxWidgets) {
            widgets[widgetCount++] = widget;
            
            // Focus first focusable widget
            if (focusedIndex == -1 && widget->canFocus()) {
                focusedIndex = widgetCount - 1;
                widgets[focusedIndex]->onFocus();
            }
        }
    }
    
    void draw(Adafruit_SSD1306& display) {
        int offset = slideOffset + (int)slideAnim.getValue();
        
        // Draw title bar
        display.fillRect(0 + offset, 0, SCREEN_WIDTH, 10, SSD1306_WHITE);
        display.setTextSize(1);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(2 + offset, 1);
        display.print(title);
        
        // Draw widgets
        display.setTextColor(SSD1306_WHITE);
        for (int i = 0; i < widgetCount; i++) {
            if (widgets[i]->isVisible()) {
                // Temporarily offset widget positions for animation
                int oldX = widgets[i]->getX();
                widgets[i]->draw(display);
            }
        }
    }
    
    void startSlideIn(bool fromRight) {
        transitioning = true;
        slideOffset = fromRight ? SCREEN_WIDTH : -SCREEN_WIDTH;
        slideAnim.start(slideOffset, 0, 300);
        slideOffset = 0;
    }
    
    void startSlideOut(bool toRight) {
        transitioning = true;
        slideAnim.start(0, toRight ? SCREEN_WIDTH : -SCREEN_WIDTH, 300);
    }
    
    bool isTransitioning() {
        if (transitioning && !slideAnim.isRunning()) {
            transitioning = false;
        }
        return transitioning;
    }
    
    void focusNext() {
        if (focusedIndex >= 0) {
            widgets[focusedIndex]->onBlur();
        }
        
        int startIndex = focusedIndex;
        do {
            focusedIndex = (focusedIndex + 1) % widgetCount;
            if (widgets[focusedIndex]->canFocus() && 
                widgets[focusedIndex]->isVisible()) {
                widgets[focusedIndex]->onFocus();
                return;
            }
        } while (focusedIndex != startIndex);
    }
    
    void focusPrevious() {
        if (focusedIndex >= 0) {
            widgets[focusedIndex]->onBlur();
        }
        
        int startIndex = focusedIndex;
        do {
            focusedIndex--;
            if (focusedIndex < 0) focusedIndex = widgetCount - 1;
            
            if (widgets[focusedIndex]->canFocus() && 
                widgets[focusedIndex]->isVisible()) {
                widgets[focusedIndex]->onFocus();
                return;
            }
        } while (focusedIndex != startIndex);
    }
    
    void clickFocused() {
        if (focusedIndex >= 0 && focusedIndex < widgetCount) {
            widgets[focusedIndex]->onClick();
        }
    }
    
    Widget* getWidget(int index) {
        if (index >= 0 && index < widgetCount) {
            return widgets[index];
        }
        return nullptr;
    }
    
    Widget* getWidgetById(const String& id) {
        for (int i = 0; i < widgetCount; i++) {
            if (widgets[i]->getId() == id) {
                return widgets[i];
            }
        }
        return nullptr;
    }
    
    int getWidgetCount() const { return widgetCount; }
    String getTitle() const { return title; }
};

// Main UI Engine class
class UIEngine {
private:
    Adafruit_SSD1306 display;
    Window** windowStack;
    int stackSize;
    int maxStackSize;
    
    // Button pins (customize as needed)
    int btnUp, btnDown, btnSelect, btnBack;
    
    // Debounce
    unsigned long lastDebounceTime[4];
    bool lastButtonState[4];
    const unsigned long debounceDelay = 50;
    
public:
    UIEngine(int btnUp = 12, int btnDown = 14, 
             int btnSelect = 27, int btnBack = 26)
        : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
          stackSize(0), maxStackSize(5),
          btnUp(btnUp), btnDown(btnDown), 
          btnSelect(btnSelect), btnBack(btnBack) {
        
        windowStack = new Window*[maxStackSize];
        for (int i = 0; i < 4; i++) {
            lastDebounceTime[i] = 0;
            lastButtonState[i] = HIGH;
        }
    }
    
    ~UIEngine() {
        for (int i = 0; i < stackSize; i++) {
            delete windowStack[i];
        }
        delete[] windowStack;
    }
    
    bool begin() {
        // Initialize buttons
        pinMode(btnUp, INPUT_PULLUP);
        pinMode(btnDown, INPUT_PULLUP);
        pinMode(btnSelect, INPUT_PULLUP);
        pinMode(btnBack, INPUT_PULLUP);
        
        // Initialize display
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            return false;
        }
        
        display.clearDisplay();
        display.display();
        return true;
    }
    
    void pushWindow(Window* window) {
        if (stackSize < maxStackSize) {
            window->startSlideIn(true);
            windowStack[stackSize++] = window;
        }
    }
    
    void popWindow() {
        if (stackSize > 1) {
            windowStack[stackSize - 1]->startSlideOut(true);
            // We'll actually remove it after the animation completes
        }
    }
    
    Window* getCurrentWindow() {
        if (stackSize > 0) {
            return windowStack[stackSize - 1];
        }
        return nullptr;
    }
    
    void update() {
        Window* current = getCurrentWindow();
        if (!current) return;
        
        // Handle button inputs
        handleInput();
        
        // Clean up completed transitions
        if (stackSize > 1 && current->isTransitioning() == false) {
            // Check if we need to remove the window after slide-out
            // (This would require additional state tracking)
        }
        
        // Render
        display.clearDisplay();
        current->draw(display);
        display.display();
    }
    
private:
    bool readButton(int pin, int index) {
        bool reading = digitalRead(pin) == LOW;
        
        if (reading != lastButtonState[index]) {
            lastDebounceTime[index] = millis();
        }
        
        bool pressed = false;
        if ((millis() - lastDebounceTime[index]) > debounceDelay) {
            if (reading && !lastButtonState[index]) {
                pressed = true;
            }
        }
        
        lastButtonState[index] = reading;
        return pressed;
    }
    
    void handleInput() {
        Window* current = getCurrentWindow();
        if (!current || current->isTransitioning()) return;
        
        if (readButton(btnUp, 0)) {
            current->focusPrevious();
        }
        
        if (readButton(btnDown, 1)) {
            current->focusNext();
        }
        
        if (readButton(btnSelect, 2)) {
            current->clickFocused();
        }
        
        if (readButton(btnBack, 3)) {
            if (stackSize > 1) {
                popWindow();
            }
        }
    }
};

#endif // MINT_UI_H
