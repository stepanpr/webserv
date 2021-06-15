#include "webServ.hpp"
 
// const char* response1 =
// "HTTP/1.1 200 OK\r\n"
// "Content-Type: text/html; charset=UTF-8\r\n\r\n"
// "<doctype !html>\n"
// "    <html>\n"
// "        <head>\n"
// "            <title>Webserv</title>\n"
// "            <style>body { background-color: #111 }\n"
// "                   h1 { font-size:4cm; text-align: center; color: black;\n"
// "                        text-shadow: 0 0 2mm red}\n"
// "            </style>\n"
// "        </head>\n"
// "<body>\n"
// "        <h1>Webserv 777!</h1>\n"
// "</body>\n"
// "</html>\r\n";



// void Server::startServer()
// {
	
// }

Server::Server()
{
	running = false; 	//запущен ли сервер

	serverFD = 0;
	result = 0;
	// sin_len = sizeof(cli_addr);
}
 
int Server::startServer()
{
	////////////////////////////////////////parceConfig()

/*----------------------------------------ЗАПОЛНЯЕМ СТРУКТУРУ------------------------------------------*/
	// Структура sockaddr_in описывает сокет для работы с протоколами IP, структура, хранящая информацию
    // об IP-адресе  слущающего сокета; https://www.opennet.ru/docs/RUS/socket/node4.html

	int port = htons((u_short)5017); //РАСПАРСИТЬ из конфигов
	srv_addr.sin_family = AF_INET; // AF_INET определяет, что используется сеть для работы с сокетом
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = port;
/*----------------------------------------СОЗДАНИЕ СОКЕТА------------------------------------------*/
	if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
	{
		err(1, "error: can't open socket");
	}
	int one = 1;
	setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)); //опции сокета; SO_REUSEADDR - повторное использование локальных адресов (принимает булево значение); http://agalakov.spb.ru/Shared/Documentation/Programming/IIAS/Os2k/os2000/doc/function/setsockopt.html
/*----------------------------------------BIND------------------------------------------*/
	//Привязка сокета к адресу
	if (bind(serverFD, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1) 
	{
			close(serverFD);
			err(1, "error: can't bind");
	}
/*----------------------------------------LISTEN------------------------------------------*/
	//Подготовка сокета к принятию входящих соединений
	if (listen(serverFD, SOMAXCONN) == -1) //SOMAXCONN - максимально возможное число одновременных TCP-соединений, можно указать нужное число
	{
		close(serverFD);
		err(1, "Can't bind");
	}
/*----------------------------------------ACCEPT------------------------------------------*/ 
	//Ожидание входящего соединения 
	// accept возвращает новый сокет, через который сервер может общаться с вновь подключенным клиентом. 
	// Старый сокет (на котором acceptбыл вызван) остается открытым на том же порту, ожидая новых подключений.

	// while(running)
	// {
		int clientFD; 						//сокет который возвращает accept
		bzero(&cli_addr, sizeof(cli_addr)); //Обнуляем структуру clnt_addr, в которую мы будем записывать адрес и порт подсоединившегося клиента
		socklen_t sin_len = sizeof(cli_addr);
		

		if ((clientFD = accept(serverFD, (struct sockaddr *) &cli_addr, &sin_len)) == -1)
		{
			perror("Can't accept");
			// continue;
		}

// /*----------------------------------------RECV------------------------------------------*/
// 		// Получение запроса

		hendlerRequest(clientFD);


// 		const int max_client_buffer_size = 1024; 						//размер буфера для хранения сохранения HTTP-запроса const int max_client_buffer_size = func(lenofbuf)
// 		char buf[max_client_buffer_size];								//РАСПАРСИТЬ! буфер для сохранения сохранения HTTP-запроса
// 		int result = recv(clientFD, buf, max_client_buffer_size, 0); 	//чтение данных из сокета который вернул accept (clientFD)
// 		//PARCE 
		
// 		if (result == -1) //SOCKET_ERROR
// 		{	// ошибка получения данных
// 			close(serverFD);
// 			err(1, "Error: socket error");
// 		}	else if (result == 0) {
//     		// соединение закрыто клиентом
// 			err(1, "connection closed...\n");
// 			shutdown(serverFD, 2); //
// 			// break ;
// 		} else if (result > 0) 
// 		{
// /*----------------------------------------SEND------------------------------------------*/
// 		// Отправка ответа
// 		std::stringstream response; // сюда будет записываться ответ клиенту
// 		std::stringstream response_body; // тело ответа
// 		//Тело ответа
// 		// response_body << "<doctype !html>\n"
// 		// << "<html>\n"
// 		// << "<head>\n <title>WebServ 1.0</title>\n"
// 		// << "<style>body { background-color: #fffff3 }\n"
// 		// << "h1 { font-size:2cm; text-align: center; margin-top: 70px; color: black; \n"
// 		// << "text-shadow: 0 0 2mm green}\n"
// 		// << "</style>\n </head>\n <body>\n"
// 		// << "<h1>WebServ 1.0</h1>\n"
// 		// << "<p>This is BODY of the test page...</p>\n"
// 		// << "<h2>Request headers:</h2>\n"
// 		// << "<pre>" << buf << "</pre>\n"
// 		// << "<em><small>Test C++ Http Server</small></em>\n"
// 		// << "</body>\n </html>\r\n";

//         {	// в путь вставляем путь из конфига в соответствии с гет-запросом (/ и тп)
// 			std::ifstream file; // создаем объект класса ifstream
// 			file.open("content/index.html"); // открываем файл
// 			if (!file) 
// 			{
// 				std::cout << "Файл не открыт\n\n"; 
// 				return -1;
// 			}
// 			else
// 			{
// 				std::cout << "Все ОК! Файл открыт!\n\n";
// 				// std::string file_buffer;
// 				char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
// 				// response_body << file;
// 				file.read(file_buffer, 300);
// 				// for(file >> file_buffer; !file.eof(); file >> file_buffer)
// 				// 	std::cout << file_buffer;
// 				response_body << file_buffer;
// 			}
// 		}

// 		// response_body << 
// 		//Ответ вместе с заголовками
// 		response << "HTTP/1.1 200 OK\r\n"
// 		<< "Version: HTTP/1.1\r\n"
// 		<< "Content-Type: text/html; charset=utf-8\r\n"
// 		<< "Content-Length: " << response_body.str().length()
// 		<< "\r\n\r\n"
// 		<< response_body.str();



// 		result = send(clientFD, response.str().c_str(), response.str().length(), 0);
// 		if (result == -1)
// 		{ //если произошла ошибка при отправле данных 
// 			err(1, "Error: socket error");
// 		}



// 			// // send(clientFD, response1, sizeof(response1), 0);
// 			// printf("!!!got connection\n");
// 			std::cout << GREEN << buf << RESET << std::endl; //вывод запроса
// 			// write(clientFD, response1, strlen(response1)); /*-1:'\0'*/
// 			close(clientFD);
// 		}
	// }

	// close(listen_socket);
	return 0;
}










