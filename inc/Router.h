#ifndef ROUTER_H_
#define ROUTER_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "UtilCommon.h"
#include <queue>

class Router : public sc_core::sc_module
{
public:

	SC_HAS_PROCESS(Router);
	
	// Constructor and Destructor
	Router(sc_core::sc_module_name ModuleName);
	virtual ~Router(){};

	// Callbacks
	virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay); 
	virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay);

	// Threads
	void ProcessRequests();
	void ProcessResponses();
	
	// Attributes
	tlm_utils::simple_target_socket<Router> m_SocketIn;
	tlm_utils::simple_initiator_socket<Router> m_SocketOutMEM;

private:
	// Attributes
	sc_core::sc_event m_NewRequestEvent;
	sc_core::sc_event m_NewResponseEvent;
	std::queue<TransRequest> m_RequestQueue;
	std::queue<TransRequest> m_ResponseQueue;
	
	enum class Device
	{
		MEM,
		MMIO,
		NONE
	};

	Device CalculateTransactionDestination(const sc_dt::uint64 Address);
};

#endif