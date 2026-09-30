#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "nama-wifi";
const char *password = "password";
const char *mqttServer = "broker.hivemq.com";
const int mqttPort = 8883;
const char *topicPerintah = "test/sensor";

const char *mqttUsername = "test";
const char *mqttPassword = "12345678";
const int ledPin = 4;
WiFiClientSecure espClient;

PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{
    String pesan;
    for (unsigned int i = 0; i < length; i++)
    {
        pesan += (char)payload[i];
    }
    Serial.print("Pesan diterima [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(pesan);

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, pesan);
    if (error)
    {
        Serial.print("Gagal parsing JSON: ");
        Serial.println(error.c_str());
        return;
    }
    const char *perintah = doc["perintah"];
    int intensitas = doc["intensitas"] | 0;

    if (String(perintah) == "ON")
    {
        ledcWrite(ledPin, intensitas);
        Serial.println("Aktuator: ON");
        Serial.print("Intensitas: ");
        Serial.println(intensitas);
    }
    else if (String(perintah) == "OFF")
    {
        ledcWrite(ledPin, 0);
        Serial.println("Aktuator: OFF");
    }
}

void hubungkanWiFi()
{
    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi berhasil terhubung!");
}

void hubungkanMQTT()
{
    while (!client.connected())
    {
        Serial.print("Menghubungkan ke broker MQTT...");
        String clientId = "ESP32Client-" + String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUsername, mqttPassword))
        {
            Serial.println("berhasil terhubung!");
            client.subscribe(topicPerintah);
            Serial.print("Subscribe ke topic: ");
            Serial.println(topicPerintah);
        }
        else
        {
            Serial.print("gagal, rc=");
            Serial.print(client.state());
            Serial.println(" coba lagi dalam 2 detik");
            delay(2000);
        }
    }
}

void setup()
{
    espClient.setInsecure();
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    hubungkanWiFi();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
}
void loop()
{
    if (!client.connected())
    {
        hubungkanMQTT();
    }
    client.loop();
}