#ifndef FILTER_H_
#define FILTER_H_

#include  <systemc.h>
#include  <systemc-ams.h>


SC_MODULE(filter) {
public:
sca_lsf :: sca_in  in;  //input  port 
sca_lsf :: sca_out  out; // output  port
sca_lsf :: sca_signal  sig; // SeNal interna
sca_lsf :: sca_dot  dot1;   
sca_lsf :: sca_sub  sub1;   

SC_CTOR(filter) : dot1("dot1", 1.0/(2.0* M_PI *1.0e3)), sub1("sub1") {
dot1.x(out);
dot1.y(sig);

sub1.x1(in);
sub1.x2(sig);
sub1.y(out);
}
};
#endif
