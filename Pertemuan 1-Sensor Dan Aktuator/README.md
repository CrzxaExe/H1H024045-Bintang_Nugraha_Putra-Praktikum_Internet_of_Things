# Modul 1 - Sensor Dan Aktuator

> [!Info]
> Memahami, mempelajari dan mengimplementasikan bagaimana akuisisi data dari sensor dan digunakan sebagai acuan kondisi untuk aktuator.

## Isi Dari README.md Ini

- Library (Yang digunakan)
- Percobaan (Kode asli, Penjelasannya)
- Pertanyan Praktikum (Source Code, Penjelasan)
- Dokumentasi

## Library

Libary yang diperlukan untuk

- DHT.h: Digunakan untuk berinteraksi dengan perangkat DHT11 atau DHT22.

## Percobaan Praktikum

### Percobaan 1A

`File : ./Code/Percobaan1A.cpp`

```cpp
#include <DHT.h>
#define DHTPIN 2 // Pin GPIO untuk DHT
#define DHTTYPE DHT11 // Tipe dari DHT
DHT dht(DHTPIN, DHTTYPE); // Instane DHT

// Fungsi yang akan di jalankan saat pertama kali esp menyala
// Menginisiasi Serial dan DHT
void setup()
{
    Serial.begin(115200);
    dht.begin();
    Serial.println("Memulai akuisisi data sensor DHT11...");
}

// Fungsi yang akan di jalankan berulang
void loop()
{
    // Variabel penampung data dari DHT
    float kelembaban = dht.readHumidity();
    float suhu = dht.readTemperature();

    // Melakukan pengecekan
    // Jika kelembaban dan suhu bernilai NaN, maka masuk block if
    // Jika false maka masuk block else
    if (isnan(kelembaban) || isnan(suhu))
    { // Menampilkan pesan error ke serial monitor
        Serial.println("Gagal membaca data dari sensor DHT11!");
    }
    else
    { // Menampilkan suhu dan kelembaban ke serial monitor
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C, Kelembaban: ");
        Serial.print(kelembaban);
        Serial.println(" %");
    }
    delay(2000); // Jeda 2 detik
}
```

### Percobaan 2A

`File : ./Code/Percobaan2A.cpp`

```cpp
#include <DHT.h>
#define DHTPIN 2 // Pin GPIO untuk DHT
#define DHTTYPE DHT11 // Tipe dari DHT
#define RELAYPIN 16 // Pin GPIO untuk Relay
DHT dht(DHTPIN, DHTTYPE); // Instance DHT
const float suhuThreshold = 30.0; // Batas ambang suhu

// Fungsi yang akan di jalankan saat pertama kali esp menyala
// Menginisiasi Serial, DHT, pin Relay dan nilai awal pin Relay
void setup()
{
    Serial.begin(115200);
    dht.begin();
    pinMode(RELAYPIN, OUTPUT);
    digitalWrite(RELAYPIN, LOW);
}

// Fungsi yang akan di jalankan berulang
void loop()
{
    float suhu = dht.readTemperature(); // Variabel suhu

    // Melakukan pengecekan
    // Jika suhu bernilai NaN, maka masuk block if
    // Jika false maka masuk block else
    if (isnan(suhu))
    { // Menampilkan pesan error ke serial monitor
        Serial.println("Gagal membaca data sensor!");
    }
    else
    { // Menampilkan suhu dan state dari Relay
        Serial.print("Suhu: ");
        Serial.print(suhu);
        Serial.print(" °C -> ");
        if (suhu > suhuThreshold)
        { // Relay menyala jika suhu lebih besar dari batas
            digitalWrite(RELAYPIN, HIGH);
            Serial.println("Aktuator: ON");
        }
        else
        { // Relay mati jika suhu kurang dari batas
            digitalWrite(RELAYPIN, LOW);
            Serial.println("Aktuator: OFF");
        }
    }
}
```

## Pertanyaan Praktikum

### Percobaan 1A - Modifikasi program agar data suhu dan kelembaban dirata-ratakan dari 5 kali pembacaan sebelum ditampilkan

`File : ./Code/Percobaan1A-Answer.cpp`

```cpp
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
```

**Penjelasan**

1. Menambahkan variabel konstanta dari total pengambilan data tiap loop

```cpp
const int TOTAL_SAMPLE = 5;
```

2. Menggunakan variabel berikut untuk dijadikan variabel temporary untuk menyimpan sum dari 5 data yang masuk

```cpp
float kelembaban = 0, suhu = 0;
```

3. Menambahkan variabel untuk menghitung data yang valid saat looping

```cpp
int dataValid = 0;
```

4. Pengulangan untuk mendapatkan data dari DHT sesuai total sample

```cpp
for (int i = 0; i < TOTAL_SAMPLE; i++)
```

5. Membaca suhu dan kelembaban tiap looping

```cpp
float s = dht.readTemperature();
float k = dht.readHumidity();
```

6. Melakukan validasi apakah data suhu dan kelembaban saat looping tersebut valid atau tidak, jika iya maka akan dijumlahkan ke variabel kelembaban dan suhu,serta dataValid bertambah 1. Delay tiap looping

```cpp
if (!isnan(s) && !isnan(k))
{
   kelembaban += k;
   suhu += s;
   dataValid++;
}
delay(100);
```

7. Saat sudah looping sebanyak TOTAL_SAMPLE, akan menampilkan suhu dan kelembaban yang sudah dirata-rata. jika error akan menampilkan pesan error. Setelah delay 2 detik, baru lanjut membaca lagi.

```cpp
if (dataValid)
{
   Serial.printf("Suhu: %.1f°C | Kelembaban: %.1f%%\n", totalSuhu / valid, totalKelembaban / valid);
}
else
{
    Serial.println("Gagal membaca sensor DHT11")
}
delay(2000);
```

### Percobaan 2A

`File : ./Code/Percobaan2A-Answer.cpp`

```cpp
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
```

**Penjelasan**

1. Menambahkan Threshold baru (High, Low)

```cpp
const float suhuThresholdHigh = 30.0;
const float suhuThresholdLow = 28.0;
```

2. Melakukan cek apakah suhu sekarang lebih besar dari batas atas dan aktuator sedang off

```cpp
if (!aktuator && suhu > suhuThresholdHigh)
{
    // Aktuator OFF dan suhu melebihi batas atas -> nyalakan
    aktuator = true;
    digitalWrite(RELAYPIN, HIGH);
    Serial.println("Aktuator: ON (melewati 30°C)");
}
```

3. Melakukan cek apakah suhu sekarang kurang dari batas bawah dan aktuator sedang on

```cpp
else if (aktuator && suhu < suhuThresholdLow)
{
    // Aktuator ON dan suhu di bawah batas bawah -> matikan
    aktuator = false;
    digitalWrite(RELAYPIN, LOW);
    Serial.println("Aktuator: OFF (di bawah 28°C)");
}
```

4. Jika suhu sekarang berada di antara batas atas dan batas bawah hanya melakukan print sesuai dengan kondisi aktuator

```cpp
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
```

## Dokumentasi

**Percobaan 1A**

![Dokumentasi Percobaan 1A](/Pertemuan%201-Sensor%20Dan%20Akuator/Dokumentasi/Dokumentasi-Percobaan1A.jpg)

**Percobaan 2A**

![Dokumentasi Percobaan 2A](/Pertemuan%201-Sensor%20Dan%20Akuator/Dokumentasi/Dokumentasi-Percobaan2A.jpg)
