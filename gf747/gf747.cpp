//
// Copyright (c) 2015, Alexander Voronkov
// All rights reserved.
//
// This software created as result of SimConnect(r) logfile analysis,
// the author didn't reverse engineer, decompile, disassemble files of any PMDG(r) software.
// The author provides this software as an example but not as any kind of end-user product,
// if you wish to use this software by any way,
// you shall solely assume all legal responsibilities in full.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the above disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without specific prior written permission.
//

// To build this software you will need Microsoft FSX Acceleration SDK and GoFlight SDK v2.21 
// The Compiler should implement C++11 standart (Build tested with MSVS2015 Express and MSVS2013 Express )


#include "stdafx.h"



#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <iomanip>
#include <map>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <algorithm>
#include <iterator>
#include <string>
#ifndef _WINDOWS
#define _WINDOWS
#endif
#include <GFDevApi.h>
#include <SimConnect.h>
#ifdef _USRDLL
#include <gauges.h>
#endif //_USRDLL
namespace logger
{
#ifdef _USRDLL
    class null_stream {};
    template <typename T>
    null_stream & operator<<(null_stream & s, T const &) {return s;}
	//manip
	null_stream & operator<<(null_stream & s, std::ostream &(std::ostream&)) { return s; }
    //std::ofstream lout("c:\\temp\\gf747.log");
    null_stream lout;
#else
    std::ostream& lout = std::cout;
#endif

}


enum data_definition_id
{
    DEFINITION_INSTRUMENT_DATA,
    DEFINITION_AP_DATA
};

enum client_data_id
{
   CDID_AIRCRAFTINSTRUMENTSDATA,
   CDID_AIRCRAFTAUTOPILOTDATA
};

enum DATA_REQUEST_ID {
    REQUEST_FIRST = -2,
    REQUEST_APDATA = 1,
    REQUEST_DATA,
    REQUEST_VISIBLE,
    REQUEST_LED
};

enum event_groups
{
    GROUP_1
};



enum sim_events
{
    EVENT_SIM_START,
    EVENT_SIM_STOP,
    EVENT_SIM_PAUSED,
    EVENT_SIM_UNPAUSED,
    EVENT_AIRCRAFT_LOADED,
    EVENT_FLIGHT_LOADED,
    EVENT_6HZ,
    EVENT_HAS_INPUT,
    EVENT_MENU_ROOT,
    EVENT_MENU_RESTART,


    IFACE_EVENTS_START = 100,
    EFISCaptainPressMINS,
    EFISCaptainIncreaseMINS,
    EFISCaptainDecreaseMINS,
    EFISCaptainResetMINS,
    EFISCaptainPressBARO,
    EFISCaptainIncreaseBARO,
    EFISCaptainDecreaseBARO,
    EFISCaptainPressBAROSTD,
    EFISCaptainPressFPV,
    EFISCaptainPressMTRS,
    EFISCaptainIncreaseNAVL,
    EFISCaptainDecreaseNAVL,
    EFISCaptainIncreaseNAVR,
    EFISCaptainDecreaseNAVR,
    EFISCaptainIncreaseNDMODE,
    EFISCaptainDecreaseNDMODE,
    EFISCaptainPressNDMODECTR,
    EFISCaptainIncreaseNDRANGE,
    EFISCaptainDecreaseNDRANGE,
    EFISCaptainPressNDRANGETFC,
    EFISCaptainPressWXR,
    EFISCaptainPressSTA,
    EFISCaptainPressWPT,
    EFISCaptainPressARPT,
    EFISCaptainPressDATA,
    EFISCaptainPressPOS,
    EFISCaptainPressTERR,
    EFISCaptainSetMINS,
    EFISCaptainSetBARO,
    EFISCaptainSetNAVL,
    EFISCaptainSetNAVR,
    EFISCaptainSetNDMODE,
    EFISCaptainSetNDRANGE,
    MCPPressATArm,
    MCPPressN1,
    MCPPressTHR,
    MCPPressSPD,
    MCPPressCO,
    MCPPressFLCH,
    MCPPressVNAV,
    MCPPressLNAV,
    MCPPressVORLOC,
    MCPPressAPP,
    MCPPressHDGSEL,
    MCPPressHDGHOLD,
    MCPPressALTHOLD,
    MCPPressVS,
    MCPPressCMDL,
    MCPPressCMDC,
    MCPPressCMDR,
    MCPPressFDL,
    MCPPressFDR,
    MCPPressAPDiseng,
    MCPIncreaseBankLimiter,
    MCPDecreaseBankLimiter,
    MCPIncreaseAlt,
    MCPDecreaseAlt,
    MCPIncreaseSpd,
    MCPDecreaseSpd,
    MCPIncreaseHdg,
    MCPDecreaseHdg,
    MCPIncreaseVS,
    MCPDecreaseVS,
    MCPSetTOGA,
    MCPResetTOGA,
    MCPPressSpdIntv,
    MCPPressAltIntv,
    MCPPressCMD,
    MCPPressCWS,
    MCPPressFD,
    MCPPressTOGA,
    MCPPressBankLimiter,
    MCPSetAltitude,
    MCPSetSpeed,
    MCPSetHeading,
    MCPSetVS,
    MCPSetFDL,
    MCPSetFDR
};

enum sim_data
{
    EFISBaro,
    EFISMins,
    EFISNavL,
    EFISNavR,
    EFISNDMode,
    EFISNDCenter,
    EFISNDRange,
    MCPHdg,
    MCPIas,
    MCPMach,
    MCPAlt,
    MCPVS,
    MCPPanelState,
    MACH_IAS,
    AP_BANK_LIM,
    AP_VS
};





struct controls_state
{
    controls_state()
    {
        clear();
    }
    void clear()
    {
       memset(this,0,sizeof(controls_state));
    }
    unsigned int fd_l:1;
    unsigned int fd_r:1;
    unsigned int at_arm:1;
    unsigned int ap_disengage:1;
    unsigned int nav_l:2;
    unsigned int nav_r:2;
    unsigned int nd_mode:2;
    unsigned int nd_range:3;
//-----
    unsigned int min_sw:1;
    unsigned int baro_sw:1;
};

struct controls_cmd
{
    controls_cmd()
    {
        clear();
    }
    void clear()
    {
        memset(this,0,sizeof(controls_cmd));
    }

    int spd_knob;
    int hdg_knob;
    int alt_knob;
    int vs_knob;
    int mins_knob;
    int baro_knob;
//--
    unsigned int mins_rst:1;
    unsigned int baro_std:1;
    unsigned int fpv:1;
    unsigned int mtrs:1;
    unsigned int ctr:1;
    unsigned int tfc:1;
//--7
    unsigned int wxr:1;
    unsigned int sta:1;
    unsigned int wpt:1;
    unsigned int arpt:1;
    unsigned int data:1;
    unsigned int pos:1;
    unsigned int terr:1;
//--15
    unsigned int thr:1;
    unsigned int spd:1;
    unsigned int c_o:1;
    unsigned int flch:1;
    unsigned int vnav:1;
    unsigned int lnav:1;
    unsigned int vorloc:1;
    unsigned int app:1;
    unsigned int hdgsel:1;
    unsigned int hdghld:1;
    unsigned int althld:1;
    unsigned int vs:1;
    unsigned int cmd_l:1;
    unsigned int cmd_c:1;
    unsigned int cmd_r:1;
    unsigned int alt_intv:1;
    unsigned int spd_intv:1;
    unsigned int bank_inc:1;
    unsigned int bank_dec:1;
    unsigned int bank_change_start:1;
    unsigned int bank_change_end:1;
    unsigned int min_sw_inc:1;
    unsigned int min_sw_dec:1;
    unsigned int baro_sw_inc:1;
    unsigned int baro_sw_dec:1;

//--34
};

struct controls_block
{
    controls_state state;
    controls_cmd cmd;
    static const long long hold_key_time_ms = 500;

};

class controls_info
{
public:
    enum loaded_states
    {
        PMDG747_UNLOADED = -1,
        PMDG747_NOTLOADED = 0,
        PMDG747_LOADED,
        PMDG747_JUSTLOADED
    };
private:
    std::mutex mtx;
    std::mutex hardware_mtx;
    int current_write;
    int current_read;
    static const int controls_size = 3;
    controls_block controls[controls_size];

    static int get_next(int current)
    {
        return (current + 1) % controls_size;
    }

