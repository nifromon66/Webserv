#include "../../include/core/Server.hpp"
#include "../../include/config/Config.hpp"
#include "../../include/config/Location.hpp" 
#include "../../include/config/ConfigParser.hpp"

#include <iostream>
#include <cstdlib>
#include <vector>

int main(int argc, char **argv) 
{
	std::string confname;
	if (argc == 1)
		confname = "config/good/default.conf";
	else if (argc == 2)
	{
		confname = argv[1];
		if (confname.size() < 5 || confname.substr(confname.size() - 5) != ".conf")
		{
			std::cerr << "Bad extension used - Usage : *.conf" << std::endl;
			return (1);
		}
	}
	else
	{
		std::cerr << "Usage: " << argv[0] << " <config_file>" << std::endl;
		return (1);
	}

	try
	{
		Config conf;
		ConfigParser parser(confname);
		parser.parseConfig(conf.servers());

		Server server;
		server.initSockets(conf);
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (1);
	}

	return (0);
}
