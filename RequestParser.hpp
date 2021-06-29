#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP
# include <iostream>
#include <sstream>
#include <map>
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

	size_t _contentLength;
	std::string getMetod();
	std::string getPath();
	std::string getProtokol();
	std::map<std::string,std::string> getHeaders();

	// RequestParser(const RequestParser &copy);
	~RequestParser();
	// RequestParser &operator=(const RequestParser &copy);

private:
	bool _is_ok;			// запрос ок! отправляем на обработку
	bool _is_host;			// есть ли хедер Host:
	bool _is_chunked;		// есть ли хедер Content-encoding: chunked
	bool _is_length;		// есть ли хедер Content-length:
	bool _is_headers_ok;	// все ли хедеры пришли?
	bool _is_body;			// пришло ли боди?
	bool _is_startline_ok;	// есть ли стартлайн

	std::stringstream _ss;
	std::string _metod;
	std::string _path;
	std::string _protokol;
	std::stringstream _bodybuffer;
	std::map<std::string,std::string> _headers;
	std::string _body;
	std::string _fullRequest;
	int		_isOk;
};

#endif
