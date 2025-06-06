server:main.o worker.o taskQueue.o tidArr.o threadPool.o tcpInit.o epoll.o transfile.o
	gcc main.o worker.o taskQueue.o tidArr.o threadPool.o tcpInit.o epoll.o transfile.o -o server -lpthread
main.o:main.c
	gcc -c main.c -o main.o -g -Wall
worker.o:worker.c
	gcc -c worker.c -o worker.o -g -Wall
test: test.o taskQueue.o
	gcc test.o taskQueue.o -o test -lpthread
test.o:test.c
	gcc -c test.c -o test.o -g -Wall
taskQueue.o:taskQueue.c
	gcc -c taskQueue.c -o taskQueue.o -g -Wall
tidArr.o:tidArr.c
	gcc -c tidArr.c -o tidArr.o -g -Wall
threadPool.o:threadPool.c
	gcc -c threadPool.c -o threadPool.o -g -Wall
tcpInit.o:tcpInit.c
	gcc -c tcpInit.c -o tcpInit.o -g -Wall
epoll.o:epoll.c
	gcc -c epoll.c -o epoll.o -g -Wall
transfile.o:transfile.c
	gcc -c transfile.c -o transfile.o -g -Wall
