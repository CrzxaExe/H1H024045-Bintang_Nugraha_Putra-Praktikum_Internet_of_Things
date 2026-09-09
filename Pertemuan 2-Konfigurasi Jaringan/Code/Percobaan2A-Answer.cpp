#include <ESP8266WiFi.h>

const char *ssid = "Your-SSID";
const char *password = "Your-Password";

const int ledPin = 2;

void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setHostname("ESP STA");
    WiFi.setAutoReconnect(true);

    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi berhasil terhubung!");
    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());

    digitalWrite(ledPin, HIGH);
}

void loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nStatus: Terhubung");
    }
    else
    {
        Serial.println("Status: Terputus");
        digitalWrite(ledPin, LOW);
    }

    Serial.print("IP Address  : ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC Address : ");
    Serial.println(WiFi.macAddress());
    Serial.print("RSSI (dBm)  : ");
    Serial.println(WiFi.RSSI());
    delay(5000);
}