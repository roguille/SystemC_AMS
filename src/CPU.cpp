#include "CPU.h"

#include "UtilCommon.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace tlm;
using namespace sc_core;

const size_t CPU::kTotalTransactions = 5;

CPU::CPU(sc_core::sc_module_name ModuleName) : sc_core::sc_module(ModuleName), m_Socket("SocketOutCPU"), m_TransVectorData(kTotalTransactions, 0)
{
	// Register callbacks for incoming interface method calls
   	m_Socket.register_nb_transport_bw(this, &CPU::nb_transport_bw);

   	// Register Threads
    SC_THREAD(GenerateRequests);

    // Print creation
    cout << name() << " init at time " << sc_time_stamp() << endl;
}

CPU::~CPU()
{

}

void CPU::GenerateRequests()
{
	const unsigned int c_iTotalTransactions = static_cast<unsigned int>(kTotalTransactions);
	const sc_time delay_between_trans_creation = sc_time(10, SC_NS);
	tlm_generic_payload trans;
    tlm_phase phase = BEGIN_REQ;
	sc_time trans_delay = sc_time(10, SC_NS);
	ID_extension* id_extension = new ID_extension();
	trans.set_extension( id_extension );

	srand(time(NULL));

    for (unsigned int iTransactionIdx = 0; iTransactionIdx < c_iTotalTransactions; iTransactionIdx++)   
    {
    	//Delay for BEGIN_REQ
		wait(delay_between_trans_creation);
    	id_extension->m_TransactionId = iTransactionIdx;
		tlm_command cmd = static_cast<tlm_command>(rand() % 2);
		sc_dt::uint64 addr = static_cast<sc_dt::uint64>(rand() % 256);
		unsigned char* data_ptr = reinterpret_cast<unsigned char*>(&m_TransVectorData[iTransactionIdx]);
		if (cmd == TLM_WRITE_COMMAND) 
			m_TransVectorData[iTransactionIdx] = iTransactionIdx; 
		
		trans.set_command( cmd );
		trans.set_address( addr );
		trans.set_data_ptr( data_ptr );   
		trans.set_data_length( sizeof(int) );
		
		cout << ">>>  " << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
		m_Socket->nb_transport_fw( trans, phase, trans_delay );
    }
}

tlm::tlm_sync_enum CPU::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
	ID_extension* id_extension = nullptr;
	trans.get_extension( id_extension );
    tlm_command cmd = trans.get_command();   
    sc_dt::uint64 adr = trans.get_address();   
    
    if (phase == BEGIN_RESP) 
    {                
		// Initiator obliged to check response status   
		if (trans.is_response_error() )   
			SC_REPORT_ERROR("TLM2", "Response error from nb_transport");
		
		// Delay
		wait(delay);
		cout << "<<<  " << name() << " BEGIN_RESP RECEIVED" << " TRANS ID " << id_extension->m_TransactionId << ", trans/bw = { " << (cmd ? 'W' : 'R') << ", " << dec << adr << " } , data = " << dec << m_TransVectorData[id_extension->m_TransactionId] << " at time " << sc_time_stamp() << endl;
		return TLM_ACCEPTED; 
    }
    return TLM_COMPLETED;
}
