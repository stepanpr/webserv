
#include "Response.hpp"

Response::Response() {}

Response::Response(RequestParser &HTTPrequest, struct s_config *config)
: _requestHeaders(HTTPrequest.getHeaders()), _requestBody(HTTPrequest.getBody()), _requestMethod(HTTPrequest.getMetod()),  _requestProtocol(HTTPrequest.getProtokol()), _requestPath(HTTPrequest.getPath()), _config(config), _fileName(HTTPrequest.getFileName())
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

	// aIdx = 1;
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


	if (_requestMethod == "GET" || _requestMethod == "HEAD")
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

					// if (stat((_config->location[i].root + '/' + _requestPath.substr(1, _requestPath.size())).c_str(), &_stat) == 0 
					// && _getMimeType(_requestPath.substr(0, _requestPath.size())) != MIME_TYPE_NOT_FOUND /*&& aIdx == 1*)
					// // && _getMimeType(_requestPath.substr(0, _requestPath.size())) != "text/html")
					// {
					// 	std::cout << RED << _getMimeType(_requestPath.substr(0, _requestPath.size()))  << RESET <<std::endl;
					// 		_statusCode = OK; /* при отключенных locations сайта, покажет ошибку */
					// 		// _fullPath = _requestPath.substr(1, _requestPath.size());
					// 		_fullPath = _config->location[i].root + '/' + _requestPath.substr(1, _requestPath.size());
					// 		_headers["Content-Type"] = _getMimeType(_requestPath.substr(1, _requestPath.size()));
					// 		// aIdx = 0;
					// }


					if (stat("www/response.log", &_stat) == 0 && _getMimeType(_requestPath.substr(0, _requestPath.size())) != MIME_TYPE_NOT_FOUND /*&& aIdx == 0*/)
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
			// if (_statusCode != OK)									
			// {
			// 	if (_statusCode != NOTALLOWED && _statusCode != REQTOOLARGE && _statusCode != INTERNALERROR)
			// 	{
			// 		_statusCode = NOTFOUND;
			// 	}
			// 	if (_statusCode == NOTFOUND)
			// 		_fullPath = _config->error_page + '/' + "404.html";
			// 	if (_statusCode == NOTALLOWED)
			// 		_fullPath = _config->error_page + '/' + "405.html";
			// 	if (_statusCode == REQTOOLARGE)
			// 		_fullPath = _config->error_page + '/' + "413.html";
			// 	if (_statusCode == INTERNALERROR)
			// 		_fullPath = _config->error_page + '/' + "500.html";
			// 	_headers["Content-Type"] = _getMimeType("error.html");
			// 	this->readBody(_fullPath);

			// }
			// std::cout << _body << '\n';
			// std::cout << _statusCode << '\n';

			/* сборка ответа */
			// responseCompose();
	}





	if (_requestMethod == "POST")
	{
		if (_requestMethod == "POST") { std::cout << BLUE << "THIS IS POST!" << RESET << "\n"; }

		/* 
		** https://moskalukov.ru/articles/51-otpravka-fajlov-cherez-formu.html
		** https://github.com.cnpmjs.org/42Curriculum/ft_webserv/blob/master/requests.cpp 
		** https://routerus.com/curl-post-request/ //curl-post-request
		*/

        _statusCode = OK;

		/* если хедер Content-Length отсутствует или его значение равно нулю то возвращаем меняем статус на BADREQUEST */
		// if (_requestHeaders.find("Content-Length:") == _requestHeaders.end() || _requestHeaders.at("Content-Length:") == "0")
		// {	
		// 	// std::cout << RED << _requestHeaders.at("Content-Length:") << RESET<< std::endl;
		// 	// _statusCode = BADREQUEST;
		// }
		// else
		// 	std::cout << RED << _requestHeaders.at("Content-Length:") << RESET<< std::endl;

		if (_statusCode == OK)
		{
			/* application/x-www-form-urlencoded * обработка формы c вызовом CGI */
			if (_requestHeaders.count("Content-Type:") && _requestHeaders.at("Content-Type:") == "application/x-www-form-urlencoded")
			{
				std::cout << YELLOW << _requestHeaders.at("Content-Type:") << RESET<< std::endl;
				std::cout << RED <<_requestBody <<RESET <<std::endl;
                std::cout << RED << _requestPath <<"requestpsth!!!!!!"<< RESET <<std::endl;
                // std::cout << RED << _ <<"relativePathToScript!!!!!!"<< RESET <<std::endl;

					setDate();
					std::string d = _date + " GTM";
				    Cgi cgi(_requestBody, _config, _requestPath, _requestHeaders, _requestMethod, d);
					// cgi.createEnv(_requestHeaders, _requestMethod);
					if ((cgi.launchCGI()) == true)
					{
                   		_statusCode = OK;
						_fullPath = _config->error_page + '/' + "send.html";
						 std::cout << RED << _statusCode <<"status111!!!!!!"<< RESET <<std::endl;
					}
        			else
    					_statusCode = NOTFOUND;
						 std::cout << RED << _statusCode <<"status222!!!!!!"<< RESET <<std::endl;

			}
			
			/* multipart/form-data * обработка отправки файла */
			else if (_requestHeaders.count("Content-Type:") && _requestHeaders.at("Content-Type:").find("multipart/form-data") != std::string::npos)
			{
				//  curl -F 'fileX=@quickie-jpeg-010.jpg' localhost:8000/cgi_bin/getfile.cgi
				//  curl -F 'fileX=@car.jpg' localhost:8005

				std::cout << YELLOW << _requestHeaders.at("Content-Type:") << RESET<< std::endl;
				//std::cout << RED <<_requestBody << RESET << std::endl;

                std::string pathToFile = "www/site.com/" + _fileName;
				//отезать вебкит и все что за ним следует
				std::ofstream w;
				w.open(pathToFile.c_str(), std::ios::out | std::ios::binary);
				w << _requestBody;

				_statusCode = OK;
				_fullPath = _config->error_page + '/' + "send.html";
			}
			else
			    _statusCode = NOTALLOWED;
		}


		/* проверка автоиндекса */
		// if (_statusCode == OK && _autoindex == true)				/* если автоиндекс включен */	
		// 	this->autoindexOn();
		// else if (_statusCode == OK && _autoindex == false)			/* если автоиндекс выключен */		
		// 	this->readBody(_fullPath);

		if (_statusCode == OK)			/* если автоиндекс выключен */		
			this->readBody(_fullPath);
	}