    bool hardware_changed;
    int gfmcppro_num;
    int gfefis_num;
    HINSTANCE hInstance;
    HANDLE hSimConnect;
    HANDLE hSimConnect2;
    HANDLE has_new;
    bool is_init;
    bool panel_state_initialized;
    loaded_states pmdg_747_loaded;
    bool panel_state_update;
    bool unpaused_time_ready;
    std::chrono::high_resolution_clock::time_point unpaused_time;

public:

    controls_info()
        : current_write(0), current_read(2)
        , hardware_changed(true), gfmcppro_num(-1)
        , gfefis_num(-1), is_init(false)
        , hSimConnect(INVALID_HANDLE_VALUE)
        , hSimConnect2(INVALID_HANDLE_VALUE)
        , panel_state_initialized(false)
        , pmdg_747_loaded(PMDG747_NOTLOADED)
        , panel_state_update(false)
        , unpaused_time_ready(false)
    {
        has_new = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    }
    ~controls_info()
    {
        ::CloseHandle(has_new);
        deinit();
    }

    void clear_display()
    {
        std::unique_lock<std::mutex> lck(mtx);
        if (gfmcppro_num >= 0)
        {
            GFMCPPro_SetADisplayText(gfmcppro_num, "   ");
            GFMCPPro_SetBDisplayText(gfmcppro_num, "     ");
            GFMCPPro_SetCDisplayText(gfmcppro_num, "     ");
            GFMCPPro_SetDDisplayText(gfmcppro_num, "     ");
            GFMCPPro_SetEDisplayText(gfmcppro_num, "     ");
            GFMCPPro_SetFDisplayText(gfmcppro_num, "   ");
            GFMCPPro_SetIndicators(gfmcppro_num, 0);
        }
    }

    void init(HINSTANCE hInst, HANDLE hSimCon, HANDLE hSimCon2)
    {
        is_init = true;
        hInstance = hInst;
        hSimConnect = hSimCon;
        hSimConnect2 = hSimCon2;
        GFDev_Init(hInst);
    }
    void deinit()
    {
        if (is_init)
        {
            clear_display();
            GFDev_Terminate();
            SimConnect_Close(hSimConnect);
            SimConnect_Close(hSimConnect2);
        }
        is_init = false;
    }

    void on_panel_state()
    {
        panel_state_initialized = true;
    }

    bool need_panel_state_update(bool clear = false)
    {
        if (!panel_state_update) return false;
        if (unpaused_time_ready && std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now()
            - unpaused_time).count() > 10000)
        {
            if (clear)
            {
                unpaused_time_ready = false;
                panel_state_update = false;
            }
            return true;
        }
        return false;
    }

    void on_unpaused()
    {
        if (panel_state_update)
        {
            unpaused_time_ready = true;
            unpaused_time = std::chrono::high_resolution_clock::now();
        }
    }

    loaded_states get_pmdg747_loaded()
    {
        loaded_states ret = pmdg_747_loaded;
        if (pmdg_747_loaded == PMDG747_JUSTLOADED)
        {
            pmdg_747_loaded = PMDG747_LOADED;
        }
        else if (pmdg_747_loaded == PMDG747_UNLOADED)
        {
            pmdg_747_loaded = PMDG747_NOTLOADED;
        }
        return ret;
    }

    void set_pmdg747_loaded(bool state)
    {
        if (state)
        {
            if (pmdg_747_loaded == PMDG747_NOTLOADED)
            {
                pmdg_747_loaded = PMDG747_JUSTLOADED;
            }
            else if (pmdg_747_loaded == PMDG747_UNLOADED)
            {
                pmdg_747_loaded = PMDG747_LOADED;
            }
        }
        else
        {
            if (pmdg_747_loaded == PMDG747_JUSTLOADED)
            {
                pmdg_747_loaded = PMDG747_NOTLOADED;
            }
            else if (pmdg_747_loaded == PMDG747_LOADED)
            {
                pmdg_747_loaded = PMDG747_UNLOADED;
            }
        }
    }



    void onreload()
    {
        if (pmdg_747_loaded == PMDG747_JUSTLOADED || pmdg_747_loaded == PMDG747_LOADED)
        {
            panel_state_update = true;
        }
        else
        {
            panel_state_update = false;
        }
        panel_state_initialized = false;
        unpaused_time_ready = false;
    }

    void on_hardware_changed()
    {
        hardware_changed = true;
        GFDev_Terminate();
        GFDev_Init(hInstance);
    }
    bool get_devs_and_changed(int& mcpnum, int& efisnum)
    {
        std::unique_lock<std::mutex> lck(mtx);
        bool ret = hardware_changed;
        if (hardware_changed)
        {
            gfmcppro_num = GFMCPPro_GetNumDevices() - 1;
            gfefis_num = GFEFIS_GetNumDevices() - 1;
            hardware_changed = false;
            logger::lout << "devs:" << gfmcppro_num << " " << gfefis_num << std::endl;
        }
        mcpnum = gfmcppro_num;
        efisnum = gfefis_num;
        return ret;
    }
    void get_devs(int& mcpnum, int& efisnum)
    {
        std::unique_lock<std::mutex> lck(mtx);
        mcpnum = gfmcppro_num;
        efisnum = gfefis_num;
    }

    bool get_new_input(unsigned int timeout_ms = 0)
    {

        if (!panel_state_initialized)
        {
            return false;
        }
        bool update = need_panel_state_update(true);

        if (::WaitForSingleObject(has_new, timeout_ms) == WAIT_TIMEOUT)
        {
            return update;
        }
        current_read = get_next(current_read);
        ::ResetEvent(has_new);
        return true;
    }
    controls_block& get_current_for_write()
    {
       return controls[current_write];
    }
    controls_block& get_current_for_read()
    {
        return controls[current_read];
    }
    void input_processed()
    {
        if (::WaitForSingleObject(has_new, 0) == WAIT_TIMEOUT)
        {
            int next = get_next(current_write);
            controls[next].state = controls[current_write].state;
            controls[next].cmd.clear();
            current_write = next;
            ::SetEvent(has_new);
        }
        SimConnect_TransmitClientEvent(hSimConnect2, SIMCONNECT_OBJECT_ID_USER,
            EVENT_HAS_INPUT, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
    }

};

struct efis_input_context: public GFEFISINPUTREPORT
{
    efis_input_context()
    {
        clear();
    }

    void clear()
    {
        nADialVal = 0;
        nBDialVal = 0;
        wSwitchState = 0;
    }

    std::chrono::high_resolution_clock::time_point fpv_pressed;
    std::chrono::high_resolution_clock::time_point mtrs_pressed;
    std::chrono::high_resolution_clock::time_point A_pressed;
    std::chrono::high_resolution_clock::time_point B_pressed;
};

struct mcppro_input_context:public GFMCPPROINPUTREPORT
{
    mcppro_input_context()
    {
        clear();
    }

    void clear()
    {
        nADialVal = 0;
        nBDialVal = 0;
        nCDialVal = 0;
        nDDialVal = 0;
        nEDialVal = 0;
        nFDialVal = 0;
        ulSwitchState = 0;
    }
    std::chrono::high_resolution_clock::time_point A_pressed;
    std::chrono::high_resolution_clock::time_point B_pressed;
    std::chrono::high_resolution_clock::time_point C_pressed;
    std::chrono::high_resolution_clock::time_point D_pressed;
    std::chrono::high_resolution_clock::time_point E_pressed;
    std::chrono::high_resolution_clock::time_point F_pressed;
};


namespace
{
const int mcp_fields_uninit_magic = 500000;
}

class mcppro_display_747
{
    int last_hdg;
    int last_ias;
    int last_alt;
    int last_vs;
    int last_panel_state;
    int last_mach_ias;
    int last_ap_vs;
    int last_bank_lim;
    double last_mach;
    bool hdg_changed;
    bool alt_changed;
    bool vs_changed;
    bool mach_ias_changed;
    bool ias_changed;
    bool mach_changed;
    bool panel_state_changed;
    unsigned long ulIndicatorState;
    bool spd_display_on;
    bool vs_display_on;
    bool show_bank_lim;




