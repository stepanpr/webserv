
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
 	+ `listen 8021` - порт который будет прослушиваться
 	+ `server_name 127.0.0.1` - IP-адрес сервера
 	+ `error_page /Users/emabel/webserv/www/default` - расположение страниц ошибок и состояний
 	+ `max_body_size 10` - масимально допустимый размер тела запроса
 	+ `cgi_alias /cgi_bin /Users/emabel/webserv/cgi_bin` - alias соответствующий разелу с cgi-скриптами
 	+ `location / {`    - соответствует запросу "/" (127.0.0.1/), устанавливаем конфигурацию для данного запроса
 		+ `index index.html` - страница которая будет загружаться в ответ на запрос
 		+ `autoindex off`   - автоиндекс (on/off)
 		+ `methods GET`     - разрешенные методы для даного раздела
 		+ `root  /Users/emabel/webserv/www/site.com`  - корень раздела
   `}`
  + `location /gallery {` - соответствует запросу "/gallery" (127.0.0.1/gallery), устанавливаем конфигурацию для данного запроса
    + ... `}`
     + wewe 
        + dfef  
  + `location /about {` - соответствует запросу "/about" (127.0.0.1/about), устанавливаем конфигурацию для данного запроса
    + ... `}`
  + `location /contact {`  - соответствует запросу "/contact" (127.0.0.1/contact), устанавливаем конфигурацию для данного запроса
    + ... `}`
`}`
