/**
 * Description :
 * Ce script simule un réseau LoRaWAN déployé sur la commune
 * rurale de Samé Kanta (7km x 7km) pour la télésurveillance
 * médicale de patients équipés de bracelets connectés.
 * Il mesure le Taux d'Erreur Paquet (PER) selon le nombre
 * de patients simulés (50, 100, 150 ou 200).
 * ============================================================
 */

// ─── Inclusion des modules NS-3 nécessaires ────────────────────────────────
#include "ns3/core-module.h"        // Fonctions de base NS-3 (temps, logs, cmd)
#include "ns3/mobility-module.h"    // Modèles de mobilité et placement des nœuds
#include "ns3/lorawan-module.h"     // Module LoRaWAN (PHY, MAC, helpers)
#include "ns3/network-module.h"     // Conteneurs de nœuds et devices réseau
#include "ns3/energy-module.h"      // Modèle de consommation énergétique
#include "ns3/propagation-loss-model.h" // Modèles de propagation radio

// ─── Espaces de noms ──────────────────────────────────────────────────────
using namespace ns3;          // Espace de noms général NS-3
using namespace lorawan;      // Espace de noms du module LoRaWAN
using namespace ns3::energy;  // Espace de noms du module énergie

// ─── Composant de journalisation (logs) ──────────────────────────────────
// Permet d'afficher les messages de simulation avec NS_LOG_INFO()
NS_LOG_COMPONENT_DEFINE("SameKantaHealth");


