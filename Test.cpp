#include "buffered_client_iostream.h"
#include "tcp_server.h"
#include "http_server.h"
#include "tcp_socket.h"
#include "STSSocket.h"
#include "thread_pool.h"
#include "acceptor_base.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <memory>

thread_pool tp(2);
class client_handler : public buffered_client_iostream{
public:
	client_handler() : buffered_client_iostream(std::make_shared<tcp_socket>()){
	//	set_condition(datasize(5));	
	}
public:
	virtual void read(void* data, size_t size) override{
		static int x=0;
		std::string str = std::to_string(++x);
		std::cout<<static_cast<char*>(data)<<std::endl;
		write(str.c_str(),str.length());
	}
};
class A : public acceptor_base{
public:
	virtual std::shared_ptr<client_iostream> get_new_client() override{
		return std::make_shared<client_handler>();
	}
	virtual void notify_accept(std::shared_ptr<client_iostream> client, acceptor_status_t status) override{
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
#if 1
	A obj;
	http_server ts(std::make_shared<tcp_socket>(), 1);
	ts.start(http_server::endpoint(1234));
#endif
	while(1){
		std::this_thread::sleep_for(std::chrono::seconds(1));
		std::cout<<"x:"<<x<<" z:"<<z<<std::endl;
	}
	tp.stop();
}
