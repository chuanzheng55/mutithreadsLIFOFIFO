CXX = gcc
lifo: lifo.o
	$(CXX) lifo.o -o lifo -lpthread -lrt
lifo.o: lifo.c
	$(CXX) -c lifo.c -lrt -lpthread
clean:
	rm *.o
	rm lifo
