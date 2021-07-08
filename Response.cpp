
#include "Response.hpp"

Response::Response() {}

Response::Response(RequestParser &HTTPrequest, struct s_config *config)
: _requestHeaders(HTTPrequest.getHeaders()), _requestBody(HTTPrequest.getBody()), _requestMethod(HTTPrequest.getMetod()),  _requestProtocol(HTTPrequest.getProtokol()), _requestPath(HTTPrequest.getPath()), _config(config)
{
	this->_statusCode = "";
	this->_autoindex = false;
}

Response::~Response() {}

//Response::Response(const Response &copy){}

//Response	&Response::operator=(const Response &copy) { return (*this); }



/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/


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
		std::cout << "!!!!!!!!!!!!!!!!!!READ BODY\n";
		this->_statusCode = INTERNALERROR;
		
	}
}





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
	if (_statusCode == NOTALLOWED) /* поля для кодов ошибок */
		_headers["Allow"] = _allowedMethods;
	if (method == "GET")
	{
		_headers["Server"] = "webserv/1.0";
		setDate();
		_headers["Date"] = _date + " GMT";
		// _headers["Content-Type"] = "text/html; charset=UTF-8";
		_headers["Title"] = "webserv (project for 21 school)";
		_headers["Content-Language"] = "en,ru";
		// _headers["Retry-After"] = "1";
		// _headers["Authorization"] = "Basic qqq : www";
	}
	if (method == "POST")
	{
		_headers["Server"] = "webserv/1.0";
		setDate();
		_headers["Date"] = _date + " GMT";
		// _headers["Content-Type"] = "text/html; charset=UTF-8";
		_headers["Title"] = "webserv (project for 21 school)";
		_headers["Content-Language"] = "en,ru";
		// _headers["Retry-After"] = "1";
		// _headers["Authorization"] = "Basic qqq : www";
	}
    if (method == "HEAD")
    {
        _headers["Server"] = "webserv/1.0";
        setDate();
        _headers["Date"] = _date + " GMT";
        _headers["Title"] = "webserv (project for 21 school)";
        _headers["Content-Language"] = "en,ru";
    }
	if (method == "DELETE")
	{
		_headers["Server"] = "webserv/1.0";
		setDate();
		_headers["Date"] = _date + " GMT";
		_headers["Content-Type"] = "text/html; charset=UTF-8";
		_headers["Title"] = "webserv (project for 21 school)";
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
	if (_requestMethod != "HEAD")
	    _response += _body;

}


/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/



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
		std::cout << "!!!!!!!!!!!!!!!!!!AUTOINDEX\n";
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
			"body { bac     kground: rgb(238,174,202); background: radial-gradient(circle, rgba(238,174,202,1) 0%, rgba(148,187,233,1) 100%); }"
			"ul { display: flex; flex-wrap: wrap; flex-direction: column; border-radius: 5px; text-align: center; padding-left: 0; margin-top: 30px;}"
			"li { display: block; border-bottom: 1px solid #673ab7; padding-bottom: 5px;}"
			"</style>\n</head>\n<body>\n";
	body += "<h1>Autoindex On:</h1>\n<ul>";
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





/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/

/* функция возвращает запрос без .html если есть расширение */
std::string Response::requestPathWithoutHTML(std::string &pathWithHTML)
{
	std::size_t pos;
	if ((pos = pathWithHTML.find("index.html")))
		return pathWithHTML.substr(0, pos);
	else if ((pos = pathWithHTML.find(".html")))
		return pathWithHTML.substr(0, pos);
	return NULL;
}

 /*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/

/* проверка допустимых methods */
bool Response::checkMethod(int &i)
{
//	int ok = 0;

	for (int f = 0; f < (int)_config->location[i].methods.size(); f++)
	{
			_allowedMethods += this->_config->location[i].methods[f];
			// std::cout << this->_config->location[i].methods[f] << " ddddd" << _allowedMethods << '\n';

		if(this->_requestMethod == this->_config->location[i].methods[f])
		{
			return true;
		}
	}
	this->_statusCode = NOTALLOWED;
	return false;
}

/* проверка max_body_size */
bool Response::checkMaxBodySize()
{
	if (atoi(_requestBody.c_str()) > atoi(_config->max_body_size.c_str()))
	{
		this->_statusCode = REQTOOLARGE;
		return false;
	}
	return true;
}


/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/

