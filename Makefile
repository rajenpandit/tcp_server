CXXFLAGS:=$(CXXFLAGS) -std=c++11 -g  -I../thread_pool
THREAD_POOL_LIB_PATH=../thread_pool
Test: tcp_socket.o Test.o tcp_server.o epoll_reactor.o
	g++ $(CXXFLAGS) Test.o tcp_socket.o tcp_server.o  epoll_reactor.o $(THREAD_POOL_LIB_PATH)/thread_pool.o  -lpthread -o Test.exe 
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

