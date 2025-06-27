#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>  // You need to install the ArduinoJson library

// WiFi credentials
const char* ssid = "Ranjith";
const char* password = "RLR12345";

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adjust address and size for your display

void setup() {
  // Initialize Serial
  Serial.begin(115200);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Fetch and display the news headlines
  displayNews();
}

void loop() {
  // You can add logic here to refresh the news at a specific time (e.g., every day at 8:00 AM)
  delay(600000);  // Refresh every 10 minutes
}

void displayNews() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("https://rss.app/feeds/v1.1/NNtHI1RYnKXjkrsK.json");  // RSS feed link
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.println("Received JSON payload: ");
      Serial.println(payload);

      // Process the JSON and extract headlines
      parseAndDisplayHeadlines(payload);

    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }
}

void parseAndDisplayHeadlines(String payload) {
  DynamicJsonDocument doc(10240);  // Adjust size if needed

  // Parse the JSON payload
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Loop through the array of news items
  JsonArray items = doc["items"];
  for (JsonObject item : items) {
    String title = item["title"];  // Get the title (headline)

    Serial.println("Headline: " + title);

    // Display the headline on the LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(title.substring(0, 16));  // Display the first 16 characters
    scrollText(title);  // Scroll the rest of the headline if it's too long

    delay(5000);  // Delay to display each headline for 5 seconds
  }
}

void scrollText(String text) {
  if (text.length() > 16) {
    for (int i = 0; i < text.length() - 15; i++) {
      lcd.setCursor(0, 1);
      lcd.print(text.substring(i, i + 16));  // Scroll text in the second row
      delay(300);  // Adjust scrolling speed
    }
  }
}
