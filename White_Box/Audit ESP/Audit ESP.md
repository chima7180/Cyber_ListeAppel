# Audit de Sécurité et de Bonnes Pratiques - Code ESP32

## 1. Résumé Exécutif

Ce rapport présente une analyse du code ESP32 utilisé dans le projet de liste d'appel par détection Bluetooth Low Energy (BLE). L'objectif est d'identifier les faiblesses de sécurité, d'évaluer la robustesse du système et d'optimiser les performances tout en appliquant des bonnes pratiques en terme de développement.

Les principaux points à améliorer concernent la protection des données sensibles (identifiants Wi-Fi/MQTT en clair, absence de chiffrement des communications), la fiabilité des connexions MQTT et Wi-Fi, ainsi que l'optimisation de la gestion des scans BLE et des ressources mémoire. Les recommandations seront classées par importance de mon point de vue en fonction de la sécurité et la stabilité du système.

---

## 2. Contexte

Cet audit est réalisé en mode White Box, avec un accès complet au code source et aux infrastructures du groupe travaillant sur l'ESP32. L'objectif du groupe ESP32 est de concevoir un système de détection des entrées et sorties et d'un détecteur de présence des étudiants dans une salle de classe à l'aide de balises Bluetooth Low Energy (BLE) et d'un réseau MQTT pour transmettre ces informations vers un serveur central. Ce système repose sur l'utilisation de microcontrôleurs ESP32 pour scanner les périphériques BLE et envoyer les données collectées via Wi-Fi.

L’audit vise à identifier les risques potentiels et à formuler des recommandations pour renforcer la sécurité et la performance du système. Cependant, le code n’a pas pu être testé dans des conditions réelles malgré plusieurs tentatives de mise en œuvre. Cette limitation a empêché une évaluation complète de la stabilité en environnement opérationnel.

### Critères d'évaluation :
- **Sécurité** : Protection des identifiants et chiffrement des communications.
- **Fiabilité** : Gestion des connexions Wi-Fi/MQTT et prévention des blocages.
- **Performance** : Optimisation du scan BLE et limitation de la consommation de ressources.
- **Bonnes pratiques** : Organisation du code et maintenabilité.

---

## 3. Analyse de Sécurité _(Critique)_

### 🔒 Stockage des Identifiants en Clair

Le code suivant montre comment les identifiants Wi-Fi et MQTT sont stockés en dur dans le programme :

```cpp
const char* ssid = "votre_SSID";
const char* password = "votre_MotDePasse";
const char* mqttUsername = "admin";
const char* mqttPassword = "mot_de_passe";
````

Cela expose le système à des risques en cas de fuite de code ou d'accès non autorisé.  
**Recommandation** : Utiliser des fichiers de configuration externes (ex: SPIFFS) ou des variables d'environnement.

---

### 🔐 Absence de Chiffrement des Communications

Le protocole MQTT est utilisé en mode non sécurisé, comme illustré dans le code ci-dessous :

```cpp
client.setServer(mqttServer, mqttPort);
client.connect("ESP32Client", mqttUsername, mqttPassword);
```

Cela expose les données à une interception sur le réseau.  
**Recommandation** : Passer à **MQTTS** (MQTT avec TLS) pour garantir la confidentialité des données.

---

### 🛡️ Exposition des Adresses MAC

Les adresses MAC des badges et des ESP32 sont transmises en clair :

```cpp
String macAddress = WiFi.macAddress();
Serial.println("MAC Wi-Fi : " + macAddress);
```

Cela peut poser des problèmes en termes de vie privée.  
**Recommandation** : Si nécessaire, anonymiser ces données ou les chiffrer avant transmission.

---

## 4. Fiabilité et Robustesse _(Important)_

### 🌐 Gestion des Connexions Wi-Fi et MQTT

Le code actuel tente de reconnecter le Wi-Fi en cas de perte de connexion mais ne gère pas un timeout prolongé :

```cpp
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
}
```

C'est un problème courant dans les systèmes embarqués comme l'ESP32 est le blocage dû à une erreur logicielle ou à une perte prolongée de connexion. Pour éviter cela, l'utilisation d'un **watchdog** est essentielle. Un **watchdog** est un timer matériel ou logiciel qui redémarre l'ESP32 automatiquement en cas de dysfonctionnement ou de boucle infinie. Il agit comme une mesure de sécurité pour garantir que l'appareil ne reste pas bloqué indéfiniment.

Voici un exemple d'implémentation d'un watchdog logiciel sur l'ESP32 :

```cpp
#include <esp_task_wdt.h>

void setup() {
    esp_task_wdt_init(5, true); // Initialisation du watchdog avec un timeout de 5 secondes
    esp_task_wdt_add(NULL); // Ajout de la tâche principale au watchdog
}

