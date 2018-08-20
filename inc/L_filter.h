#ifndef L_FILTER_H_
#define L_FILTER_H_

#include "systemc.h"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "UtilCommon.h"
#include <queue>

SC_MODULE(lp_eln) {
sca_tdf::sca_in<double> in; 
sca_tdf::sca_out<double> out; 
sca_eln::sca_node  n1,n2; // electrical nodes 
sca_eln::sca_node_ref  gnd;
sca_c c; sca_r r; // capacitor and resistor 
sca_eln::sca_tdf::sca_vsource   vin; // TDF to voltage converter 
sca_eln::sca_tdf::sca_vsink  vout; // voltage to TDF converter
lp_eln(sc_module_name n,double freq_cut):c("c"),r("r"),vin("vin"),("vout") 
	double  R = 1000.; // choose fixed R 
	double  C = 1/(2*M_PI*R*freq_cut); // and compute C relative to it
     vin.p(n1); vin.n(gnd); vin.ctrl(in);
     vout.p(n2); vout.tdf_voltage(out);
     c.value = C; 
     c.p(n2); c.n(gnd);
     r.value = R; 
     r.n(n1); r.p(n2); 
   } 
};
