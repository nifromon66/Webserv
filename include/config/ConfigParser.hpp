#ifndef CONFIGPARSER_HPP
# define CONFIGPARSER_HPP

#include "../config/Config.hpp"
#include "../config/Location.hpp"
#include "../httpResponse/FileUtils.hpp"
#include "../Utils/HttpNormalize.utils.hpp"

#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <cstdlib>

#define DELIMITERS "{};"
#define WHITESPACE " \t\n\r"

typedef std::vector<std::string>::iterator token_iterator;

class ConfigParser
{
    public:

        ConfigParser(const std::string& configFilePath = "");
        ConfigParser(const ConfigParser &copy);
        ConfigParser &operator=(const ConfigParser &other);
        ~ConfigParser();

        void parseConfig(std::vector<ServerConfig>& serverConfigs);

        std::vector<ServerConfig>& getServers(); 

    private:

        std::string                   _configFilePath;
        std::vector<ServerConfig>     _servers; 

        std::string                     readFileContent();
        std::vector<std::string>        tokenize(const std::string& content);

        void	parseServerBlock(ServerConfig& currentServerConfig, 
                                token_iterator& it, 
                                const std::vector<std::string>& allTokens);

        void	parseLocationBlock(LocationConfig& currentLocation, 
                                token_iterator& it, 
                                const std::vector<std::string>& allTokens);     

        void    routeDirective(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens);

        // Fonctions de parsing spécifiques au contexte SERVER
        void    parseListen(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens);
        void    parseRoot(ServerConfig& currentServerConfig, token_iterator& it);
        void    parseServerName(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens);
        void    parseErrorPage(ServerConfig& currentServerConfig, token_iterator& it, const std::vector<std::string>& allTokens);
        void    parseServerMaxBodySize(ServerConfig& config, token_iterator& it, const std::vector<std::string>& tokens);
        void    parseServerIndex(ServerConfig& config, token_iterator& it, const std::vector<std::string>& allTokens);

        // Routeur de niveau LOCATION
        void    routeLocationDirective(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens);

      // --- Fonctions de parsing spécifiques au contexte LOCATION ---
        void    parseLocationRoot(LocationConfig& currentLocation, token_iterator& it);
        void    parseLocationIndex(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens);
        void    parseLocationAllowMethods(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens);
        void    parseLocationMaxBodySize(LocationConfig& currentLocation, token_iterator& it);
        void    parseLocationMaxHeaderSize(LocationConfig& currentLocation, token_iterator& it);
        void    parseLocationUploadDir(LocationConfig& currentLocation, token_iterator& it);
        void    parseLocationCgiPass(LocationConfig& currentLocation, token_iterator& it, const std::vector<std::string>& allTokens);
        void    finalizeLocation(LocationConfig& loc, const ServerConfig& server);
        void    parseLocationAutoIndex(LocationConfig& currentLocation, token_iterator& it);
        void    parseLocationReturn(LocationConfig& loc, token_iterator& it, const std::vector<std::string>& allTokens);

        long    parseSize(const std::string& value);
};

class ConfigParserException : public std::exception
{
    public:
        ConfigParserException(const std::string& msg);
        virtual ~ConfigParserException() throw() {}
        virtual const char* what() const throw() { return _msg.c_str(); }
    private:
        std::string _msg;
};

#endif