int Server::hendlerRequest(int clientFD)
{
	/*----------------------------------------RECV------------------------------------------*/
		// Получение запроса




		const int max_client_buffer_size = 1024; 						//размер буфера для хранения сохранения HTTP-запроса const int max_client_buffer_size = func(lenofbuf)
		char buf[max_client_buffer_size];								//РАСПАРСИТЬ! буфер для сохранения сохранения HTTP-запроса
		int result = recv(clientFD, buf, max_client_buffer_size, 0); 	//чтение данных из сокета который вернул accept (clientFD)
		//PARCE 
		
		if (result == -1) //SOCKET_ERROR
		{	// ошибка получения данных
			close(serverFD);
			err(1, "Error: socket error");
		}	else if (result == 0) {
    		// соединение закрыто клиентом
			err(1, "connection closed...\n");
			shutdown(serverFD, 2); //
			// break ;
		} else if (result > 0) 
		{
/*----------------------------------------SEND------------------------------------------*/
		// Отправка ответа
		std::stringstream response; // сюда будет записываться ответ клиенту
		std::stringstream response_body; // тело ответа
		//Тело ответа
		// response_body << "<doctype !html>\n"
		// << "<html>\n"
		// << "<head>\n <title>WebServ 1.0</title>\n"
		// << "<style>body { background-color: #fffff3 }\n"
		// << "h1 { font-size:2cm; text-align: center; margin-top: 70px; color: black; \n"
		// << "text-shadow: 0 0 2mm green}\n"
		// << "</style>\n </head>\n <body>\n"
		// << "<h1>WebServ 1.0</h1>\n"
		// << "<p>This is BODY of the test page...</p>\n"
		// << "<h2>Request headers:</h2>\n"
		// << "<pre>" << buf << "</pre>\n"
		// << "<em><small>Test C++ Http Server</small></em>\n"
		// << "</body>\n </html>\r\n";

        {	// в путь вставляем путь из конфига в соответствии с гет-запросом (/ и тп)
			std::ifstream file; // создаем объект класса ifstream
			file.open("content/index.html"); // открываем файл
			if (!file) 
			{
				std::cout << "Файл не открыт\n\n"; 
				return -1;
			}
			else
			{
				std::cout << "Все ОК! Файл открыт!\n\n";
				// std::string file_buffer;
				char *file_buffer = new char[1000 + 1]; file_buffer[1000] = 0;
				// response_body << file;
				file.read(file_buffer, 300);
				// for(file >> file_buffer; !file.eof(); file >> file_buffer)
				// 	std::cout << file_buffer;
				response_body << file_buffer;
			}
		}

		// response_body << 
		//Ответ вместе с заголовками
		response << "HTTP/1.1 200 OK\r\n"
		<< "Version: HTTP/1.1\r\n"
		<< "Content-Type: text/html; charset=utf-8\r\n"
		<< "Content-Length: " << response_body.str().length()
		<< "\r\n\r\n"
		<< response_body.str();



		result = send(clientFD, response.str().c_str(), response.str().length(), 0);
		if (result == -1)
		{ //если произошла ошибка при отправле данных 
			err(1, "Error: socket error");
		}



			// // send(clientFD, response1, sizeof(response1), 0);
			// printf("!!!got connection\n");
			std::cout << GREEN << buf << RESET << std::endl; //вывод запроса
			// write(clientFD, response1, strlen(response1)); /*-1:'\0'*/
			close(clientFD);
		}

		return 0;
}




















int main()
{
	Server serv;
	serv.startServer();
	return 0;
}




// http://www.otavin.narod.ru/lecture/DS/labs/1-sockets/sockets.html !!! fork()

// https://code-live.ru/post/cpp-http-server-over-sockets/
// https://www.cyberforum.ru/cpp-beginners/thread1251233.html
// https://www.opennet.ru/docs/RUS/linux_base/node248.html
// https://www.linux.org.ru/forum/development/143672
// https://www.opennet.ru/docs/RUS/ipcbook/node82.html shutdown

// https://github.com/LambdaSchool/C-Web-Server/tree/master/src !!! structura