#include "filter.h"
int  sc_main(int argc , char *argv []){

// seNales
sca_lsf :: sca_signal  sig;
sca_lsf :: sca_signal  dummy;

// Seno predefinido y filtro
filter  flt("filter");
sca_lsf :: sca_source  source("source" ,0,2.5,2.5,1.0e3);

// generacion puntos a tiempos dados
source.set_timestep (10.0,  sc_core ::SC_NS);

// conectar
source.y(sig);
flt.in(sig);
flt.out(dummy);

//Graficar resultados a archillo
sca_trace_file* file=sca_create_vcd_trace_file("Filtro.vcd");
sca_trace(file ,flt.in,"in");
sca_trace(file ,flt.out ,"out");
sc_start(5, SC_MS);
sca_close_vcd_trace_file(file);
return 0;
}
