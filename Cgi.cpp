

#include "Cgi.hpp"

// Cgi::Cgi() {}
// Cgi::~Cgi()
// {         
// 	for (int i = 0; _varsArray[i]; ++i)
//         free(_varsArray[i]);
// }
// Cgi &Cgi::operator=(const Cgi &copy)
// {
// 	return *this;
// }
// Cgi::Cgi(const Cgi &cgi) { *this = cgi; }

Cgi::Cgi(std::string body, struct s_config *config, std::string pathToScript, 
std::map<std::string, std::string> requestHeaders, std::string requestMethod, std::string &date) 
:  _config(config), _body(body), _pathToScript(pathToScript), _date(date)
{
	std::cout << CYAN << "!CGI is working" << RESET << "\n";

	_varsArray = NULL;
	// _fullPathToScript = ;
	_varsArray = getVarsArray(setVariables(requestHeaders, requestMethod));

}


std::vector<std::string> Cgi::setVariables(std::map<std::string, std::string> requestHeaders, std::string requestMethod)
{
	// std::vector<std::string> env;
    // _vars.push_back("AUTH_TYPE=" + locations.auth_data.AuthType);
    _vars.push_back("CONTENT_LENGTH=" + toString(_body.size()));
	if (requestHeaders.find("Content-Type") != requestHeaders.end())
    	_vars.push_back("CONTENT_TYPE=" + requestHeaders.find("Content-Type")->second);
	else
		_vars.push_back("CONTENT_TYPE=");
    _vars.push_back("GATEWAY_INTERFACE=CGI/1.1");
    // _vars.push_back(getPathInfo("PATH_INFO=" + );
    // _vars.push_back("PATH_TRANSLATED=" + );
	if (requestHeaders.count("query_string"))
   		_vars.push_back("QUERY_STRING=" + requestHeaders.find("query_string")->second);
	else
        _vars.push_back("QUERY_STRING=");
    _vars.push_back("REMOTE_ADDR=" + _config->server_name);
    // _vars.push_back("REMOTE_IDENT=" + );
    // _vars.push_back("REMOTE_USER=" + );
	if (requestMethod.size() > 1)
    	_vars.push_back("REQUEST_METHOD=" + requestMethod);
	else
        _vars.push_back("REQUEST_METHOD=");
    // _vars.push_back("RЕQUEST_URI=" + );
    // _vars.push_back("SERVER_NAME=" + _config->server_name);
    _vars.push_back("SERVER_PORT=" + _config->listen);
    _vars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _vars.push_back("SERVER_SOFTWARE=web_server");
	_vars.push_back("DATE_LOCAL=" + _date);
	_vars.push_back("SERVER_NAME=" + toString( _config->serverID));

	
	// for (int i = 0; i < (int)_vars.size(); i++)
	// 	std::cout << _vars[i] << std::endl;
	return _vars;
}



char **Cgi::getVarsArray(std::vector<std::string> vars) 
{
    char **pString = (char **) malloc(sizeof(char **) * (vars.size() + 1));
    std::vector<std::string>::const_iterator j = vars.begin();
    std::string::size_type i = 0;
    for (; i < vars.size(); ++i) {
        pString[i] = strdup(j->c_str());
        j++;
    }
    pString[i] = NULL;
    return pString;
}




/*
** считывает созданный скриптом db.py файл db_[ID сервера].tmp
** и формирует на основе его дынных html-страницу db_[ID сервера].html
*/
void Cgi::addToDataBase()
{
	std::string nameOfTMP = "./db/db_" + toString(_config->serverID) + ".tmp";
	std::string nameOfHTML = "./db/db_" + toString(_config->serverID) + ".html";

	std::ifstream db(nameOfTMP.c_str(), std::ios::in);
	std::string line;
	std::string dataBaseBuffer;
	while(getline(db, line))
	{
		dataBaseBuffer += line;
		dataBaseBuffer += "\n";
	}

	// std::cout << dataBaseBuffer;
	std::ofstream html(nameOfHTML.c_str(), std::ios::out | std::ios::trunc);
	html << "<!DOCTYPE html>\n<html>\n<head>\n<title>webserv - DataBase</title>\n";
	html << "</head>\n<body>\n";
	html << dataBaseBuffer;
	html << "\n</body>\n</html>";

	db.close();  
}