// ============================================================
//                       PROGRAMME PRINCIPAL
// ============================================================
int main(int argc, char* argv[]) {

    // ─── Paramètres configurables de la simulation ────────────────────────
    // Ces valeurs peuvent être modifiées en ligne de commande
    // Ex : ./ns3 run "same-kanta-health --nPatients=150"
    int    nPatients   = 100;   // Nombre de patients (nœuds terminaux)
    int    nGateways   = 2;     // Nombre de passerelles LoRaWAN
    double simTime     = 3600;  // Durée de la simulation en secondes (1 heure)
    double pathLossExp = 3.0;   // Exposant d'affaiblissement n (végétation dense)

    // ─── Lecture des arguments en ligne de commande ───────────────────────
    CommandLine cmd;
    cmd.AddValue("nPatients", "Nombre de patients",    nPatients);
    cmd.AddValue("nGateways", "Nombre de passerelles", nGateways);
    cmd.AddValue("simTime",   "Duree simulation (s)",  simTime);
    cmd.Parse(argc, argv);

    // ─── Activation des logs de simulation ───────────────────────────────
    LogComponentEnable("SameKantaHealth", LOG_LEVEL_INFO);

    // ─── Affichage des paramètres de simulation ───────────────────────────
    NS_LOG_INFO("=== Simulation LoRaWAN - Same Kanta, Sedhiou ===");
    NS_LOG_INFO("Patients    : " << nPatients);
    NS_LOG_INFO("Passerelles : " << nGateways);
    NS_LOG_INFO("Zone        : 7 km x 7 km");


    // =========================================================
    // ÉTAPE 1 : MODÈLE DE PROPAGATION RADIO
    // Modèle Log-Distance Path Loss
    // PL(d) = PL(d0) + 10*n*log10(d/d0) + Xσ
    // =========================================================

    // Création du modèle Log-Distance avec les paramètres de Samé Kanta
    Ptr<LogDistancePropagationLossModel> logDist =
        CreateObject<LogDistancePropagationLossModel>();

    // Exposant d'affaiblissement n = 3.0 (végétation dense casamançaise)
    logDist->SetAttribute("Exponent", DoubleValue(pathLossExp));

    // Distance de référence d0 = 1000 m (1 km)
    logDist->SetAttribute("ReferenceDistance", DoubleValue(1000.0));

    // Pertes à la distance de référence PL(d0) = 127.41 dB
    // Calculé via le modèle de Friis à 868 MHz pour d0 = 1 km
    logDist->SetAttribute("ReferenceLoss", DoubleValue(127.41));

    // Modèle de délai de propagation à vitesse constante (vitesse de la lumière)
    Ptr<PropagationDelayModel> delay =
        CreateObject<ConstantSpeedPropagationDelayModel>();

    // Création du canal LoRa avec les modèles de pertes et de délai
    Ptr<LoraChannel> channel = CreateObject<LoraChannel>(logDist, delay);


    // =========================================================
    // ÉTAPE 2 : CONFIGURATION DES HELPERS LoRaWAN
    // Les helpers facilitent la configuration des couches PHY et MAC
    // =========================================================

    LoraPhyHelper    phyHelper;  // Helper pour la couche physique (PHY)
    LorawanMacHelper macHelper;  // Helper pour la couche d'accès (MAC)
    LoraHelper       helper;     // Helper général du module LoRaWAN

    // Association du canal LoRa à la couche physique
    phyHelper.SetChannel(channel);

    // Activation du tracker de paquets pour mesurer le PER
    // Le tracker enregistre chaque paquet émis et reçu
    helper.EnablePacketTracking();


    // =========================================================
    // ÉTAPE 3 : CRÉATION ET PLACEMENT DES NŒUDS PATIENTS
    // (End Devices — Bracelets connectés)
    // =========================================================

    // Création du conteneur de nœuds pour les patients
    NodeContainer endDevices;
    endDevices.Create(nPatients); // 100 patients par défaut

    // Configuration de la mobilité : placement aléatoire uniforme
    // sur la zone 7km x 7km de Samé Kanta
    MobilityHelper mobility;
    mobility.SetPositionAllocator(
        "ns3::RandomRectanglePositionAllocator",
        "X", StringValue("ns3::UniformRandomVariable[Min=0|Max=7000]"), // X ∈ [0, 7000 m]
        "Y", StringValue("ns3::UniformRandomVariable[Min=0|Max=7000]")  // Y ∈ [0, 7000 m]
    );

    // Les patients sont fixes (bracelets portés, pas de déplacement simulé)
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(endDevices);

    // Configuration de la couche PHY pour les End Devices
    phyHelper.SetDeviceType(LoraPhyHelper::ED);

    // Configuration de la couche MAC en Classe A (mode le plus économe)
    // Classe A : le nœud ouvre deux fenêtres de réception après chaque émission
    macHelper.SetDeviceType(LorawanMacHelper::ED_A);

    // Installation de la pile LoRaWAN sur les nœuds patients
    // Retourne un conteneur de NetDevice pour les End Devices
    NetDeviceContainer endDevicesND = helper.Install(phyHelper, macHelper, endDevices);


    // =========================================================
    // ÉTAPE 4 : CRÉATION ET PLACEMENT DES PASSERELLES
    // (Gateways — Infrastructure réseau)
    // =========================================================

    // Création du conteneur de nœuds pour les passerelles
    NodeContainer gateways;
    gateways.Create(nGateways); // 2 passerelles

    // Positionnement des passerelles en quinconce sur la zone
    // pour maximiser la couverture des 49 km²
    Ptr<ListPositionAllocator> gwPos = CreateObject<ListPositionAllocator>();
    gwPos->Add(Vector(1750, 3500, 15)); // Passerelle 1 : quart gauche, hauteur 15m
    gwPos->Add(Vector(5250, 3500, 15)); // Passerelle 2 : quart droit, hauteur 15m

    // Configuration de la mobilité des passerelles (fixes sur les toits)
    MobilityHelper gwMobility;
    gwMobility.SetPositionAllocator(gwPos);
    gwMobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    gwMobility.Install(gateways);

    // Configuration de la couche PHY pour les passerelles
    phyHelper.SetDeviceType(LoraPhyHelper::GW);

    // Configuration de la couche MAC pour les passerelles
    macHelper.SetDeviceType(LorawanMacHelper::GW);

    // Installation de la pile LoRaWAN sur les passerelles
    helper.Install(phyHelper, macHelper, gateways);


    // =========================================================
    // ÉTAPE 5 : CONFIGURATION ADAPTATIVE DU SPREADING FACTOR
    // Mécanisme ADR (Adaptive Data Rate)
    // Assigne automatiquement SF7-SF12 selon la distance au GW
    // =========================================================

    // SF7 (portée ~2km) → patients proches des passerelles
    // SF12 (portée ~15km) → patients éloignés des passerelles
    macHelper.SetSpreadingFactorsUp(endDevices, gateways, channel);


    // =========================================================
    // ÉTAPE 6 : GÉNÉRATION DU TRAFIC MÉDICAL
    // Simulation des données biomédicales envoyées périodiquement
    // Rythme cardiaque (4 bytes) + Température (2 bytes) = 6 bytes
    // =========================================================

    PeriodicSenderHelper senderHelper;

    // Un paquet toutes les 30 secondes (surveillance continue non intrusive)
    senderHelper.SetPeriod(Seconds(30));

    // Taille du paquet : 6 bytes (4 FC + 2 Temp)
    senderHelper.SetPacketSize(6);

    // Installation de l'application sur tous les patients
    ApplicationContainer apps = senderHelper.Install(endDevices);

    // Démarrage des transmissions à t=1s (après initialisation réseau)
    apps.Start(Seconds(1.0));

    // Arrêt des transmissions à la fin de la simulation (3600s = 1h)
    apps.Stop(Seconds(simTime));


    // =========================================================
    // ÉTAPE 7 : LANCEMENT DE LA SIMULATION
    // =========================================================

    NS_LOG_INFO("Demarrage de la simulation...");

    // Définition de la durée totale de la simulation
    Simulator::Stop(Seconds(simTime));

    // Exécution de la simulation (événements discrets)
    Simulator::Run();

    // Libération des ressources après la simulation
    Simulator::Destroy();


    // =========================================================
    // ÉTAPE 8 : COLLECTE ET AFFICHAGE DES RÉSULTATS
    // Calcul du PER (Packet Error Rate)
    // PER = (Paquets envoyés - Paquets reçus) / Paquets envoyés × 100
    // =========================================================

    // Récupération du tracker de paquets
    LoraPacketTracker& tracker = helper.GetPacketTracker();

    // Comptage global des paquets sur toute la durée de simulation
    // Retourne une chaîne "paquets_envoyes paquets_recus"
    std::string results = tracker.CountMacPacketsGlobally(
        Seconds(0), Seconds(simTime));

    NS_LOG_INFO("Comptage paquets (envoyes recus) : " << results);

    // Extraction des valeurs depuis la chaîne de résultats
    std::istringstream iss(results);
    double sent = 0, received = 0;
    iss >> sent >> received;

    // Calcul du PER (Packet Error Rate) en pourcentage
    double per = 0.0;
    if (sent > 0) per = ((sent - received) / sent) * 100.0;

    // ─── Affichage final des indicateurs de performance ───────────────────
    NS_LOG_INFO("========== RESULTATS ==========");
    NS_LOG_INFO("Paquets envoyes    : " << sent);
    NS_LOG_INFO("Paquets recus      : " << received);
    NS_LOG_INFO("PER                : " << per << " %");
    NS_LOG_INFO("================================");

    return 0;
}