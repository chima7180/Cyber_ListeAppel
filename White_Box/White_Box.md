# Rôle WhiteBox : Audit du Système BLE

Le rôle de l'agent **WhiteBox** est d’auditer le système de Liste d'appel par détecteur Bluetooth Low Energy (BLE) pour évaluer sa robustesse, sa sécurité et son efficacité. En ayant un accès complet au code source, aux configurations et à l’architecture, l'agent WhiteBox identifie les vulnérabilités potentielles, évalue les performances, et propose des recommandations.


## 2. Portée de l'Audit

Cet audit couvre plusieurs aspects techniques du projet, notamment :

- **ESP32** : Analyse du code source des détecteurs BLE et de leur interaction avec le réseau Wi-Fi et MQTT.
    
- **Communications MQTT** : Évaluation de la sécurité et de l'efficacité des transmissions entre l'ESP32 et le serveur central.
    
- **Gestion du BLE** : Optimisation de la détection des badges étudiants et vérification de la gestion des scans.
    
- **Sécurité des données** : Protection des identifiants, chiffrement des communications et gestion des informations personnelles.


## 3. Méthodologie

L'audit suit une approche en plusieurs étapes :

1. **Analyse statique du code** : Lecture du code ESP32 pour identifier les vulnérabilités potentielles.
    
2. **Tests en environnement contrôlé** (Tentatives réalisées sans validation complète en conditions réelles): Exécution du programme sur un ESP32 avec simulation de différents scénarios (perte de connexion, forte charge de scans BLE, interruptions réseau).
    
3. **Vérification des standards de sécurité** : Comparaison avec les bonnes pratiques en matière de sécurisation des communications IoT.
    
4. **Propositions d'optimisation** : Recommandations pour améliorer la sécurité et la performance du code.


## 4. Résultats

Les détails complets des audits sont présentés dans chaque dossier Audit en .md, qui fournit une analyse approfondie des points forts et des vulnérabilités identifiés, ainsi que les recommandations classées par priorité.
