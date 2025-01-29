# Audit des Modules ESP32 (Simulateur et Scanner BLE)

## Introduction

Ce rapport présente l'analyse des modules ESP32 dédiés à la simulation et au scanning Bluetooth Low Energy (BLE). L'objectif est d'assurer une conception robuste, efficace et adaptée au scénario d'entrée/sortie basé sur la proximité, tout en appliquant des principes de sécurité et d'optimisation des ressources.


Contexte : Le système est destiné à gérer les entrées et sorties d'étudiants dans une salle via des dispositifs Bluetooth Low Energy (BLE). Pour cela, deux modules ESP32 sont utilisés : un simulateur pour représenter les badges BLE (les adresses @MAC des ESP32 serviront de badges pour simplifier) et un scanner pour détecter ces badges à proximité de la porte.

---

## Analyse du Code Simulateur

(Cf. fichier Simulateur.ino)

### Fonctionnalités Principales
Le module simulateur ESP32 diffuse un signal iBeacon avec les paramètres essentiels :
- **UUID unique** : Identifie chaque périphérique de manière univoque.
- **Major et Minor ID** : Permettent de différencier des groupes ou des instances.
- **RSSI (Puissance du Signal)** : Définit la force du signal.

### Points Forts
1. **Minimalisme et efficacité** : Le code se concentre uniquement sur les paramètres nécessaires à la diffusion.
2. **Compatibilité** : L'utilisation du standard iBeacon garantit une reconnaissance universelle par les scanners compatibles BLE.
3. **Stabilité** : La diffusion périodique est gérée automatiquement par la bibliothèque BLE.

### Recommandations
- **Documenter les limites** : Le simulateur peut être affecté par les interférences ou les obstacles physiques.
- **Tests avancés** : Vérifier les performances dans des environnements complexes avec plusieurs périphériques BLE.

---

## Analyse du Code Scanner

(Cf. fichier Scanner.ino)

### Fonctionnalités Principales
Le scanner ESP32 détecte les périphériques BLE en fonction de leur proximité et de leurs caractéristiques spécifiques :
- **Filtrage par RSSI** : Seuls les périphériques proches (RSSI > -70) sont pris en compte, équivalent à 2 mètres environ.
- **Reconnaissance des iBeacons** : Identifie les appareils compatibles avec les normes Apple iBeacon.

### Points Forts
1. **Filtrage efficace** : Limitation des périphériques détectés à ceux réellement proches, correspondant à un scénario passant la porte Entrée/Sortie.
2. **Logique modulaire** : Le callback personnalisé facilite l'ajout ou la modification des critères de détection.
3. **Performances optimisées** : Nettoyage des résultats après chaque scan pour économiser la mémoire.

### Recommandations
- **Sécurité des données** : Ajouter des mécanismes pour empêcher la reconnaissance de périphériques spoofés.
- **Amélioration du seuil RSSI** : Tester différentes valeurs pour ajuster la sensibilité du scanner selon les environnements.

---

## Recommandations Générales

### Sécurité
- **Chiffrement des données** : Prévoir un chiffrement des communications lors de l'intégration avec MQTT et le Raspberry Pi.

- **Authentification des périphériques** : Ajouter un mécanisme pour vérifier que seuls les simulateurs autorisés peuvent être détectés. 
	- Pour cela, le scanner pourrait inclure un mécanisme d'authentification ou de filtrage avancé, par exemple :
		- **UUID autorisé** : Configurer le scanner pour qu'il ne détecte que les périphériques dont l'UUID correspond à une liste approuvée.
		- WhiteList : Implémenter une base de données locale ou distante avec les adresses MAC autorisées.

### Performances
- **Calibration RSSI** : Réaliser des mesures sur terrain pour ajuster les seuils selon l'environnement.
- **Tests de robustesse** : Simuler des scénarios avec plusieurs simulateurs pour identifier les limites du scanner.
