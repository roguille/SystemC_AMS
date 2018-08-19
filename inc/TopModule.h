#ifndef TOPMODULE_H_
#define TOPMODULE_H_

#include "systemc.h"
#include "Memory.h"
#include "CPU.h"
#include "Router.h"

#include <memory>

class TopModule : public sc_core::sc_module
{
public:

	SC_CTOR(TopModule)
	{
		// Instantiate components   
		mp_Mem = std::make_unique<Memory>("Memory");
		mp_Cpu = std::make_unique<CPU>("CPU");
		mp_Router = std::make_unique<Router>("Router");

		// Bind sockets
		mp_Cpu->m_Socket.bind(mp_Router->m_SocketIn);
		mp_Router->m_SocketOutMEM.bind(mp_Mem->m_Socket);
	}
	
	virtual ~TopModule()
	{

	};

	std::unique_ptr<Memory> mp_Mem;
	std::unique_ptr<CPU> mp_Cpu;
	std::unique_ptr<Router> mp_Router;
};

#endif