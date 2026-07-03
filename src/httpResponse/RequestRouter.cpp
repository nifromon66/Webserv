#include "../../include/httpResponse/RequestRouter.hpp"

RequestRouter::RequestRouter()
{}

RequestRouter::RequestRouter(const RequestRouter &copy)
{
	*this = copy;
}

RequestRouter &RequestRouter::operator=(const RequestRouter &other)
{
	if (this != &other)
	{
	}
	return *this;
}

RequestRouter::~RequestRouter()
{}

bool RequestRouter::isCgiMatch(const std::string& uri, const std::string& locPath) const
{
	if (locPath.empty() || locPath[0] != '~')
		return (false);

	// On cherche l'extension dans la config (ex: "~ \.bla$")
	// Simplification : on cherche ".bla"
	size_t dotPos = locPath.find('.');
	if (dotPos == std::string::npos)
		return (false);

	std::string extension = locPath.substr(dotPos);

	// Enlever le '$' final s'il est présent
	if (!extension.empty() && extension[extension.length() - 1] == '$')
		extension = extension.substr(0, extension.length() - 1);

	// Vérifie si l'URI finit par cette extension
	if (uri.length() >= extension.length())
		return (uri.compare(uri.length() - extension.length(), extension.length(), extension) == 0);
	return (false);
}

void	RequestRouter::checkMethodAndSize(const HttpRequest& req, const LocationConfig& loc) const
{
	std::string method = req.get_method().get_method();

	if (loc.redirection.first != 0)
		throw HttpRequestStatus(loc.redirection.first, loc.redirection.second);

	if (req.get_body().get_body().size() > loc.client_max_body_size)
		throw HttpRequestStatus(413, "Content Too Large");

	if (!isMethodAllowed(method, loc))
		throw HttpRequestStatus(405, "Method Not Allowed");
}

HttpResponse RequestRouter::handle(const HttpRequest& req, const ServerConfig& conf, int error) const
{
	if (error != 0 && error != 200)
		return buildErrorResponse(error, conf);

	try
	{
		std::string uri = req.get_target().get_path();
		const LocationConfig& loc = resolveLocation(uri, conf);

		checkMethodAndSize(req, loc);

		std::string method = req.get_method().get_method();

		if (method == "GET")
			return handleGet(req, loc, conf);
		else if (method == "DELETE")
			return handleDelete(req, loc, conf);
		else if (method == "POST")
			return handlePost(req, loc, conf);
		else
			return buildErrorResponse(501, conf);
	}
	catch (const HttpRequestStatus& e)
	{
		int code = e.getCode();

		// Gestion des redirections
		if (code >= 300 && code < 400)
		{
			HttpResponse redirectResp;
			redirectResp.setStatusCode(code);
			redirectResp.setHeader("Location", e.getMessage());
			return redirectResp;
		}

		if (code == 405)
		{
			try
			{
				const LocationConfig& loc = resolveLocation(req.get_target().get_path(), conf);
				HttpResponse response = buildErrorResponse(405, conf);
				response.setHeader("Allow", joinMethods(loc.allowed_methods));
				return response;
			}
			catch (...)
			{
				return buildErrorResponse(405, conf);
			}
		}
		return buildErrorResponse(code, conf);
	}
	catch (const std::exception& e)
	{
		return buildErrorResponse(500, conf);
	}
}

std::string RequestRouter::joinMethods(const std::vector<std::string>& methods) const
{
	std::string result;

	for (size_t i = 0; i < methods.size(); ++i)
	{
		// On ajoute la méthode (ex: "GET")
		result += methods[i];

		// Si ce n'est pas la dernière, on ajoute la virgule de séparation
		if (i < methods.size() - 1)
			result += ", ";
	}
	return result; // Retourne "GET, POST, DELETE"
}