    static unsigned set_indicator_by_panel_state(unsigned panel_state)
    {
// panel state bitmask(0=ap L,1=ap R,2=ap C,3=ap dis,4=f/d,5=fo f/d,6=a/t,7=thr,8=spd,9=flch,10=vnav,11=lnav,12=loc,13=app,15=althld,16=v/s,19=hdghold,25=spddisplay,27=v/sdisplay )
//          0x00000001 apL  //0
//          0x00000002 apR
//          0x00000004 apC
//          0x00000008 apDis
//          0x00000010 fdL
//          0x00000020 fdR // 5
//          0x00000040 at
//          0x00000080 thr
//          0x00000100 spd
//          0x00000200 flch
//          0x00000400 vnav //10
//          0x00000800 lnav
//          0x00001000 loc
//          0x00002000 app
//          0x00004000
//          0x00008000 althld    //15
//          0x00010000 vs    //16
//          0x00020000
//          0x00040000
//          0x00080000 hdghold
//          0x00100000
//          0x00200000
//          0x00400000
//          0x00800000
//          0x01000000
//          0x02000000 spddisplay
//          0x04000000
//          0x08000000 vsdisplay
//
//  CMD A -> AP L
//  CMD B -> AP R
//  CWS A -> AP C
//
         return   ((panel_state & 0x00000003u) << 2) // apL apR
                | ((panel_state & 0x00000C00u) >> 10)   // vnav lnav
                | ((panel_state & 0x00000004u) << 7) // apC
                | ((panel_state & 0x00000040u) >> 2 ) // at
                | ((panel_state & 0x00000010u) << 10 ) // fdL
                | ((panel_state & 0x00000020u) << 18 ) // fdR
                | ((panel_state & 0x00000380u) << 8 ) // thr spd flch
                | ((panel_state & 0x00001000u) >> 5) // loc
                | ((panel_state & 0x00002000u) << 6) // app
                | ((panel_state & 0x00018000u) << 5) //  althld vs
                | ((panel_state & 0x00080000u) >> 1) // hdg
                ;

    }


public:
    mcppro_display_747()
        : last_hdg(mcp_fields_uninit_magic)
          ,last_ias(mcp_fields_uninit_magic)
          ,last_alt(mcp_fields_uninit_magic)
          ,last_vs(mcp_fields_uninit_magic)
          ,last_panel_state(0)
          ,last_mach_ias(0)
          ,last_mach(0.0)
          ,last_ap_vs(0)
          ,last_bank_lim(0)
          ,hdg_changed(false)
          ,alt_changed(false)
          ,vs_changed(false)
          ,mach_ias_changed(false)
          ,ias_changed(false)
          ,mach_changed(false)
          ,panel_state_changed(false)
          ,ulIndicatorState(0ul)
          ,spd_display_on(false)
          ,vs_display_on(false)
          ,show_bank_lim(false)

    {
    }



    void set_hdg(int arg)
    {
        if (arg != last_hdg)
        {
            last_hdg = arg;
            hdg_changed = true;
        }
    }

    void set_bank_lim(int arg)
    {
        if (arg != last_bank_lim)
        {
            if (last_bank_lim < 10 && arg == 25)
            {
                last_bank_lim = 0;
            }
            else
            {
                last_bank_lim = arg;
            }
            hdg_changed = true;
        }
    }

    void show_bank(bool arg)
    {
        if (arg != show_bank_lim)
        {
            show_bank_lim = arg;
            hdg_changed = true;
        }
    }


    void set_ias(int arg)
    {
        if (arg != last_ias)
        {
            last_ias = arg;
            ias_changed = true;
        }
    }

    void set_alt(int arg)
    {
        if (arg != last_alt)
        {
            last_alt = arg;
            alt_changed = true;
        }
    }

    void set_vs(int arg)
    {
        if (arg != last_vs)
        {
            last_vs = arg;
            vs_changed = vs_display_on;
        }
    }

    void set_ap_vs(int arg)
    {
        last_ap_vs = arg;
    }


    void set_panel_state(int arg)
    {
        if (arg != last_panel_state)
        {
            bool disp_on = (arg & 0x02000000u)? true: false; // ias/mach disp
            if (disp_on != spd_display_on)
            {
                spd_display_on = disp_on;
                mach_ias_changed = true;
            }
            disp_on = (arg & 0x08000000u)? true : false ; // v/s disp
            if (disp_on != vs_display_on)
            {
                vs_display_on = disp_on;
                vs_changed = true;
            }

            last_panel_state = arg;
            unsigned ind = set_indicator_by_panel_state(last_panel_state);
            if ( ind != ulIndicatorState)
            {
                ulIndicatorState = ind;
                panel_state_changed = true;
            }

        }
    }

    void set_mach_ias(int arg)
    {
        if (arg != last_mach_ias)
        {
            last_mach_ias = arg;
            mach_ias_changed = true;
        }
    }

    void set_mach(double arg)
    {
        if (std::abs(arg - last_mach) > 0.00005 )
        {
            last_mach = arg;
            mach_changed = true;
        }
    }
    void invalidate()
    {
        mach_ias_changed = true;
        hdg_changed = true;
        vs_changed = true;
        alt_changed = true;
        panel_state_changed = true;
    }

    void update_mcppro(int devnum)
    {
        char szNumericMsg[8] = {0};
        if (mach_ias_changed || mach_changed || ias_changed)
        {
            if (mach_ias_changed
                || (last_mach_ias && mach_changed)
                || (!last_mach_ias && ias_changed)
                )
            {
                if (!spd_display_on )
                {
                    sprintf( szNumericMsg, "     ");
                }
                else if (last_mach_ias )
                {
                    sprintf( szNumericMsg, " %05.3f", last_mach);
                }
                else
                {
                    sprintf( szNumericMsg, " %3d", last_ias);
                }
                GFMCPPro_SetBDisplayText(devnum, szNumericMsg);
            }
            mach_ias_changed = false;
            ias_changed = false;
            mach_changed = false;
        }

        if (hdg_changed)
        {
            if (show_bank_lim )
            {
                if (last_bank_lim)
                {
                    sprintf( szNumericMsg, "b%02d", last_bank_lim);
                }
                else
                {
                    sprintf( szNumericMsg, " a ");
                }
            }
            else
            {
                sprintf( szNumericMsg, "%03d", (last_hdg == 360)?0:last_hdg);
            }
             GFMCPPro_SetCDisplayText(devnum, szNumericMsg);

            hdg_changed = false;
        }
        if (vs_changed)
        {
            if (!vs_display_on)
            {
                sprintf( szNumericMsg, "     ");
            }
            else
            {
                sprintf( szNumericMsg, "%+05d", last_vs);
            }
            GFMCPPro_SetEDisplayText(devnum, szNumericMsg);
            vs_changed = false;
        }
        if (alt_changed)
        {
            sprintf( szNumericMsg, "%05d", last_alt);
            GFMCPPro_SetDDisplayText(devnum, szNumericMsg);
            alt_changed = false;
        }
        if (panel_state_changed)
        {
            GFMCPPro_SetIndicators(devnum, ulIndicatorState);
            panel_state_changed = false;
        }
    }

    controls_state sim_controls_state;
};



namespace //globals
{
    controls_info global_controls_info;
}






