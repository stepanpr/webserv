
# webserv

*** 
### Directories:
 1. `/config/` - файлы конфигураций
 2. `/www/` - основной раздел для контента
 3. `/www/default/` - директория служебных станиц (errors, etc)
 4. `/www/site.com/` - сайт-пример установленный по умолчанию
 5. `/www/YourSite.com/` - новые разделы необходимо размещать в каталоге www
 6.  `/cgi_bin/` - раздел с cgi-скриптами
 7.  `/bd/` - раздел "базы данных", где генерируются html-страницы с данными

***
### description:
webserv проект HTTP-сервера, ядро которого основано на POLL.
Обрабатывает GET, POST и DELETE запросы.
По умолчанию в "site.com" подключен скрипт "bd.py", считывающий данные с форм (страница "contact.html)
и добавляющий эти данные в генерируемую html-страницу в раздел `/bd/`

***
#### examples:
 use `make` to compile
 then run `./webserv` for start program
 or `./webserv [path to configuration file]` to run webserv with your configuration file
 
 #### commands for check:
 * для записи нового файла на сервер методом POST, используйте комманду: `curl -X POST -H "Content-Type: text/plain" --data "TEXT body of file" 127.0.0.1:8021/www/site.com/NewFile`
 * для удаления файла с сервера методом DELETE, используйте комманду: `curl -X DELETE 127.0.0.1:8021/www/site.com/NewFile`
 * для отправки файла на сервер, используйте комманду: `curl -F 'fileX=@car.jpg' localhost:8021` (файл запишется в корень сайта)


