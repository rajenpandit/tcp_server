#include "tcp_connection.h"
#include "server_socket.h"
#include <mutex>

void tcp_connection::remove_client(int fd){
	_reactor.remove_descriptor(fd);
	++_max_connection;
}
/* 
 * tcp_connection::client_handler function will be called whenever client socket will receive an event.
 */
void tcp_connection::client_handler(std::shared_ptr<fdbase> fdb, unsigned int events){
	std::shared_ptr<client_iostream> client = std::dynamic_pointer_cast<client_iostream>(fdb);
	std::cout<<"Event Flag:"<<events<<std::endl;
	if(client != nullptr){
		if(client->get_mutex().try_lock())
		{
			_threads->add_task(make_task([=](){
						//std::lock_guard<std::mutex> lk(client);
						std::lock_guard<std::mutex> lk(*client, std::adopt_lock);
						std::cout<<events <<"& ("<<EPOLLRDHUP<<"|"<<EPOLLHUP<<")"<<(events & (EPOLLRDHUP | EPOLLHUP))<<std::endl;
						if(events & (EPOLLRDHUP | EPOLLHUP)){
						std::cout<<"Closing connection\n";
						client->close();
						}
						else{
						std::cout<<"Notifying client\n";
						client->notify_read(events);
						}
						}));
		}
	}
}
void tcp_connection::release_connection(client_iostream* client){
	if(client != nullptr){
		std::string key = client->get_id();
		auto it = _client_map.find(key);
		if( it != _client_map.end()){
			{
				std::lock_guard<std::mutex> lk(_client_map_mutex);
				auto& connection_que = it->second;
				std::shared_ptr<fdbase> fd = _reactor.remove_descriptor(client->get_fd());
				if(fd != nullptr){
					connection_que.push(fd);
				}
			}
		}
	}
}
/* tcp_connection::accept funtion will be called when server accepts a new connection from client 
 * once connection is accepted, the new connection client's socket will be passed as a parameter by
 * user's callback function _accept.
 */
void tcp_connection::accept(std::shared_ptr<fdbase> fdb,__attribute__((unused)) unsigned int events){
	//socket_base client_socket=_socket.get_new_socket();
	try{
		std::lock_guard<std::mutex> lk(*fdb);
		server_socket& ssocket = (dynamic_cast<server_socket&>(*fdb));
		std::shared_ptr<client_iostream> client=_acceptor->get_new_client();
		if(ssocket->accept(*client))
		{
			std::cout<<"Max Connection:"<<_max_connection<<std::endl;
			if(_max_connection == 0){
				client->close();
				return;
			}
			using std::placeholders::_1;
			using std::placeholders::_2;
#if 0
			if(!client_socket.make_socket_non_block())
			{
				std::cout<<"unable to make non block"<<std::endl;
			}
#endif
			client->register_close_handler(std::bind(&tcp_connection::remove_client,this,_1));
			_reactor.register_descriptor(client,std::bind(&tcp_connection::client_handler,this,_1,_2));
			_acceptor->notify_accept(client, acceptor_base::ACCEPT_SUCCESS);
			--_max_connection;
		}
		else{
			_acceptor->notify_accept(client, acceptor_base::ACCEPT_FAILED);
		}
	}
	catch(const std::bad_cast& e){
	}
}
void tcp_connection::set_error(connection_status_t status, error_t error){
	_connection_status=status;
	_connection_error=error;
	if(_error_handler)
		_error_handler(_connection_status, _connection_error);
}

void tcp_connection::start_listening(std::shared_ptr<acceptor_base> acceptor,const endpoint &e,unsigned int max_connection,
		__attribute__((unused)) bool block) noexcept{
	_acceptor = acceptor;	
	_max_connection = max_connection;
	std::unique_ptr<socket_base> socket = _socket_factory.get_socket();
//	socket.make_socket_non_block();
	if(e.ip.length()){
		if(!socket->create(e.port,e.ip.c_str())){	
			set_error(SYNC_SENT,CREATE_FAILED);	
		}
	}	
	else{
		if(!socket->create(e.port)){
			set_error(SYNC_SENT,CREATE_FAILED);	
		}
	}
	if(!socket->bind()){
		set_error(SYNC_SENT,BIND_FAILED);	
	}	
	if(!socket->listen(e.backlog)){
		set_error(SYNC_SENT,LISTEN_FAILED);	
	}

	using std::placeholders::_1;
	using std::placeholders::_2;
	_reactor.register_descriptor(std::make_shared<server_socket>(std::move(socket)),std::bind(&tcp_connection::accept,this,_1,_2));
}