void map_747_events(HANDLE hSimConnect)
{
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressMINS,       "PMDG.747-400.EFIS Captain.Press MINS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseMINS,    "PMDG.747-400.EFIS Captain.Increase MINS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseMINS,    "PMDG.747-400.EFIS Captain.Decrease MINS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainResetMINS,       "PMDG.747-400.EFIS Captain.Reset MINS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressBARO,       "PMDG.747-400.EFIS Captain.Press BARO");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseBARO,    "PMDG.747-400.EFIS Captain.Increase BARO");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseBARO,    "PMDG.747-400.EFIS Captain.Decrease BARO");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressBAROSTD,    "PMDG.747-400.EFIS Captain.Press BARO STD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressFPV,        "PMDG.747-400.EFIS Captain.Press FPV");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressMTRS,       "PMDG.747-400.EFIS Captain.Press MTRS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNAVL,    "PMDG.747-400.EFIS Captain.Increase NAVL");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNAVL,    "PMDG.747-400.EFIS Captain.Decrease NAVL");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNAVR,    "PMDG.747-400.EFIS Captain.Increase NAVR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNAVR,    "PMDG.747-400.EFIS Captain.Decrease NAVR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNDMODE,  "PMDG.747-400.EFIS Captain.Increase NDMODE");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNDMODE,  "PMDG.747-400.EFIS Captain.Decrease NDMODE");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressNDMODECTR,  "PMDG.747-400.EFIS Captain.Press NDMODE CTR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNDRANGE, "PMDG.747-400.EFIS Captain.Increase NDRANGE");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNDRANGE, "PMDG.747-400.EFIS Captain.Decrease NDRANGE");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressNDRANGETFC, "PMDG.747-400.EFIS Captain.Press NDRANGE TFC");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressWXR,        "PMDG.747-400.EFIS Captain.Press WXR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressSTA,        "PMDG.747-400.EFIS Captain.Press STA");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressWPT,        "PMDG.747-400.EFIS Captain.Press WPT");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressARPT,       "PMDG.747-400.EFIS Captain.Press ARPT");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressDATA,       "PMDG.747-400.EFIS Captain.Press DATA");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressPOS,        "PMDG.747-400.EFIS Captain.Press POS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainPressTERR,       "PMDG.747-400.EFIS Captain.Press TERR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainSetMINS,         "PMDG.747-400.EFIS Captain.Set MINS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainSetBARO,         "PMDG.747-400.EFIS Captain.Set BARO");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNAVL,         "PMDG.747-400.EFIS Captain.Set NAVL");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNAVR,         "PMDG.747-400.EFIS Captain.Set NAVR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNDMODE,       "PMDG.747-400.EFIS Captain.Set NDMODE");
    SimConnect_MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNDRANGE,      "PMDG.747-400.EFIS Captain.Set NDRANGE"    );
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressATArm,          "PMDG.747-400.MCP.Press AT Arm");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressN1,             "PMDG.747-400.MCP.Press N1");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressTHR,            "PMDG.747-400.MCP.Press THR");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressSPD,            "PMDG.747-400.MCP.Press SPD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressCO,             "PMDG.747-400.MCP.Press C/O");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressFLCH,           "PMDG.747-400.MCP.Press FLCH");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressVNAV,           "PMDG.747-400.MCP.Press VNAV");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressLNAV,           "PMDG.747-400.MCP.Press LNAV");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressVORLOC,         "PMDG.747-400.MCP.Press VORLOC");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressAPP,            "PMDG.747-400.MCP.Press APP");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressHDGSEL,         "PMDG.747-400.MCP.Press HDG SEL");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressHDGHOLD,        "PMDG.747-400.MCP.Press HDG HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressALTHOLD,        "PMDG.747-400.MCP.Press ALT HOLD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressVS,             "PMDG.747-400.MCP.Press V/S");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressCMDL,           "PMDG.747-400.MCP.Press CMD L");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressCMDC,           "PMDG.747-400.MCP.Press CMD C");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressCMDR,           "PMDG.747-400.MCP.Press CMD R");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressFDL,            "PMDG.747-400.MCP.Press FD L");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressFDR,            "PMDG.747-400.MCP.Press FD R");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressAPDiseng,       "PMDG.747-400.MCP.Press AP Diseng");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPIncreaseBankLimiter, "PMDG.747-400.MCP.Increase Bank Limiter");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPDecreaseBankLimiter, "PMDG.747-400.MCP.Decrease Bank Limiter");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPIncreaseAlt,         "PMDG.747-400.MCP.Increase Alt");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPDecreaseAlt,         "PMDG.747-400.MCP.Decrease Alt");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPIncreaseSpd,         "PMDG.747-400.MCP.Increase Spd");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPDecreaseSpd,         "PMDG.747-400.MCP.Decrease Spd");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPIncreaseHdg,         "PMDG.747-400.MCP.Increase Hdg");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPDecreaseHdg,         "PMDG.747-400.MCP.Decrease Hdg");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPIncreaseVS,          "PMDG.747-400.MCP.Increase V/S");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPDecreaseVS,          "PMDG.747-400.MCP.Decrease V/S");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetTOGA,             "PMDG.747-400.MCP.Set TO/GA");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPResetTOGA,           "PMDG.747-400.MCP.Reset TO/GA");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressSpdIntv,        "PMDG.747-400.MCP.Press Spd Intv");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressAltIntv,        "PMDG.747-400.MCP.Press Alt Intv");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressCMD,            "PMDG.747-400.MCP.Press CMD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressCWS,            "PMDG.747-400.MCP.Press CWS");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressFD,             "PMDG.747-400.MCP.Press FD");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressTOGA,           "PMDG.747-400.MCP.Press TO/GA");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPPressBankLimiter,    "PMDG.747-400.MCP.Press Bank Limiter");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetAltitude,         "PMDG.747-400.MCP.Set Altitude");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetSpeed,            "PMDG.747-400.MCP.Set Speed");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetHeading,          "PMDG.747-400.MCP.Set Heading");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetVS,               "PMDG.747-400.MCP.Set V/S");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetFDL,              "PMDG.747-400.MCP.Set F/D L");
    SimConnect_MapClientEventToSimEvent(hSimConnect, MCPSetFDR,              "PMDG.747-400.MCP.Set F/D R");
    /*
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressMINS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainIncreaseMINS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainDecreaseMINS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainResetMINS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressBARO);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainIncreaseBARO);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainDecreaseBARO);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressBAROSTD);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressFPV);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressMTRS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainIncreaseNAVL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainDecreaseNAVL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainIncreaseNAVR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainDecreaseNAVR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainIncreaseNDMODE);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainDecreaseNDMODE);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressNDMODECTR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainIncreaseNDRANGE);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainDecreaseNDRANGE);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressNDRANGETFC);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressWXR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressSTA);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressWPT);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressARPT);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressDATA);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressPOS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainPressTERR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainSetMINS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainSetBARO);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainSetNAVL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainSetNAVR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainSetNDMODE);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,EFISCaptainSetNDRANGE);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressATArm);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressN1);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressTHR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressSPD);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressCO);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressFLCH);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressVNAV);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressLNAV);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressVORLOC);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressAPP);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressHDGSEL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressHDGHOLD);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressALTHOLD);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressVS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressCMDL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressCMDC);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressCMDR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressFDL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressFDR);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressAPDiseng);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPIncreaseBankLimiter);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPDecreaseBankLimiter);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPIncreaseAlt);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPDecreaseAlt);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPIncreaseSpd);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPDecreaseSpd);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPIncreaseHdg);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPDecreaseHdg);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPIncreaseVS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPDecreaseVS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetTOGA);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPResetTOGA);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressSpdIntv);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressAltIntv);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressCMD);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressCWS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressFD);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressTOGA);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPPressBankLimiter);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetAltitude);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetSpeed);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetHeading);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetVS);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetFDL);
    SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1,MCPSetFDR);
    */
}

void map_747_vars(HANDLE hSimConnect)
{
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,   0, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISBaro);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,   4, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISMins);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,  20, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISNavL);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,  24, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISNavR);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,  28, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISNDMode);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,  32, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISNDCenter);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA,  36, SIMCONNECT_CLIENTDATATYPE_INT32,0,EFISNDRange);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 132, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPHdg);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 136, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPIas);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 140, SIMCONNECT_CLIENTDATATYPE_FLOAT64,0,MCPMach);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 148, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPAlt);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 152, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPVS); //seems MCP in reality uses another var from AP data
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 160, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPPanelState);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_AP_DATA, 16, SIMCONNECT_CLIENTDATATYPE_INT32,0, MACH_IAS);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_AP_DATA, 108, SIMCONNECT_CLIENTDATATYPE_INT32,0, AP_BANK_LIM);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_AP_DATA, 140, SIMCONNECT_CLIENTDATATYPE_INT32,0, AP_VS);
    SimConnect_MapClientDataNameToID(hSimConnect,"SDD_AIRCRAFTAUTOPILOTDATA", CDID_AIRCRAFTAUTOPILOTDATA);
    SimConnect_MapClientDataNameToID(hSimConnect,"SDD_AIRCRAFTINSTRUMENTSDATA", CDID_AIRCRAFTINSTRUMENTSDATA);
    SimConnect_RequestClientData(hSimConnect,  CDID_AIRCRAFTAUTOPILOTDATA, REQUEST_FIRST, DEFINITION_AP_DATA, SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED, 0, 0, 0);
    SimConnect_RequestClientData(hSimConnect,  CDID_AIRCRAFTINSTRUMENTSDATA, REQUEST_FIRST, DEFINITION_INSTRUMENT_DATA, SIMCONNECT_CLIENT_DATA_PERIOD_ONCE, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED, 0, 0, 0);
    SimConnect_RequestClientData(hSimConnect,  CDID_AIRCRAFTAUTOPILOTDATA, REQUEST_APDATA, DEFINITION_AP_DATA, SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED|SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED, 0, 0, 0);
    SimConnect_RequestClientData(hSimConnect,  CDID_AIRCRAFTINSTRUMENTSDATA, REQUEST_DATA, DEFINITION_INSTRUMENT_DATA, SIMCONNECT_CLIENT_DATA_PERIOD_ON_SET, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED|SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_CHANGED , 0, 0, 0);

}