////////////////////////////////////////////////////////////DELETE

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

////////////////////////////////////////////////////////////DELETE










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



	/* сборка ответа */
	responseCompose();



	{ /* вывод сообщения в соответствии со _statusCode*/
		
		if (_statusCode == OK)
			std::cout << GREEN_B << "OK: " << WHITE <<"response will be send to client " << BLUE << "(ID: " << _config->serverID << ")" << RESET<< std::endl << std::endl; //изменить если 404
		else if (_statusCode == BADREQUEST)
			std::cout  << RED_B << "KO: " << WHITE <<"request is bad, error \"400\" will be send to client" << BLUE << "(ID: " << _config->serverID << ")" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == NOTFOUND)
			std::cout  << RED_B << "KO: " << WHITE <<"content not found, error \"404\" will be send to client" << BLUE << "(ID: " << _config->serverID << ")" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == NOTALLOWED)
			std::cout  << RED_B << "KO: " << WHITE <<"request method not allowed, error \"405\" will be send to client" << BLUE << "(ID: " << _config->serverID << ")" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == REQTOOLARGE)
			std::cout  << RED_B << "KO: " << WHITE <<"request is too large, error \"413\" will be send to client" << BLUE << "(ID: " << _config->serverID << ")" << RESET << std::endl << std::endl; //изменить если 404
		else if (_statusCode == INTERNALERROR)
			std::cout  << RED_B << "KO: " << WHITE <<"problems with server settings, error \"500\" will be send to client" << BLUE << "(ID: " << _config->serverID << ")" << RESET << std::endl << std::endl; //изменить если 404
	}



	{ /* запись ответа в лог-файл */
		//  std::cout << _response << '\n'; //вывод ответа

		std::ofstream save_response("www/response.log", std::ios::app);

		if (!save_response)
		{
			std::cout << YELLOW <<"webserv: cannot be written responce.log" << RESET << std::endl;
		}
		else
		{
			save_response << _response << std::endl << std::endl;
			save_response << "=================================================================" << std::endl;
		}
	}





return _response;

}























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