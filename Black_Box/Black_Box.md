# Position en Black Box

En position Black Box, nous disposont d'aucune information concernant le contexte et le système.
On va structurer notre approche a travers les deux seuls technologies qui puissent etre atteinte de l'exterieur de la salle : 
- Le WiFI
- Le Bluetooth

---

## WIFI

### Étape 1 : Reconnaissance du réseau Wi-Fi

L'attaquant commence par une phase d'observation et de collecte d'informations.

Il va dans un premier temps faire scan des réseaux Wi-Fi à proximité :

#### Action : 
En utilisant des outils simples comme Airodump-ng ou des applications gratuites sur smartphone, il va pouvoir identifier les SSID des réseaux, leur type de sécurité (WPA2/WPA3), les canaux utilisés et les appareils connectés.

#### Objectif :
Le but est d'identifier le réseau Wi-Fi cible puis de repérer les appareils connectés (Raspberry Pi, ESP32, etc.).

#### Limites :
Si le réseau est masqué ou utilise WPA3, il sera difficile d’en tirer beaucoup d'informations. 

---

Il va aussi chercher à détecter des requêtes probe :
Une requête probe est un message envoyé par un appareil pour rechercher un réseau Wi-Fi spécifique ou annoncer qu'il est prêt à se connecter.
Ces requêtes peuvent contenir :
- Le SSID (nom du réseau Wi-Fi) recherché.
- L’adresse MAC de l’appareil.
- Des informations sur les capacités de l’appareil (types de cryptage pris en charge, fréquence, etc.).

#### Action :
Il va surveiller les requêtes probe (des appareils cherchant à se connecter au réseau).

#### Objectif : 
Il veut pouvoir identifier les appareils qui communiquent avec le Wi-Fi (par exemple, les ESP32 ou smartphones des élèves).

#### Outils possibles : Wireshark, Bettercap, ou tout outil d’analyse réseau gratuit.

--- 

### Étape 2 : Tentatives d’attaques possibles

#### Surcharge du Wi-Fi (attaque de déni de service - DoS)

##### Principe :

L'attaquant envoie un grand nombre de requêtes ou de paquets pour saturer le point d'accès Wi-Fi, ce qui entraîne des déconnexions ou un arrêt temporaire du service.

##### Méthodes possibles :

##### Flood de paquets de désauthentification :
Envoie de paquets désauthentifiant les appareils connectés au réseau.

Outil : Aireplay-ng.

Exemple :
```
aireplay-ng --deauth 100 -a [MAC_point_d'accès] wlan0
```

##### Flood de paquets ping :
Saturation du point d'accès via un outil comme hping3.

##### Limites:
L'attaquant doit avoir le matériel nécessaire tel qu'une carte réseau prenant en charge le mode monitor.
Et si le réseau utilise WPA3 ou des protections comme PMF (Protected Management Frames), cette attaque peut être inefficace.

##### Impact potentiel :
Interruption temporaire du système, car les ESP32 et Raspberry Pi risquent de perdre la connexion.
Faible probabilité de détection de l’attaque si l’infrastructure ne surveille pas le trafic.

--- 

#### Usurpation d’un point d’accès (Evil Twin)

##### Principe :

L'attaquant crée un faux point d'accès avec le même SSID que le réseau cible.
Les appareils légitimes (ou les élèves) peuvent accidentellement se connecter au faux point d’accès.

Outil : Airbase-ng ou des outils comme WiFi Pineapple.

Exemple :
```
airbase-ng -e "Nom_du_WiFi_cible" -c 6 wlan0
```

##### Objectif :
L'attaquant veut capturer les identifiants Wi-Fi ou intercepter les données échangées pour simuler une détection incorrecte des élèves en manipulant les données.

##### Limites :
Cela nécessite des ressources matérielles supplémentaires (une antenne Wi-Fi puissante).
Et si le système utilise un chiffrement fort (WPA2/WPA3), cela limite l’accès aux données sensibles.

---

