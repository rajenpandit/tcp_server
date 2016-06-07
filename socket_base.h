#ifndef __SOCKET_BASE_H__02032016_RAJEN__
#define __SOCKET_BASE_H__02032016_RAJEN__

#include <memory>
#include <iostream>
#include <mutex>
class socket_base{

public: //NOTE: All virtual function must be inherited by derived class
	virtual const int& get_fd() = 0;

	virtual bool create(int port) = 0;
	virtual bool create(int port,const char *ip) = 0;
	virtual bool bind(bool reuse_add=true, bool keep_alive=true, bool no_delay=true) = 0;
	virtual bool listen(int backlog) = 0;
	virtual bool accept(socket_base &socket) = 0;
	virtual bool connect() = 0;
	virtual bool send(const void *buffer, size_t size) = 0;
	virtual bool send(const void *buffer, size_t size, int flags) = 0;
	virtual bool receive(void *buffer,size_t size,bool block=true) = 0;
	virtual bool close() = 0;
	virtual bool is_connected() = 0;
public:
	std::mutex _mutex;
	bool _is_connected;
private:
	std::shared_ptr<socket_base> _socket;
	std::function<void(socket_base& , unsigned int)> _event_handler;
	std::function<void(socket_base&)> _close_event_handler;
};
#endif
