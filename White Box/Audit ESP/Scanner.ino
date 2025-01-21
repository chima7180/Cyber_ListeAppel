// Ce programme utilise un module ESP32 pour scanner les périphériques Bluetooth Low Energy (BLE) 
// à proximité, principalement pour détecter les iBeacons. Le scan se fait pendant une durée 
// déterminée (3 secondes) et filtre les périphériques en fonction de la puissance du signal RSSI 
// (seuil de -70 dBm). Lorsqu'un iBeacon est détecté, il affiche l'adresse MAC et le RSSI du périphérique 
// sur le moniteur série. Ce code utilise les bibliothèques BLE pour initialiser un scan BLE et traiter 
// les résultats à l'aide d'un callback personnalisé.

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

// Configuration
#define SCAN_TIME_ENTREE_SORTIE 3 // Durée du scan pour Entrée/Sortie (en secondes)
#define RSSI_THRESHOLD -70       // Seuil RSSI pour détecter les périphériques proches

BLEScan *pBLEScan;

// Callback personnalisé pour gérer les périphériques détectés
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Vérifie si le périphérique a des données fabricant et si le RSSI est suffisant
    if (advertisedDevice.haveManufacturerData() && advertisedDevice.getRSSI() >= RSSI_THRESHOLD) {
      String manufacturerData = advertisedDevice.getManufacturerData();

      // Vérifie si c'est un iBeacon (fabricant Apple par exemple, 0x4C00)
      if (manufacturerData.length() >= 25 && 
          manufacturerData[0] == 0x4C && manufacturerData[1] == 0x00) { 
        Serial.println("iBeacon détecté à proximité !");

        // Adresse MAC
        Serial.print("Adresse MAC : ");
        Serial.println(advertisedDevice.getAddress().toString());

        // RSSI
        Serial.print("RSSI : ");
        Serial.println(advertisedDevice.getRSSI());
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du scan BLE pour Entrée/Sortie...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Crée le scanner BLE
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); // Active le scan actif pour plus de détails
}

void loop() {
  Serial.println("Début du scan Entrée/Sortie...");
  pBLEScan->start(SCAN_TIME_ENTREE_SORTIE, false); // Lance le scan BLE
  pBLEScan->clearResults();                       // Libère la mémoire
  delay(2000);                                    // Pause pour éviter les répétitions immédiates
}
