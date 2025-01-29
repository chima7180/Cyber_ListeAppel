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