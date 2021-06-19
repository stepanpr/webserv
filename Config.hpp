#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <iostream>
# include <string>
# include "Server.hpp"

# define DEFAULT_CONFIG_PATH "default.conf"

class Config
{
public:
	Config();
	Config(const Config &copy);
	~Config();
	Config &operator=(const Config &copy);

	void setPath(const std::string &path);

	bool isValid();

	Server *createServers();

private:
	std::string _path;
};

#endif