namespace
{

int quit = 0;
bool second_buf_filled = false;
unsigned char buf[2048][2] = {0};
int cur_buf = 0;
bool is_pmdg747_loaded(const std::string& filename)
{

    std::string upcased;
    std::transform(filename.begin(), filename.end(), std::back_inserter(upcased), toupper);
    //it is not fair upcase, but we just need to find latin string
    if (upcased.rfind("PMDG747-400") == std::string::npos)
    {
        return false;
    }
    return true;
}
}

BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
    quit = 1;
    return TRUE;
};

void traceerror(HRESULT hr, LPCWSTR tsz)
{
    if (FAILED(hr))
    {
        wchar_t tszBuf[1024] = { 0 };
        swprintf(tszBuf, sizeof(tszBuf)/sizeof(tszBuf[0]), L"%s returned hr = %08X\n", tsz, hr);
        OutputDebugStringW(tszBuf);
    }
}





namespace press_knob
{


enum returns
{
    NoAction,
    Increment,
    Decrement,
    Press,
    Hold,
    Released,
    ReleasedAfterRotate
};

returns pressed_knob_input(short & dialcontext, short dialval, unsigned short delta)
{
    const short new_middle = 10000;
    if ((std::abs(dialcontext - new_middle) < delta) || (std::abs(dialcontext) < delta))
    {
        dialcontext += dialval;
    }

    if ((dialcontext + delta) <= 0 || (dialcontext >= delta && dialcontext < (delta * 10)) )
    {
        dialcontext += new_middle;
    }

    if (dialcontext <= (new_middle - delta) && dialcontext >= (delta * 10))
    {
        dialcontext += delta;
        return Decrement;
    }
    else if (dialcontext >= (new_middle + delta))
    {
        dialcontext -= delta;
        return Increment;
    }
    return NoAction;
}


returns process_knob(int & unpressedval, short& dialcontext, short dialval,
        unsigned long prev_state, unsigned long new_state, unsigned long mask,
        std::chrono::high_resolution_clock::time_point& press_tp,
        unsigned short delta, unsigned short delta2)
{
    bool last_press = (prev_state & mask) ? true : false;
    bool new_press = (new_state & mask) ? true : false;
    returns ret = NoAction;

    if (new_press && last_press) //already pressed and holding
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now()
                    - press_tp).count() > controls_block::hold_key_time_ms)
        {
            ret = Hold;
        }
        ret = pressed_knob_input(dialcontext, dialval, delta);
    }
    else if ( !last_press && !new_press) // not pressed
    {
        unpressedval += dialval;
    }
    else if (last_press && !new_press) //released
    {
        if (std::abs(dialcontext + dialval) < delta2)
        {
            ret = Released;
        }
        else
        {
            ret = ReleasedAfterRotate;
        }
        dialcontext = 0;
    }
    else if (!last_press && new_press) //pressed
    {
        press_tp = std::chrono::high_resolution_clock::now();
        ret = Press;
    }
    return ret;
}


}





void fill_efis_data(controls_block& ctr, LPGFEFISINPUTREPORT r)
{
    static efis_input_context ctx;
    ctr.state.nav_l = ((r->wSwitchState) & GFEFIS_SW_10)?1:(((r->wSwitchState) & GFEFIS_SW_9)?2:0);
    ctr.state.nav_r = ((r->wSwitchState) & GFEFIS_SW_12)?1:(((r->wSwitchState) & GFEFIS_SW_11)?2:0);
    ctr.state.nd_mode = (r->bSelectors & GFEFIS_MASK_SEL1);
    ctr.state.nd_range = ((r->bSelectors & GFEFIS_MASK_SEL2) >> 4) + 1;

    unsigned short pressed = (ctx.wSwitchState ^ r->wSwitchState) & (r->wSwitchState);
    unsigned short released = (ctx.wSwitchState ^ r->wSwitchState) & ~(r->wSwitchState);

    ctr.cmd.wxr = (pressed & GFEFIS_SW_7)?1:0;
    ctr.cmd.sta = (pressed & GFEFIS_SW_6)?1:0;
    ctr.cmd.wpt = (pressed & GFEFIS_SW_5)?1:0;
    ctr.cmd.arpt = (pressed & GFEFIS_SW_4)?1:0;
    ctr.cmd.data = (pressed & GFEFIS_SW_3)?1:0;
    ctr.cmd.pos = (pressed & GFEFIS_SW_2)?1:0;
    ctr.cmd.terr = (pressed & GFEFIS_SW_1)?1:0;


    const unsigned short delta = 3;
    switch( press_knob::process_knob(ctr.cmd.mins_knob, ctx.nADialVal, r->nADialVal,
            ctx.wSwitchState, r->wSwitchState, GFEFIS_SW_15, ctx.A_pressed, delta, 1))
    {
        case press_knob::Increment: ctr.cmd.min_sw_inc = 1; break;
        case press_knob::Decrement: ctr.cmd.min_sw_dec = 1; break;
        case press_knob::Released: ctr.cmd.mins_rst = 1; break;
        default:
            ;
    }

    switch( press_knob::process_knob(ctr.cmd.baro_knob, ctx.nBDialVal, r->nBDialVal,
            ctx.wSwitchState, r->wSwitchState, GFEFIS_SW_16, ctx.B_pressed, delta, 1))
    {
        case press_knob::Increment: ctr.cmd.baro_sw_inc = 1; break;
        case press_knob::Decrement: ctr.cmd.baro_sw_dec = 1; break;
        case press_knob::Released: ctr.cmd.baro_std = 1; break;
        default:
            ;
    }

    if (pressed & GFEFIS_SW_13) //MTRS
    {
        ctx.mtrs_pressed = std::chrono::high_resolution_clock::now();
    }

    if (pressed & GFEFIS_SW_14) //FPV
    {
        ctx.fpv_pressed = std::chrono::high_resolution_clock::now();
    }


    if (released & GFEFIS_SW_13) //MTRS
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now()
                    - ctx.mtrs_pressed).count() > controls_block::hold_key_time_ms)
        {
            ctr.cmd.tfc = 1;
        }
        else
        {
            ctr.cmd.mtrs = 1;
        }
    }

    if (released & GFEFIS_SW_14) //FPV
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::high_resolution_clock::now()
                    - ctx.fpv_pressed).count() > controls_block::hold_key_time_ms)
        {
            ctr.cmd.ctr = 1;
        }
        else
        {
            ctr.cmd.fpv = 1;
        }
    }
    ctx.wSwitchState = r->wSwitchState;
}

void fill_mcp_data(controls_block& ctr, LPGFMCPPROINPUTREPORT r)
{
    static mcppro_input_context ctx;

    ctr.state.ap_disengage = ((r->ulSwitchState) & GFMCPPRO_SW_23)?1:0;
    ctr.state.at_arm = ((r->ulSwitchState) & GFMCPPRO_SW_5)?1:0;
    ctr.state.fd_l = ((r->ulSwitchState) & GFMCPPRO_SW_15)?1:0;
    ctr.state.fd_r = ((r->ulSwitchState) & GFMCPPRO_SW_24)?1:0;

    unsigned long pressed = (ctx.ulSwitchState ^ r->ulSwitchState) & (r->ulSwitchState);
    unsigned long released = (ctx.ulSwitchState ^ r->ulSwitchState) & ~(r->ulSwitchState);


    ctr.cmd.vnav = (pressed & GFMCPPRO_SW_1)?1:0;
    ctr.cmd.lnav = (pressed & GFMCPPRO_SW_2)?1:0;
    ctr.cmd.cmd_l = (pressed & GFMCPPRO_SW_3)?1:0;
    ctr.cmd.cmd_r = (pressed & GFMCPPRO_SW_4)?1:0;
    ctr.cmd.cmd_c = (pressed & GFMCPPRO_SW_10)?1:0;
    ctr.cmd.c_o = (pressed & GFMCPPRO_SW_6)?1:0;
    ctr.cmd.spd_intv = ((pressed & GFMCPPRO_SW_7) || (pressed & GFMCPPRO_SW_12))?1:0;

    ctr.cmd.vorloc = (pressed & GFMCPPRO_SW_8)?1:0;
    ctr.cmd.alt_intv = ((pressed & GFMCPPRO_SW_9) || (pressed & GFMCPPRO_SW_14))?1:0;

    ctr.cmd.thr = (pressed & GFMCPPRO_SW_16)?1:0;
    ctr.cmd.spd = (pressed & GFMCPPRO_SW_17)?1:0;
    ctr.cmd.flch = (pressed & GFMCPPRO_SW_18)?1:0;
    ctr.cmd.hdghld = (pressed & GFMCPPRO_SW_19)?1:0;
    ctr.cmd.app = (pressed & GFMCPPRO_SW_20)?1:0;
    ctr.cmd.althld = (pressed & GFMCPPRO_SW_21)?1:0;
    ctr.cmd.vs = (pressed & GFMCPPRO_SW_22)?1:0;

    switch( press_knob::process_knob(ctr.cmd.hdg_knob, ctx.nCDialVal, r->nCDialVal,
            ctx.ulSwitchState, r->ulSwitchState, GFMCPPRO_SW_13, ctx.C_pressed, 2, 1))
    {
        case press_knob::Increment: ctr.cmd.bank_inc = 1; break;
        case press_knob::Decrement: ctr.cmd.bank_dec = 1; break;
        case press_knob::Released: ctr.cmd.hdgsel = 1; break;
        case press_knob::ReleasedAfterRotate: ctr.cmd.bank_change_end = 1; break;
           case press_knob::Hold: ctr.cmd.bank_change_start = 1; break;
        default:
            ;
    }

    ctr.cmd.spd_knob += r->nBDialVal;
    ctr.cmd.alt_knob += r->nDDialVal;
    ctr.cmd.vs_knob += r->nEDialVal;

    ctx.ulSwitchState = r->ulSwitchState;
}



