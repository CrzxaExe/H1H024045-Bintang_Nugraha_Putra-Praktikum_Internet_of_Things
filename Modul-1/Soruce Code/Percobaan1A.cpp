#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    dht.begin();
    Serial.println("Memulai akuisisi data sensor DHT22...");
}
void loop()
{
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();
    if (isnan(kelembaban) || isnan(suhu))
    {
        Serial.println("Gagal membaca data dari sensor DHT22!");
    }
    else
    {
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C, Kelembaban: ");
        Serial.print(kelembaban);
        Serial.println(" %");
    }
    delay(2000);
}
