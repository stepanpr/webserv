
#include "Response.hpp"

Response::Response()
{
}

Response::Response(RequestParser &HTTPrequest, struct s_config *config)
: _requestHeaders(HTTPrequest.getHeaders()), _requestMethod(HTTPrequest.getMetod()), _requestPath(HTTPrequest.getPath()), _requestProtocol(HTTPrequest.getProtokol()), _config(config)
{

}


Response::~Response()
{
}

Response::Response(const Response &copy)
{
}

Response	&Response::operator=(const Response &copy)
{
	return (*this);
}


/*-------------------------------------------------------------------------------*/

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
		_headers["Authorization"] = "Basic qqq : www";
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


std::string Response::responseInit()
{
	int isExist = 0; //существует ли location с таким запросом

	// _headers = HTTPrequest.getHeaders();
	// std::cout << _method << " " << _path << " " << _protocol <<'\n';
	// std::cout << _config->error_page << " " << _config->listen << " " << _config->location[0].methods <<'\n';
	// for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end() ; it++)
	// {
	// 	std::cout << it->first << " " << it->second << '\n';
	// }

	if (_requestMethod == "GET")
	{

		if (_requestPath != "/favicon.ico")
		{
			// std::cout << config->listen << '\n';
			for (int i = 0; i < _config->location.size(); i++)
			{
				if (_config->location[i].location == _requestPath)			//если, у нас не корень и запрос совпадает с каким-то локейшеном (с маской локейшена)
				{
					_statusCode = OK;
					_fullPath = _config->location[i].root + '/' + _config->location[i].index; //_path
					std::cout << _config->location[i].location << " "  << _requestPath << '\n';
					std::cout << _fullPath << '\n';
					isExist = 1;
					break ;
				}
			}
			if (isExist == 0)				//если не найден файл
			{
					_statusCode = NOTFOUND;
					_fullPath = _config->error_page + '/' + "404.html";
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
		
			readBody(_fullPath);
			// std::cout << _body << '\n';
			// std::cout << _statusCode << '\n';
			responseCompose();
			std::cout << _response << '\n';
		}
	}

	if (_requestMethod == "POST")
	{

	}

	if (_requestMethod == "DELETE")
	{
		if (remove(_requestPath.c_str()) == 0)
		{
			_statusCode = OK;
			_fullPath = "путь к странице уведомляющей об удалении";
			readBody(_fullPath);
			// std::cout <<
		}
		else
		{
			_statusCode = NOTFOUND;
			_fullPath = _config->error_page + '/' + "404.html";
		}
		readBody(_fullPath);
		responseCompose();
	}



	/* вывод сообщения в соответствии со _statusCode*/
	if (_statusCode == OK)
		std::cout << GREEN_B << "OK: " << WHITE <<"response will be send to client" << RESET << std::endl << std::endl; //изменить если 404
	else if (_statusCode == NOTFOUND)
		std::cout  << RED_B << "KO: " << WHITE <<"content not found, error \"404\" will be send to client" << RESET << std::endl << std::endl; //изменить если 404

// 	// std::map<std::string, std::string>::iterator it;
// 	// it = headers.find("Connection:");
// 	// std::cout << it->first << " " << it->second << '\n';

	// std::stringstream response;
// 	std::stringstream response_body;////////////////////////////////////

// /* формирование ответа на основе обработанного запроса */
// if (_requestPath != "/favicon.ico")
// {
// 	std::ifstream file; // создаем объект класса ifstream
// 	char *file_buffer = new char[10000 + 1]; file_buffer[10000] = 0;    //поменять! просчтитать длину файла и выделить столько памяти.

// // www/site.com/index.html
// 	file.open(_fullPath.c_str()); 	//пытаемся открыть файл по запросу

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
// 		// return -1;
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


		
// 		response << "HTTP/1.1 200 OK\r\n"
// 			<< "Version: HTTP/1.1\r\n"
// 		<< "Content-Type: text/html; charset=utf-8\r\n"
// 		<< "Content-Length: " << response_body.str().length()
// 		<< "\r\n\r\n"
// 		<< response_body.str();

// 		// if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
// 		// {
// 		// 	printf("Error on call 'send': %s\n", strerror(errno));
// 		// 	return -1;
// 		// }
// 		file.close();
// 		response_body.clear();
// 	}

// }
// 	_fullPath.clear();





// return response.str();
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
