#ifndef CPU_H_
#define CPU_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include <vector>

class CPU : public sc_core::sc_module
{
public:
	SC_HAS_PROCESS(CPU);

	// Constructor and Destructor
	CPU(sc_core::sc_module_name ModuleName);
	virtual ~CPU();
	
	// Callbacks
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay);

	// Threads
	void GenerateRequests();
	
	// Attributes
	tlm_utils::simple_initiator_socket<CPU> m_Socket;

private:
	// Attributes
	std::vector<int> m_TransVectorData;

	// Constants
	static const size_t kTotalTransactions;
};

#endif