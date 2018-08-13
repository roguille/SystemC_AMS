#ifndef TOPMODULE_H_
#define TOPMODULE_H_

#include "systemc.h"
#include "Memory.h"
#include "CPU.h"

#include <memory>

class TopModule : public sc_core::sc_module
{
public:

	SC_CTOR(TopModule)
	{
		// Instantiate components   
		mp_Mem = std::make_unique<Memory>("Memory");
		mp_Cpu = std::make_unique<CPU>("CPU");

		// Bind sockets
		mp_Cpu->m_Socket.bind(mp_Mem->m_Socket);
	}
	
	virtual ~TopModule()
	{

	};

	std::unique_ptr<Memory> mp_Mem;
	std::unique_ptr<CPU> mp_Cpu;
};

#endif