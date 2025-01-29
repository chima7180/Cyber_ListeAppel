// Ce programme configure un module ESP32 pour diffuser un signal iBeacon BLE. Il utilise un UUID, 
// un Major ID et un Minor ID uniques pour identifier le périphérique. Le programme définit également 
// la puissance du signal (RSSI) pour simuler un périphérique iBeacon, puis lance la diffusion de ce 
// signal à l'aide des bibliothèques BLE de l'ESP32. Le simulateur reste en fonctionnement pour diffuser 
// en continu les données d'un iBeacon, ce qui permet de simuler la présence d'un badge BLE.

#include <BLEDevice.h>
#include <BLEBeacon.h>

#define BEACON_UUID "12345678-1234-1234-1234-123456789abc" // UUID unique du badge
#define BEACON_MAJOR 1                                     // Major ID
#define BEACON_MINOR 42                                    // Minor ID
#define BEACON_POWER -59                                   // Signal Power (RSSI)

void setup() {
  Serial.begin(115200);
  Serial.println("Initialisation du simulateur iBeacon...");

  // Initialisation BLE
  BLEDevice::init("");

  // Configuration iBeacon
  BLEBeacon beacon;
  beacon.setManufacturerId(0x4C00); // Apple iBeacon
  beacon.setProximityUUID(BLEUUID(BEACON_UUID));
  beacon.setMajor(BEACON_MAJOR);
  beacon.setMinor(BEACON_MINOR);
  beacon.setSignalPower(BEACON_POWER);

  // Configuration des données d'annonce
  BLEAdvertisementData advertisementData;
  advertisementData.setManufacturerData(beacon.getData());
  
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setAdvertisementData(advertisementData);
  pAdvertising->start();

  Serial.println("iBeacon en cours de diffusion...");
}

void loop() {
  // La diffusion est gérée automatiquement
}
