#ifndef __SERVER_SOCKET_H_05032016_RAJEN__
#define __SERVER_SOCKET_H_05032016_RAJEN__

#include "socket.h"
#include "fdbase.h"

class server_socket : public fdbase{
public:
#if 0
	server_socket(socket_base&& soc) : _socket(std::move(soc)){
	}
#endif
	server_socket(std::shared_ptr<socket_base> soc) : _socket(soc){
	}
	server_socket(socket& soc) : _socket(soc){
	}
public:
	virtual socket& get_socket() override{
		return _socket;
	}
private:
	socket _socket;
};
#endif
