#include "RequestParser.hpp"

RequestParser::RequestParser(void) 
{
		_is_startline_ok = false; 
		_is_headers_ok = false;
		_is_host = false;
		_is_body = false;
		_is_chunked = false;
		_is_length = false;
		_is_multipart = false;
	// _is_post = false;
}

// RequestParser::RequestParser(std::string buf)
// {
// }

int RequestParser::RequestWaiter(const char *str, int len)
{
	size_t pos = 0;
	std::string space = " ";
	std::string separator = "\r\n";
	std::string double_separator = "\r\n\r\n";
	std::string startline;
	std::string headers;
	std::string buf;
	std::string tmp_header;
	std::string tmp_header_rigth;


	std::string new_str = (char*)str;	//  Приводим к стрингу
	buf.append(new_str);				//  Добавляем приходящую строку в буфер

	buf_all.append(new_str);
	

// std::cout << "!!!!!!!!" << new_str <<std::endl;

	
	
	
	pos = buf.find(separator);
	//	Стартлайн
	if ( (pos != std::string::npos) && _is_startline_ok != true ) // если есть \r\n и не было стартлайна, то парсим и удаляем все что до него
	{
		startline = buf.substr(0, pos);
		buf.erase(buf.begin(), buf.begin() + startline.length() + separator.length());
		pos = startline.find(" ");		// Достали метод из заголовка
		_metod = startline.substr(0, pos);
		startline.erase(0, _metod.length() + space.length());
		pos = startline.find(" ");		// Достали путь из заголовка
		_path = startline.substr(0, pos);
		startline.erase(0, _path.length() + space.length());
		pos = startline.find(" ");		// Достали протокол из заголовка
		_protokol = startline.substr(0, pos);
		startline.erase();
		_is_startline_ok = true;
	}

	
	
	//	Стартлайн спарсили, парсим хедеры
	if (((pos = buf.find(double_separator)) != std::string::npos))  // если есть \r\n\r\n парсим и удаляем все что до него
	{
		if (_is_headers_ok != true) // если не было хедеров
		{
			headers = buf.substr(0, pos + double_separator.length());  // отрезаем хедеры

			buf.erase(0, pos + double_separator.length());

			while ((pos = headers.find(separator)) != std::string::npos) // Отрезаем по 1-му хедеру и кладем в мап
			{
				tmp_header = headers.substr(0, pos);
				size_t pos2 = 0;
				pos2 = tmp_header.find(" ");
				tmp_header_rigth = tmp_header.substr(0, pos2);
				tmp_header.erase(0, tmp_header_rigth.length() + space.length());
				_headers.insert(std::pair<std::string,std::string>(tmp_header_rigth, tmp_header));
				headers.erase(0, tmp_header.length() + tmp_header_rigth.length() + separator.length() + space.length());
			}
		}
		_is_headers_ok = true;
	}



	if (_is_headers_ok == true) // понимаем будет ли тело?
	{
		std::map<std::string,std::string>::iterator it = _headers.begin();
		for (it = _headers.begin(); it != _headers.end(); ++it)
		{
			if ((it->first.compare("Transfer-Encoding:") == 0) && (it->second.compare("chunked") == 0))
				_is_chunked = true;
			if ((it->first.compare("Content-Length:") == 0))
			{
				// _contentLength = std::stoi(it->second);
				_contentLength = atoi(it->second.c_str());
				_is_length = true;
				_global_len = 0;
			}
			if ((it->first.compare("Host:") == 0))
				_is_host = true;
			if ((it->first.compare("Content-Type:") == 0) && (it->second.find("multipart/form-data;") != std::string::npos))
				_is_multipart = true;

		}
	}

	//  начинаем читать тело Chunked
	if  (_is_chunked == true)
		{
			while (_is_body != true)
			{
				std::string bodydigit;

				long int bodydigit_dec;
				char * pEnd;

				pos = buf.find(separator);		// Нашли цифру
				bodydigit = buf.substr(0, pos);
				bodydigit_dec = strtol(bodydigit.c_str(), &pEnd, 16);
				if (bodydigit_dec > 0)
				{
					buf.erase(buf.begin(), buf.begin() + bodydigit.length() + separator.length());
					_bodybuffer << buf.substr(0, bodydigit_dec); // Положили в буфер
					buf.erase(buf.begin(), buf.begin() + bodydigit_dec + separator.length());
				}
				if (bodydigit_dec == 0)
				{
					_is_body = true;
					buf.erase(buf.begin(), buf.end());
				}
			}
		}


	/*------------------------------------------------------------------*/
	if (buf.find("Content-Disposition:"))
	{
		//записывем навание файла
		std::cout << "CONTENT OK!!!!!!!!!!!!!!!!!!!!1" << '\n';
	}
	/*
		добваить проверку на WebKit - если нашли, то удаляем!
	*/
	//если находим типы соответствующие зпросу POST, то _is_post = true
	if (_is_post == false)
	{
		for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it)
		{
			// std::cout << it->first << " " << it->second << '\n';
			if ((it->first == "Content-Type:" && it->second == "application/x-www-form-urlencoded") 
			|| (it->first == "Content-Type:" && it->second.find("multipart/form-data") != std::string::npos))
			{
				std::cout << it->first << " " << it->second << '\n';
				_is_post = true;
			}
		}
	}
		// контент length обработка
	else if  (((_is_length == true) || (_contentLength > 0)) && _is_post == true)
	{
		// if ((_contentLength > _global_len) ) //Content-Type: multipart/form-data;
		// {
			_bodybuffer << buf;
			_global_len = _global_len + len;
		// }
		std::ofstream fileTmp("www/file.tmp", std::ios::app);
		fileTmp << _bodybuffer.str();
		if (_global_len >= _contentLength)
		{
			_is_post = false;
		}
	}
	/*------------------------------------------------------------------*/

			// if (it->first == "Content-Type:" && it->second.find("multipart/form-data") != std::string::npos)
			// {
			// 	std::cout << it->first << " " << it->second << '\n';
			// 	std::ofstream fileTmp("www/file.tmp", std::ios::app);
			// 	fileTmp << _bodybuffer.str();

			// }
	

	// контент length обработка
	// if  ((_is_length == true) || (_contentLength > 0))
	// {
	// 	if ((_contentLength > _global_len) ) //Content-Type: multipart/form-data;
	// 	{
	// 		// if ((pos = buf.find(double_separator)) != std::string::npos)
	// 		// {
	// 		// 	buf.erase(0, pos);
	// 		// }
	// 		_bodybuffer << buf;
	// 		_global_len = _global_len + len;
	// 	}
	// }

	std::cout<< "!!!!!!!! " << _contentLength << ' ' << len <<'\n';
	std::cout << "_is_multipart:" << _is_multipart << ' ' << " _is_length:" << _is_length << " _global_len:" << _global_len << '\n' << '\n';

	std::cout << _bodybuffer.str() << '\n';

	// std::ofstream fileTmp("www/file.tmp", std::ios::app);
	// fileTmp << _bodybuffer.str();


	if (!(_metod.empty()) && !(_path.empty()) && !(_protokol.empty()) && (_is_headers_ok == true) )  //  проверяем стартлайн, мапу с хедерами и выставляем флаг is_ok
	{
		if (((_is_chunked == true) || (_is_length == true)) && (_is_body == true)) // если есть чанк или лен и боди собралось
					_isOk = 1;
		if ( (_is_host == true) && (_is_chunked == false) && (_is_length == false) ) // если есть тока хост
					_isOk = 1;
	}

	return (_isOk);

}

// std::string RequestParser::addBody()
// {
// 	return (_bodybuffer.str().c_str());
// }

void RequestParser::PrintMap()
{
	std::cout << "map contains:\n";
	std::map<std::string,std::string>::iterator it = _headers.begin();
  	for (it = _headers.begin(); it != _headers.end(); ++it)
    std::cout << it->first << " " << it->second << '\n';

}

std::string RequestParser::getMetod()
{
	return (_metod);
}

std::string RequestParser::getPath()
{
	return (_path);
}

std::string RequestParser::getProtokol()
{
	return (_protokol);
}

std::map<std::string,std::string> RequestParser::getHeaders()
{
	return (_headers);
}

std::string RequestParser::getBody()
{
	return (_bodybuffer.str());
}

// RequestParser::RequestParser(const RequestParser &copy)
// {
// }

// RequestParser	&RequestParser::operator=(const RequestParser &copy)
// {
// 	return (*this);
// }

RequestParser::~RequestParser()
{ }