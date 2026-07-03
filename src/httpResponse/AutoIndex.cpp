#include "../../include/httpResponse/AutoIndex.hpp"

AutoIndex::AutoIndex()
{}

AutoIndex::~AutoIndex()
{}

// Génère une page HTML listant les fichiers d'un dossier quand aucun fichier index n'est trouvé.
std::string AutoIndex::buildAutoIndexPage(const std::string& physicalPath, const std::string& requestUri)
{
	DIR* dir = opendir(physicalPath.c_str());
	if (dir == NULL)
		throw std::runtime_error("Could not open directory for autoindex");

	// Préparation du HTML
	std::stringstream html;

	html << "<!DOCTYPE html>\n"
		<< "<html>\n"
		<< "<head><title>Index of " << requestUri << "</title></head>\n"
		<< "<body>\n"
		<< "<h1>Index of " << requestUri << "</h1>\n"
		<< "<hr>\n"
		<< "<ul>\n";

	struct dirent* entry;

	// Lecture des entrées
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;

		// Ignorer le dossier courant '.'
		if (name == ".") 
			continue;

		std::string fullPath = physicalPath;
		if (fullPath[fullPath.length() - 1] != '/') 
			fullPath += "/";
		fullPath += name;

		// bool isDir = false;
		struct stat statbuf;
		if (stat(fullPath.c_str(), &statbuf) == 0)
		{
			if (S_ISDIR(statbuf.st_mode))  // isDir = true;
				name += "/"; // Ajouter un slash visuel pour les dossiers
		}

		// Construction du lien
		// Si l'URI ne finit pas par '/', on l'ajoute pour que le lien relatif fonctionne
		std::string href = name;

		html << "<li><a href=\"" << href << "\">" << name << "</a></li>\n";
	}

	// Fermeture et fin du HTML
	closedir(dir);

	html << "</ul>\n"
		 << "<hr>\n"
		 << "</body>\n"
		 << "</html>";

	return html.str();
}