#### Brute force sur la clé Wi-Fi

##### Principe :
L'attaquant capture un handshake (échange initial entre un appareil et le point d'accès) pour essayer de casser la clé Wi-Fi.

Outil : Aircrack-ng, hashcat.

Exemple :

Capture du handshake :
```
airodump-ng --channel 6 --bssid [MAC_point_d'accès] -w capture wlan0
```

Brute force :
```
aircrack-ng -w wordlist.txt -b [MAC_point_d'accès] capture.cap
```

#### Limites:
Il est nécessaire d'avoir un dictionnaire de mots de passe ou beaucoup de puissance de calcul (surtout contre WPA2/WPA3).
Deplus, si la clé Wi-Fi est robuste (aléatoire et longue), l’attaque échouera probablement.

---

### Conclusion Wi-Fi

Un attaquant avec peu de ressources pourrait réussir :

- Une attaque de déni de service (DoS) pour perturber temporairement le fonctionnement.
- Un Evil Twin si les utilisateurs ou les appareils se connectent naïvement à son point d'accès.
- Le brute force serait difficile et peu probable contre un mot de passe robuste.

---

## Bluetooth

### Étape 1 : Reconnaissance des appareils Bluetooth

Comme pour le WiFi l'attaquant comment par une phase d'observation et va faire un scan des appareils Bluetooth et des services BLE.


#### Action : 
Utilisation d’un scanner Bluetooth pour détecter les appareils actifs dans la salle (ESP32, smartphones, etc.).

#### Outils :

Il va pouvoiur utiliser hcitool :
```
hcitool scan
```
Ou encore Bettercap ou Bluetoothctl pour une analyse plus poussée.

#### Objectif :
Il va vouloir identifier les appareils actifs et collecter leurs adresses MAC et, si possible, leurs UUID (services exposés).

---

#### Détection des services BLE :

#### Action : 
Exploration des services exposés sur les appareils BLE (comme les ESP32).

#### Outil : 
gatttool ou des scripts Python BLE.

Exemple :
```
gatttool -b [MAC_address] --characteristics
```

---

### Étape 2 : Attaques possibles

#### Exploitation de services ouverts

##### Principe :
Si l’ESP32 expose des services BLE sans protection (authentification ou chiffrement), l’attaquant peut y accéder pour lire ou écrire des données.
Exemple : Modifier les informations de détection ou envoyer de fausses données.

##### Outils : 
BLEAH, gatttool.

##### Limites :
Nécessite que le système BLE soit mal configuré.
Si les services sont protégés par chiffrement, l’attaque échouera.

#### Attaque par force brute sur l’appairage

##### Principe :
L'attaquant tente de casser le code PIN lors de l’appairage.

##### Outils : Btcrack, scripts d’automatisation.

##### Limites :
Long et complexe contre un PIN robuste.
Inefficace si le système utilise le mode "Secure Connections Only".

#### Attaque de spoofing BLE

##### Principe :
L’attaquant usurpe l’identité d’un appareil Bluetooth légitime pour se connecter au système.

##### Outils : 
Bettercap, BLEAH.

##### Limites:
Si l’appareil cible utilise un chiffrement ou des UUID dynamiques, cela devient quasi impossible.

---

### Conclusion Bluetooth
Un attaquant avec peu de ressources pourrait :
- Accéder à des services BLE ouverts ou mal configurés.
- Perturber les communications via un déni de service BLE.
- Les attaques plus complexes comme le spoofing ou le cracking du PIN nécessitent davantage de connaissances et de matériel.

---

## Résumé final

### Wi-Fi : 
Les attaques réalistes seraient :
- Le déni de service (DoS) pour perturber le système.
- Une tentative d’attaque Evil Twin, mais avec un succès limité si des protections existent.

### Bluetooth : 
Les attaques réalistes seraient :
- L’accès à des services BLE mal protégés (si présents).
- Les attaques avancées seraient trop complexes à réaliser sans ressources importantes.
