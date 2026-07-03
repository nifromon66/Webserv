#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <vector>
#include <map>

#include <limits>
#include <iostream>
#include <utility> // Pour std::pair

// Stocke les directives spécifiques à un chemin d'URL
class LocationConfig
{
	public:

		LocationConfig();
		LocationConfig(const LocationConfig &copy);
		LocationConfig &operator=(const LocationConfig &other);
		~LocationConfig();

		// --- Directives de Configuration  ---
		std::string							path;								// Le chemin d'URI (ex: "/upload")
		std::string							root;								// Chemin racine sur le système de fichiers (ex: "/var/www/html")
		std::vector<std::string>			allowed_methods;					// Méthodes autorisées (ex: "GET", "POST")
		bool								autoindex;							// Activation de l'indexation automatique des répertoires (true/false)
		std::vector<std::string>			index_files;						// Fichiers par défaut à chercher (ex: "index.html")
		size_t								client_max_body_size;				// Taille max du corps (en octets, size_t::max() par défaut)
		size_t								client_max_header_size;				// Taille max du header
		std::pair<int, std::string>			redirection;						// Redirection (Code, URL)
		std::string							upload_dir;							// Répertoire de stockage pour les requêtes POST/PUT
		std::map<std::string, std::string>	cgi_executors;						// Mappage Extension -> Exécutable CGI
		std::map<int, std::string>			error_pages;						// Mappage Code d'erreur -> Chemin du fichier .html

		std::string							return_url;							// Pour stocker "/new-page.html"
		int									return_code;						// Pour stocker 301

};
#endif