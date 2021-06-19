#include <iostream>
#include "Config.hpp"

int	main(int argc, char **argv)
{
	Config config;
	// std::vector<Config> servers;

	if (argc > 2)
	{
		std::cout << "webserv: wrong number of arguments" << std::endl;
		std::cout << "usage: ./webserv [path_to_configuration_file]" << std::endl;
		return (1);
	}
	else if (argc == 2)
		config.setPath(std::string(argv[1]));
	config.readConfig();
	if (!config.isValid())
	{
		std::cout << "webserv: configuration file syntax is wrong" << std::endl;
		return (1);
	}
	// Server *servers = config.createServers();
	config.createServers();

}