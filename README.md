Interface module between PMDG 747-400X and Goflight MCPPro + EFIS
-------------------------------------------------------------------------------------------------------

This software created as result of SimConnect(r) logfile analysis,
the author didn't reverse engineer, decompile, disassemble files of any PMDG(r) software.
The author provides this software as an example but not as any kind of end-user product,
if you wish to use this software by any way,
you shall solely assume all legal responsibilities in full.

To build this software you will need Microsoft FSX Acceleration SDK and GoFlight SDK v2.21
The Compiler should implement C++11 standart (Build tested with MSVS2015 Express and MSVS2013 Express )

This software implements 2-way interfacing between pmdg-747-400X and GoFlight MCPPro+EFIS

This software can be used as standalone executable, or as FSX module,
module version provides more reliable bank angle switch handling, and hides yoke on start

Pushed rotate of MCPPro Heading encoder works as bank angle switch
Push without rotate of MCPPro Heading encoder works as HDG Sel
Pushed rotate of EFIS DH/MDA encoder works as DH/MDA switch
Pushed rotate of EFIS BARO encoder switches between inHg and HPA
Long push of EFIS FPV works as CTR 
Long push of EFIS MTRS works as TFC
MCPPro CMD A works as CMD L
MCPPro CMD B works as CMD R
MCPPro CWS A works as CMD C

All other pushes works like their 747-400 counterparts