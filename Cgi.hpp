
#ifndef CGI
# define CGI
# include "main.hpp"
#include "structure.h"
#include "Response.hpp"
#include "main.hpp"


#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
// #include <process.h> /* Required for _spawnv */
// #include <io.h>
#include <fcntl.h>
#include <string>
#include <vector>





class Cgi
{
	private:
		// std::map<std::string, std::string> variables;
		std::vector<std::string> _vars;
		char **_varsArray;
		// std::string _vars;


		// std::map<std::string, std::string> _requestHeaders;
		// std::string 			_requestMethod;
		struct s_config			*_config;
		std::string 			_body;
		std::string 			_pathToCgi;


		

	public:
		// Cgi();
		Cgi(std::string body, struct s_config *config, std::string pathToCgi, std::map<std::string, std::string> requestHeaders, std::string requestMethod);
		Cgi(const Cgi &cgi);
		// ~Cgi();n
		// Cgi &operator=(const Cgi &copy);

		std::vector<std::string>  setVariables(std::map<std::string, std::string> requestHeaders, std::string requestMethod);
		char **getVarsArray(std::vector<std::string> vars);
		void launchCGI();

};



#endif



// https://habr.com/ru/post/254621/
// http://angel07.webservis.ru/perl/env.html
// https://bez-logiki.ru/web-zapisi/zagolovki-zaprosov-i-otvetov-cgi
// http://massnet.narod.ru/chapter7.html
// https://russianblogs.com/article/48051026604/
// http://www.php.su/learnphp/cgi/?interface
// http://www.cyberguru.ru/web/web-programming/cgi-tutor.html?start=14
// http://www.xserver.ru/computer/langprogr/cgi/7/

// std::map<std::string, std::string> mime_types{ {1, 2}, {3, 4}, {6, 5}, {8, 9}, {6, 8}, {3, 4}, {6, 7} };

