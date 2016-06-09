#include "tcp_server.h"
#include <mutex>

void tcp_server::remove_client(int fd){
	_reactor.remove_descriptor(fd);
}
/* 
 * tcp_server::client_handler function will be called whenever client socket will receive an event.
 */
void tcp_server::client_handler(fdbase& fdb, unsigned int events){
	try{
		client_iostream& client = dynamic_cast<client_iostream&>(fdb);
		if(client.get_mutex().try_lock())
		{
			_threads->add_task(make_task([&,this](){
						//std::lock_guard<std::mutex> lk(client);
						std::lock_guard<std::mutex> lk(client, std::adopt_lock);
						if(events & (EPOLLRDHUP | EPOLLHUP)){
						client.close();
						}
						else{
						client.notify_read(events);
						}
						}));
		}
	}
	catch(const std::bad_cast& e){
	}
}
/* tcp_server::accept funtion will be called when server accepts a new connection from client 
 * once connection is accepted, the new connection client's socket will be passed as a parameter by
 * user's callback function _accept.
 */
void tcp_server::accept(fdbase& fdb, unsigned int events){
	//socket_base client_socket=_socket.get_new_socket();
	try{
		std::lock_guard<std::mutex> lk(fdb);
		socket& ssocket = (dynamic_cast<server_socket&>(fdb));
		std::shared_ptr<client_iostream> client=_accept.get_new_client();
		socket& client_socket = *client;
		if(ssocket.accept(client_socket))
		{
			using std::placeholders::_1;
			using std::placeholders::_2;
#if 0
			if(!client_socket.make_socket_non_block())
			{
			}
#endif
			client->register_close_handler(std::bind(&tcp_server::remove_client,this,_1));
			_reactor.register_descriptor(client,std::bind(&tcp_server::client_handler,this,_1,_2));
			_accept.notify_accept(client, acceptor_base::ACCEPT_SUCCESS);
		}
		else{
			_accept.notify_accept(client, acceptor_base::ACCEPT_FAILED);
		}
	}
	catch(const std::bad_cast& e){
	}
}

void tcp_server::set_error(server_status_t status, error_t error){
	_server_status=status;
	_server_error=error;
	if(_error_handler)
		_error_handler(_server_status, _server_error);
}


//void tcp_server::start(int port, const char* ip, int backlog) noexcept{
void tcp_server::start(const endpoint &e, bool block) noexcept{
	socket& socket = *_server_socket;
//	socket.make_socket_non_block();
	if(e.ip.length()){
		if(!socket.create(e.port,e.ip.c_str())){	
			set_error(START_ATTEMPTED,CREATE_FAILED);	
		}
	}	
	else{
		if(!socket.create(e.port)){
			set_error(START_ATTEMPTED,CREATE_FAILED);	
		}
	}
	if(!socket.bind()){
		set_error(START_ATTEMPTED,BIND_FAILED);	
	}	
	if(!socket.listen(e.backlog)){
		set_error(START_ATTEMPTED,LISTEN_FAILED);	
	}

	using std::placeholders::_1;
	using std::placeholders::_2;
	_threads->start(); // no action will be taken if threads are already running
	_reactor.register_descriptor(_server_socket,std::bind(&tcp_server::accept,this,_1,_2));
	_reactor.run();
}
