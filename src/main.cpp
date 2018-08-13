#include "systemc.h"
#include "systemc-ams.h"

#include "UtilCommon.h"
#include "Memory.h"
#include "CPU.h"
#include "TopModule.h"

using namespace sc_core;

int sc_main(int argc, char* argv[])
{
    TopModule top("TopModule");
    sc_start();
	return 0;
}