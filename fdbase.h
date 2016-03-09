#ifndef __FDBASE_H__05032016_H__
#define __FDBASE_H__05032016_H__
#include "socket.h"
class fdbase{
public:
	virtual socket& get_socket() = 0;	
};
#endif
