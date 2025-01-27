# Position en Grey Box  

Dans cette position et ce contexte, nous sommes des étudiants disposant de certains accès et informations, qui sont les suivants :  
- Accès au réseau WiFi de la salle.  
- Connaissance de l'infrastructure de la classe :  
  - Adresses IP des équipements.  
  - Liste des équipements.  
  - Configuration des VLAN.  
  - Serveurs disponibles et leurs accès.  

---

## Première approche  le WIFI

Pour une première approche, nous avons adopté une stratégie visant à collecter un maximum d’informations sur les hôtes disponibles sur le réseau WiFi.  
Pour ce faire, nous avons utilisé l’outil **Nmap**, qui permet de réaliser une cartographie réseau et d’identifier les services actifs sur les équipements connectés.
Le but pour nous ici est de savoir si il est simple ou non de reperer les dispositifs (Raspeberry Pi et ESP32) sur le wifi et de les identifier. Le but ici est de comprendre comment cela fonctionne et faire un script qui me donnera les adresses IP rechercher ainsi que certaine information comme les ports ouverts.


Voici une version corrigée et améliorée de ton texte :  

---

### Test et compréhension de Nmap  

#### Exploration de base  
La première étape consiste à comprendre les commandes de base. Nous avons testé la commande suivante pour lister toutes les adresses IP présentes sur le réseau WiFi de la salle :  

```bash
chima@PcInge:~$ nmap -sn 198.18.16.0/22
Starting Nmap 7.80 ( https://nmap.org ) at 2025-01-16 15:29 CET
Nmap scan report for susie.wlan.stri (198.18.16.1)
Host is up (0.0028s latency).
Nmap scan report for unifi.stri (198.18.16.2)
Host is up (0.091s latency).
[...]
Nmap scan report for DESKTOP-EM8PVLL (198.18.16.64)
Host is up (0.0081s latency).
[...]
Nmap done: 1024 IP addresses (18 hosts up) scanned in 14.14 seconds
```

#### Observations :  
Grâce à cette commande, nous avons obtenu plusieurs informations utiles :  
- Les noms des appareils sur le réseau (ex. : `susie.wlan.stri`).  
- Les adresses IP correspondantes.  
- La latence de chaque hôte.  
- Le nombre total d’appareils actifs sur le réseau (18 dans cet exemple).  

---

#### Scan de ports sur un appareil spécifique  
Ensuite, nous avons effectué un scan de ports sur un appareil précis, en l’occurrence la Raspberry Pi présente dans la salle :  

```bash
chima@PcInge:~$ sudo nmap -sS -sV 198.18.16.70
Starting Nmap 7.80 ( https://nmap.org ) at 2025-01-16 15:18 CET
Nmap scan report for 198.18.16.70
Host is up (0.0075s latency).
Not shown: 998 closed ports
PORT   STATE SERVICE    VERSION
22/tcp open  ssh        OpenSSH 9.2p1 Debian 2+deb12u4 (protocol 2.0)
53/tcp open  tcpwrapped
Service Info: OS: Linux; CPE: cpe:/o:linux:linux_kernel
```

#### Résultats :  
- Les ports ouverts ont été identifiés :  
  - Port **22/tcp** : Service SSH avec une version spécifique (**OpenSSH 9.2p1**).  
  - Port **53/tcp** : Service identifié comme **tcpwrapped**.  
- Le système d’exploitation détecté : **Linux**.  

Nous avons ainsi appris à cartographier un réseau, à identifier les appareils connectés et à obtenir des informations détaillées sur un hôte, comme les ports ouverts et le système d’exploitation utilisé.  

---

### Script et automatisation  

Dans cette section, nous allons créer un script pour :  
1. Identifier les appareils spécifiques que nous recherchons.  
2. Scanner leurs ports ouverts.  

