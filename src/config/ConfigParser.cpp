#include "../../include/config/ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string& configFilePath) :
	_configFilePath(configFilePath)
{}

ConfigParser::ConfigParser(const ConfigParser& other) 
{
	*this = other;
}

ConfigParser& ConfigParser::operator=(const ConfigParser& other)
{
	if (this != &other)
	{
		this->_configFilePath = other._configFilePath;
		this->_servers = other._servers;
	}
	return *this;
}

ConfigParser::~ConfigParser() 
{}

// Lecture du fichier de configuration, suppression des commentaires et normalisation des espaces et retours à la ligne.
// Retourne la chaîne de contenu nettoyée et linéarisée
std::string ConfigParser::readFileContent()
{
	std::ifstream			file(this->_configFilePath.c_str());
	std::string				line;
	std::string				content;
	std::string::size_type	commentPos;
	
	if (!file.is_open()) 
		throw ConfigParserException("Unable to open config file: " + this->_configFilePath);
	while (std::getline(file, line))
	{
		commentPos = line.find('#');
		if (commentPos != std::string::npos)
			line = line.substr(0, commentPos);
		for (std::string::iterator it = line.begin(); it != line.end(); ++it)
		{
			if (*it == '\t' || *it == '\r' || *it == '\n') 
				*it = ' '; // Remplacement par un espace simple
		}
		if (!line.empty()) 
		{
			content += line;
			content += " "; 
		}
	}
	file.close();
	return content;
}

// Découpe le contenu en tokens (mots et symboles) en ignorant les espaces
std::vector<std::string> ConfigParser::tokenize(const std::string& content)
{
	std::vector<std::string> tokens;
	std::string::size_type start = 0;
	std::string::size_type end = 0;

	start = content.find_first_not_of(" "); // Suppression initiale des espaces blancs en début de fichier
	
	while (start != std::string::npos)
	{
		end = content.find_first_of(" " DELIMITERS, start); // Trouver le prochain délimiteur (espace ou symbole)

		if (end == std::string::npos)	// Ajouter le mot trouvé
		{
			tokens.push_back(content.substr(start));
			break;
		}

		if (end > start) // Si le token est du texte (ex: "listen" ou "8080")
			tokens.push_back(content.substr(start, end - start));
		
		// Ajouter le symbole seul ({, }, ;)
		if (std::string(DELIMITERS).find(content[end]) != std::string::npos) 
			tokens.push_back(std::string(1, content[end]));

		// Avancer au-delà du caractère courant, puis au-delà des espaces
		start = end + 1;
		start = content.find_first_not_of(WHITESPACE, start);
	}
	return tokens;
}

std::vector<ServerConfig>& ConfigParser::getServers() 
{
	return _servers;
}

// Lit le fichier de configuration, tokenize le contenu, et construit les objets Server
void ConfigParser::parseConfig(std::vector<ServerConfig>& serverConfigs)
{
	std::string content = readFileContent();
	std::vector<std::string> tokens = tokenize(content);

	for (token_iterator it = tokens.begin(); it != tokens.end(); /* l'avance est gérée dans le corps */) 
	{
		if (it == tokens.end())
			break;
		if (*it == "server")
		{
			ServerConfig currentServerConfig;
			++it; // Avance sur '{'
			if (it == tokens.end() || *it != "{")
				throw ConfigParserException("Expected '{' after 'server'");
			parseServerBlock(currentServerConfig, it, tokens);
			if (currentServerConfig.locations.empty()) 
			{
				std::cout << "⚠️  Config Warning: Server '" << currentServerConfig.server_names[0]
						<< "' has no location. Creating default '/' location." << std::endl;

				LocationConfig defaultLoc;
				defaultLoc.path = "/";
				
				// Héritage automatique des propriétés du serveur
				defaultLoc.root = currentServerConfig.root.empty() ? "./www" : currentServerConfig.root;
				defaultLoc.index_files = currentServerConfig.index_files;
				defaultLoc.client_max_body_size = currentServerConfig.clientMaxBodySize;
				defaultLoc.autoindex = false; // Par sécurité
				
				// On ajoute cette location virtuelle
				currentServerConfig.addLocation(defaultLoc);
			}
			if (it == tokens.end() || *it != "}")
				throw ConfigParserException("Syntax Error: Server block missing closing brace '}'.");
			++it;

			serverConfigs.push_back(currentServerConfig);
		}
		else if (*it == "}")
			throw ConfigParserException("Unexpected closing brace '}' at global scope.");
		else 
			// Tout autre token ('listen', 'root', 'location' etc.) est une erreur de syntaxe
			throw ConfigParserException("Unexpected token: " + *it + " at global scope. Expected 'server'.");
	}
}