std::string Response::responseInit()
{
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


	if (_requestMethod == "GET")
	{


		// if (_requestPath != "/favicon.ico")
		// if (_getMimeType("type.html") == "text/html")
		// {
			std::cout << _requestPath.substr(1, _requestPath.size()) << "THIS IS HTML\n";

			// std::cout << config->listen << '\n';
			for (int i = 0; i < (int)_config->location.size(); i++)
			{
				if (_config->location[i].location == _requestPath || (_config->location[i].location ==  _requestPath.substr(0, _requestPath.find(".html"))) 
				|| (_config->location[i].location ==  _requestPath.substr(0, _requestPath.find("index.html")))) // requestPathWithoutHTML(_requestPath))		//если, запрос совпадает с каким-то локейшеном (с маской локейшена); сравниваем также возвтратом функциии, которая образает .html в конце запроса
				{
					if (_config->location[i].autoindex == "on")				//если включен автоиндекс
					{
						this->_autoindex = true;
					}
					if (!checkMethod(i) || !checkMaxBodySize())
					{
						// std::cout << _statusCode << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n";
						break ;	
					}
					// else	
					// {
					_path = _config->location[i].root + '/';
					_fullPath = _config->location[i].root + '/' + _config->location[i].index; //_path
					_headers["Content-Type"] = _getMimeType(_config->location[i].index); /* устанавливаем MIME тип индекса */
					// std::cout << _config->location[i].location << " "  << _requestPath << '\n';
					// std::cout << _fullPath << '\n';
					_statusCode = OK;
					// isExist = 1;
				
					break ;
					// }
				}
				else //if (_config->location[i].location != _requestPath && _config->location[i].location != _requestPath.substr(0, _requestPath.find(".html")))
				{	//если это файл он существует и имеет mime тип, то также отправляем в респонс, записываем в ответ боди
					if (stat("www/response.log", &_stat) == 0 && _getMimeType(_requestPath.substr(0, _requestPath.size())) != MIME_TYPE_NOT_FOUND)
					{
							_statusCode = OK; /* при отключенных locations сайта, покажет ошибку */
							_fullPath = _requestPath.substr(1, _requestPath.size());
							_headers["Content-Type"] = _getMimeType(_requestPath.substr(1, _requestPath.size()));
					}
					// std::cout <<stat("www/response.log", &_stat) << "STAT\n";
					// if (_getMimeType(_requestPath.substr(0, _requestPath.size())) == MIME_TYPE_NOT_FOUND || stat("www/response.log", &_stat) == -1)
					// {
					// 	continue ;
					// }
				}
			}

			/* проверка автоиндекса */
			if (_statusCode == OK && _autoindex == true)				/* если автоиндекс включен */	
				this->autoindexOn();
			else if (_statusCode == OK && _autoindex == false)			/* если автоиндекс выключен */		
				this->readBody(_fullPath);
			
			/* если статускод содержит ошибку */
			if (_statusCode != OK)									
			{
				if (_statusCode != NOTALLOWED && _statusCode != REQTOOLARGE && _statusCode != INTERNALERROR)
				{
					_statusCode = NOTFOUND;
				}
				if (_statusCode == NOTFOUND)
					_fullPath = _config->error_page + '/' + "404.html";
				if (_statusCode == NOTALLOWED)
					_fullPath = _config->error_page + '/' + "405.html";
				if (_statusCode == REQTOOLARGE)
					_fullPath = _config->error_page + '/' + "413.html";
				if (_statusCode == INTERNALERROR)
					_fullPath = _config->error_page + '/' + "500.html";
				_headers["Content-Type"] = _getMimeType("error.html");
				this->readBody(_fullPath);

			}
			// std::cout << _body << '\n';
			// std::cout << _statusCode << '\n';

			/* сборка ответа */
			responseCompose();
	}





	if (_requestMethod == "POST")
	{
		if (_requestMethod == "POST") { std::cout << BLUE << "THIS IS POST!" << RESET << "\n"; }

		/* 
		** https://moskalukov.ru/articles/51-otpravka-fajlov-cherez-formu.html
		** https://github.com.cnpmjs.org/42Curriculum/ft_webserv/blob/master/requests.cpp 
		** https://routerus.com/curl-post-request/ //curl-post-request
		*/
		/* проверяем существование скрипта */
		std::string relativePathToScript = _requestPath.substr(1, _requestPath.length());
		// std::cout << RED << relativePathToScript << RESET <<std::endl;
//		if (stat(relativePathToScript.c_str(), &_stat) >= 0)
//		{
//			// std::cout << RED << "STAT!!!!!!"<< RESET <<std::endl;
//			_statusCode = OK;
//		}
//		else
            _statusCode = OK;
//			_statusCode = NOTFOUND;
		
		//если в данном локейшене есть cgi_path  и  локейшн поддерживает пост
		//и если файл cgi существует stat("www/response.log", &_stat) == 0


		/* проверяем наличие location, а также доступных методов */
		// for (int i = 0; i < _config->location.size(); i++)
		// {
		// 	if (_config->location[i].location == _requestPath || (_config->location[i].location ==  _requestPath.substr(0, _requestPath.find(".html"))) 
		// 	|| (_config->location[i].location ==  _requestPath.substr(0, _requestPath.find("index.html")))) // requestPathWithoutHTML(_requestPath))		//если, запрос совпадает с каким-то локейшеном (с маской локейшена); сравниваем также возвтратом функциии, которая образает .html в конце запроса
		// 	{
		// 		if (_config->location[i].autoindex == "on")				//если включен автоиндекс
		// 		{
		// 			this->_autoindex = true;
		// 		}
		// 		if (!checkMethod(i) || !checkMaxBodySize())
		// 		{
		// 			break ;	
		// 		}
		// 		// else	
		// 		// {
		// 		_path = _config->location[i].root + '/';
		// 		_fullPath = _config->location[i].root + '/' + _config->location[i].index; //_path
		// 		_headers["Content-Type"] = _getMimeType(_config->location[i].index); /* устанавливаем MIME тип индекса */
		// 		_statusCode = OK;
		// 		break ;
		// 		// }
		// 	}
		// 	// else //if (_config->location[i].location != _requestPath && _config->location[i].location != _requestPath.substr(0, _requestPath.find(".html")))
		// 	// {	//если это файл он существует и имеет mime тип, то также отправляем в респонс, записываем в ответ боди
		// 	// 	if (stat("www/response.log", &_stat) == 0 && _getMimeType(_requestPath.substr(0, _requestPath.size())) != MIME_TYPE_NOT_FOUND)
		// 	// 	{
		// 	// 			_statusCode = OK; /* при отключенных locations сайта, покажет ошибку */
		// 	// 			_fullPath = _requestPath.substr(1, _requestPath.size());
		// 	// 			_headers["Content-Type"] = _getMimeType(_requestPath.substr(1, _requestPath.size()));
		// 	// 	}
		// 	// 	// std::cout <<stat("www/response.log", &_stat) << "STAT\n";
		// 	// 	// if (_getMimeType(_requestPath.substr(0, _requestPath.size())) == MIME_TYPE_NOT_FOUND || stat("www/response.log", &_stat) == -1)
		// 	// 	// {
		// 	// 	// 	continue ;
		// 	// 	// }
		// 	// }
		// }

		/* если хедер Content-Length отсутствует или его значение равно нулю то возвращаем меняем статус на BADREQUEST */
		if (_requestHeaders.find("Content-Length:") == _requestHeaders.end() || _requestHeaders.at("Content-Length:") == "0")
		{	
//			std::cout << RED << _requestHeaders.at("Content-Length:") << RESET<< std::endl;
			// _statusCode = BADREQUEST;
		}
		// else
		// 	std::cout << RED << _requestHeaders.at("Content-Length:") << RESET<< std::endl;


		

		if (_statusCode == OK)
		{
			/* application/x-www-form-urlencoded * обработка формы c вызовом CGI */
			if (_requestHeaders.count("Content-Type:") && _requestHeaders.at("Content-Type:") == "application/x-www-form-urlencoded")
			{
				std::cout << YELLOW << _requestHeaders.at("Content-Type:") << RESET<< std::endl;
				std::cout << RED << "1"<<_requestBody <<RESET <<std::endl;
                if (stat(relativePathToScript.c_str(), &_stat) >= 0)
                {
                    // std::cout << RED << "STAT!!!!!!"<< RESET <<std::endl;
//                     if (_config->location[this].CGI)
//				        Cgi cgi(_requestBody, _config);
                    _statusCode = OK;
                }
                else
                    _statusCode = NOTFOUND;

			}
			
			/* multipart/form-data * обработка отправки файла */
			else if (_requestHeaders.count("Content-Type:") && _requestHeaders.at("Content-Type:").find("multipart/form-data") != std::string::npos)
			{
				std::cout << YELLOW << _requestHeaders.at("Content-Type:") << RESET<< std::endl;
				std::cout << RED << "2" <<_requestBody << RESET << std::endl;


				//отезать вебкит и все что за ним следует
				std::ofstream w;
				w.open("file", std::ios::out | std::ios::binary);
				w << _requestBody;
			}
			else
			    _statusCode = NOTALLOWED;
              //_fullPath = _config->location[0].root + '/' + _config->location[0].index;

			// for (std::map<std::string, std::string>::iterator it = _requestHeaders.begin(); it != _requestHeaders.end() ; it++)
			// {
				
			// 	/* проверка на локейшн ?*/
			// 	// std::cout << it->first << " " << it->second << '\n';

			// 	/* обработка формы c вызовом CGI */
			// 	if (it->first == "Content-Type:" && it->second == "application/x-www-form-urlencoded")
			// 	{
			// 		std::cout << it->first << " " << it->second << '\n';
			// 		std::cout <<RED << "1"<<_requestBody <<RESET <<std::endl;
			// 		// if (_config->location[this].CGI)
			// 		// 	Cgi cgi(_requestBody, _config);
			// 	}

			// 	/* обработка отправки файла */
			// 	if (it->first == "Content-Type:" && it->second.find("multipart/form-data") != std::string::npos)
			// 	{
			// 		std::cout << it->first << " " << it->second << '\n';
			// 		std::cout << RED << "2" <<_requestBody << RESET << std::endl;
			// 		// std::cout <<CYAN <<_requestHeaders <<RESET <<std::endl;
			// 		// std::cout <<CYAN <<_requestHeaders <<RESET <<std::endl;

			// 		// char buff[100000];
			// 		// std::string line;
			// 		// std::ifstream r("www/file.tmp");
			// 		// if (r.is_open())
			// 		// {
			// 			std::ofstream w;
			// 			w.open("file", std::ios::out | std::ios::binary); 
			// 			// while (getline(_requestBody.c_str(), line))
			// 			// {
			// 				w << _requestBody;
			// 				// std::cout << line << std::endl;
			// 			// }
			// 		// }
			// 		// std::remove("www/file.tmp");
			// 		// std::cout <<RED << "1" << line <<RESET <<std::endl;
			// 		// file << _requestBody;
			// 		// std::cout << it->first << " " << it->second << "OKOKOKOKOKOKOKOKOKKOK" << it->second.find("ddd") <<'\n';
			// 		// if ( it->second.find("multipart/form-data") == std::string::npos)
			// 		// 	std::cout << "NON FOUND!!!!!!!" <<'\n';

			// 	}
			// 	// if ()

			// }
		}


		/* проверка автоиндекса */
		if (_statusCode == OK && _autoindex == true)				/* если автоиндекс включен */	
			this->autoindexOn();
		else if (_statusCode == OK && _autoindex == false)			/* если автоиндекс выключен */		
			this->readBody(_fullPath);
		
		/* если статускод содержит ошибку */
		if (_statusCode != OK)									
		{
			if (_statusCode != BADREQUEST && _statusCode != NOTALLOWED && _statusCode != REQTOOLARGE && _statusCode != INTERNALERROR)
			{	/* защита от возможных непредвиденных ошибок - в этом случае выводи 404 */
				_statusCode = NOTFOUND;
			}
			if (_statusCode == BADREQUEST)
				_fullPath = _config->error_page + '/' + "400.html";
			if (_statusCode == NOTFOUND)
				_fullPath = _config->error_page + '/' + "404.html";
			if (_statusCode == NOTALLOWED)
				_fullPath = _config->error_page + '/' + "405.html";
			if (_statusCode == REQTOOLARGE)
				_fullPath = _config->error_page + '/' + "413.html";
			if (_statusCode == INTERNALERROR)
				_fullPath = _config->error_page + '/' + "500.html";
			_headers["Content-Type"] = _getMimeType("error.html");
			this->readBody(_fullPath);

		}
			// std::cout << _body << '\n';
			// std::cout << _statusCode << '\n';

			/* сборка ответа */
			responseCompose();
		// for (std::map<std::string, std::string>::iterator it = _requestHeaders.begin(); it != _requestHeaders.end() ; it++)
		// {
		// 	if (it->first == "Content-Disposition:")
		// 		std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
		// 	// 
		// 	// std::cout << it->first << " " << it->second << '\n';
		// 	if (it->first == "Connection:")
		// 	 	 std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
		// 	// {
		// 	// 	// close(pfd_array[1 + i].fd);
		// 	// 	std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
		// 	// }
		// 	// if (it->first == "")
		// }
		// 	// _requestHeaders.find("Content-Disposition");
		// _statusCode = OK;
		// 	std::cout << BLUE << _requestHeaders.find("Content-Disposition:")->second << RESET << "\n";
		// 	std::cout << "BODY:" << BLUE << _requestBody << RESET << "\n";

		// responseCompose();

	}

    if (_requestMethod == "HEAD")
    {
        _statusCode = OK;
        _fullPath = _config->location[0].root + '/' + _config->location[0].index;
        this->readBody(_fullPath);
        responseCompose();

    }

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
		else if (_statusCode == NOTALLOWED)
			std::cout  << RED_B << "KO: " << WHITE <<"request method not allowed, error \"405\" will be send to client" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == REQTOOLARGE)
			std::cout  << RED_B << "KO: " << WHITE <<"request is too large, error \"404\" will be send to client" << RESET << std::endl << std::endl; //изменить если 404
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










