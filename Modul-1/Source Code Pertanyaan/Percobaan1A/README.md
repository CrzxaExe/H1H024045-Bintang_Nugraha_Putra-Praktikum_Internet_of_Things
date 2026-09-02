# Modifikasi program agar data suhu dan kelembaban dirata-ratakan dari 5 kali pembacaan sebelum ditampilkan

> [!NOTE]
> Baris | Kode

## Perubahan pada kode

1. Menambahkan variabel konstanta dari total pengambilan data tiap loop

```cpp
 6 | const int TOTAL_SAMPLE = 5;
```

2. Menggunakan variabel berikut untuk dijadikan variabel temporary untuk menyimpan sum dari 5 data yang masuk

```cpp
16 | float kelembaban = 0, suhu = 0;
```

3. Menambahkan variabel untuk menghitung data yang valid saat looping

```cpp
17 | int dataValid = 0;
```

4. Pengulangan untuk mendapatkan data dari DHT sesuai total sample

```cpp
19 | for (int i = 0; i < TOTAL_SAMPLE; i++)
```

5. Membaca suhu dan kelembaban tiap looping

```cpp
21 | float s = dht.readTemperature();
22 | float k = dht.readHumidity();
```

6. Melakukan validasi apakah data suhu dan kelembaban saat looping tersebut valid atau tidak, jika iya maka akan dijumlahkan ke variabel kelembaban dan suhu,serta dataValid bertambah 1. Delay tiap looping

```cpp
24 | if (!isnan(s) && !isnan(k))
25 | {
26 |     kelembaban += k;
27 |     suhu += s;
28 |     dataValid++;
29 | }
30 | delay(100);
```

7. Saat sudah looping sebanyak TOTAL_SAMPLE, akan menampilkan suhu dan kelembaban yang sudah dirata-rata. jika error akan menampilkan pesan error. Setelah delay 2 detik, baru lanjut membaca lagi.

```cpp
33 | if (dataValid)
34 | {
35 |     Serial.printf("Suhu: %.1f°C | Kelembaban: %.1f%%\n", totalSuhu / valid, totalKelembaban / valid);
36 | }
37 | else
38 | {
39 |     Serial.println("Gagal membaca sensor DHT11")
40 | }
41 | delay(2000);
```
