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

// -------------------------------------------------------------------------
// Easing Curve
// -------------------------------------------------------------------------

class EasingCurve {
public:
    struct Point {
        float progress;
        float value;
    };

    static float ease(float t) {
        if (t <= 0.0f) return 0.0f;
        if (t >= 1.0f) return 1.0f;

        // Curve data moved inside function to prevent linker "multiple definition" errors
        static const Point curve[] = {
            {0.000f, 0.000f}, {0.025f, 0.221f}, {0.052f, 0.421f}, {0.080f, 0.592f},
            {0.109f, 0.733f}, {0.140f, 0.852f}, {0.156f, 0.901f}, {0.173f, 0.946f},
            {0.190f, 0.984f}, {0.208f, 1.017f}, {0.227f, 1.045f}, {0.247f, 1.068f},
            {0.272f, 1.089f}, {0.299f, 1.102f}, {0.328f, 1.109f}, {0.361f, 1.109f},
            {0.391f, 1.105f}, {0.425f, 1.096f}, {0.547f, 1.052f}, {0.598f, 1.035f},
            {0.642f, 1.024f}, {0.686f, 1.015f}, {0.743f, 1.007f}, {0.807f, 1.002f},
            {0.879f, 1.000f}, {1.000f, 1.000f}
        };
        static const int CURVE_POINTS = sizeof(curve) / sizeof(curve[0]);

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

// -------------------------------------------------------------------------
// Animation
// -------------------------------------------------------------------------

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

// -------------------------------------------------------------------------
// Widget Base Class
// -------------------------------------------------------------------------

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
    
    // UPDATED: draw now accepts offsets for animation
    virtual void draw(Adafruit_SSD1306& display, int offsetX, int offsetY) = 0;
    
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

// -------------------------------------------------------------------------
// Label Widget
// -------------------------------------------------------------------------

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
    
    void draw(Adafruit_SSD1306& display, int offsetX, int offsetY) override {
        if (!visible) return;
        
        display.setTextSize(textSize);
        display.setTextColor(SSD1306_WHITE);
        
        int drawX = x + offsetX;
        int drawY = y + offsetY;

        if (centered) {
            drawX = (x + offsetX) - (text.length() * 6 * textSize) / 2;
        }
        
        display.setCursor(drawX, drawY);
        display.print(text);
    }
    
    void setText(const String& newText) {
        text = newText;
        width = text.length() * 6 * textSize;
    }
    
    String getText() const { return text; }
};

// -------------------------------------------------------------------------
// Button Widget
// -------------------------------------------------------------------------

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
    
    void draw(Adafruit_SSD1306& display, int offsetX, int offsetY) override {
        if (!visible) return;
        
        float animValue = pressAnim.getValue();
        int pressOffset = (int)animValue;
        
        int drawX = x + offsetX;
        int drawY = y + offsetY;

        // Draw button border
        if (focused) {
            display.fillRect(drawX - 1 + pressOffset, drawY - 1 + pressOffset, 
                           width + 2, height + 2, SSD1306_WHITE);
            display.fillRect(drawX + pressOffset, drawY + pressOffset, 
                           width, height, SSD1306_BLACK);
        } else {
            display.drawRect(drawX + pressOffset, drawY + pressOffset, 
                           width, height, SSD1306_WHITE);
        }
        
        // Draw text centered
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE); // Always white for monochrome contrast
        int textX = drawX + (width - text.length() * 6) / 2 + pressOffset;
        int textY = drawY + (height - 8) / 2 + pressOffset;
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

// -------------------------------------------------------------------------
// Checkbox Widget
// -------------------------------------------------------------------------

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
    
    void draw(Adafruit_SSD1306& display, int offsetX, int offsetY) override {
        if (!visible) return;
        
        float animValue = checkAnim.getValue();
        int drawX = x + offsetX;
        int drawY = y + offsetY;
        
        // Draw checkbox border
        if (focused) {
            display.fillRect(drawX - 1, drawY - 1, 14, 14, SSD1306_WHITE);
            display.fillRect(drawX, drawY, 12, 12, SSD1306_BLACK);
        }
        display.drawRect(drawX, drawY, 12, 12, SSD1306_WHITE);
        
        // Draw checkmark with animation
        // Visual calculation: if checked, scale up from 0 to 8. If unchecked, scale down.
        // Logic handled by start() params in onClick.
        // We use animValue as the size.
        
        int size = (int)(8 * animValue);
        // Fallback for static state if animation finished
        if (!checkAnim.isRunning() && checked) size = 8;
        if (!checkAnim.isRunning() && !checked) size = 0;

        if (size > 0) {
            // Center the box
            int offset = (12 - size) / 2;
            display.fillRect(drawX + offset, drawY + offset, size, size, SSD1306_WHITE);
        }
        
        // Draw label
        if (label.length() > 0) {
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(drawX + 16, drawY + 2);
            display.print(label);
        }
    }
    
