VERSION=1.0.0.0.2
CXXFLAGS:=$(CXXFLAGS) -std=c++14 -g  -Wall -Wextra -I../thread_pool
THREAD_POOL_LIB_PATH=../thread_pool
OBJS=tcp_socket.o epoll_reactor.o tcp_connection.o
all: $(OBJS) release
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
	rm *.o
install:
	cp *.h /opt/STSLib/include
	cp libtcpserver.a /opt/STSLib
clean:
	rm *.o libtcpserver.a