// Identifie et retourne le bloc de configuration Location le plus spécifique correspondant à l'URI demandé
const	LocationConfig& RequestRouter::resolveLocation(const std::string& uri, const ServerConfig& conf) const
{
	const LocationConfig* bestMatch = NULL;
	size_t bestMatchLength = 0;

	//std::cout << "DEBUG:conf.location size = " << conf.locations.size() << std::endl;

	for (size_t i = 0; i < conf.locations.size(); ++i)
	{
		const LocationConfig &loc = conf.locations[i];

		// 1. PRIORITÉ CGI
		if (isCgiMatch(uri, loc.path))
			return loc; // On retourne immédiatement la règle CGI (prioritaire)

		// 2. PREFIX MATCH STANDARD
		if (uri.find(loc.path) == 0)
		{
			if (loc.path.length() > bestMatchLength)
			{
				bestMatchLength = loc.path.length();
				bestMatch = &loc;
			}
		}
		// 3. --- AJOUT POUR LE CAS DU SLASH MANQUANT ---
		// Si l'URI est "/directory" et que la config est "/directory/"
		else if (uri + "/" == loc.path)
		{
			if (loc.path.length() > bestMatchLength)
				bestMatch = &loc;
		}
	}
	if (bestMatch != NULL)
		return *bestMatch;
	throw std::runtime_error("File not found or no matching location.");
}

std::string RequestRouter::tryFindIndex(const std::string& dirPath, const std::vector<std::string>& indexes) const
{
	for (size_t i = 0; i < indexes.size(); ++i)
	{
		std::string potentialIndex = dirPath;
		if (potentialIndex[potentialIndex.length() - 1] != '/')
			potentialIndex += "/";
		potentialIndex += indexes[i];

		if (FileUtils::pathExists(potentialIndex))
			return potentialIndex;
	}
	return "";
}

// Résout le chemin complet sur le système de fichiers en fonction de l'URI et de la configuration de la location
// Ex: URI "/directory/file.bla" avec location.path "/directory/" et location.root "./YoupiBanane/"
//     → Résultat: "./YoupiBanane/file.bla"
std::string RequestRouter::resolveResourcePath(const std::string& uri, const LocationConfig& loc) const
{
	std::string root = loc.root;
	//std::cout << "DEBUG: loc.root = " << loc.root << std::endl;
	if (!root.empty() && root[root.length() - 1] != '/')
		root += "/";

	std::string relativePath;

	if (uri.find(loc.path) == 0)
		relativePath = uri.substr(loc.path.length());   // ex: /directory/file.bla -> file.bla
	// ex: /directory alors que la location est /directory/
	else if (loc.path.length() > 0 && uri == loc.path.substr(0, loc.path.length() - 1))
		relativePath = "";
	else
		relativePath = uri;

	std::string fullPath = root + relativePath;     // ex: ./YoupiBanane/ + "" -> ./YoupiBanane/

	return fullPath;
}

// Lit un fichier et prépare la réponse HTTP 200.
// Vérifie :
//   - si le fichier n'existe pas → 404
//   - si le fichier est vide (ou lecture impossible) → 403
//   - sinon → lecture + détection du MIME type + réponse 200.
HttpResponse RequestRouter::serveFile(const std::string& filePath) const
{
	HttpResponse response;

	if (!FileUtils::pathExists(filePath))
		throw std::runtime_error("404_FILE_NOT_FOUND");

	std::string fileContent = FileUtils::readFile(filePath);

	response.setStatusCode(200);
	response.setBody(fileContent);
	response.setHeader("Content-Type", MimeTypes::getType(filePath));
	return response;
}

