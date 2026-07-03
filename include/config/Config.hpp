#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "../config/Location.hpp"

#include <string>
#include <vector>
#include <map>

#include <fstream>
#include <iostream>

struct ListenEndpoint 
{
	std::string	host;  // "0.0.0.0" par défaut
	int			port;  // ex: 80
};

struct ServerConfig 
{
	std::vector<ListenEndpoint>	listens;
	std::string					server_name;
	std::string					root;

	long						clientMaxBodySize;
	long						clientMaxHeaderSize;
	std::map<int, std::string>	error_pages;
	std::vector<std::string>	server_names;
	std::vector<LocationConfig> locations;
	std::vector<int>			ip;
	std::vector<std::string> 	index_files;
	
	void addLocation(const LocationConfig& loc) 
	{
		locations.push_back(loc); 
	}
	
	ServerConfig() : clientMaxBodySize(101048576), clientMaxHeaderSize(8192) {}
};

class Config
{
	public:
		Config();
		Config(const Config &copy);
		Config &operator=(const Config &other);
		~Config();

		const std::vector<ServerConfig>& servers() const;
		std::vector<ServerConfig>& servers();

	private:
		std::vector<ServerConfig> _servers;
};

#endif
