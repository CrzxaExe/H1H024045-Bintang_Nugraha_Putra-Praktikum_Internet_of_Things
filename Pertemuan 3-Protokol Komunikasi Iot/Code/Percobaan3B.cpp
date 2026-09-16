#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid = "nama-wifi";
const char *password = "password";

const char *mqttServer =
    "mqtt.server";

const int mqttPort = 8883;

const char *mqttUsername = "test";
const char *mqttPassword = "12345678";

const char *mqttTopic =
    "test/sensor";

WiFiClientSecure espClient;
PubSubClient client(espClient);

void hubungkanWiFi()
{

    WiFi.begin(ssid, password);

    Serial.print("Menghubungkan ke WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {

        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi berhasil terhubung!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void hubungkanMQTT()
{

    while (!client.connected())
    {

        Serial.println();
        Serial.print("Menghubungkan ke broker MQTT...");

        String clientId =
            "ESP8266Client-" +
            String(ESP.getChipId(), HEX);

        Serial.print(" Client ID: ");
        Serial.println(clientId);

        if (client.connect(
                clientId.c_str(),
                mqttUsername,
                mqttPassword))
        {

            Serial.println("MQTT berhasil terhubung!");

            Serial.print("Broker : ");
            Serial.println(mqttServer);

            Serial.print("Port   : ");
            Serial.println(mqttPort);

            Serial.println("================================");
        }
        else
        {

            Serial.print("MQTT gagal, rc=");
            Serial.println(client.state());

            Serial.println("Mencoba lagi dalam 2 detik...");

            delay(2000);
        }
    }
}

void setup()
{

    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("================================");
    Serial.println(" ESP8266 MQTT - HiveMQ Cloud");
    Serial.println("================================");

    hubungkanWiFi();

    espClient.setInsecure();

    client.setServer(
        mqttServer,
        mqttPort);

    hubungkanMQTT();
}

void loop()
{

    if (!client.connected())
    {

        Serial.println();
        Serial.println("MQTT terputus!");

        hubungkanMQTT();
    }

    client.loop();

    JsonDocument doc;

    doc["suhu"] = 28.5;
    doc["kelembaban"] = 65.0;

    char buffer[128];

    serializeJson(doc, buffer);

    bool berhasil =
        client.publish(
            mqttTopic,
            buffer);

    if (berhasil)
    {

        Serial.println();
        Serial.println("Data berhasil dikirim!");

        Serial.print("Topic   : ");
        Serial.println(mqttTopic);

        Serial.print("Payload : ");
        Serial.println(buffer);
    }
    else
    {

        Serial.println();
        Serial.println("Gagal mengirim data!");
    }

    delay(5000);
}