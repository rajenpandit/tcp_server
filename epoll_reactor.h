#ifndef __EPOLL_REACTOR_H__26022016_RAJEN__
#define __EPOLL_REACTOR_H__26022016_RAJEN___
#include <exception>
#include <string>
#include <sstream>
#include <sys/epoll.h>
#include <functional>
#include <map>
#include <memory>
#include "fdbase.h"

//void (call_back_fun_t*)(int fd, unsigned int events);
class epoll_call_back
{
public:
	epoll_call_back( const std::shared_ptr<fdbase>& fd, std::function<void(fdbase&, unsigned int)> call_back_fun ) : _fd(fd), 
	_call_back_fun(call_back_fun){
	}
public:
	void operator () (unsigned int events){
		_call_back_fun(*_fd, events );
	}
private:
	std::shared_ptr<fdbase> _fd;
	std::function<void(fdbase&, unsigned int)> _call_back_fun;
};



//typedef std::function<void(int, unsigned int)> call_back_fun_t;
class epoll_reactor_exception : public std::exception{
public:
	epoll_reactor_exception(const std::string& error) : _error(error){}
	virtual const char* what() const noexcept override{
		return _error.c_str();
	}
private:
	std::string _error;
};

class epoll_reactor{
public:
	epoll_reactor(unsigned int max_events):_max_events(max_events){
		if(_max_events == 0)
			_max_events = 1;
		_efd = epoll_create1 (0);
		if (_efd == -1){
			throw epoll_reactor_exception("Error:"+std::to_string(_efd));
		}
	}
public:
	bool register_descriptor(std::shared_ptr<fdbase> fdb, std::function<void(fdbase&, unsigned int)> call_back_fun);
	bool remove_descriptor(int fd);
	bool run();
private:
	int _efd;
	unsigned int _max_events;
	std::map<int,std::shared_ptr<epoll_call_back>> _callback_map;
};
#endif
