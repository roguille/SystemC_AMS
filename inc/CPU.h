#ifndef CPU_H_
#define CPU_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"

#include <queue>

class CPU : public sc_core::sc_module
{
public:
	SC_HAS_PROCESS(CPU);
	CPU(sc_core::sc_module_name ModuleName);
	virtual ~CPU(){};
	
	void RequestGenerationThread();
	virtual tlm::tlm_sync_enum nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_time& delay); 
	tlm_utils::simple_initiator_socket<CPU> m_Socket;
	int m_Data;
};

#endif