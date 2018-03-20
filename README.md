<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/fr/"><img alt="Licence Creative Commons" style="border-width:0" src="https://i.creativecommons.org/l/by-nc-sa/3.0/fr/88x31.png" /></a><br />Les différentes partie de ce projet sont mise à disposition selon les termes de la <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/fr/">Licence Creative Commons Attribution - Pas d’Utilisation Commerciale - Partage dans les Mêmes Conditions 3.0 France</a>.

---

Projet de réveil interactif faite par des collégiens de 3ème durant le semaine de découverte du Labex Persyval-Lab

---
# Configuration
- Installez le logiciel arduino [lien](https://www.arduino.cc/en/Main/Software)
- Branchez le reveil à un ordinateur
- Dans arduino choisissez le port COM (Menu -> Outils -> Port)
- Ouvrez le moniteur série (la petite loupe à gauche)
- Dans la nouvelle fenêtre en bas à droite, changez 9600 baud par 115200 baud
- Avec un équipement wifi (ordinateur, téléphone, tablette) connectez vous au réseau reveiliot
- Ouvrez un navigateur à l'adresse http://192.168.4.1/
- Suivez les instructions pour configurer le wifi. 
- Une fois effectué, regardez dans le moniteur série Arduino et trouvez une ligne "IP address: 192.168.X.X" (Exemple 192.168.1.100)
- Si elle n'apparait pas, fermez et réouvrez le moniteur et appuyez sur la porte de la maison
- Retenez cette adresse.
- Vous pouvez désormais configurer le réveil en allant à l'adresse http://192.168.X.X/ depuis un équipement connecté au réseau du domicile 


# Installation
- Installer le logiciel arduino [lien](https://www.arduino.cc/en/Main/Software)
- Installer le plugin pour la carte Wemos D1 mini (ESP8266). Suivre l'étape 1 du tutoriel suivant : [lien](https://projetsdiy.fr/programmer-esp8266-ide-arduino-librairies-gpio-web-serveur-client/)
- Ajouter les bibliotèques suivantes (suivre ce tutoriel [lien](https://www.arduino.cc/en/Guide/Libraries))
  - EEPROM
  - Adafruit NeoPixel
  - ArduinoJson
  - Bounce2
  - WifiManager

# Dessin SVG
La maquette de la maison est basée sur le modèle de [elzekool](https://www.thingiverse.com/elzekool)
https://www.thingiverse.com/thing:1078585