HttpResponse RequestRouter::handleGet(const HttpRequest& req, const LocationConfig& loc, const ServerConfig& conf) const
{
	//std::cout << "DEBUG LOCATION SELECTED: Path configuré = [" << loc.path << "]" << std::endl;
	//std::cout << "DEBUG CHECK REDIRECT: url=[" << loc.return_url << "] code=" << loc.return_code << std::endl;
	// Verification de redirection
	if (!loc.return_url.empty())
	{
		//std::cout << "DEBUG REDIRECT: Application de la redirection !" << std::endl;
		HttpResponse redirectResponse;
		// Si le code n'est pas valide (ex: 0), on met 302 par défaut, sinon le code de la config
		int code = (loc.return_code > 0) ? loc.return_code : 302;
		redirectResponse.setStatusCode(code);
		redirectResponse.setHeader("Location", loc.return_url);
		redirectResponse.setHeader("Content-Length", "0");
		redirectResponse.setHeader("Connection", "close");          // Connection close lors d'une redirection
		return redirectResponse;
	}
	try
	{
		std::string path = resolveResourcePath(req.get_target().get_path(), loc);
		//std::cout << "DEBUG GET: URI demandée = [" << req.get_target().get_path() << "]" << std::endl;
		//std::cout << "DEBUG GET: Chemin résolu = [" << path << "]" << std::endl;
		//std::cout << "DEBUG GET: isDirectory?  = " << (FileUtils::isDirectory(path) ? "OUI" : "NON") << std::endl;
		if (!FileUtils::pathExists(path))
			return buildErrorResponse(404, conf);
		if (FileUtils::isDirectory(path))
		{
			std::string uri = req.get_target().get_path();

			// A. Redirection 301 si le slash final manque
			if (uri.empty() || uri[uri.length() - 1] != '/')
			{
				HttpResponse redirectResponse;
				redirectResponse.setStatusCode(301);
				redirectResponse.setHeader("Location", uri + "/");
				redirectResponse.setHeader("Content-Length", "0");
				return redirectResponse;
			}

			// On cherche d'abord si un fichier index existe (ex: index.html, youpi.bad_extension)
			std::string indexPath = tryFindIndex(path, loc.index_files);

			if (!indexPath.empty())
			{
				// On a trouvé un index ! On le sert comme un fichier normal.
				// On vérifie que ce n'est pas un dossier lui-même
				if (!FileUtils::isDirectory(indexPath))
				{
					//std::cout << "DEBUG INDEX: Fichier trouvé -> " << indexPath << std::endl;
					return serveFile(indexPath);
				}
			}

			// Si aucun index n'a été trouvé, on regarde si on a le droit de lister le dossier
			if (loc.autoindex)
			{
				//std::cout << "DEBUG AUTOINDEX: Génération de la liste..." << std::endl;
				std::string htmlContent = AutoIndex::buildAutoIndexPage(path, req.get_target().get_path());

				HttpResponse response;
				response.setStatusCode(200);
				response.setBody(htmlContent);
				response.setHeader("Content-Type", "text/html; charset=utf-8");
				response.setHeader("Content-Length", FileUtils::intToString(htmlContent.size()));
				return response;
			}

			// C'est un dossier, pas d'index trouvé, et autoindex est OFF -> 403 Forbidden.
			return buildErrorResponse(403, conf);
		}
		// Ce n'est pas un dossier, c'est donc un fichier. On l'envoie.
		return serveFile(path);
	}
	catch (const std::exception& e)
	{
		std::string msg = e.what();

		std::cerr << "Error in handleGet: " << msg << std::endl;
		if (msg.find("Could not open directory") != std::string::npos)
			return buildErrorResponse(404, conf);
		if (msg.find("403") != std::string::npos || msg == "403_FORBIDDEN")
			return buildErrorResponse(403, conf);
		if (msg.find("404") != std::string::npos || msg == "404_FILE_NOT_FOUND")
			return buildErrorResponse(404, conf);
		return buildErrorResponse(500, conf);
	}
}

HttpResponse RequestRouter::handleDelete(const HttpRequest& req, const LocationConfig& loc, const ServerConfig& conf) const
{
	HttpResponse response;

	try
	{
		std::string filePath = resolveResourcePath(req.get_target().get_path(), loc);

		if (!FileUtils::pathExists(filePath))
			return buildErrorResponse(404, conf);

		if(FileUtils::isDirectory(filePath))
			return buildErrorResponse(403, conf);

		// Vérification des droits d'écriture
		if (access(filePath.c_str(), W_OK) != 0)
			return buildErrorResponse(403, conf);

		if(FileUtils::deleteFile(filePath))
		{
			response.setStatusCode(204); // Succès
			return response;
		}
		else
			return buildErrorResponse(500, conf);
	}
	catch(const std::exception& e)
	{
		std::cerr << "Exception in handleDelete: " << e.what() << std::endl;
		return buildErrorResponse(500, conf);
	}
}

