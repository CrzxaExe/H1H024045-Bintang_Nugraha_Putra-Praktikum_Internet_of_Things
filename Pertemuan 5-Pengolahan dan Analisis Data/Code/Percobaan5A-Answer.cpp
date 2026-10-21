
#include <math.h> //  Menyediakan fungsi sqrtf() untuk menghitung akar kuadrat.

// #include <DHT.h>

// #define DHTPIN 4
// #define DHTTYPE DHT22

// DHT dht(DHTPIN, DHTTYPE);

const int N = 5; // Jumlah sampel moving average
float buffer[N]; // Menyimpan N pembacaan terakhir
int indeks = 0;  // Posisi penulisan berikutnya pada buffer
int jumlah = 0;  // Jumlah sampel yang sudah terisi

float suhuMin = 1000.0;
float suhuMax = -1000.0;

float hitungMovingAverage(float nilaiBaru) {
    buffer[indeks] = nilaiBaru;
    indeks = (indeks + 1) % N;

    if (jumlah < N) jumlah++;

    float total = 0;

    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }

    return total / jumlah;
}

//  Fungsi untuk menghitung simpangan baku data pada buffer.
float hitungSimpanganBaku() {
    //  Menghentikan perhitungan jika belum ada sampel.
    if (jumlah == 0) return 0.0;

    //  Variabel untuk menampung total seluruh data buffer.
    float total = 0.0;

    //  Menjumlahkan semua sampel yang sudah tersimpan.
    for (int i = 0; i < jumlah; i++) {
        total += buffer[i];
    }

    //  Menghitung rata-rata sampel yang ada di buffer.
    float rataRata = total / jumlah;

    //  Variabel untuk menampung jumlah kuadrat selisih.
    float totalKuadratSelisih = 0.0;

    //  Menghitung selisih setiap sampel terhadap rata-rata.
    for (int i = 0; i < jumlah; i++) {
        float selisih = buffer[i] - rataRata;

        //  Mengkuadratkan selisih agar nilai negatif
        // tidak saling menghilangkan dengan nilai positif.
        totalKuadratSelisih += selisih * selisih;
    }

    // Menghitung varians populasi dengan membagi jumlah
    //  kuadrat selisih dengan jumlah sampel.
    float varians = totalKuadratSelisih / jumlah;

    // Mengembalikan akar kuadrat varians sebagai simpangan baku.
    return sqrtf(varians);
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

void setup() {
    Serial.begin(115200);
    // dht.begin();
}

void loop() {
    float suhu = bacaSuhuMock(); // dht.readTemperature();

    if (isnan(suhu)) {
        Serial.println("Pembacaan gagal, data diabaikan");
    } else {
        float suhuFilter = hitungMovingAverage(suhu);

        // Menghitung simpangan baku buffer terbaru.
        float simpanganBaku = hitungSimpanganBaku();

        if (suhu < suhuMin) suhuMin = suhu;
        if (suhu > suhuMax) suhuMax = suhu;

        Serial.print("Mentah: ");
        Serial.print(suhu);
        Serial.print(" | Filter: ");
        Serial.print(suhuFilter);

        //  Menampilkan simpangan baku dengan 3 angka desimal.
        Serial.print(" | Simpangan Baku: ");
        Serial.print(simpanganBaku, 3);
        Serial.print(" | Min: ");
        Serial.print(suhuMin);
        Serial.print(" | Max: ");
        Serial.println(suhuMax);
    }

    delay(2000);
}
