#ifndef __SOCKET_H__04032016_RAJEN__
#define __SOCKET_H__04032016_RAJEN__

#include <memory>
#include <iostream>
#include <mutex>
#include <unistd.h>
#include <fcntl.h>
#include "socket_base.h"
class socket{
public:
#if 0
	socket(socket_base&& soc) : _socket(std::make_shared<socket_base>(std::move(soc))){
	}
#endif
	socket(std::shared_ptr<socket_base> soc) : _socket(soc){
	}
	socket(const socket& socket){
	}
	void operator = (const socket& socket){
	}
	socket(socket&& socket){
	}
	void operator = (socket&& socket){
	}
	~socket(){
		//std::cout<<"Destructor called"<<std::endl;
	}
public:
	socket_base& operator * (){
		return *_socket;
	}
#if 0
public:
	/* register a event handler function, which will be notified when data arrives in socket 
	 * NOTE: this function is not valid in case socket is not register with epoll_reactor
	 */
	void register_event_handler(std::function<void(socket& socket, unsigned int events)> event_handler){
		_event_handler = event_handler;
		_socket->_event_handler = _event_handler;
	}
	void notify_event(unsigned int events){
		if(_event_handler!=nullptr)
			_event_handler(*this, events);	
	}

	void register_close_event_handler(std::function<void(socket& socket)> close_event_handler){
		_close_event_handler = close_event_handler;
		_socket->_close_event_handler = close_event_handler;
	}
	void notify_close(){
		if(_close_event_handler != nullptr)
			_close_event_handler(*this);
	}
#endif

public: //NOTE: All  function must be inherited by derived class
	const int get_fd(){
		return _socket->get_fd();	
	}
	bool create(int port){
		return _socket->create(port);
	}
	bool create(int port,const char *ip){
		return _socket->create(port,ip);
	}
	bool bind(bool reuse_add=true, bool keep_alive=true, bool no_delay=true){
		return _socket->bind(reuse_add,keep_alive,no_delay);
	}
	bool listen(int backlog){
		return _socket->listen(backlog);
	}
	bool accept(socket &socket){
		return _socket->accept(*socket);
	}
	bool connect(const char* ip){
		return _socket->connect(ip);
	}
	bool send(const void *buffer, size_t size){
		return _socket->send(buffer,size);
	}
	bool send(const void *buffer, size_t size, int flags){
		return _socket->send(buffer,size,flags);
	}
	bool receive(void *buffer,size_t size,bool block=true){
		return _socket->receive(buffer, size, block);
	}
	bool close(){
		return _socket->close();
	}
	bool is_connected(){
		return _socket->is_connected();
	}
#if 0
	bool make_socket_non_block(){
		int flags, s;
		int sfd = get_fd();
		flags = fcntl (sfd, F_GETFL, 0);
		if (flags == -1){
			return false;
		}
		flags |= O_NONBLOCK;
		std::cout<<"set non blocking socket:"<<sfd<<" flag:"<<flags<<std::endl;
		s = fcntl (sfd, F_SETFL, flags);
		if (s == -1){
			return false;
		}
		return true;
	}
#endif
#if 0
public:
	template<typename T>
	T& getSocket(){
		return *(dynamic_cast<T*>(_socket.get()));
	}
	socket_base& get_socket(){
		return *_socket.get();
	}

#endif
public:
	std::mutex _mutex;
	bool _is_connected;
private:
	std::shared_ptr<socket_base> _socket;
	std::function<void(socket& , unsigned int)> _event_handler;
	std::function<void(socket&)> _close_event_handler;
};
#endif
