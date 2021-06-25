#ifndef WEBSERV_TEAM_STRUCTURE_H
# define WEBSERV_TEAM_STRUCTURE_H

# include <string>
# include <vector>

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
	int serverID;
	std::string listen;
	std::string server_name;
	std::string error_page;
	std::vector<t_location> location;
}				t_config;

#endif
