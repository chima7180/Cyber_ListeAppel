#!/bin/bash

# Activation de l'adaptateur Bluetooth
echo "Activation de l'adaptateur Bluetooth..."
sudo hciconfig hci0 up

# Scanner les appareils BLE
echo "Recherche des appareils BLE à proximité. Patientez..."
ble_scan_results=$(sudo hcitool lescan --duplicates 2>/dev/null)

# Filtrer les ESP32
echo "Recherche des ESP32..."
esp32_devices=$(echo "$ble_scan_results" | grep -i "ESP")

# Filtrer les beacons
echo "Recherche des beacons..."
beacon_devices=$(echo "$ble_scan_results" | grep -i "Beacon")

# Affichage des résultats
if [ -n "$esp32_devices" ]; then
    echo "ESP32 détectés :"
    echo "$esp32_devices"
else
    echo "Aucun ESP32 détecté."
fi

if [ -n "$beacon_devices" ]; then
    echo "Beacons détectés :"
    echo "$beacon_devices"
else
    echo "Aucun beacon détecté."
fi

echo "Analyse Bluetooth terminée."
