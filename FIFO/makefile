CXX = gcc
fifo: FIFO.o
	$(CXX) FIFO.o -o fifo -lpthread -lrt
FIFO.o: FIFO.c
	$(CXX) -c FIFO.c -lrt -lpthread
clean:
	rm *.o
	rm fifo
