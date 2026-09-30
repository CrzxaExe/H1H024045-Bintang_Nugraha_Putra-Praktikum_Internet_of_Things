#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <DHT.h>
const char *ssid = "Crzx";
const char *password = "CrzxaExe3";
const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 8883;
const char *topicData = "test/data";
const char *topicPerintah = "test/sensor";
const char *topicBuzzer = "test/buzzer";

const char *mqttUsername = "test";
const char *mqttPassword = "12345678";

#define DHTPIN 2
#define DHTTYPE DHT11
const int ledPin = 12;
const int buzzerPin = 27;

DHT dht(DHTPIN, DHTTYPE);
WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long waktuTerakhirPublish = 0;
const long intervalPublish = 5000;

void callback(char *topic, byte *payload, unsigned int length)
{
    String pesan;

    for (unsigned int i = 0; i < length; i++)
        pesan += (char)payload[i];

    JsonDocument doc;
    if (deserializeJson(doc, pesan))
        return;

    const char *perintah = doc["perintah"];

    if (String(topic) == topicPerintah)
    {
        digitalWrite(ledPin, String(perintah) == "ON" ? HIGH : LOW);
    }

    else if (String(topic) == topicBuzzer)
    {
        digitalWrite(buzzerPin, String(perintah) == "ON" ? HIGH : LOW);
    }
}

void hubungkanWiFi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
        delay(500);
    Serial.println("WiFi berhasil terhubung!");
}

void hubungkanMQTT()
{
    while (!client.connected())
    {
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUsername, mqttPassword))
        {
            client.subscribe(topicPerintah);
            client.subscribe(topicBuzzer);
            Serial.println("Terhubung dan subscribe topic perintah");
        }
        else
        {
            delay(2000);
        }
    }
}

void setup()
{
    espClient.setInsecure();
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    dht.begin();
    hubungkanWiFi();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}
void loop()
{
    if (!client.connected())
        hubungkanMQTT();
    client.loop();
    if (millis() - waktuTerakhirPublish > intervalPublish)
    {
        waktuTerakhirPublish = millis();
        float suhu = dht.readTemperature();
        if (!isnan(suhu))
        {
            JsonDocument doc;
            doc["suhu"] = random(24, 30);
            char buffer[128];
            serializeJson(doc, buffer);
            client.publish(topicData, buffer);
            Serial.print("Data terkirim: ");
            Serial.println(buffer);
        }
    }
}