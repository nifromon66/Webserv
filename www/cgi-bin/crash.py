#!/usr/bin/env python3
import sys

# Division par zéro -> Crash
a = 1 / 0

print("Content-Type: text/html\r\n\r\n")
print("<h1>Je vne serai jamais affiche...</h1>")

# on devrait avoir une erreur 500 - bad cgi output   
