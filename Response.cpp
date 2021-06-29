
#include "Response.hpp"

Response::Response() {}

Response::Response(RequestParser &HTTPrequest, struct s_config *config)
: _requestHeaders(HTTPrequest.getHeaders()), _requestBody(HTTPrequest.getBody()), _requestMethod(HTTPrequest.getMetod()), _requestPath(HTTPrequest.getPath()), _requestProtocol(HTTPrequest.getProtokol()), _config(config)
{
	this->_statusCode = "";
	this->_autoindex = false;
}

Response::~Response() {}

Response::Response(const Response &copy){}

Response	&Response::operator=(const Response &copy) { return (*this); }


/*-------------------------------------------------------------------------------*/

/* чтение из файла */
std::string Response::fileToStr(char const filename[])
{
	int fd;

	if ((fd = open(filename, O_RDONLY)) == -1)
		throw Exceptions();
	std::string buf;
	char bufc[32786];
	int ret;
	while ((ret = read(fd, &bufc, 32786)))
	{
		if (ret == -1)
			throw Exceptions();
		buf += std::string(bufc, ret);
	}
	close(fd);
	return buf;
}

/* чтение из файла в _body */
void Response::readBody(std::string &path)
{
	try
	{
		_body = fileToStr(path.c_str());
	}
	catch (std::exception &e)
	{
		this->_statusCode = INTERNALERROR;
	}
}
/*-------------------------------------------------------------------------------*/

/* текущее время */
void Response::setDate()
{
	struct tm tm;
	struct timeval tv;
	char buf[29];
	gettimeofday(&tv, NULL);
	std::string seconds = toString((long long int)tv.tv_sec);
	strptime(seconds.c_str(), "%s", &tm);
	strftime(buf, sizeof(buf), "%a %d %b %Y %T", &tm);
	this->_date = buf;
}

/* установка хедеров */
void Response::writeHeaders(std::string &method)
{
	if (method == "GET")
	{
		_headers["Server"] = "webserv/1.0";
		setDate();
		_headers["Date"] = _date + " GMT";
		_headers["Content-Type"] = "text/html; charset=UTF-8";
		_headers["Title"] = "webserv (project for 21 school)";
		_headers["Content-Language"] = "en,ru";
		_headers["Retry-After"] = "1";
		// _headers["Authorization"] = "Basic qqq : www";
	}
	if (method == "DELETE")
	{
		_headers["Server"] = "webserv/1.0";
		setDate();
		_headers["Date"] = _date + " GMT";
		_headers["Content-Type"] = "text/html; charset=UTF-8";
		// _headers["Title"] = "webserv (project for 21 school)";
		// _headers["Authorization"] = "Basic qqq : www";
	}
}

/*-------------------------------------------------------------------------------*/

/* финальная сборка ответа */
void Response::responseCompose()
{
	_response = "HTTP/1.1 ";
	_response += _statusCode;
	_response += "\r\n";
	// if (this->getBody().size() == 0)
	// 	checkBody();
	writeHeaders(this->_requestMethod);
	std::map<std::string, std::string>::iterator it = _headers.begin();
	std::map<std::string, std::string>::iterator ite = _headers.end();
	while (it != ite)
	{
		_response += (*it).first;
		_response += ": ";
		_response += (*it).second;
		_response += "\r\n";
		it++;
	}
	_response += "Content-Length: ";
	_response += toString(_body.size());
	_response += "\r\n\r\n";
	_response += _body;
}

/*-------------------------------------------------------------------------------*/

// https://www.youtube.com/watch?v=j9yL30R6npk
/* создание _body при включенном автоиндексе */
void Response::autoindexOn()
{
	DIR *dir;
	struct dirent *current;
	std::string body;
	// std::string path_to_open = this->_config->location[i].root
	std::string relativePath = _path + '.';

	dir = opendir(relativePath.c_str());
	if (dir == NULL)
	{
		this->_statusCode = INTERNALERROR;
		// this->_fullPath = _config->error_page + '/' + "404.html";
		// this->readBody(_fullPath);
		// std::cout << "!!!!!!!33333\n";
		return ;
	}
	body = "<html>\n<head>";
	body += "<title>webserv - AutoIndexOn</title>\n"
			"<style> "
			" * { margin: 0; padding: 0; }"
			"h1 { text-align: center; font-size: 25px; margin-top: 30px;}"
			"a { text-decoration: none; color: black; font-size: 20px;}"
			"body { background: rgb(238,174,202); background: radial-gradient(circle, rgba(238,174,202,1) 0%, rgba(148,187,233,1) 100%); }"
			"ul { display: flex; flex-wrap: wrap; flex-direction: column; border-radius: 5px; text-align: center; padding-left: 0; margin-top: 30px;}"
			"li { display: block; border-bottom: 1px solid #673ab7; padding-bottom: 5px;}"
			"</style>\n</head>\n<body>\n";
	body += "<h1>Directory listing:</h1>\n<ul>";
	// current = readdir(dir);
	while ((current = readdir(dir)) != NULL)
	{
		if (current->d_name[0] != '.')
		{
			// printf("%s\n", current->d_name);
			// body += "<a href=\"" + _path + current->d_name + "\">";
			body += "<li><a href=\"";
			body += current->d_name;
			body += "\">";

			body += current->d_name;
			body += "</a></li>";
		}
		// current = readdir(dir);
	}
	closedir(dir);
	body += "</ul></body>\n</html>\n";
	this->_body = body;
}





