#ifndef __TCP_SERVER_H__02032016_RAJEN__
#define __TCP_SERVER_H__02032016_RAJEN__

#include "server_socket.h"
#include "client_iostream.h"
#include "epoll_reactor.h"
#include "thread_pool.h"
#include "acceptor_base.h"
#include <functional>
#include <map>

class tcp_server{
public:
	enum server_status_t{IDLE,START_ATTEMPTED,RUNNING,STOPPED};
	enum error_t{NO_ERROR,CREATE_FAILED,BIND_FAILED,LISTEN_FAILED,RECEIVE_ERROR,UNKNOWN_ERROR};
public:
	class endpoint{
	public:
		endpoint(int port, const std::string& ip="", int backlog=5){
			this->port = port;
			this->ip = ip;
			this->backlog = backlog;
		}
	private:
		int port;
		std::string ip;
		int backlog;
	friend class tcp_server;
	};
public:
/* Constructor with max_thread */
	tcp_server(std::shared_ptr<socket_base> socket, unsigned int max_thread, acceptor_base& accept , 
			std::function<void(server_status_t, error_t)> error_handler=nullptr) : 
		_server_socket(std::make_shared<server_socket>(socket)),_threads(std::make_shared<thread_pool>(max_thread)), _accept(accept),
		_error_handler(error_handler),_reactor(1000){
	}
/* Constructor with thread_pool */
	tcp_server(std::shared_ptr<socket_base> socket, std::shared_ptr<thread_pool> threads, acceptor_base& accept, 
			std::function<void(server_status_t, error_t)> error_handler=nullptr) : 
		_server_socket(std::make_shared<server_socket>(socket)),_threads(threads), _accept(accept),
		_error_handler(error_handler),_reactor(1000){
	}
public:
	tcp_server(tcp_server&)=delete;
	tcp_server(tcp_server&&)=delete;
	void operator=(tcp_server&)=delete;
	void operator=(tcp_server&&)=delete;
	~tcp_server(){
	}
public:
	void remove_client(int fd);
	void client_handler(fdbase& fdb, unsigned int events);	
	void accept(fdbase& fdb, unsigned int events);
	void start(const endpoint &e, bool block=true) noexcept;	
	server_status_t get_server_status() const{
		return _server_status;
	}	
	error_t get_error() const{
		return _server_error;
	}	
private:
	void set_error(server_status_t status, error_t error);
private:
	std::shared_ptr<server_socket> _server_socket;
	std::shared_ptr<thread_pool> _threads;
	server_status_t _server_status;
	error_t _server_error;
	epoll_reactor _reactor;
	std::function<void(server_status_t, error_t)> _error_handler;	
	acceptor_base& _accept;
	//std::function<std::shared_ptr<client_iostream>()> _accept;
//	std::function<void(socket_base&)> _accept;
	std::map<int,std::shared_ptr<fdbase>> _client_map;
};
#endif
