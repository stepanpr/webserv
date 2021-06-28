
#include "Response.hpp"

Response::Response()
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


std::string Response::responseInit(RequestParser &HTTPrequest, struct s_config *config)
{


	std::map<std::string, std::string> headers = HTTPrequest.getHeaders();
	// std::cout << HTTPrequest.getMetod() << " " << HTTPrequest.getPath() << " " << HTTPrequest.getProtokol() <<'\n';
	// for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end() ; it++)
	// {
	// 	std::cout << it->first << " " << it->second << '\n';
	// }

	/* обработка method */
	std::string method = HTTPrequest.getMetod();

	/* обработка path */
	std::string path;
	int isExist = 0; //существует ли location с таким запросом

	// std::cout << config->listen << '\n';
	for (int i =0; i < config->location.size(); i++)
	{
		if (config->location[i].location == HTTPrequest.getPath())			//если, у нас не корень и запрос совпадает с каким-то локейшеном (с маской локейшена)
		{
			path = config->location[i].root + '/' + config->location[i].index;
			std::cout << config->location[i].location << " "  << HTTPrequest.getPath() << '\n';
			std::cout << path << '\n';
			isExist = 1;
			break ;
		}
	}
	if (isExist == 0)				//если не найден файл
		path = config->error_page + '/' + "404.html";

	/* обработка protocol */
	std::string protocol = HTTPrequest.getProtokol();



	/* обработка заголовков */
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end() ; it++)
	{
		std::cout << it->first << " " << it->second << '\n';
		// if (it->first == "Connection:" && it->second == "close")
		// {
		// 	// close(pfd_array[1 + i].fd);
		// 	std::cout << "GOOD!!!!!!!!!!!!"<< it->first << " : " << it->second << '\n';
		// }
	}


	// std::map<std::string, std::string>::iterator it;
	// it = headers.find("Connection:");
	// std::cout << it->first << " " << it->second << '\n';

	std::stringstream response;
	std::stringstream response_body;////////////////////////////////////

/* формирование ответа на основе обработанного запроса */
if (HTTPrequest.getPath() != "/favicon.ico")
{
	std::ifstream file; // создаем объект класса ifstream
	char *file_buffer = new char[10000 + 1]; file_buffer[10000] = 0;    //поменять!

// www/site.com/index.html
	file.open(path.c_str()); 	//пытаемся открыть файл по запросу

	if (!file) 								//нужного контента нету
	{
		std::ifstream error_404;
		file.open("www/default/404.html");
		if (!file)
		{
			std::cout << YELLOW << "error: content not found!" << RESET << std::endl;  //обработать
		}
		else
		{
			error_404.read(file_buffer, 10000);
			response_body << file_buffer;
		}
		// return -1;
	}
	else									//контейнт найден
	{
		std::cout << isExist <<'\n';
		if (isExist)
			std::cout << GREEN_B << "OK: " << WHITE <<"response will be send to client" << RESET << std::endl << std::endl; //изменить если 404
		else
			std::cout  << RED_B << "KO: " << WHITE <<"content not found, error-page will be send to client" << RESET << std::endl << std::endl; //изменить если 404
		// std::string file_buffer;
		// char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
		// response_body << file;
		file.read(file_buffer, 100000);
		// for(file >> file_buffer; !file.eof(); file >> file_buffer)
		// 	std::cout << file_buffer;
		response_body << file_buffer;
	// }


		
		response << "HTTP/1.1 200 OK\r\n"
			<< "Version: HTTP/1.1\r\n"
		<< "Content-Type: text/html; charset=utf-8\r\n"
		<< "Content-Length: " << response_body.str().length()
		<< "\r\n\r\n"
		<< response_body.str();

		// if(-1 == send(pfd_array[1 + i].fd, response.str().c_str(), response.str().length(), 0))
		// {
		// 	printf("Error on call 'send': %s\n", strerror(errno));
		// 	return -1;
		// }
	}

}
	path.clear();





return response.str();


}