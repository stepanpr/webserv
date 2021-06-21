
#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <iostream>
# include <sstream>
# include <fstream>
# include <string>
# include <map>
# include <vector>
# include <iomanip>
#include <unistd.h>
# include "Server.hpp"
# define DEFAULT_CONFIG_PATH "config/default.conf"

class Server;

typedef struct 	s_location
{
	std::string location;
	std::string index;
	std::string autoindex;
	std::string methods;
	std::string root;
}				t_location;

typedef struct 	s_config
{
	std::string listen;
	// int port;
	std::string server_name;
	std::string error_page;
	std::vector<t_location> location;	
}				t_config;

class Config
{
	private:
		bool _isValid; 
		std::string _path;
		int _numOfServers;
		std::vector<t_config> _configs;
		std::vector<Server> _servers;


	public:
		/* constructors, destructor */
		Config();								//записываем в _path дефолтный конфиг
		Config(std::string const &path);		//записываем в _path конфиг который передали
		Config(const Config &copy);
		~Config();
		Config &operator=(const Config &copy);

		/* setters, getters */
		void setPath(const std::string &path);
		std::string getPath();
		std::vector<t_config> getConfigs();
		bool isValid(); //возвращаем bool - is valid or no

		/* config parser/handler */
		void load(std::string path);
		void read(std::vector<std::string> &data);
		void showConfig();


		/*  congig handler */
		void createServers();


};



//enter для продолжения
template<typename M> 
void enter(M message)
{
	int i = -1;
	std::cout << std::setw(30) << std::endl << message << std::endl;
	while (1)
	{
		char str = std::cin.get();
		if (str == 0x0A)
			break ;
		++i;
	}
}

# define RED  "\e[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\e[0;33m"
# define BLUE "\033[0;34m"
# define PURPLE "\033[0;35m"
# define CYAN "\e[0;36m"
# define WHITE "\e[0;37m"
# define RED_B "\033[1;31m"
# define GREEN_B "\033[1;32m"
# define YELLOW_B "\e[1;33m"
# define BLUE_B "\033[1;34m"
# define PURPLE_B "\033[1;35m"
# define CYAN_B "\e[1;36m"
# define WHITE_B "\e[1;37m"
# define RESET "\033[0m"

#endif





















































// https://demin.ws/blog/russian/2009/02/08/reading-file-via-stl-iterators/
// https://server.179.ru/tasks/cpp/total/161.html

// https://www.8host.com/blog/algoritmy-vybora-blokov-server-i-location-v-nginx/

// https://community.vscale.io/hc/ru/community/posts/209008505-%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0-%D0%B8-%D0%BF%D0%B5%D1%80%D0%B2%D0%B8%D1%87%D0%BD%D0%B0%D1%8F-%D0%BD%D0%B0%D1%81%D1%82%D1%80%D0%BE%D0%B9%D0%BA%D0%B0-Nginx

// internet-technologies.ru/articles/reshenie-problem-svyazannyh-s-oshibkoy-nginx-403-forbidden.html