# Modul x - Judul

> [!INFO]
> Ringkasan praktikum

## Isi Dari README.md Ini

- [Library (Yang digunakan)](#library)
- [Percobaan (Kode asli, Penjelasannya)](#percobaan-praktikum)
- [Pertanyaan Praktikum (Source Code, Penjelasan)](#pertanyaan-praktikum)
- [Dokumentasi](#dokumentasi)

## Library

Libary yang diperlukan untuk

- Nama Library: deskripsi

## Percobaan Praktikum

### Percobaan 1..

`File : ...`

```cpp
// #include <DHT.h>

// #define DHTPIN 4
// #define DHTTYPE DHT22

// DHT dht(DHTPIN, DHTTYPE);
// Harusnya pakai dht tapi engga di sediain kampus

const int N = 5;                  // jumlah sampel moving average
float buffer[N];                  // menyimpan N pembacaan terakhir
int indeks = 0;                   // posisi penulisan berikutnya pada buffer
int jumlah = 0;                   // jumlah sampel yang sudah terisi

float suhuMin = 1000.0; // Batas normal maksimum suhu
float suhuMax = -1000.0; // Batas normal minimum suhu

// Fungsi menghtiung Moving Average
float hitungMovingAverage(float nilaiBaru) {
  buffer[indeks] = nilaiBaru; // memasukan nilai baru ke buffer
  indeks = (indeks + 1) % N; // mengatur ulang indeks menjadi indeks sebelumnya + 1 yang selanjutnya ambil sisa bagi dengan panjang total data
  if (jumlah < N) jumlah++; // jumlah kurang dari total data maka tambah 1 angka

  float total = 0;
  for (int i = 0; i < jumlah; i++) { // sum dari data saat ini
    total += buffer[i];
  }
  return total / jumlah; // mengembalikan nilai total / jumlah
}

// Mock function pengganti sensor DHT22
float bacaSuhuMock() {
    static unsigned long jumlahBaca = 0;
    jumlahBaca++;

    // Suhu dasar berubah secara bertahap
    float suhuDasar = 28.0 + 2.0 * sin(jumlahBaca * 0.08);

    // Noise kecil untuk mensimulasikan fluktuasi sensor
    float noise = random(-5, 6) * 0.1;

    float suhu = suhuDasar + noise;

    // Lonjakan suhu setiap 25 pembacaan untuk simulasi anomali
    if (jumlahBaca % 25 == 0) {
        suhu += 8.0;
    }

    return suhu;
}

// Fungsi yang akan di panggil sekali saat ESP dinyalakan
void setup() {
  Serial.begin(115200); // Mengatur serial monitor agar berjalan di bautrate 115200
  // dht.begin();
}

void loop() {
  float suhu = bacaSuhuMock(); //dht.readTemperature(); , menggunakan fungsi tersebut untuk menghasilkan data acak pengganti dht

  if (isnan(suhu)) { // Mengecek apakah nilai suhu itu valid dan bukan nan, jika iya maka tampilkan pesan error
    Serial.println("Pembacaan gagal, data diabaikan");
  } else {
    float suhuFilter = hitungMovingAverage(suhu); // memanggil fungsi filtering

    if (suhu < suhuMin) suhuMin = suhu; // mengganti nilai suhu minimum, jika suhu saat ini lebih kecil dari yang tersimpan
    if (suhu > suhuMax) suhuMax = suhu; // mengganti nilai suhu maksimal, jika suhu saat ini lebih besar dari yang tersimpan

    // Menampilkan data
    Serial.print("Mentah: ");
    Serial.print(suhu);
    Serial.print(" | Filter: ");
    Serial.print(suhuFilter);
    Serial.print(" | Min: ");
    Serial.print(suhuMin);
    Serial.print(" | Max: ");
    Serial.println(suhuMax);
  }

  delay(2000); // Jeda 2 detik
}
```

### Percobaan 2..

`File : ...`

```py
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com" # domain brocker
PORT = 1883 # port broker
TOPIC = "test/sensor" # = topicData ESP32
JUMLAH_SAMPEL = 30 # jumlah data

data = [] # variabel penampung data

# fungsi yang akan dijalankan saat terhubung ke mqtt
def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC) # melakukan subscribe ke topik yang ditentukan

# fungsi yang akan selalu dipanggil saat client menerima pesan
def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode()) # melakukan decode dari string ke json yang diterima client
        suhu = float(payload["suhu"]) # nilai suhu dari mqtt
    except (ValueError, KeyError): # jika terjadi error, maka tampilkan pesan error dan menunggu pesan selanjutnya
        print("Data tidak valid, diabaikan")
        return

    # memasukan data ke variabel data bersamaan dengan waktu menerima data tersebut, lalu tampilkan
    data.append({"waktu": time.strftime("%Y-%m-%d %H:%M:%S"), "suhu": suhu})
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL: # jika data sudah mencapai 30, maka mematikan koneksi client
        client.disconnect()

# -- Tahap 1: akuisisi data dari MQTT
# setup mqtt
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2) # membuat client
client.on_connect = on_connect # mengatur fungsi saat berhasil terkoneksi
client.on_message = on_message # mengatur fungsi saat ada pesan masuk
client.connect(BROKER, PORT) # memulai koneksi ke mqtt
client.loop_forever() # menjaga koneksi dan juga menerima pesan masuk

# -- Tahap 2: simpan ke CSV
df = pd.DataFrame(data) # membuat objek panda
df.to_csv("data_suhu.csv", index=False) # menyimpan data menjadi csv
print("Data tersimpan di data_suhu.csv")

# -- Tahap 3: analisis statistik dan deteksi anomali (z-score)
rata2 = df["suhu"].mean() # nilai rata-rata
std = df["suhu"].std() # nilai simpangan baku

# menentukan z score
if std == 0:
    df["zscore"] = 0.0
else:
    df["zscore"] = (df["suhu"] - rata2) / std

df["anomali"] = df["zscore"].abs() > 2 # nilai anomali, dimana jika nilai z score absolute lebih besar dari 2 maka di anggap anomali

# menampilkan hasil rekap
print(f"Rata-rata: {rata2:.2f}")
print(f"Std deviasi: {std:.2f}")
print(f"Minimum : {df['suhu'].min():.2f}")
print(f"Maksimum: {df['suhu'].max():.2f}")
print(f"Jumlah anomali: {df['anomali'].sum()}")

# -- Tahap 4: visualisasi
plt.figure(figsize=(10,4))  # Membuat kanvas grafik berukuran 10 × 4 inci.
plt.plot(df.index, df["suhu"], marker="o", label="Suhu")  # Menggambar suhu berdasarkan urutan data dengan penanda titik.
anom = df[df["anomali"]]  # Memilih baris data yang terdeteksi sebagai anomali.
plt.scatter(anom.index, anom["suhu"], color="red", s=100, label="Anomali", zorder=3)  # Menandai anomali dengan titik merah.
plt.axhline(rata2, color="gray", linestyle="--", label="Rata-rata")  # Menambahkan garis putus-putus pada nilai rata-rata.
plt.xlabel("Urutan data")  # Memberi label pada sumbu horizontal.
plt.ylabel("Suhu (°C)")  # Memberi label pada sumbu vertikal.
plt.title("Data Suhu dan Deteksi Anomali")  # Memberi judul grafik.
plt.legend()  # Menampilkan keterangan untuk setiap garis dan penanda.
plt.tight_layout()  # Merapikan jarak elemen agar tidak saling bertumpuk.
plt.savefig("grafik_suhu.png") # simpan gambar
plt.show() # tampilkan dalam bentuk window
```

## Pertanyaan Praktikum

### Percobaan 1..

`File : ...`

```cpp
// Source code
```

**Penjelasan**

1. ..

```cpp
// Code snippet
```

2. ..

```cpp
// Code snippet
```

### Percobaan 2..

`File : ...`

```cpp
// Source code
```

**Penjelasan**

1. ..

```cpp
// Code snippet
```

2. ..

```cpp
// Code snippet
```

## Dokumentasi
