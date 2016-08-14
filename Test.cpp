#include "buffered_client_iostream.h"
#include "tcp_socket.h"
#include "tcp_socket_factory.h"
#include "thread_pool.h"
#include "acceptor_base.h"
#include "tcp_connection.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

thread_pool tp(2);
class client_handler : public client_iostream{
public:
	client_handler() : client_iostream(std::make_unique<tcp_socket>()){
	//	set_condition(datasize(5));	
	}
	~client_handler()
	{
		std::cout<<"Destroying client"<<std::endl;
	}
public:
	virtual void read(void* data, size_t size) override{
		static int x=0;
		std::string str = std::to_string(++x);
		std::cout<<"data:"<<static_cast<char*>(data)<<std::endl;
		write(str.c_str(),str.length());
	}
};
class A : public acceptor_base{
public:
	virtual std::shared_ptr<client_iostream> get_new_client() override{
		return std::make_shared<client_handler>();
	}
	virtual void notify_accept(std::shared_ptr<client_iostream> client,__attribute__((unused)) acceptor_status_t status) override{
		client->write("hello\n",6);
	}
};
void fun(int &x,int y){
	for(int i=0;i<10;i++){
		std::cout<<"tid:"<<std::this_thread::get_id()<<std::endl;
		x+=y;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main(){
	using std::placeholders::_1;
	using std::placeholders::_2;
	int x=10;
	int z=1;
	tp.start();
#if 0
	tp.add_task(make_task(fun,x,2));
	tp.add_task(make_task(fun,z,1));
	tp.add_task(make_task(fun,z,4));
	tp.add_task(make_task(fun,x,4));
#endif
#if 0
	A obj;
	tcp_server ts(std::make_shared<tcp_socket>(), 1,obj);
	ts.start(tcp_server::endpoint(1234));
#endif
#if 1
	tcp_socket_factory sf;
	tcp_connection ts(sf,2);
#if 0
	client_handler* client = ts.get_connection<client_handler>("127.0.0.1",2345);
	std::string str ("Hello");
	if(client)
		client->write(str.c_str(),str.length());	
#endif

	ts.start_listening(std::make_shared<A>(),tcp_connection::endpoint(1234));
	ts.start_reactor();
#endif
	while(1){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout<<"x:"<<x<<" z:"<<z<<std::endl;
	}
	tp.stop();
}
