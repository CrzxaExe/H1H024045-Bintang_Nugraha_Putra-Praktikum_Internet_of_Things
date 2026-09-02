# Modifikasi agar menggunakan dua ambang batas (histerisis)

> [!NOTE]
> Baris | Kode

## Perubahan pada kode

1. Menambahkan Threshold baru (High, Low)

```cpp
 8 | const float suhuThresholdHigh = 30.0;
 9 | const float suhuThresholdLow = 28.0;
```

2. Melakukan cek apakah suhu sekarang lebih besar dari batas atas dan aktuator sedang off

```cpp
36 | if (!aktuator && suhu > suhuThresholdHigh)
37 | {
38 |     // Aktuator OFF dan suhu melebihi batas atas -> nyalakan
39 |     aktuator = true;
$0 |     digitalWrite(RELAYPIN, HIGH);
41 |     Serial.println("Aktuator: ON (melewati 30°C)");
42 | }
```

3. Melakukan cek apakah suhu sekarang kurang dari batas bawah dan aktuator sedang on

```cpp
43 | else if (aktuator && suhu < suhuThresholdLow)
44 | {
45 |     // Aktuator ON dan suhu di bawah batas bawah -> matikan
46 |     aktuator = false;
47 |     digitalWrite(RELAYPIN, LOW);
48 |     Serial.println("Aktuator: OFF (di bawah 28°C)");
49 | }
```

4. Jika suhu sekarang berada di antara batas atas dan batas bawah hanya melakukan print sesuai dengan kondisi aktuator

```cpp
50 | else
51 | {
52 |     // Kondisi di antara ambang batas - pertahankan status
53 |     if (aktuator)
54 |     {
55 |         Serial.println("Aktuator: ON (di zona histerisis)");
56 |    }
57 |    else
58 |    {
59 |        Serial.println("Aktuator: OFF (di zona histerisis)");
60 |    }
61 | }
```