void ConfigParser::parseServerBlock(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens)
{
	++it;

	while (it != allTokens.end() && *it != "}")
	{
		const std::string& directive = *it;

		if (directive == "location") 
		{
			LocationConfig newLocation; // Constructeur -> client_max_body_size initialisé à -1
			std::string pathBuffer; 

			++it; // après "location"
			if (it == allTokens.end())
				throw ConfigParserException("Unexpected end after 'location'.");

			if (*it == "=" || *it == "~" || *it == "~*" || *it == "^~")
			{
				pathBuffer = *it;
				pathBuffer += " ";
				++it;
				if (it == allTokens.end())
					throw ConfigParserException("Location pattern missing after modifier.");
			}

			pathBuffer += *it;
			++it;

			newLocation.path = pathBuffer;
			parseLocationBlock(newLocation, it, allTokens);

			// Si après le parsing, la taille est toujours à -1 ,
			// cela veut dire que l'utilisateur n'a pas mis de "client_max_body_size" dans le bloc location.
			// On applique donc la valeur par défaut du SERVEUR.
			if (newLocation.client_max_body_size == static_cast<size_t>(-1)) 
				newLocation.client_max_body_size = currentServerConfig.clientMaxBodySize;

			finalizeLocation(newLocation, currentServerConfig);
			currentServerConfig.addLocation(newLocation);
		}
		else if (directive == "server")
			throw ConfigParserException("Syntax Error: server block included inside another server block. ");
		else 
			routeDirective(currentServerConfig, it, allTokens); 
	}
}

void ConfigParser::parseLocationBlock(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens) 
{
	if (it == allTokens.end() || *it != "{")
		throw ConfigParserException("Expected '{' after location path.");
	++it; 
	while (it != allTokens.end() && *it != "}")
	{
		const std::string& directive = *it;
		
		if (directive == "server" || directive == "location") 
			throw ConfigParserException("Syntax Error: Nested blocks ('server' or 'location') are not allowed here.");
		routeLocationDirective(currentLocation, it, allTokens);
	}
	if (*it != "}")
		throw ConfigParserException("Syntax Error: Location block missing closing brace '}'.");
	++it;
}

void ConfigParser::parseServerName(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens) 
{
	if (it == allTokens.end())
		throw ConfigParserException("Unexpected end of file after server_name directive.");

	while (it != allTokens.end() && *it != ";")
	{
		 if (*it == "listen" || *it == "root" || *it == "location" || *it == "server_name" || *it == "error_page")
			throw ConfigParserException("Missing semicolon after server_name directive before next directive: " + *it);

		currentServerConfig.server_names.push_back(*it);
		++it;
	}
	if (it == allTokens.end() || *it != ";")
		throw ConfigParserException("Missing semicolon after server_name directive.");
	++it;
}

void ConfigParser::parseRoot(ServerConfig& currentServerConfig, token_iterator& it) 
{
	// 'it' pointe sur la valeur (ex: "www/html")
	currentServerConfig.root = *it; 

	++it;
	if (*it != ";")
		throw ConfigParserException("Missing semicolon after root directive.");
	++it;
}

static bool is_valid_ip(ServerConfig& currentServerConfig, const std::string& host) 
{
	if (host.empty() || host == "*" || host == "0.0.0.0" || host == "localhost") 
	{
		currentServerConfig.ip.assign(4, 0);
		return true;
	}
	std::stringstream	ss(host);
	std::string			segment;
	std::vector<int> 	ip_tab;
	int count = 0;

	while (std::getline(ss, segment, '.'))
	{
		if (segment.empty() || segment.length() > 3)
			return false;

		for (size_t i = 0; i < segment.length(); ++i)
		{
			if (!std::isdigit(segment[i])) 
				return false;
		}

		int val = std::atoi(segment.c_str());
		if (val < 0 || val > 255)
			return false;
		ip_tab.push_back(val);
		count++;
	}
	if (count != 4)
		return false;
	currentServerConfig.ip = ip_tab;
	return true;
}

