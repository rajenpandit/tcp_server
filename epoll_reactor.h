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
/*
*	epoll_call_back class provides an interface to epoll_reactor 
*	to maintain lifetime for a socket / fdbase.
*/
class epoll_call_back
{
public:
	epoll_call_back( std::shared_ptr<fdbase> fd, std::function<void(std::shared_ptr<fdbase>fdbase, unsigned int)> call_back_fun ) : _fd(fd), 
	_call_back_fun(call_back_fun){
	}
public:
	void operator () (unsigned int events){
		_call_back_fun(_fd, events );
	}
	std::shared_ptr<fdbase> get_fd(){
		return _fd;
	}
private:
	std::shared_ptr<fdbase> _fd;
	std::function<void(std::shared_ptr<fdbase>, unsigned int)> _call_back_fun;
};









/*
*	epoll_reactor_exception class inherites property of std::exception,
*	which shall be use by epoll_reactor to handle runtime exceptions.
*/
class epoll_reactor_exception : public std::exception{
public:
	epoll_reactor_exception(const std::string& error) : _error(error){}
	virtual const char* what() const noexcept override{
		return _error.c_str();
	}
private:
	std::string _error;
};








/* 
*	epoll_reactor class manages socket descriptors or fds. 
*	It calls registered evenet handler function whenever fd 
*	is ready for read/write operations.
*/

class epoll_reactor{
public:
	epoll_reactor(unsigned int max_events):_max_events(max_events){
		if(_max_events == 0)
			_max_events = 1;
		_efd = epoll_create1 (0);
		if (_efd == -1){
			throw epoll_reactor_exception("Error:"+std::to_string(_efd));
		}
		is_running=false;
	}
public:
	/* associates a callback function to a descriptor, 
	   epoll_reactor invokes call_back function once descriptor 
	   is ready for read/write*/
	bool register_descriptor(std::shared_ptr<fdbase> fdb, std::function<void(std::shared_ptr<fdbase>, unsigned int)> call_back_fun);
	std::shared_ptr<fdbase> remove_descriptor(int fd);
	/* Start epoll_reactor */
	bool run();
	void stop(){
		is_running=false;
	}
private:
	int _efd;
	bool is_running;
	unsigned int _max_events;
	std::map<int,std::shared_ptr<epoll_call_back>> _callback_map;
	std::mutex _mutex;
};
#endif
