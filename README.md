Location-oriented encrypted chat

--------------how to start----------------------

g++-5, проверьте чтоб он у вас был, если будете собирать.
Путь к компилятору, если он стоит в нестандартном пути, можно задать так:
CXX=/usr/bin/g++-5 make

Запускается так:
server:
$ ./server/src/BUILD/chatserver —sert server.pem —port 9001

client:
$ ./client/src/BUILD/chatclient —server localhost:9001