void ConfigParser::parseListen(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens)
{
	ListenEndpoint	newEndpoint;
	std::string		token = *it;

	// --- 1. Cas "IP:PORT" ---
	std::size_t separatorPos = token.find(':');
	if (separatorPos != std::string::npos)
	{
		newEndpoint.host = token.substr(0, separatorPos);
		newEndpoint.port = std::atoi(token.substr(separatorPos + 1).c_str());
		++it;
	}
	else
	{
		++it;
		// --- 2. Cas "IP PORT" ---
		if (is_valid_ip(currentServerConfig, token))
		{
			if (it == allTokens.end() || *it == ";")
				throw ConfigParserException("Listen directive IP missing Port value.");
			
			newEndpoint.host = token;
			newEndpoint.port = std::atoi(it->c_str());
			++it;
		}
		// --- 3. Cas "PORT" seul ---
		else
		{
			newEndpoint.host = "0.0.0.0";
			newEndpoint.port = std::atoi(token.c_str());
		}
	}

	// --- Vérifications générales ---
	if (!is_valid_ip(currentServerConfig, newEndpoint.host))
		throw ConfigParserException("Invalid host specified in listen directive: " + newEndpoint.host);

	if (newEndpoint.port <= 0 || newEndpoint.port > 65535)
		throw ConfigParserException("Invalid port number: " + FileUtils::intToString(newEndpoint.port));

	if (it == allTokens.end() || *it != ";")
		throw ConfigParserException("Missing semicolon after listen directive.");

	++it;
	currentServerConfig.listens.push_back(newEndpoint);
}

void ConfigParser::parseErrorPage(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens)
{
	// 'it' entre dans cette fonction en pointant sur le code '404' (après l'avance du routeur)

	if (it == allTokens.end() || *it == ";")
		throw ConfigParserException("error_page missing HTTP code.");

	int code = atoi((*it).c_str());
	++it;

	if (it == allTokens.end() || *it == ";")
		throw ConfigParserException("error_page missing file path.");
		
	std::string path = *it;
	++it;
	currentServerConfig.error_pages[code] = path;

	if (*it != ";")
		throw ConfigParserException("error_page: Missing semicolon after path.");
	++it;
}

void ConfigParser::parseServerIndex(ServerConfig& config, token_iterator& it, const std::vector<std::string>& allTokens) 
{
	while (it != allTokens.end() && *it != ";") 
	{
		config.index_files.push_back(*it);
		++it;
	}
	if (it == allTokens.end())
		throw ConfigParserException("Missing ';' after index directive");
	++it;
}

// Détermine quelle fonction spécialisée est nécessaire pour consommer la directive suivate
void ConfigParser::routeDirective(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens)
{
	const std::string directive = *it;

	++it; 

	if (it == allTokens.end() || *it == "}") 
		throw ConfigParserException("Directive '" + directive + "' missing value or semicolon.");

	if (directive == "listen")
		parseListen(currentServerConfig, it, allTokens);
	else if (directive == "root")
		parseRoot(currentServerConfig, it);
	else if (directive == "server_name")
		parseServerName(currentServerConfig, it, allTokens);
	else if (directive == "error_page")
		parseErrorPage(currentServerConfig, it, allTokens);
	else if (directive == "client_max_body_size")
		parseServerMaxBodySize(currentServerConfig, it, allTokens);
	else if (directive == "index")
		parseServerIndex(currentServerConfig, it, allTokens);
	else if (directive == "location") 
	{
		LocationConfig loc;
		parseLocationBlock(loc, it, allTokens);
		finalizeLocation(loc, currentServerConfig);
		currentServerConfig.addLocation(loc);
	}
	else
		throw ConfigParserException("Unknown directive: " + directive);
}

// gère les directives du fichier de config (allow_methods, root, index, upload_store, cgi_pass, client_max_body_size).
void ConfigParser::routeLocationDirective(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens)
{
	const std::string directive = *it;
	
	++it;
	if (it == allTokens.end() || *it == "}") 
		throw ConfigParserException("Directive '" + directive + "' missing value or semicolon.");
	
	if (directive == "root")
		parseLocationRoot(currentLocation, it);
	else if (directive == "autoindex")
		parseLocationAutoIndex(currentLocation, it);
	else if(directive == "index")
		parseLocationIndex(currentLocation, it, allTokens);
	else if (directive == "allow_methods")
		parseLocationAllowMethods(currentLocation, it, allTokens);
	else if (directive == "client_max_body_size")
		parseLocationMaxBodySize(currentLocation, it);
	else if (directive == "client_max_header_size")
		parseLocationMaxHeaderSize(currentLocation, it);
	else if (directive == "upload_store")
		parseLocationUploadDir(currentLocation, it);
	else if (directive == "cgi_pass")
		parseLocationCgiPass(currentLocation, it, allTokens);
	else if (directive == "return")
		parseLocationReturn(currentLocation, it, allTokens);
	else
		throw ConfigParserException("Unknown directive inside location block: " + directive);
}

