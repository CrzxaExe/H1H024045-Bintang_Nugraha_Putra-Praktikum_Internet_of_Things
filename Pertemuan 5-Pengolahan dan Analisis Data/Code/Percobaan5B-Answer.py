
import json
import time
import paho.mqtt.client as mqtt
import pandas as pd
import matplotlib.pyplot as plt

BROKER = "broker.hivemq.com"
PORT = 1883
TOPIC = "test/sensor"  # Topic data ESP32
JUMLAH_SAMPEL = 30      # Jumlah data yang dikumpulkan

# Menentukan ukuran jendela moving average.
WINDOW_SIZE = 5

data = []

def on_connect(client, userdata, flags, reason_code, properties):
    print("Terhubung ke broker, kode:", reason_code)
    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        suhu = float(payload["suhu"])
    except (ValueError, KeyError):
        print("Data tidak valid, diabaikan")
        return

    data.append({
        "waktu": time.strftime("%Y-%m-%d %H:%M:%S"),
        "suhu": suhu
    })
    print(f"[{len(data)}/{JUMLAH_SAMPEL}] suhu = {suhu}")

    if len(data) >= JUMLAH_SAMPEL:
        client.disconnect()

# -- Tahap 1: akuisisi data dari MQTT
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.connect(BROKER, PORT)
client.loop_forever()

# -- Tahap 2: simpan ke CSV
df = pd.DataFrame(data)
df.to_csv("data_suhu.csv", index=False)
print("Data tersimpan di data_suhu.csv")

# Membaca kembali data dari file CSV.
df = pd.read_csv("data_suhu.csv")

# Menghitung moving average dengan jendela 5 data.
# min_periods=1 berarti perhitungan tetap dilakukan saat data
# yang tersedia di awal belum mencapai 5 sampel.
df["suhu_ma"] = df["suhu"].rolling(
    window=WINDOW_SIZE,
    min_periods=1
).mean()

# -- Tahap 3: deteksi anomali menggunakan z-score

# METODE 1: z-score berdasarkan suhu mentah.
rata2 = df["suhu"].mean()
std = df["suhu"].std()

if std == 0:
    df["zscore_mentah"] = 0.0
else:
    df["zscore_mentah"] = (df["suhu"] - rata2) / std

df["anomali_mentah"] = df["zscore_mentah"].abs() > 2

# Menghitung rata-rata data hasil moving average.
rata2_ma = df["suhu_ma"].mean()

# Menghitung simpangan baku hasil moving average.
std_ma = df["suhu_ma"].std()

# Menghitung z-score dari data hasil moving average.
if pd.isna(std_ma) or std_ma == 0:
    df["zscore_ma"] = 0.0
else:
    df["zscore_ma"] = (df["suhu_ma"] - rata2_ma) / std_ma

# Menentukan anomali berdasarkan z-score moving average.
df["anomali_ma"] = df["zscore_ma"].abs() > 2

# Menghitung jumlah anomali dari kedua metode.
jumlah_anomali_mentah = int(df["anomali_mentah"].sum())
jumlah_anomali_ma = int(df["anomali_ma"].sum())

# Menghitung selisih jumlah anomali kedua metode.
selisih_anomali = jumlah_anomali_mentah - jumlah_anomali_ma

# Menyimpan hasil pemrosesan ke CSV baru.
df.to_csv("hasil_analisis_suhu.csv", index=False)

print("\n=== Statistik suhu mentah ===")
print(f"Rata-rata       : {rata2:.2f}")
print(f"Std deviasi     : {std:.2f}")
print(f"Minimum         : {df['suhu'].min():.2f}")
print(f"Maksimum        : {df['suhu'].max():.2f}")
print(f"Jumlah anomali  : {jumlah_anomali_mentah}")

# Menampilkan statistik dan hasil metode moving average.
print("\n=== Statistik suhu moving average ===")
print(f"Rata-rata       : {rata2_ma:.2f}")
print(f"Std deviasi     : {std_ma:.2f}")
print(f"Jumlah anomali  : {jumlah_anomali_ma}")

# Menampilkan perbandingan hasil deteksi anomali.
print("\n=== Perbandingan deteksi anomali ===")
print(f"Anomali suhu mentah      : {jumlah_anomali_mentah}")
print(f"Anomali setelah MA (5)   : {jumlah_anomali_ma}")

if selisih_anomali > 0:
    print(f"Anomali berkurang        : {selisih_anomali}")
elif selisih_anomali < 0:
    print(f"Anomali bertambah        : {abs(selisih_anomali)}")
else:
    print("Jumlah anomali sama pada kedua metode.")

print("Hasil lengkap tersimpan di hasil_analisis_suhu.csv")

# -- Tahap 4: visualisasi

plt.figure(figsize=(12, 5))

# Data suhu asli dan hasil moving average.
plt.plot(
    df.index, df["suhu"],
    marker="o", markersize=4,
    label="Suhu mentah", alpha=0.65
)
plt.plot(
    df.index, df["suhu_ma"],
    marker=".", linewidth=2,
    label="Moving average (5)"
)

# Menandai anomali berdasarkan suhu mentah.
anom_mentah = df[df["anomali_mentah"]]
plt.scatter(
    anom_mentah.index, anom_mentah["suhu"],
    color="red", marker="x", s=100,
    label="Anomali suhu mentah", zorder=3
)

# Menandai anomali berdasarkan moving average.
anom_ma = df[df["anomali_ma"]]
plt.scatter(
    anom_ma.index, anom_ma["suhu_ma"],
    color="purple", marker="D", s=60,
    label="Anomali moving average", zorder=4
)

plt.axhline(
    rata2, color="gray", linestyle="--",
    label="Rata-rata suhu mentah"
)

plt.xlabel("Urutan data")
plt.ylabel("Suhu (°C)")
plt.title("Perbandingan Deteksi Anomali Sebelum dan Sesudah Moving Average")
plt.legend()
plt.tight_layout()
plt.savefig("grafik_suhu.png")
plt.show()
