<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Webserv Tester Dashboard</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    <div class="container">
        <header>
            <h1>⚡ Webserv Dashboard</h1>
            <p>Panneau de contrôle et de test des méthodes HTTP</p>
        </header>

        <main class="grid">
            <section class="card get-card">
                <h2>🔵 Test GET (Fichier statique)</h2>
                <div class="control-group">
                    <input type="text" id="getPath" value="/index.html" placeholder="/chemin/fichier.html">
                    <button onclick="sendGet()">Envoyer GET</button>
                </div>
                <div class="preview" id="getPreview">Aperçu ici...</div>
            </section>

            <section class="card post-card">
                <h2>🟢 Test POST (Upload Image/Fichier)</h2>
                <form id="uploadForm">
                    <input type="file" id="fileInput" name="file">
                    <button type="button" onclick="sendPostFile()">Upload Fichier</button>
                </form>
                <div id="postResponse" class="response-box">En attente...</div>
            </section>

            <section class="card cgi-card">
                <h2>🟣 Test CGI (Python/PHP)</h2>
                <div class="control-group">
                    <input type="text" id="cgiInput" placeholder="Tape ton nom...">
                    <button onclick="sendCGI()">Exécuter CGI</button>
                </div>
                <div id="cgiResponse" class="response-box">Résultat CGI...</div>
            </section>

            <section class="card delete-card">
                <h2>🔴 Test DELETE</h2>
                <div class="control-group">
                    <input type="text" id="deletePath" placeholder="/uploads/image.png">
                    <button onclick="sendDelete()">Supprimer</button>
                </div>
                <div id="deleteResponse" class="response-box">Statut...</div>
            </section>
        </main>

        <section class="logs">
            <h3>🖥️ Logs Réponse Serveur (Headers & Status)</h3>
            <pre id="serverLogs">Aucune requête envoyée.</pre>
        </section>
    </div>
    <script src="script.js"></script>
</body>
</html>