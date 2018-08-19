#include "Router.h"

using namespace std;
using namespace tlm;
using namespace sc_core;
using namespace sc_dt;

Router::Router(sc_core::sc_module_name ModuleName) : sc_core::sc_module(ModuleName), m_SocketIn("SocketInRouter"), m_SocketOutMEM("SocketOutToMemFromRouter")
{
	// Register callbacks for incoming interface method calls
	m_SocketIn.register_nb_transport_fw(this, &Router::nb_transport_fw);
	m_SocketOutMEM.register_nb_transport_bw(this, &Router::nb_transport_bw);

	// Register Threads
	SC_THREAD(ProcessRequests);
	SC_THREAD(ProcessResponses);

	// Print creation
    cout << name() << " init at time " << sc_time_stamp() << endl;
}

tlm::tlm_sync_enum Router::nb_transport_bw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
{
	ID_extension* id_extension = nullptr;
	trans.get_extension( id_extension );
    if (phase == BEGIN_RESP && nullptr != id_extension) 
    {           
		// Initiator obliged to check response status   
		if (trans.is_response_error() )   
			SC_REPORT_ERROR("TLM2", "Response error from nb_transport");

		TransRequest NewResponse;
	  	NewResponse.m_Id 			= id_extension->m_TransactionId;
	  	NewResponse.m_Cmd 			= trans.get_command();
	  	NewResponse.m_DataPtr 		= trans.get_data_ptr();
	  	NewResponse.m_DataLenght 	= trans.get_data_length();
	  	NewResponse.m_Addr 			= trans.get_address();
		m_ResponseQueue.push(NewResponse);
		
		// Delay
		wait(delay);
		m_NewResponseEvent.notify();
		cout << name () << " BEGIN_RESP RECEIVED" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
		return TLM_ACCEPTED; 
    }
    return TLM_COMPLETED;
}

tlm::tlm_sync_enum Router::nb_transport_fw(tlm::tlm_generic_payload& trans, tlm::tlm_phase& phase, sc_core::sc_time& delay)
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
		m_NewRequestEvent.notify();
		cout << name() << " BEGIN_REQ RECEIVED" << " TRANS ID " << dec << NewRequest.m_Id << " at time " << sc_time_stamp() << endl;
		return TLM_ACCEPTED;
	}
	return TLM_COMPLETED; 
}

void Router::ProcessRequests()
{
	const sc_time process_request_delay = sc_time(20, SC_NS);
	Device target_device = Device::NONE;
	tlm_generic_payload trans;
	tlm_phase phase = BEGIN_REQ;
	sc_time trans_delay = sc_time(10, SC_NS);
    ID_extension* id_extension = new ID_extension();
    trans.set_extension( id_extension );
	
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
    		
    		sc_dt::uint64 addr = trans.get_address();

    		target_device = CalculateTransactionDestination(addr);

    		wait(process_request_delay);
    		if(Device::MEM == target_device)
    		{
    			cout << name() << " BEGIN_REQ SENT" << " TRANS ID " << id_extension->m_TransactionId << " at time " << sc_time_stamp() << endl;
				m_SocketOutMEM->nb_transport_fw( trans, phase, trans_delay );
    		}
    		else if(Device::MMIO == target_device)
    		{
    			// do something
    		}
    		else
    		{
    			// do something
    		}
      	}
      	else
      	{
      		wait(m_NewRequestEvent);
      	}
	}
}

void Router::ProcessResponses()
{
	const sc_time process_response_delay = sc_time(20, SC_NS);
	tlm_generic_payload trans;
	tlm_phase phase = BEGIN_RESP;
	sc_time trans_delay = sc_time(10, SC_NS);
  	ID_extension* id_extension = new ID_extension();
  	trans.set_extension( id_extension );
	
	while(true)
	{
		// Wait for an event to pop out of the back end of the queue   
      	if(!m_ResponseQueue.empty())
      	{
      		TransRequest request = m_ResponseQueue.front();     
    		m_ResponseQueue.pop();
    		
    		id_extension->m_TransactionId = request.m_Id;
    		trans.set_command(request.m_Cmd);
    		trans.set_data_ptr(request.m_DataPtr);
    		trans.set_data_length(request.m_DataLenght);
    		trans.set_address(request.m_Addr);
    		
    		// Obliged to set response status to indicate successful completion   
    		trans.set_response_status( tlm::TLM_OK_RESPONSE );  
    		wait(process_response_delay);
    		
    		std::cout << name() << " BEGIN_RESP SENT" << " TRANS ID " << std::dec << id_extension->m_TransactionId <<  " at time " << sc_time_stamp() << std::endl;
    		
    		// Call on backward path to complete the transaction  
    		m_SocketIn->nb_transport_bw( trans, phase, trans_delay);   
      	}
      	else
      	{
      		wait(m_NewResponseEvent);
      	}
	}
}

Router::Device Router::CalculateTransactionDestination(const sc_dt::uint64 Address)
{
	const sc_dt::uint64 c_MaxAddressSupportedByMem = 255;
	Device target_device = Device::NONE;
	if(c_MaxAddressSupportedByMem >= Address)
	{
		target_device = Device::MEM;
	}
	else
	{
		target_device = Device::MMIO;
	}
	return target_device;
}
