CXXFLAGS:=$(CXXFLAGS) -std=c++11 -g -I../thread_pool
Test: tcp_socket.o Test.o tcp_server.o epoll_reactor.o
	g++ $(CXXFLAGS) Test.o tcp_socket.o tcp_server.o epoll_reactor.o ../thread_pool/thread_pool.o -lpthread -o Test.exe 
tcp_socket.o:
	g++ $(CXXFLAGS) -c tcp_socket.cpp
Test.o:
	g++ $(CXXFLAGS) -c Test.cpp
tcp_server.o:
	g++ $(CXXFLAGS) -c tcp_server.cpp
epoll_reactor.o:
	g++ $(CXXFLAGS) -c epoll_reactor.cpp
clean:
	rm *.o

