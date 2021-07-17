
# webserv
***

### description:

*webserv* - проект HTTP-сервера, ядро которого основано на POLL.
Обрабатывает GET, POST и DELETE запросы.
По умолчанию в "site.com" подключен скрипт "bd.py", считывающий данные с форм (страница "contact.html)
и добавляющий эти данные в генерируемую html-страницу в раздел `/bd/`

### directories:

1. `/config/` - файлы конфигураций
2. `/www/` - основной раздел для контента
3. `/www/default/` - директория служебных станиц (errors, etc)
4. `/www/site.com/` - сайт-пример установленный по умолчанию
5. `/www/YourSite.com/` - новые разделы необходимо размещать в каталоге www
6. `/cgi_bin/` - раздел с cgi-скриптами
7. `/bd/` - раздел "базы данных", где генерируются html-страницы с данными


### examples:
 use `make` to compile
 then run `./webserv` for start program
 or `./webserv [path to configuration file]` to run webserv with your configuration file
 
### commands for check:

 * для записи нового файла на сервер методом POST, используйте комманду: `curl -X POST -H "Content-Type: text/plain" --data "TEXT body of file" 127.0.0.1:8021/www/site.com/NewFile`
 * для удаления файла с сервера методом DELETE, используйте комманду: `curl -X DELETE 127.0.0.1:8021/www/site.com/NewFile`
 * для отправки файла на сервер, используйте комманду: `curl -F 'fileX=@car.jpg' localhost:8021` (файл запишется в корень сайта)

### default configuration

 + *server {* - раздел сервера
 	+ *listen 8021* - порт который будет прослушиваться
 	+ *server_name 127.0.0.1* - IP-адрес сервера
 	+ *error_page /Users/emabel/webserv/www/default* - расположение страниц ошибок и состояний
 	+ `max_body_size 10` - масимально допустимый размер тела запроса
	cgi_alias /cgi_bin /Users/emabel/webserv/cgi_bin
       location / {
        index index.html
        autoindex off
        methods GET
        root  /Users/emabel/webserv/www/site.com
        }
       location /gallery {
        index gallery.html
        autoindex off
        methods GET POST HEAD
        root  /Users/emabel/webserv/www/site.com
        }
       location /about {
        index about.html
        autoindex off
        methods POST GET
        root  /Users/emabel/webserv/www/site.com
        }
       location /contact {
        index contact.html
        autoindex off
        methods POST GET DELETE
        root  /Users/emabel/webserv/www/site.com
        }
}
