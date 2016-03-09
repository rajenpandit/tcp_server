#ifndef __CLIENT_IOSTREAM_H__05032016_RAJEN_H__
#define __CLIENT_IOSTREAM_H__05032016_RAJEN_H__
#include "fdbase.h"
#include "socket.h"
#include "vector"
class client_iostream : public fdbase{
public:
#if 0
	client_iostream(socket_base&& soc) : _socket(std::move(soc)){
	}
#endif
	client_iostream(std::shared_ptr<socket_base> soc) : _socket(soc){
	}
	client_iostream(socket& soc) : _socket(soc){
	}
public:
	virtual socket& get_socket() override{
		return _socket;
	}
	bool close(){
		_close_handler(_socket.get_fd());
		return _socket.close();
	}
	virtual bool write(const void* data, size_t size){
		if(_socket.is_connected())
			return _socket.send(data, size);
		return false;
	}
	virtual void notify_read(unsigned int events){
		std::vector<char> _data;
		char ch;
		while(_socket.receive(&ch, 1, false)){
			_data.push_back(ch);
		}
		read(&_data[0],_data.size());
	}
public:
	virtual void read(void* data, size_t size) = 0;
public:
	void register_close_handler(std::function<void(int)> fun){
		_close_handler = fun;
	}
public:
	std::mutex _mutex;
protected:
	socket _socket;
	std::function<void(int)> _close_handler;
};
#endif
