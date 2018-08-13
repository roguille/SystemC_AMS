#include "CPU.h"
#include "UtilCommon.h"

#include <iostream>

CPU::CPU(sc_core::sc_module_name ModuleName) : sc_core::sc_module(ModuleName)
{
	// Register callbacks for incoming interface method calls
   	m_Socket.register_nb_transport_bw(this, &CPU::nb_transport_bw);
    SC_THREAD(RequestGenerationThread); 
}

void CPU::RequestGenerationThread()
{
	using namespace std;
	using namespace tlm;

	const unsigned int c_iTotalTransactions = 100;
	tlm_generic_payload trans;
    ID_extension* id_extension = new ID_extension();
    tlm_phase phase = BEGIN_REQ;
	sc_time delay = sc_time(10, SC_NS);
	trans.set_extension( id_extension );

	cout << name() << " init at time " << sc_time_stamp() << endl;

    for (unsigned int iTransactionIdx = 0; iTransactionIdx < c_iTotalTransactions; iTransactionIdx++)   
    {
    	//Delay for BEGIN_REQ
		wait(delay);
    	id_extension->m_TransactionId = iTransactionIdx;
		tlm_command cmd = static_cast<tlm_command>(rand() % 2);
		if (cmd == TLM_WRITE_COMMAND) 
			m_Data = iTransactionIdx; 
		
		trans.set_command( cmd );
		trans.set_address( rand() % 0xFF );
		trans.set_data_ptr( reinterpret_cast<unsigned char*>(&m_Data) );   
		trans.set_data_length( sizeof(int) );
		
		cout << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
		m_Socket->nb_transport_fw( trans, phase, delay );
		//wait(delay);
    }
}

tlm::tlm_sync_enum CPU::nb_transport_bw( tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
	using namespace tlm;
	ID_extension* id_extension = nullptr;
	trans.get_extension( id_extension );
    tlm_command cmd = trans.get_command();   
    sc_dt::uint64 adr = trans.get_address();   
    
    if (phase == BEGIN_RESP) 
    {                
		// Initiator obliged to check response status   
		if (trans.is_response_error() )   
			SC_REPORT_ERROR("TLM2", "Response error from nb_transport");   
		
		cout << name() << " TRANS ID " << id_extension->m_TransactionId << " trans/bw = { " << (cmd ? 'W' : 'R') << ", " << dec << adr << " } , data = " << dec << m_Data << " at time " << sc_time_stamp() << ", delay = " << delay << endl;
		
		// Delay
		wait(delay);
		cout << name () << " BEGIN_RESP RECEIVED" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
    }
    return TLM_ACCEPTED;   
}
