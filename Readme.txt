# 🌐 Optimisation d'un Réseau LPWAN pour la Télésurveillance Médicale en Zone Rurale

## 📌 Présentation du projet

Ce projet consiste à concevoir et simuler une architecture réseau **LPWAN (Low Power Wide Area Network)** basée sur la technologie **LoRaWAN**, destinée à la télésurveillance médicale dans les zones rurales à faible couverture réseau.

L'étude prend comme scénario de déploiement la commune rurale de **Samé Kanta (Sédhiou, Sénégal)**, sur une superficie de 7 km × 7 km.

L'objectif est d'évaluer les performances du réseau en fonction du nombre de patients connectés, tout en étudiant la fiabilité des transmissions de données médicales.

## 🎯 Objectifs

- Concevoir une architecture LoRaWAN adaptée aux zones rurales enclavées.
- Simuler la communication entre des bracelets médicaux connectés et des passerelles LoRaWAN.
- Évaluer l'impact du nombre de patients sur les performances du réseau.
- Mesurer le taux d'erreur paquet (PER).
- Étudier l'adaptation du Spreading Factor (SF) selon la distance aux passerelles.
- Contribuer à la réflexion sur des solutions de télésurveillance médicale économes en énergie.

## 🏗️ Architecture de simulation

Le scénario repose sur les composants suivants :

| Composant | Configuration |
|---|---|
| Zone de déploiement | 7 km × 7 km |
| Patients (End Devices) | 100 par défaut |
| Passerelles LoRaWAN | 2 |
| Technologie | LoRaWAN |
| Classe des dispositifs | Classe A |
| Durée de simulation | 3600 secondes |
| Période d'émission | 30 secondes |
| Taille des paquets | 6 octets |
| Modèle de propagation | Log-Distance Path Loss |
| Exposant de perte | 3.0 |
| Spreading Factor | SF7 à SF12 |

Les patients sont représentés par des nœuds terminaux fixes, répartis aléatoirement dans la zone de simulation.

Chaque dispositif transmet périodiquement des données biomédicales simulées :
- Fréquence cardiaque (4 octets).
- Température corporelle (2 octets).

## 🛠️ Technologies utilisées

- **C++** : développement du programme de simulation.
- **NS-3** : simulateur de réseaux informatiques.
- **Module LoRaWAN pour NS-3** : modélisation des communications LoRaWAN.
- **Propagation Models** : simulation des pertes de propagation radio.
- **Energy Module** : module NS-3 disponible pour la modélisation énergétique.

## ⚙️ Fonctionnement du système

Le programme suit huit étapes principales :

1. Configuration du modèle de propagation radio.
2. Initialisation des helpers PHY et MAC LoRaWAN.
3. Création et positionnement des nœuds patients.
4. Création et positionnement des passerelles.
5. Configuration adaptative du Spreading Factor.
6. Génération du trafic médical périodique.
7. Exécution de la simulation.
8. Collecte et analyse des résultats.

## 📊 Indicateur de performance

Le principal indicateur étudié est le **Packet Error Rate (PER)**, qui permet d'évaluer la proportion de paquets non reçus.

La formule utilisée est :

\[
PER = \frac{P_{envoyés}-P_{reçus}}{P_{envoyés}}\times100
\]

Avec :
- \(P_{envoyés}\) : nombre total de paquets transmis.
- \(P_{reçus}\) : nombre total de paquets reçus.

Un PER faible indique une meilleure fiabilité des communications.

## 🚀 Installation et exécution

### Prérequis

- Un environnement Linux recommandé.
- NS-3 installé et configuré.
- Le module LoRaWAN compatible avec la version de NS-3 utilisée.
- Un compilateur C++ compatible.

### 1. Préparer le projet

Placer le fichier source `same-kanta-health.cc` dans le dossier `scratch` de NS-3.

### 2. Compiler NS-3

Depuis le répertoire principal de NS-3 :

```bash
./ns3 build
```

### 3. Lancer la simulation

Pour exécuter la simulation avec les paramètres par défaut :

```bash
./ns3 run "scratch/same-kanta-health"
```

Pour simuler 150 patients :

```bash
./ns3 run "scratch/same-kanta-health --nPatients=150"
```

Pour modifier la durée de simulation :

```bash
./ns3 run "scratch/same-kanta-health --simTime=7200"
```

Les paramètres disponibles en ligne de commande sont :
- `nPatients` : nombre de patients.
- `nGateways` : nombre de passerelles configuré.
- `simTime` : durée de simulation en secondes.

**Remarque :** les positions des passerelles sont définies explicitement pour deux passerelles dans le code actuel. Une adaptation du positionnement est nécessaire pour modifier leur nombre.

## 📈 Résultats attendus

À la fin de l'exécution, le programme affiche :

- Le nombre total de paquets envoyés.
- Le nombre total de paquets reçus.
- Le taux d'erreur paquet (PER) en pourcentage.

Ces résultats permettent d'étudier la fiabilité du réseau selon la densité des dispositifs connectés.

## 🔮 Perspectives d'amélioration

Plusieurs évolutions peuvent être envisagées :

- Intégration d'un modèle de Machine Learning pour optimiser dynamiquement les paramètres réseau.
- Analyse approfondie de la consommation énergétique des dispositifs.
- Comparaison des performances selon différents scénarios de déploiement.
- Développement d'un tableau de bord pour visualiser les indicateurs réseau.
- Extension de la simulation à des scénarios médicaux plus complexes.

## 👩‍💻 Auteur

**Racky Sakho SOW**

Étudiante en Master Intelligence Artificielle  
Université Iba Der Thiam de Thiès – Sénégal

GitHub : [@hadjasakho06-cmd](https://github.com/hadjasakho06-cmd)

---

*Projet universitaire – Réseaux sans fil, IoT et Intelligence Artificielle.*