
Server: TcpServer.o
	g++ TcpServer.o -o Server

obj = TcpServer.cpp TcpServer.hpp TcpServer.h 

TcpServer.o: $(obj)
	g++ -c $(obj) 

clean:
	rm -rf *.o *.gch Server
