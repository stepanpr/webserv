#include "Config.hpp"

Config::Config(): _path(std::string(DEFAULT_CONFIG_PATH))
{
}

Config::~Config()
{
}

Config::Config(const Config &copy)
{
}

Config	&Config::operator=(const Config &copy)
{
	return (*this);
}

bool Config::isValid()
{
	//check if configuration file argv[1] is valid
	return true;
}

void Config::setPath(const std::string &path)
{
	_path = path;
}
