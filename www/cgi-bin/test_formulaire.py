#!/usr/bin/python3
# import cgi
# import cgitb
import os
import sys
import urllib.parse

# Active le debug pour voir les erreurs dans le navigateur si ça plante
# cgitb.enable()

print("Content-Type: text/html; charset=utf-8")
# print("Connection: close")
print() # Ligne vide obligatoire

print("<!DOCTYPE html>")
print("<html><head><title>Réponse CGI</title></head><body>")
print("<h1>Résultat du CGI Python</h1>")

# --- 1. Récupération des données brutes (Body) ---
try:
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
except (ValueError, TypeError):
    content_length = 0

body = ""
if content_length > 0:
    # On lit exactement le nombre d'octets annoncé
    body = sys.stdin.read(content_length)

print(f"<h3>Données brutes reçues (Body):</h3>")
print(f"<pre>{body}</pre>")

# --- 2. Parsing des données (pour être propre) ---
# Le format est "key=value&key2=value2"
# On utilise une librairie standard pour décoder ça proprement
params = urllib.parse.parse_qs(body)

# On récupère la valeur du champ 'data' de ton HTML
user_input = params.get('data', ['Nothing sent'])[0]

print("<h3>Données traitées :</h3>")
print(f"<p>Tu as écrit : <b>{user_input}</b></p>")

print("<a href=\"/index.html\">Retour au formulaire</a>")
print("</body></html>")