    bool canFocus() const override { return true; }
    
    void onClick() override {
        checked = !checked;
        // Animate size from 0->1 or 1->0
        checkAnim.start(checked ? 0.0f : 1.0f, 
                        checked ? 1.0f : 0.0f, 150);
        
        if (onChange) {
            onChange(checked);
        }
    }
    
    bool isChecked() const { return checked; }
    void setChecked(bool c) {
        if (c != checked) {
            checked = c;
            checkAnim.start(checked ? 0.0f : 1.0f, 
                          checked ? 1.0f : 0.0f, 150);
        }
    }
    
    void setLabel(const String& newLabel) { label = newLabel; }
    String getLabel() const { return label; }
};

// -------------------------------------------------------------------------
// Window
// -------------------------------------------------------------------------

class Window {
private:
    String title;
    Widget** widgets;
    int widgetCount;
    int maxWidgets;
    int focusedIndex;
    Animation slideAnim;
    bool transitioning;
    int slideStart;
    int slideEnd;
    
public:
    Window(const String& title, int maxWidgets = 10)
        : title(title), widgetCount(0), maxWidgets(maxWidgets), 
          focusedIndex(-1), transitioning(false), slideStart(0), slideEnd(0) {
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
    
    void draw(Adafruit_SSD1306& display, int globalXOffset = 0, int globalYOffset = 0) {
        int animOffset = 0;
        if (transitioning) {
            animOffset = (int)slideAnim.getValue();
        } else {
            animOffset = slideEnd;
        }

        int totalXOffset = globalXOffset + animOffset;
        int totalYOffset = globalYOffset;
        
        // Draw title bar
        display.fillRect(totalXOffset, totalYOffset, SCREEN_WIDTH, 10, SSD1306_WHITE);
        display.setTextSize(1);
        display.setTextColor(SSD1306_BLACK);
        display.setCursor(totalXOffset + 2, totalYOffset + 1);
        display.print(title);
        
        // Draw widgets
        display.setTextColor(SSD1306_WHITE);
        for (int i = 0; i < widgetCount; i++) {
            if (widgets[i]->isVisible()) {
                widgets[i]->draw(display, totalXOffset, totalYOffset);
            }
        }

        // Check animation status
        if (transitioning && !slideAnim.isRunning()) {
            transitioning = false;
        }
    }
    
    void startSlideIn(bool fromRight) {
        transitioning = true;
        slideStart = fromRight ? SCREEN_WIDTH : -SCREEN_WIDTH;
        slideEnd = 0;
        slideAnim.start(slideStart, slideEnd, 250);
    }
    
    void startSlideOut(bool toRight) {
        transitioning = true;
        slideStart = 0;
        slideEnd = toRight ? SCREEN_WIDTH : -SCREEN_WIDTH;
        slideAnim.start(slideStart, slideEnd, 250);
    }
    
    bool isTransitioning() {
        if (transitioning && !slideAnim.isRunning()) {
            transitioning = false;
        }
        return transitioning;
    }
    
    void focusNext() {
        if (widgetCount == 0) return;
        
        int start = focusedIndex;
        if (start == -1) start = 0;

        int current = start;
        do {
            current = (current + 1) % widgetCount;
            if (widgets[current]->canFocus() && widgets[current]->isVisible()) {
                if (focusedIndex != -1) widgets[focusedIndex]->onBlur();
                focusedIndex = current;
                widgets[focusedIndex]->onFocus();
                return;
            }
        } while (current != start);
    }
    
    void focusPrevious() {
        if (widgetCount == 0) return;
        
        int start = focusedIndex;
        if (start == -1) start = 0;

        int current = start;
        do {
            current--;
            if (current < 0) current = widgetCount - 1;
            
            if (widgets[current]->canFocus() && widgets[current]->isVisible()) {
                if (focusedIndex != -1) widgets[focusedIndex]->onBlur();
                focusedIndex = current;
                widgets[focusedIndex]->onFocus();
                return;
            }
        } while (current != start);
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

// -------------------------------------------------------------------------
// UI Engine
// -------------------------------------------------------------------------

class UIEngine {
private:
    Adafruit_SSD1306 display;
    Window** windowStack;
    int stackSize;
    int maxStackSize;
    bool popping; // State to track if we are currently popping a window
    
    // Button pins
    int btnUp, btnDown, btnSelect, btnBack;
    
    // Debounce
    unsigned long lastDebounceTime[4];
    bool lastButtonReading[4];
    bool buttonState[4];
    bool lastStableState[4];
    const unsigned long debounceDelay = 30; // Shorter delay for snappier feel
    
public:
    UIEngine(int btnUp = 12, int btnDown = 14, 
             int btnSelect = 27, int btnBack = 26)
        : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
          stackSize(0), maxStackSize(5), popping(false),
          btnUp(btnUp), btnDown(btnDown), 
          btnSelect(btnSelect), btnBack(btnBack) {
        
        windowStack = new Window*[maxStackSize];
        for (int i = 0; i < 4; i++) {
            lastDebounceTime[i] = 0;
            lastButtonReading[i] = false;
            buttonState[i] = false;
            lastStableState[i] = false;
        }
    }
    
    ~UIEngine() {
        // Warning: This does not delete the windows themselves if created externally
        // But for this simple engine, we assume stack management is enough
        delete[] windowStack;
    }
    
    bool begin() {
        pinMode(btnUp, INPUT_PULLUP);
        pinMode(btnDown, INPUT_PULLUP);
        pinMode(btnSelect, INPUT_PULLUP);
        
        #ifdef ESP8266
        if (btnBack == 16) pinMode(btnBack, INPUT);
        else pinMode(btnBack, INPUT_PULLUP);
        #else
        pinMode(btnBack, INPUT_PULLUP);
        #endif
        
        // Initialize I2C first!
        Wire.begin();
        
        // Address 0x3C for 128x64
        if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            return false;
        }
        
        // Initialize button states
        for (int i = 0; i < 4; i++) {
            lastDebounceTime[i] = millis();
        }
        
        display.clearDisplay();
        display.display();
        return true;
    }
    
    void pushWindow(Window* window) {
        if (stackSize < maxStackSize) {
            window->startSlideIn(true);
            windowStack[stackSize++] = window;
            popping = false;
        }
    }
    
    void popWindow() {
        if (stackSize > 1 && !popping) {
            Window* current = windowStack[stackSize - 1];
            current->startSlideOut(true); // Slide out to the right
            popping = true;
        }
    }
    
    Window* getCurrentWindow() {
        if (stackSize > 0) {
            return windowStack[stackSize - 1];
        }
        return nullptr;
    }
    
    Adafruit_SSD1306& getDisplay() {
        return display;
    }
    
    void update() {
        Window* current = getCurrentWindow();
        if (!current) return;
        
        handleInput();
        
        display.clearDisplay();
        
        // RENDER LOGIC
        
        // If the top window is transitioning, draw the window UNDER it first
        // so we don't see a black background.
        if (stackSize > 1 && current->isTransitioning()) {
             // Draw the previous window at (0,0) - it stays static
             windowStack[stackSize - 2]->draw(display, 0, 0);
        }
        
        // Draw the current window (which might be moving)
        current->draw(display);
        
        // STATE LOGIC
        
        // If we were popping and the animation is done, remove from stack
        if (popping && !current->isTransitioning()) {
            stackSize--;
            popping = false;
            // The previous window is now current. 
            // Note: We do NOT delete the popped window object here.
            // Ownership remains with the creator/global scope.
        }
        
        display.display();
    }
    
private:
    bool readButton(int pin, int index) {
        bool reading = (digitalRead(pin) == LOW);
        
        if (reading != lastButtonReading[index]) {
            lastDebounceTime[index] = millis();
            lastButtonReading[index] = reading;
        }
        
        if ((millis() - lastDebounceTime[index]) > debounceDelay) {
            if (reading != buttonState[index]) {
                buttonState[index] = reading;
                if (buttonState[index] && !lastStableState[index]) {
                    lastStableState[index] = true;
                    return true;
                }
                if (!buttonState[index] && lastStableState[index]) {
                    lastStableState[index] = false;
                }
            }
        }
        return false;
    }
    
    void handleInput() {
        Window* current = getCurrentWindow();
        // Block input during transitions
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
            // Only allow manual pop if not already popping
            if (stackSize > 1) {
                popWindow();
            }
        }
    }
};

#endif // MINT_UI_H
