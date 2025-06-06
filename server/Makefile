server:main.o worker.o taskQueue.o tidArr.o threadPool.o epoll.o tcpInit.o transfile.o
	gcc $^ -o $@ -lpthread
testTaskQueue:taskQueue.o testTaskQueue.o
	gcc $^ -o $@ -lpthread
main.o:main.c
	gcc -c $^ -o $@ -g -Wall
worker.o:worker.c
	gcc -c $^ -o $@ -g -Wall
taskQueue.o:taskQueue.c
	gcc -c $^ -o $@ -g -Wall
tidArr.o:tidArr.c
	gcc -c $^ -o $@ -g -Wall
threadPool.o:threadPool.c
	gcc -c $^ -o $@ -g -Wall
epoll.o:epoll.c
	gcc -c $^ -o $@ -g -Wall
tcpInit.o:tcpInit.c
	gcc -c $^ -o $@ -g -Wall
transfile.o:transfile.c
	gcc -c $^ -o $@ -g -Wall
testTaskQueue.o:testTaskQueue.c
	gcc -c $^ -o $@ -g -Wall
clean:
	rm *.o server