/*-------------------------------------------------------------------------------*/

/* функция возвращает запрос без .html если есть расширение */
std::string Response::requestPathWithoutHTML(std::string &pathWithHTML)
{
	std::size_t pos = pathWithHTML.find(".html");
	// std::string result = pathWithHTML.substr(0, pos);
	return pathWithHTML.substr(0, pos);
}

/*-------------------------------------------------------------------------------*/

std::string Response::responseInit()
{
	// int isExist = 0; //существует ли location с таким запросом

	// _headers = HTTPrequest.getHeaders();
	// std::cout << _method << " " << _path << " " << _protocol <<'\n';
	// std::cout << _config->error_page << " " << _config->listen << " " << _config->location[0].methods <<'\n';
	// for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end() ; it++)
	// {
	// 	std::cout << it->first << " " << it->second << '\n';
	// }
// https://github.com.cnpmjs.org/42Curriculum/ft_webserv/blob/master/http_serv.cpp
// https://github.com/t0mm4rx/webserv/blob/main/sources/RequestInterpretor.cpp
	/* обработка заголовков */
	// for (std::map<std::string, std::string>::iterator it = _requestHeaders.begin(); it != _requestHeaders.end() ; it++)
	// {
	// 	// std::cout << it->first << " " << it->second << '\n';
	// 	// if (it->first == "Connection:" && it->second == "close")
	// 	// {
	// 	// 	// close(pfd_array[1 + i].fd);
	// 	// 	std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
	// 	// }
	// 	// if (it->first == "")
	// }


	/* проверка max_body_size */
	if (atoi(_requestBody.c_str()) > atoi(_config->max_body_size.c_str()))
	{
		std::cout << "GOOD!!!!!!!!!!!!!!!!!!!!!!@!@!@!@!@!!@!@!!!!@!@!@!@!@!@!@!!@!@!@" << '\n';
		std::cout << atoi(_requestBody.c_str()) << " " << atoi(_config->max_body_size.c_str()) << " \n";
		_statusCode = REQTOOLARGE;
	}
	/* проверка допустимых methods */
	// if 






	if (_requestMethod == "GET")
	{

		if (_requestPath != "/favicon.ico")
		{
			// std::cout << config->listen << '\n';
			for (int i = 0; i < _config->location.size(); i++)
			{
				if (_config->location[i].location == _requestPath || _config->location[i].location == requestPathWithoutHTML(_requestPath))		//если, запрос совпадает с каким-то локейшеном (с маской локейшена); сравниваем также возвтратом функциии, которая образает .html в конце запроса
				{
					_statusCode = OK;
					if (_config->location[i].autoindex == "on")				//если включен автоиндекс
					{
						this->_autoindex = true;
					}
					// else	
					// {
					_path = _config->location[i].root + '/';
					_fullPath = _config->location[i].root + '/' + _config->location[i].index; //_path
					std::cout << _config->location[i].location << " "  << _requestPath << '\n';
					std::cout << _fullPath << '\n';
					// isExist = 1;
					break ;
					// }
				}
			}
			if (_statusCode != OK)				//если не найден файл
			{
					_statusCode = NOTFOUND;
					// _fullPath = _config->error_page + '/' + "404.html";
			}

			/* обработка заголовков */
			// for (std::map<std::string, std::string>::iterator it = _requestHeaders.begin(); it != _requestHeaders.end() ; it++)
			// {
			// 	std::cout << it->first << " " << it->second << '\n';
			// 	// if (it->first == "Connection:" && it->second == "close")
			// 	// {
			// 	// 	// close(pfd_array[1 + i].fd);
			// 	// 	std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
			// 	// }
			// }

			if (_statusCode == OK && _autoindex == true)				/* если автоиндекс включен */	
				this->autoindexOn();
			else if (_statusCode == OK && _autoindex == false)			/* если автоиндекс выключен */		
				this->readBody(_fullPath);
			
			
			
			if (_statusCode != OK)								/* если статускод содержит ошибку */	
			{
				if (_statusCode == NOTFOUND)
					_fullPath = _config->error_page + '/' + "404.html";
				if (_statusCode == REQTOOLARGE)
					_fullPath = _config->error_page + '/' + "413.html";
				if (_statusCode == INTERNALERROR)
					_fullPath = _config->error_page + '/' + "500.html";
				this->readBody(_fullPath);

			}

			// std::cout << _body << '\n';
			// std::cout << _statusCode << '\n';
			responseCompose();
		}
	}





	// if (_requestMethod == "POST")
	// {


	// }

	// if (_requestMethod == "DELETE")
	// {
	// 	if (remove(_requestPath.c_str()) == 0)
	// 	{
	// 		_statusCode = OK;
	// 		_fullPath = "путь к странице уведомляющей об удалении";
	// 		readBody(_fullPath);
	// 		// std::cout <<
	// 	}
	// 	else
	// 	{
	// 		_statusCode = NOTFOUND;
	// 		_fullPath = _config->error_page + '/' + "404.html";
	// 	}
	// 	readBody(_fullPath);
	// 	responseCompose();
	// }













	{ /* вывод сообщения в соответствии со _statusCode*/
		
		if (_statusCode == OK)
			std::cout << GREEN_B << "OK: " << WHITE <<"response will be send to client" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == NOTFOUND)
			std::cout  << RED_B << "KO: " << WHITE <<"content not found, error \"404\" will be send to client" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == INTERNALERROR)
			std::cout  << RED_B << "KO: " << WHITE <<"problems with server settings, error \"500\" will be send to client" << RESET << std::endl << std::endl; //изменить если 404
	}



	{ /* запись ответа в лог-файл */
		std::cout << _response << '\n'; //вывод ответа

		std::ofstream save_response("www/response.log", std::ios::app);

		if (!save_response)
		{
			std::cout << "Uh oh, SomeText.txt could not be opened for writing!" << std::endl;
		}
		else
		{
			save_response << _response << std::endl << std::endl;
			save_response << "=================================================================" << std::endl;
		}
	}





