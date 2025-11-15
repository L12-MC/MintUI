/*
 * MintUI Example for ESP32 with SSD1306
 * 
 * This example demonstrates all features of MintUI:
 * - Multiple windows with navigation
 * - Buttons with callbacks
 * - Labels for displaying text
 * - Checkboxes with state management
 * - Smooth animations using CSS easing curves
 * - Window transitions
 * 
 * Hardware connections:
 * - SSD1306 OLED: SDA -> GPIO21, SCL -> GPIO22
 * - Button UP: GPIO12 (with internal pullup)
 * - Button DOWN: GPIO14 (with internal pullup)
 * - Button SELECT: GPIO27 (with internal pullup)
 * - Button BACK: GPIO26 (with internal pullup)
 */

#include "MintUi.h"

// Global MintUI instance
UIEngine* ui;

// Global state variables
int counter = 0;
bool option1 = false;
bool option2 = true;

// Forward declarations
void createMainWindow();
void createSettingsWindow();
void createAboutWindow();

// Button callbacks
void onIncrementClick() {
    counter++;
    Window* mainWin = ui->getCurrentWindow();
    if (mainWin) {
        Label* counterLabel = (Label*)mainWin->getWidgetById("counter");
        if (counterLabel) {
            counterLabel->setText("Count: " + String(counter));
        }
    }
}

void onDecrementClick() {
    counter--;
    Window* mainWin = ui->getCurrentWindow();
    if (mainWin) {
        Label* counterLabel = (Label*)mainWin->getWidgetById("counter");
        if (counterLabel) {
            counterLabel->setText("Count: " + String(counter));
        }
    }
}

void onResetClick() {
    counter = 0;
    Window* mainWin = ui->getCurrentWindow();
    if (mainWin) {
        Label* counterLabel = (Label*)mainWin->getWidgetById("counter");
        if (counterLabel) {
            counterLabel->setText("Count: " + String(counter));
        }
    }
}

void onSettingsClick() {
    createSettingsWindow();
}

void onAboutClick() {
    createAboutWindow();
}

void onOption1Change(bool checked) {
    option1 = checked;
    Serial.println("Option 1: " + String(checked ? "ON" : "OFF"));
}

void onOption2Change(bool checked) {
    option2 = checked;
    Serial.println("Option 2: " + String(checked ? "ON" : "OFF"));
}

void createMainWindow() {
    Window* mainWin = new Window("Main Menu", 20);
    
    // Title label
    Label* title = new Label(64, 15, "MintUI Demo", 1, true);
    mainWin->addWidget(title);
    
    // Counter label
    Label* counterLabel = new Label(64, 28, "Count: " + String(counter), 1, true, "counter");
    mainWin->addWidget(counterLabel);
    
    // Buttons row 1
    Button* incBtn = new Button(5, 38, 36, 12, "+", onIncrementClick);
    mainWin->addWidget(incBtn);
    
    Button* decBtn = new Button(46, 38, 36, 12, "-", onDecrementClick);
    mainWin->addWidget(decBtn);
    
    Button* resetBtn = new Button(87, 38, 36, 12, "Reset", onResetClick);
    mainWin->addWidget(resetBtn);
    
    // Navigation buttons
    Button* settingsBtn = new Button(5, 52, 56, 11, "Settings", onSettingsClick);
    mainWin->addWidget(settingsBtn);
    
    Button* aboutBtn = new Button(67, 52, 56, 11, "About", onAboutClick);
    mainWin->addWidget(aboutBtn);
    
    ui->pushWindow(mainWin);
}

void createSettingsWindow() {
    Window* settingsWin = new Window("Settings", 15);
    
    // Settings label
    Label* label = new Label(10, 15, "Configuration", 1);
    settingsWin->addWidget(label);
    
    // Checkboxes
    Checkbox* check1 = new Checkbox(10, 28, "Enable WiFi", option1, onOption1Change, "option1");
    settingsWin->addWidget(check1);
    
    Checkbox* check2 = new Checkbox(10, 42, "Auto Save", option2, onOption2Change, "option2");
    settingsWin->addWidget(check2);
    
    // Info label
    Label* infoLabel = new Label(10, 56, "Press BACK to return", 1);
    settingsWin->addWidget(infoLabel);
    
    ui->pushWindow(settingsWin);
}

void createAboutWindow() {
    Window* aboutWin = new Window("About", 10);
    
    // About labels
    Label* line1 = new Label(10, 15, "MintUI v1.0", 1);
    aboutWin->addWidget(line1);
    
    Label* line2 = new Label(10, 26, "ESP32 + SSD1306", 1);
    aboutWin->addWidget(line2);
    
    Label* line3 = new Label(10, 37, "Smooth animations", 1);
    aboutWin->addWidget(line3);
    
    Label* line4 = new Label(10, 48, "with CSS curves", 1);
    aboutWin->addWidget(line4);
    
    ui->pushWindow(aboutWin);
}

void setup() {
    Serial.begin(115200);
    Serial.println("MintUI starting...");
    
    // Initialize UI Engine with button pins
    // UP, DOWN, SELECT, BACK
    ui = new UIEngine(12, 14, 27, 26);
    
    if (!ui->begin()) {
        Serial.println("Failed to initialize display!");
        while (1);
    }
    
    Serial.println("Display initialized");
    
    // Create and show main window
    createMainWindow();
    
    Serial.println("MintUI ready!");
}

void loop() {
    // Update UI engine (handles input and rendering)
    ui->update();
    
    // Small delay to prevent excessive CPU usage
    delay(10);
}
