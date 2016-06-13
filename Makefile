CXXFLAGS:=$(CXXFLAGS) -std=c++14 -O3  -Wall -Wextra -I../thread_pool
THREAD_POOL_LIB_PATH=../thread_pool
OBJS=tcp_socket.o epoll_reactor.o tcp_connection.o
Test:Test.o $(OBJS) 
	g++ $(CXXFLAGS) Test.o $(OBJS) $(THREAD_POOL_LIB_PATH)/thread_pool.o  -lpthread -o Test.exe 
tcp_socket.o:
	g++ $(CXXFLAGS) -c tcp_socket.cpp
Test.o:
	g++ $(CXXFLAGS) -c Test.cpp
tcp_server.o:
	g++ $(CXXFLAGS) -c tcp_server.cpp
epoll_reactor.o:
	g++ $(CXXFLAGS) -c epoll_reactor.cpp
tcp_connection.o:
	g++ $(CXXFLAGS) -c tcp_connection.cpp
release:
	ar -rv libtcpserver.a $(OBJS)
clean:
	rm *.o libtcpserver.a