static BOOL InputCallback( GFDEV_HW_TYPE HWType, unsigned short wParam, LPGFINPUTREPORTTYPE lParam )
{
    BOOL bInputProcessed = FALSE;
    // any streams here is not thread safe, this callback running in separate thread
    if (NULL != lParam) // sanity check...
    {
        int gfmcppro_num = -1;
        int gfefis_num = -1;
        global_controls_info.get_devs(gfmcppro_num, gfefis_num);

        if (GFDEVHWTYPE_GFMCPPRO == HWType && wParam == gfmcppro_num)
        {
            LPGFMCPPROINPUTREPORT r = reinterpret_cast<LPGFMCPPROINPUTREPORT>(lParam);
/*            logger::lout << "M" << std::setw(8) << r->nADialVal << std::setw(8) << r->nBDialVal << std::setw(8) << r->nCDialVal
                << std::setw(8) << r->nDDialVal << std::setw(8) << r->nEDialVal << std::setw(8) << r->nFDialVal << std::hex << std::setw(10) << r->ulSwitchState << std::endl;*/
            controls_block& ctr = global_controls_info.get_current_for_write();
            fill_mcp_data(ctr, r);
            global_controls_info.input_processed();
            bInputProcessed = TRUE;
        }
        else if (GFDEVHWTYPE_GFEFIS == HWType  && wParam == gfefis_num)
        {
            LPGFEFISINPUTREPORT r = reinterpret_cast<LPGFEFISINPUTREPORT>(lParam);
/*            logger::lout << "E" << std::setw(8) << r->nADialVal << std::setw(8) << r->nBDialVal << std::setw(8) << std::hex << int(r->bSelectors)
                << std::hex << std::setw(10) << r->wSwitchState << std::endl;*/
            controls_block& ctr = global_controls_info.get_current_for_write();
            fill_efis_data(ctr, r);
            global_controls_info.input_processed();
            bInputProcessed = TRUE;
        }
    }

    return bInputProcessed;
}


void GFInitialUpdate(int gfmcppro_num, int gfefis_num)
{
    controls_block& ctr = global_controls_info.get_current_for_write();
    if (gfmcppro_num >= 0)
    {
        GFMCPPROINPUTREPORT r;
        GFMCPPro_GetControlVals(
            gfmcppro_num, &(r.nADialVal), &(r.nBDialVal), &(r.nCDialVal), &(r.nDDialVal), &(r.nEDialVal), &(r.nFDialVal), &(r.ulSwitchState));
        fill_mcp_data(ctr, &r);
    }
    if (gfefis_num >= 0)
    {
        GFEFISINPUTREPORT r;
        GFEFIS_GetControlVals( gfefis_num, &(r.nADialVal), &(r.nBDialVal), &(r.wSwitchState), &(r.bSelectors ));
        fill_efis_data(ctr, &r);
    }
    global_controls_info.input_processed();
}

void remove_menu(HANDLE hSimConnect)
{
    SimConnect_MenuDeleteItem(hSimConnect, EVENT_MENU_ROOT);
}
void add_menu(HANDLE hSimConnect)
{
    remove_menu(hSimConnect);
    HRESULT hr = SimConnect_MenuAddItem(hSimConnect, "GF<->747 &Interface", EVENT_MENU_ROOT, 0);
    if (hr == S_OK)
    {
        SimConnect_MenuAddSubItem(hSimConnect, EVENT_MENU_ROOT, "&Restart GFDev", EVENT_MENU_RESTART, 0);
    }
}




