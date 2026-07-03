#!/bin/bash

# ==========================================
# CONFIGURATION
# ==========================================
URL="http://localhost:8080"
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

pause() {
    echo -e "${BLUE}>>> Appuyez sur [ENTRÉE] pour continuer...${NC}"
    read
    echo "-----------------------------------------------------"
}

# ==========================================
# 0. NETTOYAGE
# ==========================================
clear
echo -e "${GREEN}=== PRÉPARATION ===${NC}"
mkdir -p www/html www/uploads www/html/post_body www/cgi-bin
rm -f www/uploads/test_upload.txt
rm -f www/html/post_body/big.txt
echo -e "Environnement prêt.\n"

# ==========================================
# 1. TESTS GET
# ==========================================
echo -e "${GREEN}=== TEST 1: GET Simple (Index) ===${NC}"
echo "Attendu : HTTP/1.1 200 OK"
echo -e "${BLUE}Réponse Serveur (Headers uniquement) :${NC}"
# -s : Silence (pas de barre de progression)
# -o /dev/null : On jette le corps de la page
# 2>&1 | grep "<" : On ne garde que les lignes de réponse du serveur
curl -s -v -o /dev/null "$URL/index.html" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 2: GET Dossier SANS slash (Redirection) ===${NC}"
echo "Attendu : 301 Moved Permanently (Location: /directory/)"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -s -v -o /dev/null "$URL/directory" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 3: GET Inexistant (404) ===${NC}"
echo "Attendu : 404 Not Found"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -s -v -o /dev/null "$URL/existe_pas.html" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 4: Methode inconnue (BLABLA) ===${NC}"
echo "Attendu : 400 Bad Request"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -s -v -X BLABLA -o /dev/null "$URL/" 2>&1 | grep "<"
pause

# ==========================================
# 2. TESTS POST
# ==========================================
echo -e "${GREEN}=== TEST 5: POST Upload (Création) ===${NC}"
echo "Attendu : 201 Created"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -s -v -o /dev/null -X POST -d "Ceci est un test" "$URL/uploads/test_upload.txt" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 6: Vérification Upload (GET) ===${NC}"
echo "Attendu : 200 OK + Contenu 'Ceci est un test'"
echo -e "${BLUE}Contenu du fichier téléchargé :${NC}"
# Ici on ne met pas -v, on veut juste voir le texte
curl -s "$URL/uploads/test_upload.txt"
echo "" 
pause

echo -e "${GREEN}=== TEST 7: POST Trop Gros (413) ===${NC}"
echo "Attendu : 413 Content Too Large"
echo -e "${BLUE}Réponse Serveur :${NC}"
head -c 150 /dev/zero | curl -v -o /dev/null -X POST --data-binary @- "$URL/post_body/big.txt" 2>&1 | grep "<"
pause

# ==========================================
# 3. TESTS DELETE
# ==========================================
echo -e "${GREEN}=== TEST 8: DELETE Fichier (Succès) ===${NC}"
echo "Attendu : 204 No Content"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -v -o /dev/null -X DELETE "$URL/uploads/test_upload.txt" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 9: Vérification Suppression (404) ===${NC}"
echo "Attendu : 404 Not Found"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -v -o /dev/null "$URL/uploads/test_upload.txt" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 10: DELETE Dossier (Interdit) ===${NC}"
echo "Attendu : 403 Forbidden"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -v -o /dev/null -X DELETE "$URL/uploads/" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 11: Méthode Interdite (405) ===${NC}"
echo "Attendu : 405 Method Not Allowed"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -v -o /dev/null -X DELETE "$URL/index.html" 2>&1 | grep "<"
pause

# ==========================================
# 4. TESTS CGI
# ==========================================
echo -e "${GREEN}=== TEST 12: CGI GET ===${NC}"
echo "Attendu : 200 OK"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -s -v -X GET -o /dev/null "$URL/cgi-bin/hello.py" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 13: CGI POST (Formulaire) ===${NC}"
echo "Attendu : 200 OK "
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -s -v -X POST \
  -H "Content-Type: application/x-www-form-urlencoded" \
  --data "data=Bonjour" \
  -o /dev/null \
  "$URL/cgi-bin/test_formulaire.py" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 14: CGI Crash (502) ===${NC}"
echo "Attendu : 502 Bad Gateway"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -v -s -o /dev/null "$URL/cgi-bin/crash.py" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== TEST 15: CGI Timeout (504) ===${NC}"
echo "Attendu : 504 Gateway Timeout (Patientez...)"
echo -e "${BLUE}Réponse Serveur :${NC}"
curl -v -s -o /dev/null "$URL/cgi-bin/infinite_loop.py" 2>&1 | grep "<"
pause

echo -e "${GREEN}=== FIN DES TESTS ===${NC}"