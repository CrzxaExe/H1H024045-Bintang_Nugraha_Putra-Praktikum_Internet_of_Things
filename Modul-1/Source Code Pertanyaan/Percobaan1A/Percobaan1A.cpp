#include <DHT.h>
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int TOTAL_SAMPLE = 5;

void setup()
{
    Serial.begin(115200);
    dht.begin();
    Serial.println("Memulai akuisisi data sensor DHT11...");
}
void loop()
{
    float kelembaban = 0, suhu = 0;
    int dataValid = 0;

    for (int i = 0; i < TOTAL_SAMPLE; i++)
    {
        float s = dht.readTemperature();
        float k = dht.readHumidity();

        if (!isnan(s) && !isnan(k))
        {
            kelembaban += k;
            suhu += s;
            dataValid++;
        }
        delay(100);
    }

    if (dataValid)
    {
        Serial.printf("Suhu: %.1f°C | Kelembaban: %.1f%%\n", totalSuhu / valid, totalKelembaban / valid);
    }
    else
    {
        Serial.println("Gagal membaca sensor DHT11")
    }
    delay(2000);
}
