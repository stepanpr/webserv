#include "webServ.hpp"
 
const char* response1 =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<doctype !html>\n"
"    <html>\n"
"        <head>\n"
"            <title>Webserv</title>\n"
"            <style>body { background-color: #111 }\n"
"                   h1 { font-size:4cm; text-align: center; color: black;\n"
"                        text-shadow: 0 0 2mm red}\n"
"            </style>\n"
"        </head>\n"
"<body>\n"
"        <h1>Webserv 777!</h1>\n"
"</body>\n"
"</html>\r\n";



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

	int port = htons((u_short)5002); //РАСПАРСИТЬ из конфигов
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
	running = true;
	/*----------------------------------------ACCEPT------------------------------------------*/ 
	//Ожидание входящего соединения 
	// accept возвращает новый сокет, через который сервер может общаться с вновь подключенным клиентом. 
	// Старый сокет (на котором acceptбыл вызван) остается открытым на том же порту, ожидая новых подключений.

	int pid;
	while(running)
	{
		int clientFD; 						//сокет который возвращает accept
		bzero(&cli_addr, sizeof(cli_addr)); //Обнуляем структуру clnt_addr, в которую мы будем записывать адрес и порт подсоединившегося клиента
		socklen_t sin_len = sizeof(cli_addr);
		

		if ((clientFD = accept(serverFD, (struct sockaddr *) &cli_addr, &sin_len)) == -1)
		{
			perror("Can't accept");
			// continue;
		}

		if ( ( pid=fork() ) == -1 ) {
			perror("Error calling fork()");
			exit(1);
    	}
		if (pid == 0) /* Мы находимся в дочернем процессе. */
		{
			close(serverFD); /* Закрываем сокет s, так как он используется родительским процессом для приема запросов, а в дочернем процессе не нужен. */
			requestHandler(clientFD);

		} /* end of "if (pid==0)" */

    	/* В это место мы попадаем после вызова fork() в родительском процессе (pid!=0). Закрываем ненужный нам сокет ns (им займется дочерний процесс) и переходим на следующую итерацию цикла while(1). */
    	close(clientFD);
	}	

	close(serverFD);
	return 0;
}









// Получение запроса
int Server::requestHandler(int &clientFD)
{

	/*----------------------------------------RECV------------------------------------------*/
	
				std::stringstream response; // сюда будет записываться ответ клиенту
			std::stringstream response_body; // тело ответа

	const int max_client_buffer_size = 1024; 						//размер буфера для хранения сохранения HTTP-запроса const int max_client_buffer_size = func(lenofbuf)
	char buf[max_client_buffer_size];								//РАСПАРСИТЬ! буфер для сохранения сохранения HTTP-запроса
	while((result = recv(clientFD, buf, max_client_buffer_size, 0)) != 0) /* Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе. В четвертом аргументе могут передаваться дополнительные опциипараметры. Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль, то клиент данных для передачи больше не имеет.*/
	{
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
			std::cout << "!!!got connection\n";
			// PARCE() of get-request
			buf[result]='\0';


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

					// response_body << 
			//Ответ вместе с заголовками
			response << "HTTP/1.1 200 OK\r\n"
			<< "Version: HTTP/1.1\r\n"
			<< "Content-Type: text/html; charset=utf-8\r\n"
			<< "Content-Length: " << response_body.str().length()
			<< "\r\n\r\n"
			<< response_body.str();

			/* На каждый полученный от клиента блок данных buf отсылаем ему ответ, содержащийся в буфере message (третий аргумент - размер буфера, четвертый - опциипараметры). */
			if ((result = send(clientFD, response.str().c_str(), response.str().length(), 0)) == -1)
			{ //если произошла ошибка при отправле данных 
				err(1, "Error: socket error");
			}
			// responseHandler(clientFD);
			

			// // send(clientFD, response1, sizeof(response1), 0);
			std::cout << GREEN << buf << RESET << std::endl; //вывод запроса
			// write(clientFD, response1, strlen(response1)); /*-1:'\0'*/
			// close(clientFD);


			// result = send(clientFD, response1, response1.length(), 0); 
			// send(ns, message, strlen(message), 0);

		}
	}
	std::cout << "Client disconnected\n";
	// fprintf(stderr, "Client disconnected\n"); /* Вышли из цикла - значит recv() вернула ноль, то есть клиент закрыл соединение. */
	/* Закрываем сокет и завершаем дочерний процесс. */
	close(clientFD);
	exit(0);
	
		




// 		const int max_client_buffer_size = 1024; 						//размер буфера для хранения сохранения HTTP-запроса const int max_client_buffer_size = func(lenofbuf)
// 		char buf[max_client_buffer_size];								//РАСПАРСИТЬ! буфер для сохранения сохранения HTTP-запроса
// 		int result = recv(clientFD, buf, max_client_buffer_size, 0); 	//чтение данных из сокета который вернул accept (clientFD)
		
		
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

		return 0;
}





//*----------------------------------------SEND------------------------------------------*/
// Отправка ответа
int Server::responseHandler(int &clientFD)
{
	std::stringstream response; // сюда будет записываться ответ клиенту
	std::stringstream response_body; // тело ответа
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

			// response_body << 
	//Ответ вместе с заголовками
	response << "HTTP/1.1 200 OK\r\n"
	<< "Version: HTTP/1.1\r\n"
	<< "Content-Type: text/html; charset=utf-8\r\n"
	<< "Content-Length: " << response_body.str().length()
	<< "\r\n\r\n"
	<< response_body.str();

	/* На каждый полученный от клиента блок данных buf отсылаем ему ответ, содержащийся в буфере message (третий аргумент - размер буфера, четвертый - опциипараметры). */
	if ((result = send(clientFD, response.str().c_str(), response.str().length(), 0)) == -1)
	{ //если произошла ошибка при отправле данных 
		err(1, "Error: socket error");
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