Copyright (c) 2015, Alexander Voronkov
All rights reserved.

Interface module between PMDG 747-400X and Goflight MCPPro + EFIS
-------------------------------------------------------------------------------------------------------

This software created as result of SimConnect(r) logfile analysis,
the author didn't reverse engineer, decompile, disassemble files of any PMDG(r) software.
The author provides this software as an example but not as any kind of end-user product,
if you wish to use this software by any way,
you shall solely assume all legal responsibilities in full.

------------------------------ LICENSE   --------------------------------------------------------------

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the above disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without specific prior written permission.


-------------------------------------------------------------------------------------------------------
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

------------------------  INSTALLATION  OF SUPPLIED BINARY FILES  ------------------------------------

Files compiled with SDK FSX+Acceleration  SimConnect.dll version 10.0.61259.0

For FSX-Steam edition you will need to install that version manually from 
"<FSX install folder (maybe 'C:\Program Files (x86)\Steam\SteamApps\common\FSX')>\SDK\Core Utilities Kit\SimConnect SDK\LegacyInterfaces\FSX-XPACK\SimConnect.msi" 


DLL option:

1) put gf747.dll into modules subfolder of FSX installation folder (maybe you will need to create it)  
2) add this to  "%APPDATA%/Microsoft/FSX/DLL.xml" file between the last  "</Launch.Addon>" and "</SimBase.Document>":

  <Launch.Addon>
    <Name>GFPMDG747</Name>
    <Disabled>False</Disabled>
    <Path>modules\gf747.dll</Path>
  </Launch.Addon>

EXE option:

1) Download and install VC++2015 x86 runtime 
2) put gf747.exe into GoFlight software installation folder (where GFDEV.DLL is placed) (maybe "C:\Program Files (x86)\GoFlight\")
3) if you want autostart software with FSX,
   add this to "%APPDATA%/Microsoft/FSX/EXE.xml" file between the last  "</Launch.Addon>" and "</SimBase.Document>":

<Launch.Addon>
    <Name>GFPMDG747EXE</Name>
    <Disabled>False</Disabled>
    <Path>C:\Program Files (x86)\GoFlight\gf747.exe</Path>
    <CommandLine></CommandLine>
</Launch.Addon>
 
Don't forget to change gf747.exe path

