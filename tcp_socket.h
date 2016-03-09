#ifndef __TCP_SOCKET_H_05032016_RAJEN_H__
#define __TCP_SOCKET_H_05032016_RAJEN_H__
#include "socket_base.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

class tcp_socket : public socket_base{
public:
	virtual const int get_fd() override{
		return _fd;
	}
	virtual bool create(int port);
	virtual bool create(int port,const char *ip);
	virtual bool bind(bool reuse_add=true, bool keep_alive=true, bool no_delay=true);
	virtual bool listen(int backlog);
	virtual bool accept(socket_base &socket);
	virtual bool connect(const char* ip);
	virtual bool send(const void *buffer, size_t size);
	virtual bool send(const void *buffer, size_t size, int flags);
	virtual bool receive(void *buffer,size_t size,bool block=true);
	virtual bool close();
	virtual bool is_connected();
private:
	int _fd;
	addrinfo _addrinfo;
	bool _is_connected;
};
#endif
