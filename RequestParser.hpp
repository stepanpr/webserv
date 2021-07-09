#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP
# include <iostream>
#include <sstream>
#include <map>
#include <fstream>
#include <stdio.h>//itoa
#include <stdlib.h>//itoa

class RequestParser
{

public:
	RequestParser();
	// RequestParser(std::string);
	// void addRequest(std::string);
	int RequestWaiter(const char *, int);
	void PrintMap();
	void Vault(std::string str);

	bool _is_post;
	// std::string addBody();

	std::string getMetod();
	std::string getPath();
	std::string getProtokol();
	std::map<std::string,std::string> getHeaders();
	std::string getBody();
	

	 RequestParser(const RequestParser &copy);
	~RequestParser();
	 RequestParser &operator=(const RequestParser &copy);

private:
	// bool _is_ok;			// запрос ок! отправляем на обработку
	bool _is_host;			// есть ли хедер Host:
	bool _is_chunked;		// есть ли хедер Content-encoding: chunked
	bool _is_headers_ok;	// все ли хедеры пришли?
	bool _is_body;			// пришло ли боди?
	bool _is_startline_ok;	// есть ли стартлайн
	bool _is_multipart;		// если постом передается файл
	bool _is_length;		// есть ли хедер Content-length:

    std::string buf;

	size_t _contentLength;
	size_t _global_len;		// если постом передается файл

	std::string _str;
    static const std::string space;
    static const std::string separator;
    static const std::string double_separator;

	std::string _bodybuffer;
	std::string _metod;
	std::string _path;
	std::string _protokol;
	std::map<std::string,std::string> _headers;
	std::string _body;
	std::string _fullRequest;
	int		_isOk;

	std::string buf_all;
};

#endif
