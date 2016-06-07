#include "epoll_reactor.h"
#include <iostream>
bool epoll_reactor::register_descriptor(std::shared_ptr<fdbase> fdb, std::function<void(std::shared_ptr<fdbase>, unsigned int)> call_back_fun){
	struct epoll_event event;
	int s;
	int fd=fdb->get_fd();
	{
		std::lock_guard<std::mutex> lk(_mutex);
		auto it = _callback_map.find(fd);
		if(it != _callback_map.end()){
			_callback_map.erase(it);
		}
		_callback_map[fd]= std::make_shared<epoll_call_back>(fdb,call_back_fun);
		event.data.ptr = _callback_map[fd].get(); //lifetime of pointer to object will be managed by _callback_map
		std::cout<<"Registering FD:"<<fd<<std::endl;
	}

	event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	s = epoll_ctl (_efd, EPOLL_CTL_ADD, fd, &event);
	if (s == -1)
	{
		std::cout<<"Error during register"<<std::endl;
		return false;
	}
	return true;
}
std::shared_ptr<fdbase> epoll_reactor::remove_descriptor(int fd){
	std::lock_guard<std::mutex> lk(_mutex);
	/* In  kernel  versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-NULL pointer in event, even though this argument is ignored. */
	if(epoll_ctl (_efd, EPOLL_CTL_DEL, fd, NULL) == -1)
		return nullptr;
	auto it = _callback_map.find(fd);
	if(it != _callback_map.end()){
		std::cout<<"Removing FD:"<<fd<<std::endl;
		std::shared_ptr<fdbase> fd = it->second->get_fd();
		_callback_map.erase(it);
		return fd;
	}
	return nullptr;
}
bool epoll_reactor::run(){
	if(is_running)
		return false;
	struct epoll_event* events = static_cast<epoll_event*>(calloc (_max_events, sizeof(epoll_event)));
	is_running=true;
	while(true)
	{
		int n = epoll_wait (_efd, events, _max_events, 5);
		for (int i = 0; i < n; i++)
		{
			epoll_call_back* epoll_call_back_p = static_cast<epoll_call_back*>(events[i].data.ptr);
			(*epoll_call_back_p)(events[i].events);
		}
		if(is_running == false){
			free(events);
			return true;
		}
	}
	is_running=false;
}
