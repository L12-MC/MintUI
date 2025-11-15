/*
 * MintUI Example for NodeMCU (ESP8266) with SSD1306
 * 
 * This example demonstrates all features of MintUI optimized for ESP8266:
 * - Multiple windows with navigation
 * - Buttons with callbacks
 * - Labels for displaying text
 * - Checkboxes with state management
 * - Smooth animations using CSS easing curves
 * - Window transitions
 * 
 * Hardware connections for NodeMCU:
 * - SSD1306 OLED: SDA -> D2 (GPIO4), SCL -> D1 (GPIO5)
 * - Button UP: D5 (GPIO14) with internal pullup
 * - Button DOWN: D6 (GPIO12) with internal pullup
 * - Button SELECT: D7 (GPIO13) with internal pullup
 * - Button BACK: D8 (GPIO15) with 10K external pullup (or use D0/GPIO16)
 * 
 * Note: NodeMCU pin labels (D0-D8) map to GPIO numbers
 * D0=GPIO16, D1=GPIO5, D2=GPIO4, D3=GPIO0, D4=GPIO2,
 * D5=GPIO14, D6=GPIO12, D7=GPIO13, D8=GPIO15
 */

#include "MintUi.h"

// Global UI Engine instance
UIEngine* ui;

// Global state variables
int counter = 0;
bool wifiEnabled = false;
bool autoSave = true;
int brightness = 50;

// Forward declarations
void createMainWindow();
void createSettingsWindow();
void createAboutWindow();
void createStatsWindow();

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

void onStatsClick() {
    createStatsWindow();
}

void onWifiChange(bool checked) {
    wifiEnabled = checked;
    Serial.println("WiFi: " + String(checked ? "ON" : "OFF"));
    
    // Simulate WiFi control
    if (checked) {
        Serial.println("Connecting to WiFi...");
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void onAutoSaveChange(bool checked) {
    autoSave = checked;
    Serial.println("Auto Save: " + String(checked ? "ON" : "OFF"));
}

void createMainWindow() {
    Window* mainWin = new Window("Main Menu", 20);
    
    // Title label
    Label* title = new Label(64, 15, "MintUI Demo", 1, true);
    mainWin->addWidget(title);
    
    // Counter label
    Label* counterLabel = new Label(64, 28, "Count: " + String(counter), 1, true, "counter");
    mainWin->addWidget(counterLabel);
    
    // Buttons row 1 - Counter controls
    Button* incBtn = new Button(5, 38, 36, 12, "+", onIncrementClick);
    mainWin->addWidget(incBtn);
    
    Button* decBtn = new Button(46, 38, 36, 12, "-", onDecrementClick);
    mainWin->addWidget(decBtn);
    
    Button* resetBtn = new Button(87, 38, 36, 12, "Reset", onResetClick);
    mainWin->addWidget(resetBtn);
    
    // Navigation buttons row 2
    Button* settingsBtn = new Button(5, 52, 37, 11, "Config", onSettingsClick);
    mainWin->addWidget(settingsBtn);
    
    Button* statsBtn = new Button(45, 52, 37, 11, "Stats", onStatsClick);
    mainWin->addWidget(statsBtn);
    
    Button* aboutBtn = new Button(85, 52, 38, 11, "About", onAboutClick);
    mainWin->addWidget(aboutBtn);
    
    ui->pushWindow(mainWin);
}

void createSettingsWindow() {
    Window* settingsWin = new Window("Settings", 15);
    
    // Settings label
    Label* label = new Label(10, 15, "Configuration", 1);
    settingsWin->addWidget(label);
    
    // Checkboxes
    Checkbox* wifiCheck = new Checkbox(10, 28, "WiFi", wifiEnabled, onWifiChange, "wifi");
    settingsWin->addWidget(wifiCheck);
    
    Checkbox* autoSaveCheck = new Checkbox(10, 42, "Auto Save", autoSave, onAutoSaveChange, "autosave");
    settingsWin->addWidget(autoSaveCheck);
    
    // Info label
    Label* infoLabel = new Label(10, 56, "BACK to return", 1);
    settingsWin->addWidget(infoLabel);
    
    ui->pushWindow(settingsWin);
}

void createStatsWindow() {
    Window* statsWin = new Window("Statistics", 15);
    
    // Get ESP8266 stats
    uint32_t freeHeap = ESP.getFreeHeap();
    uint32_t chipId = ESP.getChipId();
    
    // Stats labels
    Label* line1 = new Label(10, 15, "NodeMCU ESP8266", 1);
    statsWin->addWidget(line1);
    
    Label* line2 = new Label(10, 26, "Chip: " + String(chipId, HEX), 1);
    statsWin->addWidget(line2);
    
    Label* line3 = new Label(10, 37, "RAM: " + String(freeHeap) + " B", 1);
    statsWin->addWidget(line3);
    
    Label* line4 = new Label(10, 48, "CPU: 80/160 MHz", 1);
    statsWin->addWidget(line4);
    
    ui->pushWindow(statsWin);
}

void createAboutWindow() {
    Window* aboutWin = new Window("About", 10);
    
    // About labels
    Label* line1 = new Label(10, 15, "MintUI v1.0", 1);
    aboutWin->addWidget(line1);
    
    Label* line2 = new Label(10, 26, "NodeMCU ESP8266", 1);
    aboutWin->addWidget(line2);
    
    Label* line3 = new Label(10, 37, "SSD1306 128x64", 1);
    aboutWin->addWidget(line3);
    
    Label* line4 = new Label(10, 48, "Smooth UI Engine", 1);
    aboutWin->addWidget(line4);
    
    ui->pushWindow(aboutWin);
}

void setup() {
    Serial.begin(115200);
    delay(100); // Give serial time to initialize
    Serial.println("\n\n=========================");
    Serial.println("MintUI NodeMCU Starting...");
    Serial.println("=========================");
    
    // Show pin configuration
    Serial.println("\nPin Configuration:");
    Serial.println("SDA: D2 (GPIO4)");
    Serial.println("SCL: D1 (GPIO5)");
    Serial.println("UP:  D5 (GPIO14)");
    Serial.println("DN:  D6 (GPIO12)");
    Serial.println("SEL: D7 (GPIO13)");
    Serial.println("BCK: D0 (GPIO16)");
    
    // Initialize UI Engine with NodeMCU GPIO pins
    // Button pins: UP(D5/GPIO14), DOWN(D6/GPIO12), SELECT(D7/GPIO13), BACK(D0/GPIO16)
    ui = new UIEngine(14, 12, 13, 16);
    
    if (!ui->begin()) {
        Serial.println("ERROR: Display initialization failed!");
        Serial.println("Check I2C connections:");
        Serial.println("  SDA -> D2 (GPIO4)");
        Serial.println("  SCL -> D1 (GPIO5)");
        while (1) {
            delay(1000);
        }
    }
    
    Serial.println("\nDisplay initialized successfully!");
    
    // Show chip info
    Serial.println("\nESP8266 Info:");
    Serial.print("Chip ID: 0x");
    Serial.println(ESP.getChipId(), HEX);
    Serial.print("Free Heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    Serial.print("CPU Frequency: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.println(" MHz");
    
    // Create and show main window
    createMainWindow();
    
    Serial.println("\n=========================");
    Serial.println("MintUI Ready!");
    Serial.println("Use buttons to navigate");
    Serial.println("=========================\n");
}

void loop() {
    // Update UI engine (handles input and rendering)
    ui->update();
    
    // Small delay to prevent excessive CPU usage
    // ESP8266 needs yield() for WiFi and system tasks
    delay(10);
    yield();
}
