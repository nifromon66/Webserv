#!/usr/bin/python3
import sys

# 1. Les Headers
# On indique au serveur (et au navigateur) que l'on envoie du HTML
print("Content-Type: text/html")

# 2. La Ligne Vide
# Indique la fin des headers et le début du body
print()

# 3. Le Body
# Le contenu HTML réel
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>Test CGI</title></head>")
print("<body>")
print("<h1>Hello World</h1>")
print("<p>Ceci est généré par un script Python.</p>")
print("</body>")
print("</html>")