#include "epoll_reactor.h"
bool epoll_reactor::register_descriptor(std::shared_ptr<fdbase> fdb, std::function<void(fdbase&, unsigned int)> call_back_fun){
	struct epoll_event event;
	int s;
	int fd=fdb->get_socket().get_fd();
//	std::cout<<" registering socket:"<<fd<<std::endl;
	auto it = _callback_map.find(fd);
	if(it != _callback_map.end()){
		_callback_map.erase(it);
	}
	std::shared_ptr<epoll_call_back> callback = std::make_shared<epoll_call_back>(fdb,call_back_fun);
	_callback_map[fd] = callback;
	event.data.ptr = callback.get(); //lifetime of pointer to object will be managed by _callback_map

	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl (_efd, EPOLL_CTL_ADD, fd, &event);
	if (s == -1)
	{
		std::cout<<"Error during register"<<std::endl;
		return false;
	}
	return true;
}
bool epoll_reactor::remove_descriptor(int fd){
	/* In  kernel  versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-NULL pointer in event, even though this argument is ignored. */
	int s = epoll_ctl (_efd, EPOLL_CTL_DEL, fd, NULL);
}
bool epoll_reactor::run(){
	struct epoll_event* events = static_cast<epoll_event*>(calloc (_max_events, sizeof(epoll_event)));
	while(true)
	{
		int n = epoll_wait (_efd, events, _max_events, -1);
		for (int i = 0; i < n; i++)
		{
			epoll_call_back* epoll_call_back_p = static_cast<epoll_call_back*>(events[i].data.ptr);
			(*epoll_call_back_p)(events[i].events);
#if 0
			if ((events[i].events & EPOLLERR) ||
					(events[i].events & EPOLLHUP) ||
					(!(events[i].events & EPOLLIN)))
			{
				/* An error occured on this fd, or the socket is not ready for reading */
				epoll_call_back_p->close();
			}
			else
			{
				/* Data is available on this fd */
				epoll_call_back_p->notify_event(events[i].events);
			}
#endif
		}
	}
}
