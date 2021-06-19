#include "Config.hpp"

Config::Config(): _path(std::string(DEFAULT_CONFIG_PATH)), _isValid(true)
{
	t_config a;
	// a.listen = "dfsdfd";
	// a.port = 2;
	Server s;

	// for(std::map<Server, t_config>::iterator i_beg = servers.begin(); i_beg != servers.end(); i_beg++)
	// {
	// 	std::cout << i_beg->second.listen; 
	// 	// servers[]
	// }
	// servers[s] = a;
	// servers.insert(std::pair<Server, t_config>(s,a));
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

/*-------------------------------------------------------------------------------*/

void Config::setPath(const std::string &path)
{
	_path = path;
}

bool Config::isValid()
{
	return _isValid;
}

/*-------------------------------------------------------------------------------*/

void Config::readConfig()
{
	load();
	parce();
}

void Config::load()
{
	// std::vector<Data> servers;
	std::ifstream file;
	// int pos = 0, start = 0;
	file.open(_path);
	if (!(file.is_open()))
	{
		std::cout << "Config path is bad\n";
		// servers.push_back(Data());
		// return servers;
		_isValid = false;
		return ;
	}
	else
	{
		std::cout << "Config file found!\n";
	}
	// std::string content(std::istreambuf_iterator<char>(file),  (std::istreambuf_iterator<char>()));
	// while(pos  < content.length() - 1)
	// {
	// 	start = content.find("server", pos);
	// 	pos =  content.find("}", start);
	// 	// servers.push_back(Data(content.substr(start, pos)));
	// }
	// return servers;
}

void Config::parce()
{

}
















/*-------------------------------------------------------------------------------*/


Server *Config::createServers()
{
	Server *d;
	// for(int i = 0; i < servers.size(); i++)
	// {
	// 	// std::cout << 
	// 	servers[]
	// }
	
	for(std::map<Server, t_config>::iterator i_beg = servers.begin(); i_beg != servers.end(); i_beg++)
	{
		std::cout << i_beg->second.listen; 
		// servers[]
	}

	return d;
}