```bash
#!/bin/bash

# Script pour détecter une Raspberry Pi et un ESP32 sur un réseau local
# Utilise nmap pour scanner et identifier les appareils

# Demander la plage d'adresses IP à scanner
read -p "Entrez la plage IP (ex: 192.168.1.0/24) : " network_range

echo "Scanning le réseau $network_range à la recherche de votre Raspberry Pi et ESP32..."
echo "Cela peut prendre un moment, merci de patienter."

# Effectuer un scan des appareils connectés
scan_results=$(nmap -sn $network_range)

# Détection de la Raspberry Pi ATTENTION LES PARAMS NE SONT PAS BON
raspberry_ip=$(echo "$scan_results" | grep -i "raspberry" -B 2 | grep "Nmap scan report" | awk '{print $5}')

if [ -z "$raspberry_ip" ]; then
    echo "Aucune Raspberry Pi détectée sur le réseau $network_range."
else
    echo "Raspberry Pi détectée avec l'adresse IP : $raspberry_ip"
    # Scanner les ports ouverts et détecter l'OS pour l'adresse IP de la Raspberry Pi
    echo "Analyse des ports ouverts et du système d'exploitation pour $raspberry_ip..."
    nmap -sS -sV -O $raspberry_ip
fi

# Détection de l'ESP32
esp32_ip=$(echo "$scan_results" | grep -iE "ESP|Espressif" -B 2 | grep "Nmap scan report" | awk '{print $5}')

if [ -z "$esp32_ip" ]; then
    echo "Aucun ESP32 détecté sur le réseau $network_range."
else
    echo "ESP32 détecté avec l'adresse IP : $esp32_ip"
    # Scanner les ports ouverts et détecter l'OS pour l'adresse IP de l'ESP32
    echo "Analyse des ports ouverts et du système d'exploitation pour $esp32_ip..."
    nmap -sS -sV -O $esp32_ip
fi

echo "Analyse terminée. Résultats ci-dessus."
```
#### Explication 

Ce script détecte les **Raspberry Pi** et **ESP32** sur un réseau local en utilisant **nmap**. Voici ses étapes :

1. **Demande de la plage IP à scanner** :
   - L'utilisateur entre une plage d'adresses IP (ex. : `192.168.1.0/24`).

2. **Scan des appareils actifs** :
   - `nmap -sn` identifie les appareils connectés dans la plage IP donnée.

3. **Recherche d'une Raspberry Pi** :
   - Le script filtre les appareils dont le nom ou le système contient "x". A MODIFIER
   - Si trouvé, il effectue un scan des ports ouverts et du système d'exploitation.

4. **Recherche d'un ESP32** :
   - Similaire à la Raspberry Pi, mais en recherchant "ESP" ou "Espressif".

5. **Affichage des résultats** :
   - Les IP des appareils détectés, ainsi que leurs ports et OS, sont affichés.

6. **Fin du script** :
   - Le script termine en affichant les résultats complets.

---

### Utilisation de ces informations 

#### Node-Red 
Pour compromettre une Raspberry Pi exposant Node-RED sans mot de passe :

1. **Accès à l'interface graphique** : 
   - Par défaut, Node-RED est accessible via un navigateur web à l'adresse `http://<IP_Raspberry>:1880`.
   - Si aucune authentification n'est configurée (ce qui est souvent le cas avec une installation basique), l'interface est librement accessible à quiconque connaît l'adresse IP de la Raspberry Pi.

2. **Modifications malveillantes** :
   - L'attaquant peut facilement modifier ou ajouter des flux dans Node-RED. Par exemple, il pourrait configurer un flux pour capturer des données sensibles, déployer des scripts malveillants, ou effectuer des actions non désirées sur d'autres appareils du réseau.

**Conclusion** : Une installation de base de Node-RED sans mot de passe rend la Raspberry Pi vulnérable à des accès non autorisés, particulièrement dans un environnement réseau partagé comme c'est le cas ici.

###.....
a continuer


### Conclusion de cette première approche

Cela fonctionne pour le Wi-Fi, mais si la Raspberry Pi est connectée en LAN, le processus restera similaire. Cependant, l'attaquant devra être présent sur le réseau LAN. Les méthodes d'attaque pour la Raspberry Pi restent identiques, mais les ESP32 ne seront plus accessibles de cette manière.

Notre conclusion est la suivante : si la Raspberry Pi est connectée au LAN et génère un réseau Wi-Fi avec un mot de passe fort et utilise un protocole de sécurisation tel que WPA3, les risques d’attaque sont considérablement réduits. De plus, un autre facteur limitant pour l’attaquant est qu’il ne connaît ni le fonctionnement interne de la Raspberry Pi ni les services qu’elle utilise.


## Seconde approche le Bluethooth 

Pour une première approche avec Bluetooth, nous avons adopté une stratégie visant à collecter un maximum d'informations sur les appareils Bluetooth environnants.
Nous avons utilisé des outils comme hcitool et bluetoothctl, qui permettent de scanner les appareils à portée et d’identifier les périphériques actifs.

