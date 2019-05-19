Location-oriented encrypted chat

--------------how to start----------------------

g++-5, проверьте чтоб он у вас был, если будете собирать.
Проверка:
>> g++ --version // не меньше 5

Дальше надо установить Boost.
Для linux:
>>sudo apt-get install libboost-all-dev
>>sudo apt-get install aptitude
>>aptitude search boost

Дальше собрать:

>>make Makefile

Путь к компилятору, если он стоит в нестандартном пути, можно задать так:
CXX=/usr/bin/g++-5 make


А дальше, при сборку, у меня падает с непонятной ошибкой :-((((


Запускается так:
server:
$ ./server/src/BUILD/chatserver --sert server.pem --port 9001

client:
$ ./client/src/BUILD/chatclient --server localhost:9001
