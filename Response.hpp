
#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "main.hpp"
// #include <stdio.h>
// #include <stdlib.h>
// #include <io.h>
#include <dirent.h>
// #include <time.h>

/* 	https://zametkinapolyah.ru/servera-i-protokoly/tema-9-http-kody-sostoyaniya-klassy-kodov-sostoyaniya-http-servera.html 
	https://ru.wikipedia.org/wiki/%D0%A1%D0%BF%D0%B8%D1%81%D0%BE%D0%BA_%D0%B7%D0%B0%D0%B3%D0%BE%D0%BB%D0%BE%D0%B2%D0%BA%D0%BE%D0%B2_HTTP 
	http://www.cyberguru.ru/web/web-programming/cgi-tutor.html?start=11 */

# define OK 			"200 OK"
// # define CREATED		"201 Created"
// # define NOCONTENT 		"204 No Content"
// # define BADREQUEST		"400 Bad Request"
// # define UNAUTHORIZED	"401 Unauthorized"
# define NOTFOUND 		"404 Not Found"
# define NOTALLOWED		"405 Method Not Allowed"
# define REQTOOLARGE	"413 Request Entity Too Large"
# define INTERNALERROR	"500 Internal Server Error"
// # define NOTIMPLEMENTED	"501 Not Implemented"

class Response
{
public:
	Response();
	Response(RequestParser &HTTPrequest, struct s_config *config);
	Response(const Response &copy);
	~Response();
	Response &operator=(const Response &copy);



	std::string responseInit();
	std::string requestPathWithoutHTML(std::string &path);
	bool checkMethod(int &i);
	bool checkMaxBodySize();
	void readBody(std::string &path);
	std::string fileToStr(char const filename[]);
	void writeHeaders(std::string &method);
	void setDate();
	void autoindexOn();
	void responseCompose();


private:
	/* поля с данными запроса */
	std::map<std::string, std::string> _requestHeaders;
	std::string _requestBody;
	std::string _requestMethod;
	std::string _requestProtocol;
	std::string _requestPath;


	/* данные конфигурационного файла */
	struct s_config			*_config;


	/* поля ответа*/
	std::string _path; 
	std::string _fullPath; //с index-файлом
	std::string _statusCode;
	std::string _date;
	std::map<std::string, std::string> _headers;
	std::string _body;
	bool 		_autoindex;


	/* укомплектовынный ответ */
	std::string _response;
};


template <typename T>
std::string toString(T val)
{
    std::ostringstream oss;
    oss<< val;
    return oss.str();
}


#endif