HttpResponse RequestRouter::handlePost(const HttpRequest& req, const LocationConfig& loc, const ServerConfig& conf) const
{
	HttpResponse response;

	if (req.get_body().get_body().size() > loc.client_max_body_size)
		return buildErrorResponse(413, conf);

	// --- LOGIQUE D'UPLOAD DE FICHIER ---

	// A. Déterminer le nom du fichier cible
	// Si l'URL est /uploads/image.jpg -> On crée image.jpg
	std::string filePath = resolveResourcePath(req.get_target().get_path(), loc);

	// B. Écrire le contenu du body dans ce fichier
	try
	{
		std::ofstream file(filePath.c_str(), std::ios::binary);
		if (!file.is_open())
			return buildErrorResponse(500, conf);

		// On écrit tout le body
		const std::string& body = req.get_body().get_body();
		file.write(body.c_str(), body.size());
		file.close();

		response.setStatusCode(201);
		response.setBody("File created successfully");
		response.setHeader("Location", req.get_target().get_path());
	}
	catch (const std::exception& e)
	{
		return buildErrorResponse(500, conf);
	}
	return response;
}

HttpResponse RequestRouter::buildErrorResponse(int statusCode, const ServerConfig& conf) const
{
	HttpResponse response;
	response.setStatusCode(statusCode);

	// On cherche si ce code d'erreur a une page associée dans la config
	std::map<int, std::string>::const_iterator it = conf.error_pages.find(statusCode);
	if (it != conf.error_pages.end())
	{
		std::string customPath = it->second;
		std::string root = conf.root;
		std::string fullPath;

		// Gestion propre des slashs pour éviter "folder//file"
		if (!root.empty() && root[root.size() - 1] == '/' && !customPath.empty() && customPath[0] == '/')
			fullPath = root + customPath.substr(1); // On enlève le slash du chemin fichier
		else if (!root.empty() && root[root.size() - 1] != '/' && !customPath.empty() && customPath[0] != '/')
			fullPath = root + "/" + customPath; // On ajoute un slash
		else
			fullPath = root + customPath; // C'est déjà bon (un seul slash présent)


		if (FileUtils::pathExists(fullPath))
		{
			std::string fileContent = FileUtils::readFile(fullPath);
			if (!fileContent.empty())
			{
				response.setBody(fileContent);
				response.setHeader("Content-Type", MimeTypes::getType(fullPath));
				response.setHeader("Content-Length", FileUtils::intToString(fileContent.size()));
				return response;
			}
		}
	}

	// Si on arrive ici, c'est qu'il n'y a pas de page custom ou qu'elle est illisible.
	std::string reasonPhrase = HttpStatus::getStatusText(statusCode);
	std::string strCode = FileUtils::intToString(statusCode);

	std::string body;
	body += "<!DOCTYPE html>\n<html>\n<head>\n";
	body += "  <title>" + strCode + " " + reasonPhrase + "</title>\n";
	body += "  <style>body{font-family: Arial, sans-serif; text-align: center; margin-top: 50px;}</style>\n";
	body += "</head>\n<body>\n";
	body += "  <h1>Error " + strCode + "</h1>\n";
	body += "  <p>" + reasonPhrase + "</p>\n";
	body += "  <hr><small>Webserv/1.0</small>\n";
	body += "</body>\n</html>";

	response.setBody(body);
	response.setHeader("Content-Type", "text/html");
	response.setHeader("Content-Length", FileUtils::intToString(body.size()));

	// Pour les erreurs, fermer la connexion
	if (statusCode >= 400)
		response.setHeader("Connection", "close");

	return response;
}

bool RequestRouter::isMethodAllowed(const std::string& method, const LocationConfig& loc) const
{
	for (size_t i = 0; i < loc.allowed_methods.size(); ++i)
	{
		if (loc.allowed_methods[i] == method)
			return true;
	}
	return false;
}