return _response;

}







































// int Server::response(struct pollfd *pfd_array, int &i, RequestParser &HTTPrequest, struct s_config &config)
// {
// 200 используются для успешных запросов.
// 300 для перенаправления.
// 400 используются, если возникла проблема с запросом.
// 500 используются, если возникла проблема с сервером.
//Хедеры на возврат:
//Content-Type: text/html; charset=UTF-8  //Content-Type: image/gif //ТИП ОТВЕТНЫХ ДАННЫХ
// Content-Type: application/zip
// Content-Disposition: attachment; filename="download.zip" //ЕСЛИ ЗАПРОСИЛИ ФАЙЛ
// Content-Length: 89123 //ДЛИНА КОНТЕНТА В БАЙТАХ
// Etag: "pub1259380237;gz" КЕШИРОВАНИЕ ??
// Last-Modified: Sat, 28 Nov 2009 03:50:37 GMT //ДАТА ПОСЛЕДНЕГО ИЗМЕНЕНИЯ
// HTTP/1.x 301 Moved Permanently
// ...
// Location: https://net.tutsplus.com/	//ПЕРЕНАПРАВЛЕНИЕ (также если код 301 или 302)
// Set-Cookie: skin=noskin; path=/; domain=.amazon.com; expires=Sun, 29-Nov-2009 21:42:28 GMT 			//УСТАНОВКА КУКИ
// Set-Cookie: session-id=120-7333518-8165026; path=/; domain=.amazon.com; expires=Sat Feb 27 08:00:00 2010 GMT
// WWW-Authenticate: Basic realm="Restricted Area" //АУТЕНТИФИКАЦИЯ ПО HTTP - браузер запрашивает аутентификацию
// Content-Encoding: gzip // ВОЗВРАЩАЕМОЕ ЗНАЧЕНИЕ СЖИМАЕТСЯ
// func parcerPath (если находим знак ?), то обрабатываем его, если обработать не получается, то рет 0

// ResponsePreparing response;

// 	std::map<std::string, std::string> headers = HTTPrequest.getHeaders();
// 	// std::cout << parseHTTPrequest.getMetod() << " " << parseHTTPrequest.getPath() << " " << parseHTTPrequest.getProtokol() <<'\n';
// 	// for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end() ; it++)
// 	// {
// 	// 	std::cout << it->first << " " << it->second << '\n';
// 	// }

// 	/* обработка method */
// 	std::string method = HTTPrequest.getMetod();

// 	/* обработка path */
// 	std::string path;
// 	int isExist = 0; //существует ли location с таким запросом
// 	for (int i =0; i < config.location.size(); i++)
// 	{
// 		if (config.location[i].location == HTTPrequest.getPath())			//если, у нас не корень и запрос совпадает с каким-то локейшеном (с маской локейшена)
// 		{
// 			path = config.location[i].root + '/' + config.location[i].index;
// 			std::cout << config.location[i].location << " "  << HTTPrequest.getPath() << '\n';
// 			std::cout << path << '\n';
// 			isExist = 1;
// 			break ;
// 		}
// 	}
// 	if (isExist == 0)				//если не найден файл
// 		path = config.error_page + '/' + "404.html";

