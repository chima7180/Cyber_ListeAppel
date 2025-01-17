# Position en Grey Box  

Dans cette position et ce contexte, nous sommes des étudiants disposant de certains accès et informations, qui sont les suivants :  
- Accès au réseau WiFi de la salle.  
- Connaissance de l'infrastructure de la classe :  
  - Adresses IP des équipements.  
  - Liste des équipements.  
  - Configuration des VLAN.  
  - Serveurs disponibles et leurs accès.  

---

## Première approche  

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
3. Automatiser des tentatives de connexion ou de récupération d’informations.  


### Conclusion de cette première approche


---
Man, on a deja des infos et des acces Wifi,

peut on recup des donnés? des trames , des id ect..