Le but ici est de savoir s'il est simple ou non de repérer les dispositifs Bluetooth tels que les ESP32 et les beacons diffusés par les téléphones des élèves, tout en comprenant leur fonctionnement. Nous avons également cherché à automatiser cette détection à l’aide d’un script pour identifier les appareils souhaités et extraire des informations utiles comme leurs adresses MAC et, dans certains cas, leurs noms ou UUID.

### Exploration de base  
La première étape consiste à comprendre les commandes de base. Nous avons testé la commande suivante pour lister toutes les adresses appareils bluethooth environnant avec leur MAC:

```bash
chima@raspi:~ $ hcitool scan
Scanning ...
        38:68:93:60:30:B9       CHIMAPC
        9E:F3:46:D1:61:2A       BL5000
```
Cette commande nous permet de voir les appareils bluethooth visible ainsi que leur mac adress et leur nom.

Nous faisons la meme chose pour le BLE (Bluethooth low energy) :

```bash
chima@raspi:~ $ sudo hcitool lescan
LE Scan ...
C4:F3:77:A3:2C:88 Modulo2
01:70:14:6D:AC:1F (unknown)
EC:81:93:2B:38:8C
94:7B:AE:66:BF:51 (unknown)
0D:89:55:A7:34:AA (unknown)
C4:F3:77:A3:2C:88 (unknown)
7B:B8:26:8E:04:E9 (unknown)
FF:5C:55:2A:D2:B7 (unknown)
EC:81:93:2B:38:8C (unknown)
C7:06:4A:DC:5A:43 (unknown)
4F:3A:C9:3E:FC:76 (unknown)
4E:FA:E5:0D:E3:29 (unknown)
5D:E2:3D:97:C4:73 (unknown)
FC:E1:21:51:B1:57 (unknown)
4E:FA:E5:0D:E3:29 (unknown)
```
Avec cette commande, nous pouvons voir tout les appareils envionnant grace a leur mac et leur nom si configurer 

### Script et automatisation  

Dans cette section, nous allons créer un script pour :  
- Identifier les appareils spécifiques que nous recherchons ESP et beacons.  


```bash
#!/bin/bash

# Activation de l'adaptateur Bluetooth
echo "Activation de l'adaptateur Bluetooth..."
sudo hciconfig hci0 up

# Scanner les appareils BLE
echo "Recherche des appareils BLE à proximité. Patientez..."
ble_scan_results=$(sudo hcitool lescan --duplicates 2>/dev/null)

# Filtrer les ESP32 A MODIF
echo "Recherche des ESP32..."
esp32_devices=$(echo "$ble_scan_results" | grep -i "ESP")

# Filtrer les beacons A MODIF
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
```
Ce script utilise les outils hciconfig, hcitool, et des commandes classiques comme grep et awk pour détecter et filtrer les appareils BLE. Il automatise la recherche des ESP32 et beacons dans l’environnement Bluetooth.
L’objectif principal est de permettre une identification rapide des périphériques pertinents, en rendant les résultats exploitables pour des analyses ou des actions ultérieures.

### Utilisation de ces informations 

#### Déni de Service et Spoofing pour Usurper un Capteur de Présence

Dans le cadre de notre système de gestion de présence, une attaque combinant **déni de service (DoS)** et **spoofing** pourrait permettre à un attaquant de se faire passer pour un capteur Bluetooth légitime.L’objectif serait de simuler une présence en cours sans y etre présent.

---

##### Étapes de l'Attaque

1. **Déni de Service sur le Capteur Légitime :**
   - L'attaquant provoque une surcharge ou perturbe les communications du capteur légitime en émettant un grand nombre de paquets malformés ou en utilisant des outils pour occuper le canal Bluetooth.
   - Cela rend le capteur incapable de communiquer avec le système central (Raspberry Pi).

2. **Spoofing pour Usurper le Capteur :**
   - L'attaquant configure un appareil Bluetooth (comme un smartphone ou un autre ESP32) pour imiter les caractéristiques du capteur légitime, notamment son adresse MAC, son nom et ses services (mqtt).
   - L’appareil usurpé émet des signaux que le système interprète comme provenant du capteur légitime.

3. **Simulation de Présence :**
   - En diffusant des données identiques à celles attendues par le système (comme un identifiant unique), l'attaquant trompe le système en lui faisant croire qu’il est physiquement présent dans la salle.

