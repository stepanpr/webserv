
#ifndef CONFIG_HPP
# define CONFIG_HPP
# include <iostream>
# include <fstream>
# include <string>
# include <map>
# include "Server.hpp"
# define DEFAULT_CONFIG_PATH "defaut.conf"


typedef struct s_config
{
	std::string listen;
	int port;
	std::string server_name;	
}	t_config;

class Config
{
	private:
		bool _isValid; 
		std::string _path;

	// 	std::string key[10] = {
	// 	"port", 
	// 	"server_name", 
	// 	"err_page", 
	// 	"body_size",
	// 	"search_dir", 
	// 	"dir_listing", 
	// 	"default_file",
	// 	"CGI_extension", 
	// 	"uploads",
	// 	"uploads_dir"
	// 	};
	// std::string defaults[10] = {
	// 	"port", 
	// 	"server_name", 
	// 	"err_page", 
	// 	"body_size",
	// 	"search_dir", 
	// 	"dir_listing", 
	// 	"default_file",
	// 	"CGI_extension", 
	// 	"uploads",
	// 	"uploads_dir"
	// 	};

	public:
		Config();
		Config(const Config &copy);
		~Config();
		Config &operator=(const Config &copy);





		std::map<Server, t_config> servers;




		/* setters, getters */
		void setPath(const std::string &path);
		bool isValid(); //возвращаем bool - is valid or no

		/*  congig handler */
		void readConfig();
		void load();
		void parce();





		/*  congig handler */
		Server *createServers();


};

#endif
