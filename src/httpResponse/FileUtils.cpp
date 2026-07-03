#include "../../include/httpResponse/FileUtils.hpp"

// retourne le contenu d’un fichier (html, txt, json, etc.) sous forme de string.
std::string FileUtils::readFile(const std::string& path) 
{
	std::ifstream ifs(path.c_str());

	if (!ifs.is_open()) 
		return (""); // Retourne une chaîne vide en cas d'échec

	std::stringstream buffer;
	buffer << ifs.rdbuf();

	return (buffer.str());
}

// retourne true si le chemin spécifié est un répertoire existant
bool FileUtils::isDirectory(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0) // stat() permet de savoir précisément si un chemin est un fichier, un dossier ou inexistant ou interdit (permissions)
		return (false); // stat() échoue → chemin inexistant ou inaccessible

	return (S_ISDIR(info.st_mode)); // Vrai si c’est un dossier
}

// Si stat retourne 0, le chemin existe. Sinon, il n'existe pas ou n'est pas accessible.
bool FileUtils::pathExists(const std::string& path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

bool FileUtils::deleteFile(const std::string& path)
{
	if (std::remove(path.c_str()) == 0) 
		return (true);
	else 
		return (false);
}

std::string FileUtils::intToString(int n)
{
	std::stringstream ss;
	ss << n;

	return (ss.str());
}

std::string	FileUtils::sizeToString(size_t n)
{
	std::ostringstream	oss;
	oss << n;

	return (oss.str());
}

// modifie fd pour le rendre non-bloquant (ajout du flag O_NONBLOCK)
int FileUtils::setFDNonBloquing(int fd)
{
	int flags = ::fcntl(fd, F_GETFL, 0);										// Récupère (F_GETFL) les flags actuels du fd
	if (flags == -1)
		throw std::runtime_error("fcntl(F_GETFL) failed");

	if (::fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) 						// Ajoute (F_SETFL) le flag O_NONBLOCK aux flags existants
		throw std::runtime_error("fcntl(F_SETFL, O_NONBLOCK) failed");

	return (fd);
}