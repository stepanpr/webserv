
#ifndef CGI
# define CGI
# include "main.hpp"
#include "structure.h"
#include "Response.hpp"





class Cgi
{
	private:
		std::map<std::string, std::string> variables;

	public:
		// Cgi();
		Cgi(std::string body, struct s_config *config);
		Cgi(const Cgi &cgi);
		// ~Cgi();
		Cgi &operator=(const Cgi &copy);

		void setVariables();

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

