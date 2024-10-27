#include "WiFiEsp.h"
#include <Arduino.h>
#include "Maix_Speech_Recognition.h"
#include "voice.h"
#include <Sipeed_ST7789.h>

#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "SSID";
char pwd[] = "PASS";
const char* server = "http://154.53.50.40:3001";

// Initialize the Wifi client library
WiFiEspClient client;

// The awesome recognizer
SpeechRecognizer rec;

// LCD setup
SPIClass spi_(SPI0);
Sipeed_ST7789 lcd(320, 240, spi_, SIPEED_ST7789_DCX_PIN, SIPEED_ST7789_RST_PIN, DMAC_CHANNEL2);

#define LABEL_TEXT_SIZE 2
#define BG_COLOR COLOR_RED

void printCenterOnLCD(Sipeed_ST7789 &lcd_, const char *msg, uint8_t textSize = LABEL_TEXT_SIZE) {
    lcd_.setCursor((lcd_.width() - (6 * textSize * strlen(msg))) / 2, (lcd_.height() - (8 * textSize)) / 2);
    lcd_.print(msg);
}

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    WiFi.init(&Serial1);
    
    // Connect to Wi-Fi
    if (WiFi.begin(ssid, pwd) != WL_CONNECTED) {
        Serial.println("WiFi connection failed");
        while (true);
    }
    Serial.println("WiFi connected");

    // Initialize LCD
    if (!lcd.begin(15000000, BG_COLOR)) {
        Serial.println("LCD initialization failed");
        while (1);
    }

    lcd.setTextSize(LABEL_TEXT_SIZE);
    lcd.setTextColor(COLOR_WHITE);
    Serial.println("Initializing model...");

    // Load voice models
    rec.begin();
    rec.addVoiceModel(0, 0, who_are_you_0, fram_num_who_are_you_0); 
    rec.addVoiceModel(0, 1, who_are_you_1, fram_num_who_are_you_1); 
    rec.addVoiceModel(0, 2, who_are_you_2, fram_num_who_are_you_2); 
    rec.addVoiceModel(0, 3, who_are_you_3, fram_num_who_are_you_3); 
    rec.addVoiceModel(0, 4, who_are_you_4, fram_num_who_are_you_3);
    rec.addVoiceModel(1, 0, what_is_digikey_0, fram_num_what_is_digikey_0); 
    rec.addVoiceModel(1, 1, what_is_digikey_1, fram_num_what_is_digikey_1); 
    rec.addVoiceModel(1, 2, what_is_digikey_2, fram_num_what_is_digikey_2); 
    rec.addVoiceModel(1, 3, what_is_digikey_3, fram_num_what_is_digikey_3); 
    rec.addVoiceModel(1, 4, what_is_digikey_4, fram_num_what_is_digikey_4); 
    rec.addVoiceModel(2, 0, what_are_you_selling_0, fram_num_what_are_you_selling_0); 
    rec.addVoiceModel(2, 1, what_are_you_selling_1, fram_num_what_are_you_selling_1); 
    rec.addVoiceModel(2, 2, what_are_you_selling_2, fram_num_what_are_you_selling_2); 
    rec.addVoiceModel(2, 3, what_are_you_selling_3, fram_num_what_are_you_selling_3); 
    rec.addVoiceModel(2, 4, what_are_you_selling_4, fram_num_what_are_you_selling_4); 
    rec.addVoiceModel(3, 0, how_can_i_make_the_order_0, fram_num_how_can_i_make_the_order_0); 
    rec.addVoiceModel(3, 1, how_can_i_make_the_order_1, fram_num_how_can_i_make_the_order_1); 
    rec.addVoiceModel(3, 2, how_can_i_make_the_order_2, fram_num_how_can_i_make_the_order_2); 
    rec.addVoiceModel(3, 3, how_can_i_make_the_order_3, fram_num_how_can_i_make_the_order_3); 
    rec.addVoiceModel(3, 4, how_can_i_make_the_order_4, fram_num_how_can_i_make_the_order_4); 
    rec.addVoiceModel(4, 0, how_can_i_cancel_the_order_0, fram_num_how_can_i_cancel_the_order_0); 
    rec.addVoiceModel(4, 1, how_can_i_cancel_the_order_1, fram_num_how_can_i_cancel_the_order_1); 
    rec.addVoiceModel(4, 2, how_can_i_cancel_the_order_2, fram_num_how_can_i_cancel_the_order_2); 
    rec.addVoiceModel(4, 3, how_can_i_cancel_the_order_3, fram_num_how_can_i_cancel_the_order_3); 
    rec.addVoiceModel(4, 4, how_can_i_cancel_the_order_4, fram_num_how_can_i_cancel_the_order_4);
    rec.addVoiceModel(5, 0, who_are_you_new_0, fram_num_who_are_you_new_0); 
    rec.addVoiceModel(5, 1, who_are_you_new_1, fram_num_who_are_you_new_1); 
    rec.addVoiceModel(5, 2, who_are_you_new_2, fram_num_who_are_you_new_2); 

    Serial.println("Model init OK!");
    lcd.fillScreen(BG_COLOR);
    printCenterOnLCD(lcd, "Voice Bot is ready!");
    delay(1000);
}

void loop() {
    lcd.fillScreen(BG_COLOR);
    printCenterOnLCD(lcd, "You can speak now..");
    
    int res = rec.recognize();
    Serial.printf("res : %d ", res);
    lcd.fillScreen(BG_COLOR);

    String userQuery;

    switch (res) {
        case 1:
            userQuery = "Who are you?";
            break;
        case 2:
            userQuery = "What is digikey?";
            break;
        case 3:
            userQuery = "What are you selling?";
            break;
        case 4:
            userQuery = "How can I make the order?";
            break;
        case 5:
            userQuery = "How can I cancel the order?";
            break;
        case 6:
            userQuery = "Who are you?";
            break;
        default:
            Serial.println("Recognition failed.");
            return; // Exit loop if recognition failed
    }

    printCenterOnLCD(lcd, userQuery.c_str());
    Serial.println("Recognized: " + userQuery);
    makeApiCall(userQuery);
    delay(1000); // Wait before recognizing again
}

void makeApiCall(String userQuery) {
    Serial.println("Attempting to connect to server...");

    if (client.connect("154.53.50.40", 80)) {
        Serial.println("Connected to server");

        // Prepare the URL with the query parameter
        String url = "/v1/user_query?user_query=" + urlencode(userQuery);

        // Make the HTTP GET request
        client.println("GET " + url + " HTTP/1.1");
        client.println("Host: 154.53.50.40:80");
        client.println("Accept: application/json");
        client.println(); // End of headers

        Serial.println("Request sent:");
        Serial.println("GET " + url);
        
        // Wait for the response
        long _startMillis = millis();
        Serial.println("Waiting for response...");
        while (!client.available() && (millis() - _startMillis < 5000)) {
            // Wait for response
        }

        // Check if we received a response
        if (client.available()) {
            Serial.println("Response from server:");
            while (client.available()) {
                String line = client.readStringUntil('\n');
                Serial.println(line);
            }
        } else {
            Serial.println("No response from server");
        }

        client.stop(); // Close the connection
    } else {
        Serial.println("Connection failed");
    }
}

// Helper function to URL-encode the query string
String urlencode(String str) {
    String encoded = "";
    for (int i = 0; i < str.length(); i++) {
        char c = str.charAt(i);
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else {
            encoded += '%';
            encoded += String(c >> 4, HEX);
            encoded += String(c & 0x0F, HEX);
        }
    }
    return encoded;
}