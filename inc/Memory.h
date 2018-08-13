#ifndef MEMORY_H_
#define MEMORY_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"
#include "UtilCommon.h"

#include <algorithm>
#include <queue>

class Memory : public sc_core::sc_module
{
public:

	SC_CTOR(Memory) : m_Socket("SocketMem"), m_Latency(70, SC_NS)
	{
		// Register callbacks for incoming interface method calls
    	m_Socket.register_nb_transport_fw(this, &Memory::nb_transport_fw);
		std::fill(m_MemData, m_MemData + SIZE, 0);
		SC_THREAD(RespondRequestThread);
	}

  	virtual ~Memory(){};

  	virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay );
  	void RespondRequestThread();
  	
	// Attributes
	enum{ SIZE = 256};
  	tlm_utils::simple_target_socket<Memory> m_Socket;
  	sc_core::sc_event m_RequestEvent;
  	sc_core::sc_time m_Latency;
	int m_MemData[SIZE];
	std::queue<TransRequest> m_RequestQueue;
};

#endif