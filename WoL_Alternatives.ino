#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>
#include <DHT11.h>
#include <ESP32Ping.h>
#include <time.h>

// WiFi network station credentials
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "TOKEN"

// Constants
const unsigned long BOT_MTBS = 1000; // mean time between scan messages
const char* pingIP = "172.23.0.50";  // IP address to ping
const int servoPin = 13; // D13 | Servo
const int dhtPin = 15; // D15 | DHT11
const String ownerID = "12345";  // Replace with your Telegram user ID

// Global Variables
unsigned long bot_lasttime; // last time messages' scan has been done
bool armed = false;
unsigned long armed_time;
int temperature = 0;

// Objects
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
Servo myservo;
DHT11 dht11(dhtPin);

// Function to check if user is authorized
bool isAuthorized(String userID) {
    return (userID == ownerID);
}

// Function to handle incoming Telegram messages
void handleNewMessages(int numNewMessages) {
    Serial.print("handleNewMessages ");
    Serial.println(numNewMessages);

    String answer;
    for (int i = 0; i < numNewMessages; i++) {
        telegramMessage &msg = bot.messages[i];
        Serial.println("Received " + msg.text);

        // Security check
        if (!isAuthorized(msg.from_id)) {
            bot.sendMessage(msg.chat_id, "You are not authorized to use this bot.", "");
            continue;
        }

        if (msg.text == "/help") {
            answer = "Available commands:\n/start\n/status\n/armed\n/trigger\n/temp\n/force";
        } else if (msg.text == "/start") {
            answer = "Welcome! Use /help to see available commands.";
        } else if (msg.text == "/status") {
            answer = "All systems operational.";
        } else if (msg.text == "/armed") {
            armed = true;
            armed_time = millis();
            myservo.attach(servoPin, 1000, 2000);
            answer = "Servo is armed for 10 seconds.";
        } else if (msg.text == "/trigger") {
            if (armed && !Ping.ping(pingIP)) {
                run();
                answer = "Servo triggered.";
            } else {
                answer = "Servo is not armed or ping to " + String(pingIP) + " responded.";
            }
        } else if (msg.text == "/temp") {
            readtemp();
            answer = "Current temperature: " + String(temperature - 5) + " °C";
        } else if (msg.text.startsWith("/force ")) {
            if (armed) {
                time_t now = time(nullptr);
                struct tm *timeinfo = localtime(&now);
                int day = timeinfo->tm_mday;
                int month = timeinfo->tm_mon + 1; 

                String correctInput = "121" + String(day) + String(month); // 121 radom code
                if (msg.text.endsWith(correctInput)) {
                    run();
                    delay(100);
                    myservo.write(0);
                     delay(200);
                    myservo.detach();
                    armed = false;
                    answer = "Forced trigger executed and disarmed.";
                } else {
                    myservo.write(0);
                    myservo.detach();
                    answer = "Incorrect input.";
                }
            } else {
                answer = "Incorrect input.";
            }
        } else {
            answer = "Unknown command. Use /help for available commands.";
        }

        bot.sendMessage(msg.chat_id, answer, "Markdown");
    }
}

// Setup bot commands
void bot_setup() {
    const String commands = F("["
                              "{\"command\":\"help\",  \"description\":\"Get bot usage help\"},"
                              "{\"command\":\"start\", \"description\":\"Start interaction with the bot\"},"
                              "{\"command\":\"status\",\"description\":\"Get current status\"},"
                              "{\"command\":\"armed\", \"description\":\"Arm the servo for 10 seconds\"},"
                              "{\"command\":\"trigger\",\"description\":\"Trigger the servo once if armed and no ping response\"},"
                              "{\"command\":\"temp\",  \"description\":\"Get current temperature\"},"
                              "{\"command\":\"force\", \"description\":\"Force trigger the servo with input 121+day+month\"}"
                              "]");
    bot.setMyCommands(commands);
}

// WiFi connection setup
void setup() {
    Serial.begin(115200);
    Serial.println();

    // Connect to WiFi
    Serial.print("Connecting to Wifi SSID ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("\nWiFi connected. IP address: ");
    Serial.println(WiFi.localIP());

    // Time configuration
    Serial.print("Retrieving time: ");
    configTime(0, 0, "pool.ntp.org"); // get UTC time via NTP
    time_t now = time(nullptr);
    while (now < 24 * 3600) {
        Serial.print(".");
        delay(100);
        now = time(nullptr);
    }
    Serial.println(now);

    // Servo setup
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    myservo.setPeriodHertz(50);    

    bot_setup();
}

// Main loop
void loop() {
    if (millis() - bot_lasttime > BOT_MTBS) {
        int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

        while (numNewMessages) {
            Serial.println("got response");
            handleNewMessages(numNewMessages);
            numNewMessages = bot.getUpdates(bot.last_message_received + 1);
        }

        bot_lasttime = millis();
    }

    // Detach servo if armed time has exceeded 10 seconds
    if (armed && millis() - armed_time > 10000) {
        myservo.write(0);
        myservo.detach();
        armed = false;
        Serial.println("Servo disarmed.");
    }
}

// Function to run the servo
void run() {
    myservo.write(0);
    delay(200);
    myservo.write(90);
    delay(200);
    myservo.write(0);
}

// Function to read temperature
void readtemp() {
    int t1 = dht11.readTemperature();
    delay(1000);
    int t2 = dht11.readTemperature();
    delay(1000);
    int t3 = dht11.readTemperature();
    delay(1000);
    int t4 = dht11.readTemperature();
    delay(1000);
    temperature = (t1 + t2 + t3 + t4) / 4;
    Serial.print(temperature - 5);
    Serial.println(" °C");
}