// gère les directives du fichier de config (allow_methods, root, index, upload_store, cgi_pass, client_max_body_size).
// Capturer le répertoire racine spécifique à cette route (ex: root YoupiBanane;)
void ConfigParser::parseLocationRoot(LocationConfig& currentLocation, token_iterator& it)
{
	currentLocation.root = *it;
	++it;
	if (*it != ";") 
		throw ConfigParserException("Missing semicolon after 'root' directive in location block.");
	++it;
}

// Capturer le ou les fichiers par défaut à chercher dans le répertoire racine (ex: index index.html youpi.bad_extension;)
void ConfigParser::parseLocationIndex(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens)
{
	// L'index peut avoir plusieurs fichiers.
	while (it != allTokens.end() && *it != ";")
	{
		currentLocation.index_files.push_back(*it); 
		++it;
	}
	if (*it != ";") 
		throw ConfigParserException("Missing semicolon after 'index' directive in location block.");
	++it;
}

std::string trim(const std::string& str) 
{
	size_t start = 0;
	while (start < str.size() && std::isspace(str[start])) 
		++start;

	if (start == str.size())
		return "";

	size_t end = str.size() - 1;
	while (end > start && std::isspace(str[end])) 
		--end;

	return str.substr(start, end - start + 1);
}

void ConfigParser::parseLocationAllowMethods(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens) 
{
	while (it != allTokens.end() && *it != ";") 
	{
		std::string method = trim(*it);
		normalize_to_uppercase(method);
		if (method != "GET" && method != "POST" && method != "DELETE" && method != "PUT")
			throw ConfigParserException(method + " = Forbidden method");
		size_t i = 0;
		for(; i < currentLocation.allowed_methods.size(); i++)
			if (currentLocation.allowed_methods[i] == method)
				continue;
		if (i == currentLocation.allowed_methods.size())
			currentLocation.allowed_methods.push_back(method);
		++it;
	}

	if (it == allTokens.end() || *it != ";")
		throw ConfigParserException("Missing semicolon in allow_methods directive.");
	++it;
}

long ConfigParser::parseSize(const std::string& value) 
{
	char* end;
	long size = std::strtol(value.c_str(), &end, 10);

	if (end == value.c_str()) 
		throw ConfigParserException("Invalid number format");
	
	if (*end != '\0') 
	{
		std::string suffix(end);
		if (suffix == "k" || suffix == "K") 
			size *= 1024;
		else if (suffix == "m" || suffix == "M")
			size *= 1024 * 1024;
		else if (suffix == "g" || suffix == "G")
			size *= 1024 * 1024 * 1024;
		else
			throw ConfigParserException("Invalid suffix");
	}
	if (size < 0)
		throw ConfigParserException("Size cannot be negative.");
	return size;
}

void ConfigParser::parseServerMaxBodySize(ServerConfig& config, token_iterator& it, const std::vector<std::string>& tokens)
{
	if (it == tokens.end() || *it == "}" || *it == ";")
		throw ConfigParserException("client_max_body_size missing value.");

	config.clientMaxBodySize = parseSize(*it);

	++it;
	if (it == tokens.end() || *it != ";")
		throw ConfigParserException("client_max_body_size missing semicolon.");
	++it;
}

// lit la limite de taille du corps de la requête client (client_max_body_size), la convertit en entier, et la stocke dans l'attribut de l'objet LocationConfig
void ConfigParser::parseLocationMaxBodySize(LocationConfig& currentLocation, token_iterator& it) 
{
	long size = parseSize(*it);
	currentLocation.client_max_body_size = static_cast<size_t>(size);

	++it;
	if (*it != ";")
		throw ConfigParserException("Missing semicolon after client_max_body_size directive.");
	++it;
}

void ConfigParser::parseLocationMaxHeaderSize(LocationConfig& currentLocation, token_iterator& it) 
{
	// 'it' pointe sur la valeur (ex: "100")
	std::string valueStr = *it;
	
	for (size_t i = 0; i < valueStr.length(); ++i)
		if (!std::isdigit(valueStr[i]))
			throw ConfigParserException("client_max_header_size must be a number: " + valueStr);

	long size = std::atol(valueStr.c_str());
	
	if (size < 0)
		throw ConfigParserException("client_max_header_size cannot be negative.");

	if (static_cast<long>(currentLocation.client_max_header_size))
		throw ConfigParserException("client_max_header_size exceeds maximum allowable size.");

	currentLocation.client_max_header_size = size;
	++it;

	if (*it != ";")
		throw ConfigParserException("Missing semicolon after client_max_header_size directive.");
	++it; 
}

