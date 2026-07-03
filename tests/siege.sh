#!/bin/bash

# Avant de lancer ce script, augmente la limite de fichiers ouverts si nécessaire :ulimit -n 4096
# Pour exécuter ce script, assurez-vous que votre serveur web est en cours d'exécution sur localhost:8080 et lancer ./tests/siege.sh dans votre terminal.

# ==========================================
# CONFIGURATION
# ==========================================
# URL="http://localhost:8080"
URL="http://127.0.0.1:8080"
TIME="10S"          # Durée du test (10 secondes)
CONCURRENT="25"     # Nombre d'utilisateurs simultanés (threads)
LOG_FILE="siege_output.log"

# Couleurs
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Vérification que siege est installé
if ! command -v siege &> /dev/null; then
    echo -e "${RED}Erreur: siege n'est pas installé.${NC}"
    echo "Sur Linux : sudo apt install siege"
    echo "Sur Mac (Brew) : brew install siege"
    exit 1
fi
# Fonction de nettoyage
cleanup() {
    echo -e "\n${BLUE}Nettoyage des fichiers temporaires...${NC}"
    rm -f $LOG_FILE
    # Siege crée parfois un fichier siege.log dans le dossier courant ou home
    rm -f siege.log
}
trap cleanup EXIT

clear
echo -e "${GREEN}=== WEBSERV STRESS TEST (SIEGE) ===${NC}"
echo -e "Cible      : ${YELLOW}$URL${NC}"
echo -e "Durée      : ${YELLOW}$TIME${NC}"
echo -e "Clients    : ${YELLOW}$CONCURRENT${NC}"
echo -e "----------------------------------------"
echo -e "${BLUE}Assurez-vous que votre serveur est lancé !${NC}"
echo -e "Appuyez sur [ENTRÉE] pour démarrer le bombardement..."
read

# ==========================================
# TEST 1 : GET SIMPLE (Index)
# ==========================================
echo -e "\n${GREEN}>>> TEST 1 : Pages Statiques (GET /)${NC}"
echo "Ce test vérifie si le serveur tient la charge sur des fichiers simples."

# -b : Benchmark mode (pas de délai entre les requêtes)
# -c : Concurrent users
# -t : Time
# -q : Quiet (masque la liste des URLS pour n'afficher que le résumé)
siege -b -c $CONCURRENT -t $TIME "$URL/" 

# ==========================================
# TEST 2 : GET FICHIER VIDE (Vitesse Pure)
# ==========================================
echo -e "\n${GREEN}>>> TEST 2 : Fichier Vide/Petit (GET /directory/)${NC}"
echo "Ce test vérifie la rapidité de réponse sur des petits contenus."
siege -b -c $CONCURRENT -t $TIME "$URL/directory/"

# ==========================================
# TEST 3 : TESTS STRESS
# ==========================================
# Attention : Sur MacOS, la limite de file descriptors est souvent basse (256).
# Si tu mets -c 1000, ton terminal va planter, pas forcément ton serveur.
echo -e "\n${RED}>>> TEST 3 : STRESS TEST (50 Users)${NC}"
echo "On double la charge..."
siege -b -c 50 -t 5S "$URL/"

# ==========================================
# TEST 4 : EXTREME STRESS
# ==========================================
echo -e "\n${RED}>>> TEST 4 : STRESS TEST (100 Users)${NC}"
echo "100 users..."
siege -b -c 100 -t 10S "$URL/"

echo -e "\n${GREEN}=== FIN DES TESTS ===${NC}"
echo "Vérifiez que 'Availability' est toujours à 100%."