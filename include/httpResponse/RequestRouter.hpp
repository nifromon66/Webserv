#ifndef REQUESTROUTER_HPP
# define REQUESTROUTER_HPP

#include "../httpRequest/HttpRequest.class.hpp"  // Contient la requête parsée
#include "../config/Location.hpp"
#include "../config/Config.hpp"
#include "../include/Error/HttpRequestStatus.error.hpp"
#include "../httpResponse/FileUtils.hpp"
#include "../httpResponse/MimeTypes.hpp"
#include "../httpResponse/HttpStatus.hpp"
#include "../httpResponse/AutoIndex.hpp"
#include "../config/ConfigParser.hpp"
#include "../httpResponse/HttpResponse.hpp"

#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h> // Pour la lecture des répertoires
#include <unistd.h>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>


class RequestRouter
{
	public:

		RequestRouter();
		RequestRouter(const RequestRouter &copy);
		RequestRouter &operator=(const RequestRouter &other);
		~RequestRouter();

		HttpResponse handle(const HttpRequest& req, const ServerConfig& conf, int error) const;
		bool isCgiMatch(const std::string& uri, const std::string& locPath) const;
		const LocationConfig& resolveLocation(const std::string& uri, const ServerConfig& conf) const;
		void checkMethodAndSize(const HttpRequest& req, const LocationConfig& loc) const;
		HttpResponse buildErrorResponse(int statusCode, const ServerConfig& conf) const;



	private:

		HttpResponse handleGet(const HttpRequest& req, const LocationConfig& loc, const ServerConfig& conf) const;
		HttpResponse handleDelete(const HttpRequest& req, const LocationConfig& loc, const ServerConfig & conf) const;
		HttpResponse handlePost(const HttpRequest& req, const LocationConfig& loc, const ServerConfig& conf) const;
		bool isMethodAllowed(const std::string& method, const LocationConfig& loc) const;
		std::string joinMethods(const std::vector<std::string>& methods) const;
		std::string resolveResourcePath(const std::string& uri, const LocationConfig& loc) const;
		HttpResponse serveFile(const std::string& filePath) const;
		std::string tryFindIndex(const std::string& dirPath, const std::vector<std::string>& indexes) const;

};

#endif