void ConfigParser::parseLocationUploadDir(LocationConfig& currentLocation, token_iterator& it)
{
	currentLocation.upload_dir = *it;
	++it;

	if (*it != ";") 
		throw ConfigParserException("Missing semicolon after upload_store directive.");
	++it;
}

// Parse la directive 'cgi_pass' et remplit la map des exécuteurs CGI (gère les syntaxes avec ou sans extension explicite)
void ConfigParser::parseLocationCgiPass(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens) 
{
	std::string firstArg = *it;
	++it;

	if (it == allTokens.end())
		throw ConfigParserException("Unexpected end of file in cgi_pass.");
	
	std::string secondArg = *it;
	std::string extension;
	std::string executorPath;

	// 2 arguments, exemple : cgi_pass .bla /path/to/cgi_test; 
	if (secondArg != ";")
	{
		extension = firstArg;
		executorPath = secondArg;
		++it; 
	}
	// 1 argument, exemple : cgi_pass /path/to/cgi_test;
	else 
	{
		executorPath = firstArg;
		std::string locPath = currentLocation.path;
		size_t dotPos = locPath.rfind('.'); // Dernier point
		
		if (dotPos != std::string::npos)
		{
			extension = locPath.substr(dotPos);
			if (!extension.empty() && extension[extension.size() - 1] == '$')
				extension.erase(extension.size() - 1);
		}
		else
			throw ConfigParserException("CGI Error: Cannot deduce extension from location '" + locPath + "'.");
	}

	if (extension.empty() || executorPath.empty())
		throw ConfigParserException("Invalid cgi_pass directive.");

	currentLocation.cgi_executors[extension] = executorPath;

	if (it == allTokens.end() || *it != ";") 
		throw ConfigParserException("Missing semicolon after cgi_pass directive.");
	++it;
}

void ConfigParser::parseLocationAutoIndex(LocationConfig& currentLocation, token_iterator& it)
{
	std::string value = *it;

	if (value == "on")
		currentLocation.autoindex = true;
	else if (value == "off")
		currentLocation.autoindex = false;
	else
		throw ConfigParserException("Invalid value for autoindex (expected 'on' or 'off'): " + value);

	++it; 
	if (*it != ";")
		throw ConfigParserException("Missing semicolon after autoindex directive.");
	++it;
}

void ConfigParser::parseLocationReturn(LocationConfig& loc, token_iterator& it, const std::vector<std::string>& allTokens)
{
	if (it == allTokens.end()) 
		throw ConfigParserException("Unexpected end of file in return directive");

	std::string arg1 = *it;
	bool isCode = true;
	
	// Vérifier si le premier argument est un code numérique (ex: 301, 302, 404)
	for (size_t i = 0; i < arg1.length(); ++i) 
	{
		if (!isdigit(arg1[i])) 
		{ 
			isCode = false; 
			break; 
		}
	}

	if (isCode)
	{
		// Cas A: "return 301 /path/;"
		loc.return_code = std::atoi(arg1.c_str());
		
		++it;
		if (it == allTokens.end() || *it == ";") 
			throw ConfigParserException("Directive 'return' avec code " + arg1 + " nécessite une URL.");
		
		loc.return_url = *it;
		
		++it;
	}
	else
	{
		// Cas B: "return /path/;" (Redirection 302 - convention Nginx)
		loc.return_code = 302;
		loc.return_url = arg1;
		++it;
	}

	if (it == allTokens.end())
		throw ConfigParserException("Missing ';' after return directive");

	if (*it != ";")
		throw ConfigParserException("Expected ';' after return directive, found: " + *it);

	++it;
}

ConfigParserException::ConfigParserException(const std::string& msg) :
	_msg(msg)
{}


// Assurer un root par défaut pour chaque location (si la location n’a pas de root, utiliser le root du serveur parent)
void ConfigParser::finalizeLocation(LocationConfig& loc, const ServerConfig& server)
{
	if (loc.root.empty())
		loc.root = server.root;

	if (loc.index_files.empty())
		loc.index_files = server.index_files;
	
	if (loc.allowed_methods.empty())
		loc.allowed_methods.push_back("GET");
}