void process_input(HANDLE hSimConnect, const controls_block& controls,  mcppro_display_747& mcppro, int gfmcppro_num, int gfefis_num)
{
    const SIMCONNECT_OBJECT_ID obj = SIMCONNECT_OBJECT_ID_USER;
    const SIMCONNECT_NOTIFICATION_GROUP_ID gid = SIMCONNECT_GROUP_PRIORITY_HIGHEST;
    const SIMCONNECT_EVENT_FLAG flag  = SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY;
    const controls_state& sim_controls_state = mcppro.sim_controls_state;
    {
        if (gfmcppro_num >= 0)
        {
            if (controls.state.fd_l != sim_controls_state.fd_l)
                SimConnect_TransmitClientEvent( hSimConnect, obj , MCPSetFDL, controls.state.fd_l ,gid , flag );
            if (controls.state.fd_r != sim_controls_state.fd_r)
                SimConnect_TransmitClientEvent( hSimConnect, obj , MCPSetFDR, controls.state.fd_r ,gid , flag );
            if (controls.state.at_arm != sim_controls_state.at_arm)
                SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressATArm, 0 ,gid , flag );
            if (controls.state.ap_disengage != sim_controls_state.ap_disengage)
                SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressAPDiseng, 0 ,gid , flag );
        }

        if (gfefis_num >= 0)
        {

            if (controls.state.nav_l != sim_controls_state.nav_l)
                SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainSetNAVL, controls.state.nav_l ,gid , flag );
            if (controls.state.nav_r != sim_controls_state.nav_r)
                SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainSetNAVR, controls.state.nav_r ,gid , flag );
            if (controls.state.nd_mode != sim_controls_state.nd_mode)
                SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainSetNDMODE, controls.state.nd_mode ,gid , flag );
            if (controls.state.nd_range != sim_controls_state.nd_range)
                SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainSetNDRANGE, controls.state.nd_range ,gid , flag );
        }

        if ((controls.cmd.min_sw_inc && !sim_controls_state.min_sw) || (controls.cmd.min_sw_dec && sim_controls_state.min_sw) )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressMINS, 0 ,gid , flag );
        if ((controls.cmd.baro_sw_inc && !sim_controls_state.baro_sw) || (controls.cmd.baro_sw_dec && sim_controls_state.baro_sw) )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressBARO, 0 ,gid , flag );
        if (controls.cmd.mins_rst )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainResetMINS, 0 ,gid , flag );
        if (controls.cmd.baro_std )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressBAROSTD, 0 ,gid , flag );
        if (controls.cmd.fpv )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressFPV, 0 ,gid , flag );
        if (controls.cmd.mtrs )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressMTRS, 0 ,gid , flag );
        if (controls.cmd.ctr )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressNDMODECTR, 0 ,gid , flag );
        if (controls.cmd.tfc )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressNDRANGETFC, 0 ,gid , flag );

        if (controls.cmd.wxr )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressWXR, 0 ,gid , flag );
        if (controls.cmd.sta )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressSTA, 0 ,gid , flag );
        if (controls.cmd.wpt )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressWPT, 0 ,gid , flag );
        if (controls.cmd.arpt )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressARPT, 0 ,gid , flag );
        if (controls.cmd.data )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressDATA, 0 ,gid , flag );
        if (controls.cmd.pos )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressPOS, 0 ,gid , flag );
        if (controls.cmd.terr )
            SimConnect_TransmitClientEvent( hSimConnect, obj , EFISCaptainPressTERR, 0 ,gid , flag );

        if (controls.cmd.thr )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressTHR, 0 ,gid , flag );
        if (controls.cmd.spd )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressSPD, 0 ,gid , flag );
        if (controls.cmd.c_o )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressCO, 0 ,gid , flag );
        if (controls.cmd.flch )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressFLCH, 0 ,gid , flag );
        if (controls.cmd.vnav )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressVNAV, 0 ,gid , flag );
        if (controls.cmd.lnav )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressLNAV, 0 ,gid , flag );
        if (controls.cmd.vorloc )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressVORLOC, 0 ,gid , flag );
        if (controls.cmd.app )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressAPP, 0 ,gid , flag );
        if (controls.cmd.hdgsel)
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressHDGSEL, 0 ,gid , flag );
        if (controls.cmd.hdghld )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressHDGHOLD, 0 ,gid , flag );
        if (controls.cmd.althld )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressALTHOLD, 0 ,gid , flag );
        if (controls.cmd.vs )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressVS, 0 ,gid , flag );
        if (controls.cmd.cmd_l )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressCMDL, 0 ,gid , flag );
        if (controls.cmd.cmd_c )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressCMDC, 0 ,gid , flag );
        if (controls.cmd.cmd_r )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressCMDR, 0 ,gid , flag );
        if (controls.cmd.alt_intv )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressAltIntv, 0 ,gid , flag );
        if (controls.cmd.spd_intv )
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPPressSpdIntv, 0 ,gid , flag );

        if (controls.cmd.bank_inc )
        {
            mcppro.show_bank(true);
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPIncreaseBankLimiter, 0 ,gid , flag );
        }
        if (controls.cmd.bank_dec )
        {
            mcppro.show_bank(true);
            SimConnect_TransmitClientEvent( hSimConnect, obj , MCPDecreaseBankLimiter, 0 ,gid , flag );
        }
        if (controls.cmd.bank_change_start )
        {
            mcppro.show_bank(true);
        }

        if (controls.cmd.bank_change_end )
        {
            mcppro.show_bank(false);
        }

        if (controls.cmd.alt_knob)
        {
            bool inc = (controls.cmd.alt_knob > 0);
            for (int i = std::abs(controls.cmd.alt_knob); i > 0; --i )
            {
                SimConnect_TransmitClientEvent( hSimConnect, obj , (inc)?MCPIncreaseAlt:MCPDecreaseAlt, 0 ,gid , flag );
            }
        }

        if (controls.cmd.hdg_knob)
        {
            bool inc = (controls.cmd.hdg_knob > 0);
            for (int i = std::abs(controls.cmd.hdg_knob); i > 0; --i )
            {
                SimConnect_TransmitClientEvent( hSimConnect, obj , (inc)?MCPIncreaseHdg:MCPDecreaseHdg, 0 ,gid , flag );
            }
        }

        if (controls.cmd.spd_knob)
        {
            bool inc = (controls.cmd.spd_knob > 0);
            for (int i = std::abs(controls.cmd.spd_knob); i > 0; --i )
            {
                SimConnect_TransmitClientEvent( hSimConnect, obj , (inc)?MCPIncreaseSpd:MCPDecreaseSpd, 0 ,gid , flag );
            }
        }

        if (controls.cmd.vs_knob)
        {
            bool inc = (controls.cmd.vs_knob > 0);
            for (int i = std::abs(controls.cmd.vs_knob); i > 0; --i )
            {
                SimConnect_TransmitClientEvent( hSimConnect, obj , (inc)?MCPIncreaseVS:MCPDecreaseVS, 0 ,gid , flag );
            }
        }

        if (controls.cmd.mins_knob)
        {
            bool inc = (controls.cmd.mins_knob > 0);
            for (int i = std::abs(controls.cmd.mins_knob); i > 0; --i )
            {
                SimConnect_TransmitClientEvent( hSimConnect, obj , (inc)?EFISCaptainIncreaseMINS:EFISCaptainDecreaseMINS, 0 ,gid , flag );
            }
        }

        if (controls.cmd.baro_knob)
        {
            bool inc = (controls.cmd.baro_knob > 0);
            for (int i = std::abs(controls.cmd.baro_knob); i > 0; --i )
            {
                SimConnect_TransmitClientEvent( hSimConnect, obj , (inc)?EFISCaptainIncreaseBARO:EFISCaptainDecreaseBARO, 0 ,gid , flag );
            }
        }

    }

}


bool process_mcppro(HANDLE hSimConnect, mcppro_display_747& mcppro, controls_info::loaded_states pmdg747_loaded) //returns true if goFlight hardware processing performs
{
    int gfmcppro_num = -1;
    int gfefis_num = -1;

    bool plane_reload = false;
    bool hwchanged = false;
    switch (pmdg747_loaded)
    {
        case controls_info::PMDG747_UNLOADED:
            logger::lout << "747 Unloaded" << std::endl;
            remove_menu(hSimConnect);
            GFDev_UnregisterInputCallback();
            SimConnect_UnsubscribeFromSystemEvent(hSimConnect, EVENT_6HZ);
            global_controls_info.clear_display();
        case controls_info::PMDG747_NOTLOADED:
            break;
        case controls_info::PMDG747_JUSTLOADED:
            logger::lout << "747 loaded" << std::endl;
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_6HZ, "6Hz");
            add_menu(hSimConnect);
            plane_reload = true;
        case controls_info::PMDG747_LOADED:
            hwchanged = global_controls_info.get_devs_and_changed(gfmcppro_num, gfefis_num);
            if (hwchanged || plane_reload)
            {
                GFInitialUpdate(gfmcppro_num, gfefis_num);
                mcppro.invalidate();
                GFDev_RegisterInputCallback(InputCallback); // Tell API to notify us on input
            }
            if (global_controls_info.get_new_input())
            {
                process_input(hSimConnect, global_controls_info.get_current_for_read(), mcppro, gfmcppro_num, gfefis_num);
            }
            return true;
        default:
            logger::lout << "unknown state " << std::endl;
    }
    return false;
}


namespace
{
#ifdef _USRDLL
    ID bank_var_id = -1;
    ID yoke_var_id = -1;

#endif //_USRDLL

    void update_var_id()
    {
#ifdef _USRDLL
        bank_var_id = check_named_variable("MCP_BankLimit_Knob_var");
        yoke_var_id = check_named_variable("yokehide");
#endif //_USRDLL
    }

    void yoke_hide()
    {
#ifdef _USRDLL
        if (yoke_var_id >= 0)
        {
            set_named_variable_value(yoke_var_id, FLOAT64(1.0));
        }
#endif
    }

    int get_max_bank_value()
    {
        int ret = -1;
#ifdef _USRDLL
        if (bank_var_id >= 0)
        {
            ret = static_cast<int>(get_named_variable_value(bank_var_id));
            ret /= 2;
        }
#endif
        return ret;
    }

}