/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/
/*==================================================================================================*/

std::string Response::_getMimeType(std::string filename)
{

	std::map<std::string, std::string> m;
	// std::cout << filename << "!!!!!!!!!!!!init!!!!!!!!!!!\n";

	int i;	
	for(i = filename.size(); i >= 0 && filename[i] != '.' && filename[i] != '/'; --i)
	{
		// std::cout << i << " : " << filename[i] << "\n";
		if ((i) == 0)
		{
			// std::cout << filename[i] << "\n";
			// std::cout << "| " << filename << "!!!!!!!!!!!!init!!!!!!!!!!!\n";
			
			return ("text/html; charset=UTF-8");
		}
		// if (filename[i] !='.')
	}

	std::string extention = std::string(filename, i + 1, filename.size() - i);

	m["aac"] = "audio/aac";
	m["abw"] = "application/x-abiword";
	m["arc"] = "application/octet-stream";
	m["avi"] = "video/x-msvideo";
	m["azw"] = "application/vnd.amazon.ebook";
	m["bin"] = "application/octet-stream";
	m["bmp"] = "image/bmp";
	m["bz"] = "application/x-bzip";
	m["bz2"] = "application/x-bzip2";
	m["csh"] = "application/x-csh";
	m["css"] = "text/css";
	m["csv"] = "text/csv";
	m["doc"] = "application/msword";
	m["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	m["eot"] = "application/vnd.ms-fontobject";
	m["epub"] = "application/epub+zip";
	m["gif"] = "image/gif";
	m["htm"] = "text/html";
	m["html"] = "text/html";
	m["ico"] = "image/x-icon";
	m["ics"] = "text/calendar";
	m["jar"] = "application/java-archive";
	m["jpeg"] = "image/jpeg";
	m["jpg"] = "image/jpeg";
	m["js"] = "application/javascript";
	m["json"] = "application/json";
	m["mid"] = "audio/midi";
	m["midi"] = "audio/midi";
	m["mpeg"] = "video/mpeg";
	m["mpkg"] = "application/vnd.apple.installer+xml";
	m["odp"] = "application/vnd.oasis.opendocument.presentation";
	m["ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	m["odt"] = "application/vnd.oasis.opendocument.text";
	m["oga"] = "audio/ogg";
	m["ogv"] = "video/ogg";
	m["ogx"] = "application/ogg";
	m["otf"] = "font/otf";
	m["png"] = "image/png";
	m["pdf"] = "application/pdf";
	m["ppt"] = "application/vnd.ms-powerpoint";
	m["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	m["rar"] = "application/x-rar-compressed";
	m["rtf"] = "application/rtf";
	m["sh"] = "application/x-sh";
	m["svg"] = "image/svg+xml";
	m["swf"] = "application/x-shockwave-flash";
	m["tar"] = "application/x-tar";
	m["tif"] = "image/tiff";
	m["tiff"] = "image/tiff";
	m["ts"] = "application/typescript";
	m["ttf"] = "font/ttf";
	m["vsd"] = "application/vnd.visio";
	m["wav"] = "audio/x-wav";
	m["weba"] = "audio/webm";
	m["webm"] = "video/webm";
	m["webp"] = "image/webp";
	m["woff"] = "font/woff";
	m["woff2"] = "font/woff2";
	m["xhtml"] = "application/xhtml+xml";
	m["xls"] = "application/vnd.ms-excel";
	m["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	m["xml"] = "application/xml";
	m["xul"] = "application/vnd.mozilla.xul+xml";
	m["zip"] = "application/zip";
	m["3gp"] = "audio/3gpp";
	m["3g2"] = "audio/3gpp2";
	m["7z"] = "application/x-7z-compressed";
	m["txt"] = "text/plain";
	m["txt"] = "text/*";
	// std::cout << extention << " : " << m[extention] << "\n";
	// return "type_not_found";

	if (m.count(extention))
	{	
		_validMymeType = true;
		return (m[extention]);
	}
	// return ("application/octet-stream");
	// std::cout << "| " << filename << "!!$$$$$$$$$$$$$$$$$$$$$$$$\n";

	_validMymeType = false;
	return MIME_TYPE_NOT_FOUND;
}