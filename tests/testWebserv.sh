#!/usr/bin/env bash

WEBSERV_BIN=./webserv
CONF_BAD_DIR=config/bad
CONF_GOOD_DIR=config/good


# couleurs
GREEN="\033[32m"
RED="\033[31m"
RESET="\033[0m"

if [ ! -x "$WEBSERV_BIN" ]; then
	echo -e "${RED}Erreur : binaire $WEBSERV_BIN introuvable ou non exécutable.${RESET}"
	exit 1
fi

if [ ! -d "$CONF_BAD_DIR" ]; then
	echo -e "${RED}Erreur : dossier $CONF_BAD_DIR introuvable.${RESET}"
	exit 1
fi

echo -e "=== Tests de configs ERRONÉES ==="

for conf in "$CONF_BAD_DIR"/*.conf; do
	[ -e "$conf" ] || continue

	test_name=$(basename "$conf")

	timeout 2s "$WEBSERV_BIN" "$conf" >/dev/null 2>&1
	status=$?

	if [ $status -eq 0 ] || [ $status -eq 124 ]; then
		# 0  -> il a accepté la conf et s’est lancé correctement
		# 124 -> timeout, donc il tourne dans server.run => conf acceptée
		echo -e "$test_name : ${RED}❌${RESET}"
	else
		# exit != 0 et != 124 -> il a détecté une erreur de conf
		echo -e "$test_name : ${GREEN}✅${RESET}"
	fi
done

echo -e "\n${YELLOW}=== Tests de configs VALABLES (doivent RÉUSSIR) ===${RESET}"

for conf in "$CONF_GOOD_DIR"/*.conf; do
	[ -e "$conf" ] || continue

	test_name=$(basename "$conf")

	timeout 2s "$WEBSERV_BIN" "$conf" >/dev/null 2>&1
	status=$?

	if [ $status -eq 0 ] || [ $status -eq 124 ]; then
		# 0  = exit propre
		# 124 = timeout → le serveur tourne → c'est ce qu'on veut
		echo -e "$test_name : ${GREEN}✅${RESET}"
	else
		echo -e "$test_name : ${RED}❌ (échec alors qu'elle est bonne)${RESET}"
	fi
done