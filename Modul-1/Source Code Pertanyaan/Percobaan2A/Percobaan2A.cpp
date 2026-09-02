#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
#define RELAYPIN 16
DHT dht(DHTPIN, DHTTYPE);

// Ambang batas histerisis
const float suhuThresholdHigh = 30.0; // Suhu untuk menyalakan aktuator (ON)
const float suhuThresholdLow = 28.0;  // Suhu untuk mematikan aktuator (OFF)

// Variabel untuk menyimpan status aktuator
bool aktuator = false; // Status awal: OFF

void setup()
{
    Serial.begin(115200);
    dht.begin();
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, LOW); // Pastikan aktuator OFF saat start
}

void loop()
{
    float suhu = dht.readTemperature();

    if (isnan(suhu))
    {
        Serial.println("Gagal membaca data sensor!");
    }
    else
    {
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C -> ");

        if (!aktuator && suhu > suhuThresholdHigh)
        {
            // Aktuator OFF dan suhu melebihi batas atas -> nyalakan
            aktuator = true;
            digitalWrite(RELAYPIN, HIGH);
            Serial.println("Aktuator: ON (melewati 30°C)");
        }
        else if (aktuator && suhu < suhuThresholdLow)
        {
            // Aktuator ON dan suhu di bawah batas bawah -> matikan
            aktuator = false;
            digitalWrite(RELAYPIN, LOW);
            Serial.println("Aktuator: OFF (di bawah 28°C)");
        }
        else
        {
            // Kondisi di antara ambang batas - pertahankan status
            if (aktuator)
            {
                Serial.println("Aktuator: ON (di zona histerisis)");
            }
            else
            {
                Serial.println("Aktuator: OFF (di zona histerisis)");
            }
        }
    }

    delay(2000); // Jeda 2 detik
}