bool Cgi::launchCGI()
{


/* (1, basic)
** версия пути к скрипту с изпользованием обоих параметров из alias (/cgi_bin/ == /usr/bin/[name]/webserv/cgi_bin/)
** первый параметр может быть отличным от "cgi_bin"
** в атрибуте формы "action" прописывать путь к скрипту /cgi_bin/script.cgi 
*/
	_pathToScript.erase(0, _config->cgi_alias[0].size());
	_fullPathToScript = _config->cgi_alias[1] + _pathToScript;
	// std::cout << CYAN << _pathToScript << RESET << std::endl;
	// std::cout << CYAN << _fullPathToScript << RESET << std::endl;


/* (2)
** 	релативная версия ссылки к скрипту без использования alias из конфиг-файла;
**  в атрибуте формы "action" прописывать только имя скрипта без /cgi_bin/ 
**	this->_fullPathToScript = "./cgi_bin" + this->_pathToScript;
*/

/* (3)
** версия пути к скрипту с изпользованием только второго параметра из alias (/usr/bin/[name]/webserv/cgi_bin/) 
** в атрибуте формы "action" прописывать только имя скрипта без /cgi_bin/
** this->_fullPathToScript = _config->cgi_alias[1]  + this->_pathToScript; 
*/



// std::cout << CYAN << _pathToScript << RESET << std::endl;
// std::cout << CYAN << _fullPathToScript << RESET << std::endl;
// std::cout << CYAN << relativePathToScript << RESET << std::endl;

	/* проверяем существует ли такой файл */
	std::string relativePathToScript = "./cgi_bin" + this->_pathToScript;
	std::cout << CYAN << relativePathToScript << RESET << std::endl;
	if (stat(relativePathToScript.c_str(), &_stat) != 0)
	{
		std::cout << RED << "webserv: " << RESET << "please, check existence if script" << std::endl;
		return false;
	}


	/* проверяем расширение файла */
	int extentionOfSctipt = EXTENTION_WITH_INTERPRETER;
	if (_pathToScript.rfind(".py") != std::string::npos)
		this->_pathToHandler = "/usr/bin/python";
	else if (_pathToScript.rfind(".php") != std::string::npos)
		this->_pathToHandler = "/usr/bin/php";
	else if (_pathToScript.rfind(".perl") != std::string::npos || _pathToScript.rfind(".pl") != std::string::npos)
		this->_pathToHandler = "/usr/bin/perl";
	else if (_pathToScript.rfind(".cgi") != std::string::npos || _pathToScript.rfind(".exe") != std::string::npos)
		extentionOfSctipt = EXTENTION_WITHOUT_INTERPRETER;
	else if (_pathToScript.rfind(".py") == std::string::npos && _pathToScript.rfind(".php") == std::string::npos && 
	_pathToScript.rfind(".perl") == std::string::npos && _pathToScript.rfind(".pl") == std::string::npos && 
	_pathToScript.rfind(".cgi") == std::string::npos && _pathToScript.rfind(".exe") == std::string::npos)
	{
		std::cout << RED << "webserv: " << RESET << "extention of the script is false" << std::endl;
		return false;
	}

	/* массив с данными для передачи в execve, для скриптов с интерпретатором и без соответственно */
	const char *arguments_cgi[4] = {_fullPathToScript.c_str(), 0, 0, 0}; 				//если скрипту не требуется интерпретатор (C++)
	const char *arguments[3] = {_pathToHandler.c_str(), _fullPathToScript.c_str(), 0}; 	//если скрипт с интепретатором 	(python, perl)

	// std::cout << CYAN << _pathToHandler << RESET << std::endl;
	// std::cout << CYAN << _pathToScript << RESET << std::endl;
	// std::cout << CYAN << _fullPathToScript << RESET << std::endl;
	// std::cout << CYAN << relativePathToScript << RESET << std::endl;
	std::cout << CYAN << _pathToHandler << RESET << std::endl;

	// std::cout <<GREEN << extentionOfSctipt <<RESET << "\n";









	pid_t		pid;
//	int			saveStdn
	// char		**env;
	std::string	newBody;

	// try {
	// 	env = this->_getEnvAsCstrArray();
	// }
	// catch (std::bad_alloc &e) {
	// 	std::cerr << RED << e.what() << RESET << std::endl;
	// }

	// SAVING STDIN AND STDOUT IN ORDER TO TURN THEM BACK TO NORMAL LATER
//	saveStdin = dup(STDIN_FILENO);
//	saveStdout = dup(STDOUT_FILENO);

	FILE	*fIn = tmpfile();
	FILE	*fOut = tmpfile();
	long	fdIn = fileno(fIn);
	long	fdOut = fileno(fOut);
	int		ret = 1;

	write(fdIn, _body.c_str(), _body.size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		throw Exceptions();
		// std::cerr << RED << "Fork crashed" << RESET << std::endl;
		// return ("Status: 500\r\n\r\n");
		return false;
	}
	else if (!pid)
	{
//		char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);


		std::cout <<GREEN << extentionOfSctipt <<RESET << "\n";

		if (extentionOfSctipt == EXTENTION_WITHOUT_INTERPRETER)
			execve(arguments_cgi[0], (char* const*)arguments_cgi, (char* const*)_varsArray); //OK c++
		else if (extentionOfSctipt == EXTENTION_WITH_INTERPRETER)
			execve(arguments[0], (char* const*)arguments, (char* const*)_varsArray);     //OK py

		// throw Exceptions();
		// execve(arguments[0], nll, (char* const*)_varsArray);

		// execve(scriptName.c_str(), nll, env);
		std::cerr << RED << "Execve crashed" << RESET << std::endl;
		// write(STDOUT_FILENO, "Status: 500\r\n\r\n", 15);
		exit(EXIT_SUCCESS);
	}
	else
	{
		char	buffer[6024] = {0};

		waitpid(-1, NULL, 0);
		lseek(fdOut, 0, SEEK_SET);

		ret = 1;
		while (ret > 0)
		{
			memset(buffer, 0, 6024);
			ret = read(fdOut, buffer, 6024 - 1);
			newBody += buffer;
		}
	}



	/* 
	** вывод возвращаемого скриптом значения
	** std::cout << newBody << std::endl;
	*/

	/* если использовался скрипт для обработки формы "db.py", то
	считаваем созданный скриптом временный файл и формируем html-страницу */
	if (_fullPathToScript.rfind("db.py") != std::string::npos)
		addToDataBase();



//	dup2(saveStdin, STDIN_FILENO);
//	dup2(saveStdout, STDOUT_FILENO);
	fclose(fIn);
	fclose(fOut);
	close(fdIn);
	close(fdOut);
//	close(saveStdin);
//	close(saveStdout);




	return true;
}





