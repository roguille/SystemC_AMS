#include "Memory.h"
#include <iostream>

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace sc_dt;

tlm_sync_enum Memory::nb_transport_fw( tlm_generic_payload& trans, tlm_phase& phase, sc_time& delay )
{
	ID_extension* id_extension = nullptr;
	unsigned char* byt = trans.get_byte_enable_ptr();
	trans.get_extension( id_extension ); 
	if(phase == BEGIN_REQ && id_extension != nullptr)
	{
	  	// Obliged to check the transaction attributes for unsupported features
	  	// and to generate the appropriate error response
	  	if (byt != 0) 
	  	{
	    	trans.set_response_status( TLM_BYTE_ENABLE_ERROR_RESPONSE );
	    	return TLM_COMPLETED;
	  	}
	  	
	  	TransRequest NewRequest;
	  	NewRequest.m_Id 		= id_extension->m_TransactionId;
	  	NewRequest.m_Cmd 		= trans.get_command();
	  	NewRequest.m_DataPtr 	= trans.get_data_ptr();
	  	NewRequest.m_DataLenght = trans.get_data_length();
	  	NewRequest.m_Addr 		= trans.get_address();
		m_RequestQueue.push(NewRequest);
		
		//Delay
		wait(delay);
		m_RequestEvent.notify();
		cout << name() << " BEGIN_REQ RECEIVED" << " TRANS ID " << dec << NewRequest.m_Id << " at time " << sc_time_stamp() << endl;
		return TLM_ACCEPTED;
	}
	return TLM_COMPLETED;
}

void Memory::RespondRequestThread()
{
	tlm_generic_payload trans;
	tlm_phase phase = BEGIN_RESP;
	sc_time delay = sc_time(10, SC_NS);
    ID_extension* id_extension = new ID_extension();
    trans.set_extension( id_extension );

    cout << name() << " init at time " << sc_time_stamp() << endl;
	
	while(true)
	{
		// Wait for an event to pop out of the back end of the queue   
      	if(!m_RequestQueue.empty())
      	{
        	TransRequest request = m_RequestQueue.front();     
        	m_RequestQueue.pop();

        	id_extension->m_TransactionId = request.m_Id;
        	trans.set_command(request.m_Cmd);
        	trans.set_data_ptr(request.m_DataPtr);
        	trans.set_data_length(request.m_DataLenght);
        	trans.set_address(request.m_Addr);

        	tlm::tlm_command cmd = trans.get_command();   
      		sc_dt::uint64    adr = trans.get_address() / 4;   
      		unsigned char*   ptr = trans.get_data_ptr();   
      		unsigned int     len = trans.get_data_length();

      		if (adr >= sc_dt::uint64(SIZE) || len > sizeof(int))   
        		SC_REPORT_ERROR("TLM2", "Target does not support given generic payload transaction");   
        	
        	// Obliged to implement read and write commands   
        	if ( cmd == tlm::TLM_READ_COMMAND )   
          		memcpy(ptr, &m_MemData[adr], len);
        	else if ( cmd == tlm::TLM_WRITE_COMMAND )   
          		memcpy(&m_MemData[adr], ptr, len); 

        	// Obliged to set response status to indicate successful completion   
        	trans.set_response_status( tlm::TLM_OK_RESPONSE );  
        	wait(m_Latency);
        	
        	std::cout << name() << " BEGIN_RESP SENT" << " TRANS ID " << std::dec << id_extension->m_TransactionId <<  " at time " << sc_time_stamp() << std::endl;
        	
        	// Call on backward path to complete the transaction  
        	m_Socket->nb_transport_bw( trans, phase, delay );   
      	}
      	else
      	{
        	wait(m_RequestEvent); 
      	}
	}
}
