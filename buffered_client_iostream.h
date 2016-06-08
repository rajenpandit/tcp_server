#ifndef __BUFFERED_CLIENT_IOSTREAM_H__06032016_RAJEN_H__
#define __BUFFERED_CLIENT_IOSTREAM_H__06032016_RAJEN_H__
#include <vector>
#include <algorithm>
#include "client_iostream.h"
#include <iostream>
class condition_t{
public:
	condition_t() : _is_true(false){
	}
public:
	virtual bool is_true() = 0;
	virtual std::vector<char>::const_iterator check(const std::vector<char>& data,int last_size)=0;
protected:
	bool _is_true;
};

class datasize_t : public condition_t{
public:
	datasize_t(size_t size){
		_size = size;
	}
public:
	virtual std::vector<char>::const_iterator check(const std::vector<char>& data, __attribute__((unused)) int last_size){
		if(data.size() >= _size)	
		{
			_is_true = true;
			return data.cbegin()+_size;
		}
		_is_true = false;
		return data.cend();
	}
	virtual bool is_true(){
		return _is_true;
	}
private:
	size_t _size;
};

class string_found_t : public condition_t{
public:
	string_found_t(const std::string &s) : _string(s){
	}
public:
	virtual std::vector<char>::const_iterator check(const std::vector<char>& data,int last_size){
		auto it = data.cbegin() + last_size;
		it = std::search(it , data.end(), _string.begin(),_string.end());		
		if(it != data.cend()){
			_is_true = true;
			return it+_string.length();
		}
		else{
			_is_true = false;
			return it;
		}
	}
	virtual bool is_true(){
		return _is_true;
	}
private:
	std::string _string;
};


class buffered_client_iostream : public client_iostream{
	public:
		buffered_client_iostream(std::unique_ptr<socket_base> soc) : client_iostream(std::move(soc)){
			init();
		}
		buffered_client_iostream(const buffered_client_iostream&) = delete;
		void operator = (const buffered_client_iostream&) = delete;
		buffered_client_iostream(buffered_client_iostream&& bc_ios) : client_iostream(std::forward<buffered_client_iostream&&>(bc_ios)){
		}
		void operator = (buffered_client_iostream && bc_ios){
			swap(*this, bc_ios);
		}
		void init(){
			_data.clear();
		}
		friend void swap(buffered_client_iostream& bc_ios1, buffered_client_iostream& bc_ios2){
			std::lock(bc_ios1._mutex, bc_ios2._mutex);
			std::lock_guard<std::mutex> lk1(bc_ios1._mutex, std::adopt_lock);
			std::lock_guard<std::mutex> lk2(bc_ios2._mutex, std::adopt_lock);

			std::swap(bc_ios1._conditions, bc_ios2._conditions);
			std::swap(bc_ios1._data, bc_ios2._data);
			std::swap(bc_ios1._segmented_data, bc_ios2._segmented_data);
			std::swap(bc_ios1._last_check, bc_ios2._last_check);
		}
	public:
		virtual void notify_read(__attribute__((unused)) unsigned int events){
			char ch;
			while(_socket->receive(&ch, 1, false)){
				_data.push_back(ch);
			}
			notify();	
		}
		bool is_data_available(){
			if( !_conditions.empty() ){
				if( !_data.empty() ){
					for(auto &condition : _conditions)
					{
						auto cit = condition->check(_data,0);
						if( condition->is_true()){
							//	read( &_data[0], cit - _data.cbegin() );
							_segmented_data.assign(_data.cbegin(),cit);					
							_data.erase(_data.cbegin(),cit);
							_last_check=0;

							return true;
						}
					}
					_last_check=_data.size();
				}
			}
			else{
				if(!_data.empty()){
					_segmented_data=_data;
					_data.clear();
					return true;
				}
			}
			return false;
		}
		std::vector<char> get_data(){
			std::vector<char> vec = _segmented_data;
			_segmented_data.clear();
			return vec;
		}
		void set_condition(std::unique_ptr<condition_t> condition){
			_conditions.push_back(std::move(condition));
		}
		void clear_conditions(){
			_conditions.clear();
		}
		void read(__attribute__((unused)) void* data, __attribute__((unused)) size_t size) {
			//Not used
		}
	public:
		virtual void notify() = 0 ;
	private:
		std::vector<std::unique_ptr<condition_t>> _conditions;
		std::vector<char> _data;
		std::vector<char> _segmented_data;
		int _last_check;
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
