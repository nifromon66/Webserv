#include "../../include/config/Config.hpp"

Config::Config( void )
{}

Config::Config(const Config &copy) : _servers(copy._servers)
{}

Config& Config::operator=(const Config &other)
{
	if (this != &other)
	{
		this->_servers = other._servers;  
	} 
	return *this;
}

Config::~Config()
{}

std::vector<ServerConfig>& Config::servers()
{
	return _servers;
}

const std::vector<ServerConfig>& Config::servers() const
{
	return _servers;
}