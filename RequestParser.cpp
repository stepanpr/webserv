#include "RequestParser.hpp"

RequestParser::RequestParser(void)
{ }

RequestParser::RequestParser(std::string buf)
{
	size_t pos = 0;
	std::string token;
	std::string token_header;
	std::string separator = "\r\n";
	std::string double_separator = "\r\n\r\n";
	std::string space = " ";
	bool is_ok = false;

	size_t counter = 0;

	pos = buf.find(double_separator);	// Отрезали запрос с хедерами
	if (pos != std::string::npos)
	{
		token = buf.substr(0, pos + separator.length());
		buf.erase(0, pos + double_separator.length());
	}

	pos = token.find(separator);		// Отрезали заголовок запроса
	if (pos != std::string::npos)
	{
		token_header = token.substr(0, pos);
		token.erase(token.begin(), token.begin() + token_header.length() + separator.length());
	}

	pos = token_header.find(" ");		// Достали метод из заголовка
	_metod = token_header.substr(0, pos);
	token_header.erase(0, _metod.length() + space.length());

	pos = token_header.find(" ");		// Достали путь из заголовка
	_path = token_header.substr(0, pos);
	token_header.erase(0, _path.length() + space.length());

	pos = token_header.find(" ");		// Достали протокол из заголовка
	_protokol = token_header.substr(0, pos);
	token_header.erase();


	std::string tmp_header;
	std::string tmp_header_rigth;

	while ((pos = token.find(separator)) != std::string::npos) // Отрезаем по 1-му хедеру и кладем в мап
	{
		tmp_header = token.substr(0, pos);
		size_t pos2 = 0;
		pos2 = tmp_header.find(" ");
		tmp_header_rigth = tmp_header.substr(0, pos2);
		tmp_header.erase(0, tmp_header_rigth.length() + space.length());
		_headers.insert(std::pair<std::string,std::string>(tmp_header_rigth, tmp_header));
		token.erase(0, tmp_header.length() + tmp_header_rigth.length() + separator.length() + space.length());
	}

	if ( !(_metod.empty()) && !(_path.empty()) && !(_protokol.empty()))  //  проверяем все ли хорошо спарсилось, выставляет флаг is_ok
	{
		std::map<std::string,std::string>::iterator it = _headers.begin();
  		for (it = _headers.begin(); it != _headers.end(); ++it)
			if ((it->first.compare("Transfer-Encoding:") == 0) && (it->second.compare("chunked") == 0))
			{
				std::cout << "This is chunked request\n";
				is_ok = true;
			}
		std::cout << is_ok << '\n';
	}

	// std::cout << buf << std::endl;

}

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

// RequestParser::RequestParser(const RequestParser &copy)
// {
// }

// RequestParser	&RequestParser::operator=(const RequestParser &copy)
// {
// 	return (*this);
// }


RequestParser::~RequestParser()
{ }