// 	/* обработка protocol */
// 	std::string protocol = HTTPrequest.getProtokol();



// 	/* обработка заголовков */
// 	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end() ; it++)
// 	{
// 		std::cout << it->first << " " << it->second << '\n';
// 		// if (it->first == "Connection:" && it->second == "close")
// 		// {
// 		// 	// close(pfd_array[1 + i].fd);
// 		// 	std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
// 		// }
// 	}


// 	// std::map<std::string, std::string>::iterator it;
// 	// it = headers.find("Connection:");
// 	// std::cout << it->first << " " << it->second << '\n';


// /* формирование ответа на основе обработанного запроса */
// if (HTTPrequest.getPath() != "/favicon.ico")
// {
// 	std::stringstream response_body;////////////////////////////////////
// 	std::ifstream file; // создаем объект класса ifstream
// 	char *file_buffer = new char[10000 + 1]; file_buffer[10000] = 0;    //поменять!

// // www/site.com/index.html
// 	file.open(path.c_str()); 	//пытаемся открыть файл по запросу

// 	if (!file) 								//нужного контента нету
// 	{
// 		std::ifstream error_404;
// 		file.open("www/default/404.html");
// 		if (!file)
// 		{
// 			std::cout << YELLOW << "error: content not found!" << RESET << std::endl;  //обработать
// 		}
// 		else
// 		{
// 			error_404.read(file_buffer, 10000);
// 			response_body << file_buffer;
// 		}
// 		return -1;
// 	}
// 	else									//контейнт найден
// 	{
// 		std::cout << isExist <<'\n';
// 		if (isExist)
// 			std::cout << GREEN_B << "OK: " << WHITE <<"response will be send to client" << RESET << std::endl << std::endl; //изменить если 404
// 		else
// 			std::cout  << RED_B << "KO: " << WHITE <<"content not found, error-page will be send to client" << RESET << std::endl << std::endl; //изменить если 404
// 		// std::string file_buffer;
// 		// char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
// 		// response_body << file;
// 		file.read(file_buffer, 100000);
// 		// for(file >> file_buffer; !file.eof(); file >> file_buffer)
// 		// 	std::cout << file_buffer;
// 		response_body << file_buffer;
// 	// }


// 		std::stringstream response;
// 		response << "HTTP/1.1 200 OK\r\n"
// 			<< "Version: HTTP/1.1\r\n"
// 		<< "Content-Type: text/html; charset=utf-8\r\n"
// 		<< "Content-Length: " << response_body.str().length()
// 		<< "\r\n\r\n"
// 		<< response_body.str();

// 	if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
// 	{
// 		printf("Error on call 'send': %s\n", strerror(errno));
// 		return -1;
// 	}
// 	}

// }
// 	path.clear();
// 	return 0;
// }





// int Server::response(struct pollfd *pfd_array, int &i)
// {
// 			std::stringstream response_body;////////////////////////////////////
// 				std::ifstream file; // создаем объект класса ifstream
// 				char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;    //поменять!


// 				file.open("www/site.com/index.html"); 	//пытаемся открыть файл по запросу

// 				if (!file) 								//нужного контента нету
// 				{
// 					std::ifstream error_404;
// 					file.open("www/default/404.html");
// 					if (!file)
// 					{
// 						std::cout << YELLOW << "error: content not found!" << RESET << std::endl;
// 					}
// 					else
// 					{
// 						error_404.read(file_buffer, 300);
// 						response_body << file_buffer;
// 					}
// 					return -1;
// 				}
// 				else									//контейнт найден
// 				{
// 					std::cout << std::endl << GREEN_B << "OK: " << WHITE <<"response will be send to client" << RESET << std::endl << std::endl;
// 					// std::string file_buffer;
// 					// char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
// 					// response_body << file;
// 					file.read(file_buffer, 300);
// 					// for(file >> file_buffer; !file.eof(); file >> file_buffer)
// 					// 	std::cout << file_buffer;
// 					response_body << file_buffer;
// 				// }


// 					std::stringstream response;
// 					// if (flag == 1)
// 						response << "HTTP/1.1 200 OK\r\n"
// 						 << "Version: HTTP/1.1\r\n"
// 					<< "Content-Type: text/html; charset=utf-8\r\n"
// 					<< "Content-Length: " << response_body.str().length()
// 					<< "\r\n\r\n"
// 					<< response_body.str();

// 					if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
// 					{
// 						printf("Error on call 'send': %s\n", strerror(errno));
// 						return -1;
// 					}
// 					}
// 					return 0;

// }
