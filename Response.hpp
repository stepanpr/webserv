
#ifndef RESPONSE_HPP
# define RESPONSE_HPP
# include "main.hpp"

/* https://zametkinapolyah.ru/servera-i-protokoly/tema-9-http-kody-sostoyaniya-klassy-kodov-sostoyaniya-http-servera.html */
# define OK 			"200 OK"
# define CREATED		"201 Created"
# define NOCONTENT 		"204 No Content"
# define BADREQUEST		"400 Bad Request"
# define UNAUTHORIZED	"401 Unauthorized"
# define NOTFOUND 		"404 Not Found"
# define NOTALLOWED		"405 Method Not Allowed"
# define REQTOOLARGE	"413 Payload Too Large"
# define INTERNALERROR	"500 Internal Server Error"
# define NOTIMPLEMENTED	"501 Not Implemented"

class Response
{
public:
	Response();
	Response(RequestParser &HTTPrequest, struct s_config *config);
	Response(const Response &copy);
	~Response();
	Response &operator=(const Response &copy);



	std::string responseInit();
	void readBody(std::string &path);
	std::string fileToStr(char const filename[]);
	void writeHeaders();
	void setDate();
	void responseCompose();


private:
	/* поля с данными запроса */
	std::map<std::string, std::string> _requestHeaders;
	std::string _requestMethod;
	std::string _requestProtocol;
	std::string _requestPath;


	/* данные конфигурационного файла */
	struct s_config			*_config;


	/* поля ответа*/
	std::string _fullPath;
	std::string _statusCode;
	std::string _date;
	std::map<std::string, std::string> _headers;
	std::string _body;


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
