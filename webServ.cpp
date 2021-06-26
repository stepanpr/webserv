#include "webServ.hpp"


Server::Server()
{
	running = false; 	//запущен ли сервер
	serverFD = 0;
	// clientFD = 0;
	result = 0;
	// sin_len = sizeof(cli_addr);
}


int Server::startServer()
{
	/////////////////////////////////////////////////////////parceConfig()

	//заполняем структуру sockaddr_in
	int port = htons((u_short)5006); //PORT РАСПАРСИТЬ из конфигов
	srv_addr.sin_family = AF_INET; // AF_INET определяет, что используется сеть для работы с сокетом
	srv_addr.sin_addr.s_addr = INADDR_ANY;
	srv_addr.sin_port = port;
	//создание сокета
	if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) == -1) //(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	{
		err(1, "error: can't open socket");
	}
	int one = 1;
	setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)); //опции сокета; SO_REUSEADDR - повторное использование локальных адресов (принимает булево значение);
	// http://agalakov.spb.ru/Shared/Documentation/Programming/IIAS/Os2k/os2000/doc/function/setsockopt.html <- по прямой ссылке не заходит, только через поиск яндекса
	// Bind: Привязка сокета к адресу
	if (bind(serverFD, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) == -1)
	{
			close(serverFD);
			err(1, "error: can't bind");
	}
	//Listen: Подготовка сокета к принятию входящих соединений
	if (listen(serverFD, SOMAXCONN) == -1) //SOMAXCONN - максимально возможное число одновременных TCP-соединений, можно указать нужное число
	{
		close(serverFD);
		err(1, "Can't bind");
	}

	running = true;
	int pid;

	while(running)
	{
		// int clientFD; 						//сокет который возвращает accept
		bzero(&cli_addr, sizeof(cli_addr)); 	//Обнуляем структуру clnt_addr, в которую мы будем записывать адрес и порт подсоединившегося клиента
		socklen_t sin_len = sizeof(cli_addr);

		//Accept: Ожидание входящего соединения
		if ((clientFD = accept(serverFD, (struct sockaddr *) &cli_addr, &sin_len)) == -1)
		{
			perror("Can't accept");
			continue;
		}
		// std::thread first(&Server::requestHandler, this);
		// pthread_create( NULL, NULL, &echo, NULL);
		if (( pid=fork() ) == -1 )
		{
			perror("Error calling fork()");
			exit(1);
    	}
		if (pid == 0) /* Мы находимся в дочернем процессе. */
		{
			// getsockname(clientFD, (sockaddr*)&cli_addr, &sin_len);
			std::cout << "\n!got connection from " << inet_ntoa((in_addr)cli_addr.sin_addr) << std::endl;
			close(serverFD); /* Закрываем сокет s, так как он используется родительским процессом для приема запросов, а в дочернем процессе не нужен. */
			requestHandler();
			exit(0);
		} /* end of "if (pid==0)" */

    	/* В это место мы попадаем после вызова fork() в родительском процессе (pid!=0). Закрываем ненужный нам сокет ns (им займется дочерний процесс) и переходим на следующую итерацию цикла while(1). */
    	close(clientFD);
		// first.join();
	}
	close(serverFD);
	return 0;
}


// Получение запроса
void Server::requestHandler()
{

	const int max_client_buffer_size = 1024;
	// размер буфера для хранения сохранения HTTP-запроса
	// const int max_client_buffer_size = func(lenofbuf)
	char buf[max_client_buffer_size];
	// РАСПАРСИТЬ! буфер для сохранения сохранения HTTP-запроса
	while((result = recv(clientFD, buf, max_client_buffer_size, 0)) != 0)
	/*	Возврат из функции recv происходит, когда модуль TCP решает передать процессу полученные
		от клиента данные. Данные возвращается в буфере buf, размер которого передается в третьем аргументе.
		В четвертом аргументе могут передаваться дополнительные опциипараметры.
		Функция возвращает число байтов, которые модуль TCP записал в буфер buf; если функция возвращает ноль,
		то клиент данных для передачи больше не имеет.*/
	{
		if (result == -1) //SOCKET_ERROR
		{
			close(serverFD);
			err(1, "Error: socket error");
		}
		else if (result == 0)
		{ 	// соединение закрыто клиентом
			err(1, "connection closed...\n");
			shutdown(serverFD, 2); //
			break ;
		}
		else if (result > 0)
		{
			// std::cout << "!got connection\n";
			//////////////////////////////////////// PARCE(buf) //////get-request

			buf[result]='\0';

			std::cout << GREEN << buf << RESET << std::endl;



			std::string str(buf);

			responseHandler(str); // << str

			// write(clientFD, response1, strlen(response1)); /*-1:'\0'*/
			// close(clientFD);
		}
	}
	// std::cout << GREEN << buf << RESET << std::endl;
	std::cout << "!client disconnected\n";
	stopServer();

	//Закрываем сокет и завершаем дочерний процесс
	close(clientFD);
	exit(0);

}

// Отправка ответа
int Server::responseHandler(std::string str)
{
	std::stringstream response_body;
	std::ifstream file; // создаем объект класса ifstream

	if (str.find("favicon.ico") != str.npos) {
		file.open("content/favicon.ico");
		std::cout << "fav ok!\n";
	}
	else {
		file.open("content/index.html"); // открываем файл
		std::cout << "index ok!\n";
	}
	if (file.is_open())
		response_body << file.rdbuf();

	response << "HTTP/1.1 200 OK\r\n" << "Version: HTTP/1.1\r\n";
	if (str.find("favicon.ico") != str.npos)
		response << "Content-Type: image/x-icon\r\n";
	else
		response << "Content-Type: text/html; charset=utf-8\r\n";

	response << "Content-Length: " << response_body.str().length()
	<< "\r\n\r\n" << response_body.str() << "\r\n\r\n";

	// response_body.ignore(10000, '\0');

	/* 	На каждый полученный от клиента блок данных buf отсылаем ему ответ,
		содержащийся в буфере message (третий аргумент - размер буфера, четвертый - опции параметры). */
	if ((result = send(clientFD, response.str().c_str(), response.str().length(), 0)) == -1)
	{ //если произошла ошибка при отправле данных
		err(1, "Error: socket error");
	}

	return 0;
}


void Server::stopServer()
{
    if (running)
    {
        running = false;
    }
}

int main()
{
	Server serv;
	serv.startServer();
	return 0;
}


// http://netcode.ru/cpp/?lang=&katID=19&skatID=175&artID=5146 !info

// http://www.otavin.narod.ru/lecture/DS/labs/1-sockets/sockets.html !!! fork()
// https://itnan.ru/post.php?c=2&p=276818 ! select

// https://www.cyberforum.ru/cpp-networks/thread825420.html  threads oldv
// https://www.unix.com/programming/17611-multi-threaded-server-pthreads-sleep.html
// https://www.sql.ru/forum/1014302/potokovyy-server-na-s
// https://web-answers.ru/c/mnogopotochnost-sozdanie-potoka-vnutri-klassa-i.html threads new

// https://dev-notes.eu/2018/06/http-server-in-c/
// https://code-live.ru/post/cpp-http-server-over-sockets/
// https://www.cyberforum.ru/cpp-beginners/thread1251233.html
// https://www.opennet.ru/docs/RUS/linux_base/node248.html
// https://www.linux.org.ru/forum/development/143672
// https://www.opennet.ru/docs/RUS/ipcbook/node82.html shutdown
// https://www.opennet.ru/docs/RUS/socket/node4.html

// https://github.com/LambdaSchool/C-Web-Server/tree/master/src !!! structura
