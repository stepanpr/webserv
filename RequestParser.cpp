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

	pos = buf.find(double_separator);	// Отрезали запрос с хедерами
	{
	if (pos != std::string::npos)
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
	token_header.erase(0, _metod.length() + 1);

	pos = token_header.find(" ");		// Достали путь из заголовка
	_path = token_header.substr(0, pos);
	token_header.erase(0, _path.length() + 1);

	pos = token_header.find(" ");		// Достали протокол из заголовка
	_protokol = token_header.substr(0, pos);
	token_header.erase();


	std::map<std::string,std::string> headers;
	std::string tmp_header;
	std::string tmp_header_rigth;

	while ((pos = token.find(separator)) != std::string::npos) // Отрезаем по 1-му хедеру и кладем в мап
	{
		tmp_header = token.substr(0, pos);
		// std::cout << tmp_header;
		size_t pos2 = 0;
		pos2 = tmp_header.find(" ");
		tmp_header_rigth = tmp_header.substr(0, pos2);
		tmp_header.erase(0, tmp_header_rigth.length() + 1);

		// std::cout << tmp_header_rigth.length() << " " << tmp_header.length() << '\n';

		headers.insert(std::pair<std::string,std::string>(tmp_header_rigth, tmp_header));

		token.erase(0, tmp_header.length() + tmp_header_rigth.length() + 1);
	}

	// std::cout << token << std::endl;

	std::cout << "\nmap contains:\n";
	std::map<std::string,std::string>::iterator it = headers.begin();
  	for (it=headers.begin(); it!=headers.end(); ++it)
    std::cout << it->first << " " << it->second << '\n';


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