void loop() {
    esp_task_wdt_reset(); // Réinitialisation du watchdog à chaque itération pour éviter un redémarrage
    delay(1000); // Simulation d'une tâche
}
```

**Recommandation** : Ajouter un watchdog pour redémarrer automatiquement l'ESP32 en cas de blocage prolongé.

---

### 📊 Vérification de l'état du Broker MQTT

Le programme ne vérifie pas si MQTT est connecté avant chaque envoi de message :

```cpp
client.publish(topic.c_str(), jsonBuffer);
```

**Recommandation** : Ajouter une vérification avant chaque publication pour éviter les pertes de données.

```cpp
if (client.connected()) {
    client.publish(topic.c_str(), jsonBuffer);
}
```

---

## 5. Performance et Optimisation _(Amélioration Possible)_

### **Observation**

#### 1. Fréquence de Scan

La fréquence actuelle de 2 secondes entre chaque scan BLE est un **bon compromis** pour la taille des messages et la gestion des périphériques dans une salle de TP. À cette fréquence, chaque ESP32 peut scanner tous les périphériques dans la salle sans saturer le réseau MQTT. Cette durée permet également de publier des messages sans trop de délai, tout en offrant suffisamment de temps pour détecter les périphériques sans perte significative.

```cpp
pBLEScan->start(2, false); // Scan Entrée/Sortie durant 2 secondes

void loop() {
    delay(2000); // Boucle toutes les 2 secondes
}
```

---

#### 2. Calcul des Messages JSON

Pour chaque périphérique détecté, les données envoyées sont les suivantes :

- **UUID** : 36 caractères
- **MAC Address** : 17 caractères
- **Major et Minor** : 4 octets
- **macBLE** : 17 caractères
- **macWIFI** : 17 caractères

En total, chaque périphérique occupe environ 60 octets. Si nous ajoutons un petit overhead pour la structure JSON (les clés et la sérialisation), chaque périphérique consommera environ **80 à 100 octets**.

Le `StaticJsonDocument<512>` utilisé pour sérialiser les données permet de stocker environ **5 à 6 périphériques** dans un seul message JSON avant d'atteindre la limite de taille de 512 octets. Cela permet de détecter et envoyer des données pour plusieurs périphériques à chaque cycle de scan.

---

#### Estimation du Nombre de Périphériques à Scanner

La capacité de chaque ESP32 à scanner les périphériques est influencée par la durée du scan et la taille du message. Actuellement, avec une **durée de scan de 2 secondes**, chaque ESP32 peut envoyer environ **5 à 6 périphériques par message**. Avec un maximum d'une quinzaine d'étudiants, le système peut donc facilement gérer la détection de tous les périphériques en quelques cycles de scan. Chaque scan dure suffisamment longtemps pour capturer les périphériques sans saturer le réseau ni trop retarder les détections suivantes.

---

### **Optimisation des Messages MQTT**

Même si l'architecture actuelle est adaptée au projet tel qu'il est prévu, quelques optimisations sur la structure des messages pourraient améliorer la gestion du réseau (en économisant de la bande passante) et la performance à long terme.

Une optimisation des messages est essentielle pour garantir une efficacité maximale. En voici quelques suggestions :

#### **Réduire la taille des messages JSON**

Si certaines informations comme `macBLE` et `macWIFI` sont identiques pour tous les périphériques d'un même ESP32, il peut être utile de ne les envoyer qu'une seule fois, plutôt que de les inclure dans chaque périphérique détecté. Par exemple :

- Envoyer un message global par ESP32 avec `macBLE` et `macWIFI` et ensuite envoyer uniquement les données des périphériques.

---

#### **Évaluation de la nécessité de la compression**

Étant donné que le nombre de périphériques dans la salle est d'environ une quinzaine, et que la taille des messages JSON n'est pas excessivement grande dans ce contexte, **la compression n'est pas essentielle** pour l'instant. Cependant, elle pourrait offrir des avantages en termes de réduction de la latence et de la charge réseau.

##### - **Avantages d'implémenter la compression** :

- Si le système devait évoluer pour supporter plus de périphériques ou pour améliorer la réactivité dans une configuration avec de nombreux capteurs.
- Si des problèmes de congestion réseau se manifestent lors des tests.
- Si la gestion de l'espace mémoire devient un enjeu, notamment si plusieurs messages doivent être stockés simultanément.


##### - **Inconvénients** :


- La mise en œuvre de la compression ajouterait une complexité supplémentaire, notamment la gestion des erreurs de compression et la nécessité de décompresser les messages du côté récepteur.
- Pour un nombre limité de périphériques (17 max), la surcharge de compression/décompression pourrait ne pas justifier les gains en efficacité.


---

## 6. Bonnes Pratiques et Modularité du Code

Une meilleure organisation du code ESP32 permettrait d'améliorer la lisibilité et la maintenance.

### Organisation recommandée du code ESP :

- **main.ino** → Fichier principal contenant `setup()` et `loop()`.
- **wifi_manager.h** → Gestion de la connexion Wi-Fi.
- **mqtt_manager.h** → Gestion des connexions et publications MQTT.
- **ble_manager.h** → Gestion des scans BLE.

---

## 7. Recommandations Finales

|Priorité|Recommandation|
|---|---|
|Critique|Stocker les identifiants Wi-Fi/MQTT de manière sécurisée.|
|Critique|Activer MQTTS pour chiffrer les communications.|
|Important|Ajouter un watchdog pour éviter les blocages.|
|Amélioration|Optimiser la taille des messages JSON envoyés.|

---

## 8. Annexes (Code & Références)

- Liens vers les codes sources
	- [Code Capteur Entrée-Sortie](Présence.ino)
	- [Code Capteur de présence](Entrée-Sortie.ino)
