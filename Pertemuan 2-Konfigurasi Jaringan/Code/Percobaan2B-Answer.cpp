#include <ESP8266WiFi.h>

const char *ap_ssid = "ESP32_AccessPoint";
const char *ap_password = "12345678";

const char *sta_ssid = "your-ssid";
const char *sta_password = "your-password";

void setup()
{
    Serial.begin(115200);

    WiFi.mode(WIFI_AP_STA);
    WiFi.setHostname("ESP AP+STA");
    WiFi.begin(sta_ssid, sta_password);

    Serial.print("Menghubungkan ke WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("\nTerhubung ke WiFi!");
        Serial.print("IP Station: ");
        Serial.println(WiFi.localIP());
    }

    WiFi.softAP(ap_ssid, ap_password);
    IPAddress apIP = WiFi.softAPIP();
    Serial.println("Access Point aktif!");
    Serial.print("IP Address    : ");
    Serial.println(apIP);
}

void loop()
{
    static unsigned long lastPrint = 0;

    if (millis() - lastPrint >= 5000)
    {
        lastPrint = millis();

        Serial.println("\n=== STATUS ===");

        // Status Station
        Serial.print("WiFi: ");
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("Terhubung");
            Serial.print("IP: ");
            Serial.println(WiFi.localIP());
        }
        else
        {
            Serial.println("Tidak terhubung");
        }

        // Status AP
        int clients = WiFi.softAPgetStationNum();
        Serial.print("AP Clients: ");
        Serial.println(clients);
    }

    delay(100);
}