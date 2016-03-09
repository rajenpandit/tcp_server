#ifndef __SOCKET_BASE_H__02032016_RAJEN__
#define __SOCKET_BASE_H__02032016_RAJEN__

#include <memory>
#include <iostream>
#include <mutex>
class socket_base{
#if 0
public:
	socket_base()=default;
	socket_base(std::shared_ptr<socket_base> socket) : _socket(socket){
	}
	socket_base(const socket_base& socket_base){
		_socket = socket_base._socket;	
		_event_handler = socket_base._event_handler;
		_close_event_handler = socket_base._close_event_handler;
		_is_connected = socket_base._is_connected;
	}
	void operator = (const socket_base& socket_base){
		_socket=socket_base._socket;	
		_event_handler = socket_base._event_handler;
		_close_event_handler = socket_base._close_event_handler;
		_is_connected = socket_base._is_connected;
	}
	socket_base(socket_base&& socket){
		std::swap(_socket, socket._socket);
		std::swap(_event_handler, socket._event_handler);
		std::swap(_close_event_handler, socket._close_event_handler);
		std::swap(_is_connected, socket._is_connected);
	}
	void operator = (socket_base&& socket){
		std::swap(_socket, socket._socket);
		std::swap(_event_handler, socket._event_handler);
		std::swap(_close_event_handler, socket._close_event_handler);
		std::swap(_is_connected, socket._is_connected);
	}
	virtual ~socket_base(){
		//std::cout<<"Destructor called"<<std::endl;
	}
public:
	/* register a event handler function, which will be notified when data arrives in socket 
	 * NOTE: this function is not valid in case socket is not register with epoll_reactor
	 */
	void register_event_handler(std::function<void(socket_base& socket, unsigned int events)> event_handler){
		_event_handler = event_handler;
		_socket->_event_handler = _event_handler;
	}
	void notify_event(unsigned int events){
		if(_event_handler!=nullptr)
			_event_handler(*this, events);	
	}
	
	void register_close_event_handler(std::function<void(socket_base& socket)> close_event_handler){
		_close_event_handler = close_event_handler;
		_socket->_close_event_handler = close_event_handler;
	}
	void notify_close(){
		if(_close_event_handler != nullptr)
			_close_event_handler(*this);
	}
#endif

public: //NOTE: All virtual function must be inherited by derived class
	virtual const int get_fd() = 0;

	virtual bool create(int port) = 0;
	virtual bool create(int port,const char *ip) = 0;
	virtual bool bind(bool reuse_add=true, bool keep_alive=true, bool no_delay=true) = 0;
	virtual bool listen(int backlog) = 0;
	virtual bool accept(socket_base &socket) = 0;
	virtual bool connect(const char* ip) = 0;
	virtual bool send(const void *buffer, size_t size) = 0;
	virtual bool send(const void *buffer, size_t size, int flags) = 0;
	virtual bool receive(void *buffer,size_t size,bool block=true) = 0;
	virtual bool close() = 0;
	virtual bool is_connected() = 0;
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
	std::function<void(socket_base& , unsigned int)> _event_handler;
	std::function<void(socket_base&)> _close_event_handler;
};
#endif
