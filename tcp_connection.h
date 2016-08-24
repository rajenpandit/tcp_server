#ifndef __TCP_CONNECTION_H__30052016_RAJEN__
#define __TCP_CONNECTION_H__30052016_RAJEN___

#include "client_iostream.h"
#include "epoll_reactor.h"
#include "thread_pool.h"
#include "acceptor_base.h"
#include "socket_factory.h"
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>
#include <limits>
#include <atomic>
namespace rpt{
class tcp_connection{
public:
	enum connection_status_t{IDLE,SYNC_SENT,ESTABLISHED,CLOSED};
	enum error_t{NO_ERROR,CREATE_FAILED,BIND_FAILED,LISTEN_FAILED,RECEIVE_ERROR,CONNECT_ERROR,UNKNOWN_ERROR};
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
	friend class tcp_connection;
	};

public:
/* Constructor with max_thread */
	tcp_connection(std::shared_ptr<socket_factory> socket_factory, unsigned int max_thread,int max_events=1000, 
			std::function<void(connection_status_t, error_t)> error_handler=nullptr) : 
		_socket_factory(socket_factory),_threads(std::make_shared<thread_pool>(max_thread)),_reactor(max_events),
		_error_handler(error_handler){
	}
/* Constructor with thread_pool */
	tcp_connection(std::shared_ptr<socket_factory> socket_factory,std::shared_ptr<thread_pool> threads,int max_events=1000, 
			std::function<void(connection_status_t, error_t)> error_handler=nullptr) : 
		_socket_factory(socket_factory), _threads(threads),_reactor(max_events),
		_error_handler(error_handler){
	}
public:
	tcp_connection(tcp_connection&)=delete;
	tcp_connection(tcp_connection&&)=delete;
	void operator=(tcp_connection&)=delete;
	void operator=(tcp_connection&&)=delete;
	~tcp_connection(){
	}
public:
	void remove_client(int fd);
	void remove_descriptor(int fd);
	void client_handler(std::shared_ptr<fdbase> fdb, unsigned int events);	
	void accept(std::shared_ptr<fdbase> fdb, unsigned int events);
	void accept_impl(std::shared_ptr<fdbase> fdb, unsigned int events);
	const std::shared_ptr<socket_factory> get_socket_factory() const{
		return _socket_factory;
	}
	connection_status_t get_connection_status() const{
		return _connection_status;
	}	
	error_t get_error() const{
		return _connection_error;
	}	
public:
	template<typename T, class... TArgs>
	T* get_connection(const std::string& ip, int port, TArgs... args);
	void release_connection(client_iostream* client);
public: //functionalities
	void start_listening(std::shared_ptr<acceptor_base> accept,const endpoint &e,unsigned int max_connection=std::numeric_limits<unsigned int>::max(),
			bool block=true) noexcept;	
	void start_reactor(bool block=true){
		_threads->start(); // no action will be taken if threads are already running
		_reactor.run(block);
			//_threads->add_task(make_task(&epoll_reactor::run,&_reactor,block));
	}
	void stop_reactor(){
		_reactor.stop();
		_threads->stop();
	}
private:
	void set_error(connection_status_t status, error_t error);
private:
	std::shared_ptr<socket_factory> _socket_factory;
	std::shared_ptr<thread_pool> _threads;
	epoll_reactor _reactor;
	std::function<void(connection_status_t, error_t)> _error_handler;	
	error_t _connection_error;
	connection_status_t _connection_status;
	std::shared_ptr<acceptor_base> _acceptor;
	std::unordered_map<std::string,std::queue<std::shared_ptr<fdbase>>> _client_map;
	std::mutex _client_map_mutex;
	std::atomic_uint _max_connection;
};

template<typename T, class... TArgs>
T* tcp_connection::get_connection(const std::string& ip, int port, TArgs... args){
	std::shared_ptr<T> client;
	std::string key = ip + std::to_string(port) + typeid(T).name();
	auto it = _client_map.find(key);
	if( it != _client_map.end()){
		{
			std::lock_guard<std::mutex> lk(_client_map_mutex);
			auto& connection_que = it->second;
			if( !connection_que.empty() ){
				auto fd = connection_que.front();	
				connection_que.pop();
				client = std::dynamic_pointer_cast<T>(fd);
			}
		}
	}
	else{
		_client_map.insert(std::pair<std::string,std::queue<std::shared_ptr<fdbase>>>(key,std::queue<std::shared_ptr<fdbase>>()));
	}
	if(client == nullptr){
		client = std::make_shared<T>(args...);
		(*client)->create(port,ip.c_str());
		if((*client)->connect() == false)
			return nullptr;
	}
//	client->register_close_handler(std::bind(&tcp_connection::remove_descriptor,this,std::placeholders::_1));
	_reactor.register_descriptor(client,std::bind(&tcp_connection::client_handler,this,std::placeholders::_1,std::placeholders::_2));
	client->set_id(key);
	return client.get();
}
}
#endif
