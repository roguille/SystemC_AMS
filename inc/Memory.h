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

	SC_HAS_PROCESS(Memory);

	// Constructor and Destructor
	Memory(sc_core::sc_module_name ModuleName);
  	virtual ~Memory();

  	// Callbacks
  	virtual tlm::tlm_sync_enum nb_transport_fw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay );
  	
  	// Threads
  	void ProcessRequests();
  	
	// Attributes
	enum{ SIZE = 256};
  	tlm_utils::simple_target_socket<Memory> m_Socket;
	
private:
	// Attributes
	int m_MemData[SIZE];
	sc_core::sc_event m_NewRequestEvent;
	std::queue<TransRequest> m_RequestQueue;
};

#endif