void CALLBACK MyDispatchProcPDR(SIMCONNECT_RECV* pData, DWORD cbData, void *ctxt)
{
    HRESULT hr;
    static mcppro_display_747 mcppro;
    mcppro_display_747 * mcp = &mcppro;
    HANDLE hSimConnect = reinterpret_cast<HANDLE>(ctxt);
    //mcppro_display_747 * mcp = reinterpret_cast<mcppro_display_747*>(ctxt);
    bool display_update = false;
    static int bank_lim = -1;
    switch(pData->dwID)
    {
        case SIMCONNECT_RECV_ID_OPEN:
        {
            break;
        }

        case SIMCONNECT_RECV_ID_EVENT:
        {
            SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;
            SIMCONNECT_RECV_EVENT_FILENAME * ev_f = (SIMCONNECT_RECV_EVENT_FILENAME*)evt;
            switch(evt->uEventID)
            {
                case EVENT_SIM_START:
                    logger::lout << "SIMSTART" << std::endl;
                    break;
                case EVENT_SIM_PAUSED:
                    logger::lout << "Paused" << std::endl;
                    break;
                case EVENT_SIM_UNPAUSED:
                    logger::lout << "Unpaused" << std::endl;
                    update_var_id();
                    global_controls_info.on_unpaused();
                    break;
                case EVENT_SIM_STOP:
                    logger::lout << "SIMSTOP" << std::endl;
                    break;
                case EVENT_MENU_RESTART:
                    global_controls_info.on_hardware_changed();
                    SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER,
                        EVENT_HAS_INPUT, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                    break;
                case EVENT_MENU_ROOT:
                    break;
                case EVENT_6HZ:
                    bank_lim = get_max_bank_value();
                    if (bank_lim >= 0)
                    {
                        mcp->set_bank_lim(bank_lim);
                    }
                    if (global_controls_info.need_panel_state_update())
                    {
                        yoke_hide();
                        SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER,
                            EVENT_HAS_INPUT, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                    }
                    display_update = true;
                    break;
                case EVENT_HAS_INPUT:
                    process_mcppro(hSimConnect, mcppro, global_controls_info.get_pmdg747_loaded());
                    break;
                default:
                   logger::lout << "EVENT " << evt->uEventID << std::endl;
                   break;
            }
            break;

        }

        case SIMCONNECT_RECV_ID_CLIENT_DATA:
        {
            SIMCONNECT_RECV_CLIENT_DATA *pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;
            int    count    = 0;
            BYTE * pD = reinterpret_cast<BYTE*>(&pObjData->dwData);

            while (count < (int) pObjData->dwDefineCount)
            {
                int id = *reinterpret_cast<int*>(pD);
                pD += sizeof(int);
                size_t tsize = sizeof(int);
                if (id == MCPMach)
                {
                    double val = *reinterpret_cast<double*>(pD);
                    //logger::lout << id << "/" << val << std::endl;
                    tsize = sizeof(double);
                    mcp->set_mach(val);
                }
                else
                {
                    int val = *reinterpret_cast<int*>(pD);
                    logger::lout << std::dec << id << "/" << val << std::endl;
                    switch (id)
                    {
                        case EFISBaro:
                            mcp->sim_controls_state.baro_sw = val;
                            break;
                        case EFISMins:
                            mcp->sim_controls_state.min_sw = val;
                            break;
                        case EFISNavL:
                            mcp->sim_controls_state.nav_l = val;
                            break;
                        case EFISNavR:
                            mcp->sim_controls_state.nav_r = val;
                            break;
                        case EFISNDMode:
                            mcp->sim_controls_state.nd_mode = val;
                            break;
                        case EFISNDRange:
                            mcp->sim_controls_state.nd_range = val;
                            break;

                        case MCPHdg:
                            mcp->set_hdg(val);
                            break;
                        case MCPIas:
                            mcp->set_ias(val);
                            break;
                        case MCPAlt:
                            mcp->set_alt(val);
                            break;
                        case MCPVS:
//                            mcp->set_vs(val);
                            break;
                        case MCPPanelState:
                            mcp->set_panel_state(val);
                            mcp->sim_controls_state.fd_l         = (val & 0x00000010u) ? 1 : 0;
                            mcp->sim_controls_state.fd_r         = (val & 0x00000020u) ? 1 : 0;
                            mcp->sim_controls_state.at_arm       = (val & 0x00000040u) ? 1 : 0;
                            mcp->sim_controls_state.ap_disengage = (val & 0x00000008u) ? 1 : 0;
                            global_controls_info.on_panel_state();
                            break;
                        case MACH_IAS:
                            mcp->set_mach_ias(val);
                            break;
                        case AP_BANK_LIM:
                            if (bank_lim < 0)
                            {
                                mcp->set_bank_lim(val);
                            }
                            break;
                        case AP_VS:
                            mcp->set_vs(val);
                            break;
                        default:
                            break;
                    }
                }
                pD += tsize;
                ++count;
            }
            display_update = true;
            break;
        }

        case SIMCONNECT_RECV_ID_EVENT_FILENAME:
        {
            SIMCONNECT_RECV_EVENT_FILENAME * ev_f = (SIMCONNECT_RECV_EVENT_FILENAME*)pData;
            switch(ev_f->uEventID)
            {
                case EVENT_AIRCRAFT_LOADED:
                    global_controls_info.set_pmdg747_loaded(is_pmdg747_loaded(ev_f->szFileName));
                    global_controls_info.onreload();
                    SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER,
                        EVENT_HAS_INPUT, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

                    logger::lout << "AircraftLoaded" << std::endl;
                    break;
                case EVENT_FLIGHT_LOADED:
                    global_controls_info.onreload();
                    logger::lout << "FlightLoaded" << std::endl;
                    break;
                default:
                   break;
            }
            break;

        }

        case SIMCONNECT_RECV_ID_SYSTEM_STATE:
        {
            SIMCONNECT_RECV_SYSTEM_STATE * ptr = (SIMCONNECT_RECV_SYSTEM_STATE*)pData;
            switch(ptr->dwRequestID)
            {
                case EVENT_SIM_START:
                    logger::lout << "START" << std::endl;
                    break;
                case EVENT_AIRCRAFT_LOADED:
                    global_controls_info.set_pmdg747_loaded(is_pmdg747_loaded(ptr->szString));
                    global_controls_info.onreload();
                    SimConnect_TransmitClientEvent(hSimConnect, SIMCONNECT_OBJECT_ID_USER,
                        EVENT_HAS_INPUT, 0, SIMCONNECT_GROUP_PRIORITY_HIGHEST, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
                    break;
                default:
                    break;
            }
            break;
        }



        case SIMCONNECT_RECV_ID_QUIT:
        {
            quit = 1;
            break;
        }

        default:
        //    printf("Unknown dwID: %d\n",pData->dwID);
            break;
    }

    if (display_update)
    {
        int gfmcppro_num = -1;
        int gfefis_num = -1;
        global_controls_info.get_devs(gfmcppro_num, gfefis_num);
        if (gfmcppro_num >= 0)
        {
            mcp->update_mcppro(gfmcppro_num);
        }
    }


}


void CALLBACK NullDispatchProcPDR(SIMCONNECT_RECV* pData, DWORD cbData, void *ctxt)
{
}

int init_stuff(HMODULE hModule, HANDLE& hSimConnect, HANDLE& hSimConnect2)
{
    int result = 0;
    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "PMDG747GFMcpPro", NULL, 0, 0, 0)))
    {
        if (SUCCEEDED(SimConnect_Open(&hSimConnect2, "PMDG747GFMcpPro2", NULL, 0, 0, 0)))
        {
            result = 0;
            global_controls_info.init(hModule, hSimConnect, hSimConnect2);
            map_747_vars(hSimConnect);
            map_747_events(hSimConnect);
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_STOP, "SimStop");
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_PAUSED, "Paused");
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_UNPAUSED, "Unpaused");
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_AIRCRAFT_LOADED, "AircraftLoaded");
            SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_FLIGHT_LOADED, "FlightLoaded");
            SimConnect_RequestSystemState(hSimConnect, EVENT_AIRCRAFT_LOADED, "AircraftLoaded");
            const char event_name[] = { "PMDG747Iface.NewInput" };
            SimConnect_MapClientEventToSimEvent(hSimConnect, EVENT_HAS_INPUT, event_name);
            SimConnect_MapClientEventToSimEvent(hSimConnect2, EVENT_HAS_INPUT, event_name);
            SimConnect_AddClientEventToNotificationGroup(hSimConnect, GROUP_1, EVENT_HAS_INPUT, TRUE);

        }
        else
        {
            result = 2;
            SimConnect_Close(hSimConnect);
        }
    }
    else
    {
        result = 1;
    }
    return result;
}

void call_dispatch(HANDLE hSimConnect, HANDLE hSimConnect2)
{
    SimConnect_CallDispatch(hSimConnect, MyDispatchProcPDR, hSimConnect);
    SimConnect_CallDispatch(hSimConnect2, NullDispatchProcPDR, NULL);
}

int main(int argc, char* argv[])
{
    SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
    HRESULT hr = NULL;
    HMODULE hModule = ::GetModuleHandle(NULL);

    int state = -1;
    try
    {
        HANDLE hSimConnect, hSimConnect2;
        state = init_stuff(hModule, hSimConnect, hSimConnect2);
        switch (state)
        {
        case 0:
            printf("\nConnected to Flight Simulator!\n");
            while (0 == quit)
            {
                call_dispatch(hSimConnect, hSimConnect2);
            }
            break;
        case 2:
            printf("Secondary Simconnect connection failed\n");
            getchar();
            break;
        case 1:
            printf("Simconnect connection failed\n");
            getchar();
            break;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}




#ifdef _USRDLL

HMODULE g_hModule = NULL;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    g_hModule = hModule;


    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:

        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}




PPANELS Panels = NULL;

GAUGESIMPORT    ImportTable =
{
    { 0x0000000F, (PPANELS)NULL },
    { 0x00000000, NULL }
};


int __stdcall DLLStart(void)
{
    if (NULL != Panels)
    {
        ImportTable.PANELSentry.fnptr = (PPANELS)Panels;
    }
    HANDLE hSimConnect, hSimConnect2;
    if (g_hModule)
    {
        if (!init_stuff(g_hModule, hSimConnect, hSimConnect2))
        {
            call_dispatch(hSimConnect, hSimConnect2);
        }
    }
    return 0;
}

//
// The DLLStop function must be present.
//
void __stdcall DLLStop(void)
{
    global_controls_info.deinit();
}


#endif //_USRDLL