---

##### Contre-Mesures pour se Protéger

1. **Authentification Renforcée :**
   - Implémenter des protocoles d'authentification Bluetooth sécurisés (ex. **Bluetooth LE Secure Connections**) pour valider les appareils connectés.
   - Associer chaque capteur à une clé cryptographique unique pour empêcher l’usurpation.

2. **Rotation des Identifiants :**
   - Configurer les capteurs pour qu’ils changent régulièrement leur adresse MAC et autres identifiants, ce qui complique le spoofing.
   - Ajouter un système de vérification dynamique qui s’assure que l’appareil répond correctement à des défis sécurisés.

3. **Signalisation d’Absence de Communication :**
   - Configurer le système pour signaler les périodes prolongées sans réponse du capteur légitime, ce qui permet de détecter les attaques de DoS rapidement.

4. **Un password complexe sur le wifi :**
   - Meme si l'attaquant usurpe l'esp, celui-ci n'ayant pas le mot de passe du réseau wifi permettant l'échange d'information sur la présence celui-ci devient bloqué. 

---

#### Attaque par Usurpation à l'aide d'une Liste Blanche

Une autre attaque efficace contre un système basé sur une **whitelist** (liste blanche) consiste à exploiter le manque de validation rigoureuse des identifiants Bluetooth pour simuler la présence d'utilisateurs autorisés. L'attaque s'appuie sur la création de faux beacons via des applications spécifiques sur un téléphone portable, permettant de répliquer les informations des dispositifs légitimes.

---

##### Étapes de l'Attaque

1. **Création de Faux Beacons :**
   - À l'aide d'applications comme **Beacon Simulator** (Android) ou **LightBlue** (iOS), l'attaquant configure un ou plusieurs beacons simulés.
   - Ces faux beacons émettent des informations copiées depuis des dispositifs autorisés (par exemple, via une observation préalable).

2. **Simulation des Identifiants Autorisés :**
   - L'attaquant configure les beacons pour imiter les **adresses MAC Bluetooth**, les **UUID**, et les **minor/major values** des appareils inscrits dans la liste blanche.
   - Si l'attaquant connaît les identifiants de plusieurs amis, il peut simuler leur présence en même temps.

3. **Diffusion des Données Trompeuses :**
   - Les faux beacons émettent les informations attendues par le système, qui les interprète comme provenant d'appareils légitimes inscrits dans la liste blanche.

4. **Bypass du Système de Présence :**
   - Le système enregistre les faux beacons comme des utilisateurs légitimes et marque ces derniers comme présents.

---


##### Contre-Mesures pour se Protéger

1. **Validation Renforcée des Identifiants :**
   - Associer chaque appareil autorisé à une **clé cryptographique unique** ou à un certificat.

2. **Détection des Anomalies :**
   - Analyser le comportement des beacons pour repérer des activités suspectes, comme des émissions simultanées depuis une même source physique.

3. **Rotation des Identifiants :**
   - Modifier régulièrement les UUID et autres identifiants des appareils inscrits dans la liste blanche, ce qui rend les informations précédemment collectées obsolètes.

---

### Conclusion

L’attaque via un système de whitelist exploite la confiance excessive dans des identifiants statiques et non sécurisés. Pour protéger le système, il est impératif d'introduire des mécanismes de validation plus robustes et de surveiller activement les comportements anormaux des appareils Bluetooth.

### Conclusion de cette partie 

Le **Bluetooth** représente probablement la zone la plus vulnérable dans le système, principalement en raison de la facilité avec laquelle un attaquant peut exploiter ses faiblesses. Il est en effet simple de :

- **Se faire passer pour un ESP32** : En imitant les identifiants uniques (adresse MAC, UUID, etc.) d'un dispositif légitime, un attaquant peut se faire passer pour un capteur de présence ou un autre appareil autorisé.
- **Créer de faux beacons** : À l'aide d'applications accessibles au grand public, il est possible de simuler plusieurs beacons en émettant des données modifiées, telles que l’adresse MAC ou les UUID de ses amis, ce qui rend ces derniers virtuellement présents dans le système.

Cette simplicité d'attaque souligne l'importance de trouver des petits stratagèmes pour empêcher cela, comme l'utilisation d'un UUID changeant et la vérification des distances.


## Conclusion 

SSID caché la conclusion