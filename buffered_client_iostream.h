#ifndef __BUFFERED_CLIENT_IOSTREAM_H__06032016_RAJEN_H__
#define __BUFFERED_CLIENT_IOSTREAM_H__06032016_RAJEN_H__
#include <vector>
#include <algorithm>
#include "client_iostream.h"
class condition_t{
public:
	virtual void push_back (const char& c){
		_buffered_data.push_back(c);
	}
	virtual bool is_true() = 0;
public:
	char* get_data(){
		if(is_true()){
			return &_current_data[0];
		}
	}
	size_t get_size(){
		return _current_data.size();
	}
	void clear(){
		_current_data.clear();
	}
protected:
	std::vector<char> _current_data;
	std::vector<char> _buffered_data;
};

class datasize_t : public condition_t{
public:
	datasize_t(size_t size){
		_size = size;
	}
public:
	virtual bool is_true(){
		if(_buffered_data.size() >= _size || _current_data.size() >= _size){
			size_t size = _current_data.size();
			if(size < _size){
				auto it = std::next(_buffered_data.begin(), _size-size);
				std::move(_buffered_data.begin(), it, std::back_inserter(_current_data));
				_buffered_data.erase(_buffered_data.begin(), it);
			}
			return true;
		}
		else{
			return false;
		}
	}
private:
	size_t _size;
};

class string_found_t : public condition_t{
public:
	string_found_t(const std::string &s) : _string(s){
	}
public:
	virtual bool is_true(){
		if(_current_data.size() > 0)
			return true;
		if(_buffered_data.size() > 0){
			auto it = _buffered_data.begin();
			it += (_index - _string.length());
			it = std::search(it , _buffered_data.end(), _string.begin(),_string.end());
			if(it != _buffered_data.end()){
				it += _string.length();	
				std::move(_buffered_data.begin(), it, std::back_inserter(_current_data));
				_buffered_data.erase(_buffered_data.begin(), it);
				_index = 0;
				return true;
			}
			else{
				_index = _buffered_data.size();
				return false;
			}
		}
		else{
			return false;
		}
		return false;
	}
private:
	std::string _string;
	size_t _index;
};


class buffered_client_iostream : public client_iostream{
public:
	buffered_client_iostream(std::shared_ptr<socket_base> soc) : client_iostream(soc){
	}
	buffered_client_iostream(socket& soc) : client_iostream(soc){
	}
public:
	virtual void notify_read(unsigned int events){
		char ch;
		if(_condition != nullptr){
			while(_socket.receive(&ch, 1, false)){
				_condition->push_back(ch);
			}
			while(_condition->is_true()){
				read(_condition->get_data(),_condition->get_size());
				_condition->clear();
			}
		}
		else{
			std::vector<char> data;
			while(_socket.receive(&ch, 1, false))
				data.push_back(ch);
			read(&data[0],data.size());
		}
	}
	void set_condition(std::unique_ptr<condition_t> condition){
		_condition = std::move(condition);
	}
private:
	std::unique_ptr<condition_t> _condition;
};

#if 1
inline
std::unique_ptr<condition_t> datasize(size_t size){
	return std::unique_ptr<condition_t>(new datasize_t(size));
}
inline
std::unique_ptr<condition_t> string_found(const std::string& str){
	return std::unique_ptr<condition_t>(new string_found_t(str));
}
#endif
#endif
