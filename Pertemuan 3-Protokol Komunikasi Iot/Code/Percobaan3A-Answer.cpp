#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char *ssid = "nama-wifi";
const char *password = "password";
const char *serverUrl = "https://httpbin.org/post";

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, password);

    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        WiFiClientSecure client;
        client.setInsecure();

        http.begin(client, serverUrl);
        http.addHeader("Content-Type", "application/json");

        JsonDocument doc;
        doc["suhu"] = 28.5;
        doc["kelembaban"] = 65.0;
        doc["uptime"] = millis();

        String requestBody;
        serializeJson(doc, requestBody);

        Serial.print("Mengirim data: ");
        Serial.println(requestBody);

        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0)
        {
            Serial.print("Kode Respon HTTP: ");
            Serial.println(httpResponseCode);
            Serial.println("Isi Respon:");
            Serial.println(http.getString());
        }
        else
        {
            Serial.print("Pengiriman gagal, kode error: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    }

    delay(10000);
}