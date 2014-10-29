
#include "stdafx.h"

#include <SimConnect.h>

#include <iostream>
#include <string.h>
#include <iomanip>
#include <map>
#include "GFDevApi.h"



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
    REQUEST_DATA,
	REQUEST_VISIBLE,
    REQUEST_LED
};

enum event_groups
{
    GROUP_1
};



namespace
{
const size_t mcp_field_size = 8;
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
    double last_mach;
    bool hdg_changed;
    bool alt_changed;
    bool vs_changed;
    bool mach_ias_changed;
    bool panel_state_changed;
    unsigned long ulIndicatorState;
    bool spd_display_on;
    bool vs_display_on;

    static unsigned set_indicator_by_panel_state(unsigned panel_state)
    {
// panel state bitmask(0=ap L,1=ap R,2=ap C,3=ap dis,4=f/d,5=fo f/d,6=a/t,7=thr,8=spd,9=flch,10=vnav,11=lnav,12=loc,13=app,15=althld,16=v/s,19=hdghold,25=spddisplay,27=v/sdisplay )
//          0x00000001 apL
//          0x00000002 apR
//          0x00000004 apC
//          0x00000008 apDis
//          0x00000010 fdL
//          0x00000020 fdR
//          0x00000040 at
//          0x00000080 thr
//          0x00000100 spd
//          0x00000200 flch
//          0x00000400 vnav
//          0x00000800 lnav
//          0x00001000 loc
//          0x00002000 app
//          0x00004000 althld
//          0x00008000 vs
//          0x00010000
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
         return  ((panel_state & 0x03u) << 2) // apL apR
                | ((panel_state & 0x0C00u) >> 10)   // vnav lnav
                | ((panel_state & 0x04u) << 7) // apC
                | ((panel_state & 0x010u) << 10 ) // fdL
                | ((panel_state & 0x020u) << 18 ) // fdR
                | ((panel_state & 0x0380u) << 4 ) // thr spd flch
                | ((panel_state & 0x01000u) >> 5) // loc
                | ((panel_state & 0x0E000u) << 6) // app althld vs
                | ((panel_state & 0x080000u) << 1) // hdg
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
          ,hdg_changed(false)
          ,alt_changed(false)
          ,vs_changed(false)
          ,mach_ias_changed(false)
          ,panel_state_changed(false)
          ,ulIndicatorState(0ul), spd_display_on(false), vs_display_on(false)
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

    void set_ias(int arg)
    {
        if (arg != last_ias)
        {
            last_ias = arg;
            mach_ias_changed = true;
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
            vs_changed = true;
        }
    }

    void set_panel_state(int arg)
    {
        if (arg != last_panel_state)
        {
            bool disp_on = arg & 0x02000000u; // ias/mach disp
            if (disp_on != spd_display_on)
            {
                spd_display_on = disp_on;
                mach_ias_changed = true;
            }
            disp_on = arg & 0x08000000u; // v/s disp
            if (disp_on != vs_display_on)
            {
                vs_display_on = disp_on;
                vs_changed = true;
            }

            last_panel_state = arg;
            unsigned ind = set_indicator_by_panel_state(last_panel_state);
            if ( ind != ulIndicatorState);
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
        if (std::abs(arg - last_mach) > 0.005 )
        {
            last_mach = arg;
            mach_ias_changed = true;
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
        if (mach_ias_changed)
        {
            if (!spd_display_on)
            {
                sprintf( szNumericMsg, " .    ");
            }
            else if (last_mach_ias == 1 )
            {
                sprintf( szNumericMsg, " %04.2f", last_mach);
            }
            else
            {
                sprintf( szNumericMsg, " %3d", last_ias);
            }
            GFMCPPro_SetBDisplayText(devnum, szNumericMsg);
            mach_ias_changed = false;
        }

        if (hdg_changed)
        {
            sprintf( szNumericMsg, "%03d", last_hdg);
            GFMCPPro_SetCDisplayText(devnum, szNumericMsg);
            hdg_changed = false;
        }
        if (vs_changed)
        {
            if (!vs_display_on)
            {
                sprintf( szNumericMsg, " .    ");
            }
            else
            {
                sprintf( szNumericMsg, "%+04d", last_vs);
            }
            GFMCPPro_SetDDisplayText(devnum, szNumericMsg);
            vs_changed = false;
        }
        if (alt_changed)
        {
            sprintf( szNumericMsg, "%05d", last_alt);
            GFMCPPro_SetEDisplayText(devnum, szNumericMsg);
            alt_changed = false;
        }
        if (panel_state_changed)
        {
            GFMCPPro_SetIndicators(devnum, ulIndicatorState);
            panel_state_changed = false;
        }
    }
};

enum sim_events
{
    EVENT_SIM_START,
    EVENT_SIM_STOP,
    EVENT_SIM_UNPAUSED,
    EVENT_AIRCRAFT_LOADED,
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
}

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
    MACH_IAS
}


void map_747_events(HANDLE hSimConnect)
{
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressMINS,       "PMDG.747-400.EFIS Captain.Press MINS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseMINS,    "PMDG.747-400.EFIS Captain.Increase MINS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseMINS,    "PMDG.747-400.EFIS Captain.Decrease MINS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainResetMINS,       "PMDG.747-400.EFIS Captain.Reset MINS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressBARO,       "PMDG.747-400.EFIS Captain.Press BARO");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseBARO,    "PMDG.747-400.EFIS Captain.Increase BARO");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseBARO,    "PMDG.747-400.EFIS Captain.Decrease BARO");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressBAROSTD,    "PMDG.747-400.EFIS Captain.Press BARO STD");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressFPV,        "PMDG.747-400.EFIS Captain.Press FPV");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressMTRS,       "PMDG.747-400.EFIS Captain.Press MTRS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNAVL,    "PMDG.747-400.EFIS Captain.Increase NAVL");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNAVL,    "PMDG.747-400.EFIS Captain.Decrease NAVL");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNAVR,    "PMDG.747-400.EFIS Captain.Increase NAVR");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNAVR,    "PMDG.747-400.EFIS Captain.Decrease NAVR");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNDMODE,  "PMDG.747-400.EFIS Captain.Increase NDMODE");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNDMODE,  "PMDG.747-400.EFIS Captain.Decrease NDMODE");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressNDMODECTR,  "PMDG.747-400.EFIS Captain.Press NDMODE CTR");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainIncreaseNDRANGE, "PMDG.747-400.EFIS Captain.Increase NDRANGE");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainDecreaseNDRANGE, "PMDG.747-400.EFIS Captain.Decrease NDRANGE");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressNDRANGETFC, "PMDG.747-400.EFIS Captain.Press NDRANGE TFC");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressWXR,        "PMDG.747-400.EFIS Captain.Press WXR");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressSTA,        "PMDG.747-400.EFIS Captain.Press STA");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressWPT,        "PMDG.747-400.EFIS Captain.Press WPT");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressARPT,       "PMDG.747-400.EFIS Captain.Press ARPT");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressDATA,       "PMDG.747-400.EFIS Captain.Press DATA");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressPOS,        "PMDG.747-400.EFIS Captain.Press POS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainPressTERR,       "PMDG.747-400.EFIS Captain.Press TERR");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainSetMINS,         "PMDG.747-400.EFIS Captain.Set MINS");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainSetBARO,         "PMDG.747-400.EFIS Captain.Set BARO");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNAVL,         "PMDG.747-400.EFIS Captain.Set NAVL");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNAVR,         "PMDG.747-400.EFIS Captain.Set NAVR");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNDMODE,       "PMDG.747-400.EFIS Captain.Set NDMODE");
    MapClientEventToSimEvent(hSimConnect, EFISCaptainSetNDRANGE,      "PMDG.747-400.EFIS Captain.Set NDRANGE"    );
    MapClientEventToSimEvent(hSimConnect, MCPPressATArm,          "PMDG.747-400.MCP.Press AT Arm");
    MapClientEventToSimEvent(hSimConnect, MCPPressN1,             "PMDG.747-400.MCP.Press N1");
    MapClientEventToSimEvent(hSimConnect, MCPPressTHR,            "PMDG.747-400.MCP.Press THR");
    MapClientEventToSimEvent(hSimConnect, MCPPressSPD,            "PMDG.747-400.MCP.Press SPD");
    MapClientEventToSimEvent(hSimConnect, MCPPressCO,             "PMDG.747-400.MCP.Press C/O");
    MapClientEventToSimEvent(hSimConnect, MCPPressFLCH,           "PMDG.747-400.MCP.Press FLCH");
    MapClientEventToSimEvent(hSimConnect, MCPPressVNAV,           "PMDG.747-400.MCP.Press VNAV");
    MapClientEventToSimEvent(hSimConnect, MCPPressLNAV,           "PMDG.747-400.MCP.Press LNAV");
    MapClientEventToSimEvent(hSimConnect, MCPPressVORLOC,         "PMDG.747-400.MCP.Press VORLOC");
    MapClientEventToSimEvent(hSimConnect, MCPPressAPP,            "PMDG.747-400.MCP.Press APP");
    MapClientEventToSimEvent(hSimConnect, MCPPressHDGSEL,         "PMDG.747-400.MCP.Press HDG SEL");
    MapClientEventToSimEvent(hSimConnect, MCPPressHDGHOLD,        "PMDG.747-400.MCP.Press HDG HOLD");
    MapClientEventToSimEvent(hSimConnect, MCPPressALTHOLD,        "PMDG.747-400.MCP.Press ALT HOLD");
    MapClientEventToSimEvent(hSimConnect, MCPPressVS,             "PMDG.747-400.MCP.Press V/S");
    MapClientEventToSimEvent(hSimConnect, MCPPressCMDL,           "PMDG.747-400.MCP.Press CMD L");
    MapClientEventToSimEvent(hSimConnect, MCPPressCMDC,           "PMDG.747-400.MCP.Press CMD C");
    MapClientEventToSimEvent(hSimConnect, MCPPressCMDR,           "PMDG.747-400.MCP.Press CMD R");
    MapClientEventToSimEvent(hSimConnect, MCPPressFDL,            "PMDG.747-400.MCP.Press FD L");
    MapClientEventToSimEvent(hSimConnect, MCPPressFDR,            "PMDG.747-400.MCP.Press FD R");
    MapClientEventToSimEvent(hSimConnect, MCPPressAPDiseng,       "PMDG.747-400.MCP.Press AP Diseng");
    MapClientEventToSimEvent(hSimConnect, MCPIncreaseBankLimiter, "PMDG.747-400.MCP.Increase Bank Limiter");
    MapClientEventToSimEvent(hSimConnect, MCPDecreaseBankLimiter, "PMDG.747-400.MCP.Decrease Bank Limiter");
    MapClientEventToSimEvent(hSimConnect, MCPIncreaseAlt,         "PMDG.747-400.MCP.Increase Alt");
    MapClientEventToSimEvent(hSimConnect, MCPDecreaseAlt,         "PMDG.747-400.MCP.Decrease Alt");
    MapClientEventToSimEvent(hSimConnect, MCPIncreaseSpd,         "PMDG.747-400.MCP.Increase Spd");
    MapClientEventToSimEvent(hSimConnect, MCPDecreaseSpd,         "PMDG.747-400.MCP.Decrease Spd");
    MapClientEventToSimEvent(hSimConnect, MCPIncreaseHdg,         "PMDG.747-400.MCP.Increase Hdg");
    MapClientEventToSimEvent(hSimConnect, MCPDecreaseHdg,         "PMDG.747-400.MCP.Decrease Hdg");
    MapClientEventToSimEvent(hSimConnect, MCPIncreaseVS,          "PMDG.747-400.MCP.Increase V/S");
    MapClientEventToSimEvent(hSimConnect, MCPDecreaseVS,          "PMDG.747-400.MCP.Decrease V/S");
    MapClientEventToSimEvent(hSimConnect, MCPSetTOGA,             "PMDG.747-400.MCP.Set TO/GA");
    MapClientEventToSimEvent(hSimConnect, MCPResetTOGA,           "PMDG.747-400.MCP.Reset TO/GA");
    MapClientEventToSimEvent(hSimConnect, MCPPressSpdIntv,        "PMDG.747-400.MCP.Press Spd Intv");
    MapClientEventToSimEvent(hSimConnect, MCPPressAltIntv,        "PMDG.747-400.MCP.Press Alt Intv");
    MapClientEventToSimEvent(hSimConnect, MCPPressCMD,            "PMDG.747-400.MCP.Press CMD");
    MapClientEventToSimEvent(hSimConnect, MCPPressCWS,            "PMDG.747-400.MCP.Press CWS");
    MapClientEventToSimEvent(hSimConnect, MCPPressFD,             "PMDG.747-400.MCP.Press FD");
    MapClientEventToSimEvent(hSimConnect, MCPPressTOGA,           "PMDG.747-400.MCP.Press TO/GA");
    MapClientEventToSimEvent(hSimConnect, MCPPressBankLimiter,    "PMDG.747-400.MCP.Press Bank Limiter");
    MapClientEventToSimEvent(hSimConnect, MCPSetAltitude,         "PMDG.747-400.MCP.Set Altitude");
    MapClientEventToSimEvent(hSimConnect, MCPSetSpeed,            "PMDG.747-400.MCP.Set Speed");
    MapClientEventToSimEvent(hSimConnect, MCPSetHeading,          "PMDG.747-400.MCP.Set Heading");
    MapClientEventToSimEvent(hSimConnect, MCPSetVS,               "PMDG.747-400.MCP.Set V/S");
    MapClientEventToSimEvent(hSimConnect, MCPSetFDL,              "PMDG.747-400.MCP.Set F/D L");
    MapClientEventToSimEvent(hSimConnect, MCPSetFDR,              "PMDG.747-400.MCP.Set F/D R");
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
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 152, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPVS);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_INSTRUMENT_DATA, 160, SIMCONNECT_CLIENTDATATYPE_INT32,0,MCPPanelState);
    SimConnect_AddToClientDataDefinition(hSimConnect, DEFINITION_AP_DATA, 16, SIMCONNECT_CLIENTDATATYPE_INT32,0, MACH_IAS);
    SimConnect_MapClientDataNameToID(hSimConnect,"SDD_AIRCRAFTAUTOPILOTDATA", CDID_AIRCRAFTAUTOPILOTDATA);
    SimConnect_MapClientDataNameToID(hSimConnect,"SDD_AIRCRAFTINSTRUMENTSDATA", CDID_AIRCRAFTINSTRUMENTSDATA);
    SimConnect_RequestClientData(hSimConnect,  CDID_AIRCRAFTAUTOPILOTDATA, REQUEST_DATA, DEFINITION_AP_DATA, SIMCONNECT_CLIENT_DATA_PERIOD_VISUAL_FRAME, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED, 0, 0, 0);
    SimConnect_RequestClientData(hSimConnect,  CDID_AIRCRAFTINSTRUMENTSDATA, REQUEST_DATA, DEFINITION_INSTRUMENT_DATA, SIMCONNECT_CLIENT_DATA_PERIOD_VISUAL_FRAME, SIMCONNECT_CLIENT_DATA_REQUEST_FLAG_TAGGED, 0, 0, 0);
}







namespace
{

int quit = 0;
bool second_buf_filled = false;
unsigned char buf[2048][2] = {0};
int cur_buf = 0;
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

std::map<int, int> int_storage;
std::map<int, double> double_storage;
std::map<int, int> offset_storage;
void CALLBACK MyDispatchProcPDR(SIMCONNECT_RECV* pData, DWORD cbData, void *ctxt)
{
    HRESULT hr;

    mcppro_display_747 * mcp = reinterpret_cast<mcppro_display_747 *>(ctxt);


    switch(pData->dwID)
    {
        case SIMCONNECT_RECV_ID_EVENT:
        {
            SIMCONNECT_RECV_EVENT *evt = (SIMCONNECT_RECV_EVENT*)pData;
			SIMCONNECT_RECV_EVENT_FILENAME * ev_f = (SIMCONNECT_RECV_EVENT_FILENAME*)evt;
			switch(evt->uEventID)
            {
                case EVENT_SIM_START:
                    std::cout << "\nSIMSTART" << std::endl;
				case EVENT_SIM_UNPAUSED:
                    std::cout << "\nUnpaused" << std::endl;

                    // Make the call for data every second, but only when it changes and
                    // only that data that has changed
//					piface->set_data_request();
                    break;
				case EVENT_AIRCRAFT_LOADED:
                    std::cout << "\nAircraftLoaded" << std::endl;
					break;
                default:
				   std::cout << "EVENT " << evt->uEventID << std::endl;
                   break;
            }
            break;

        }

		case SIMCONNECT_RECV_ID_CLIENT_DATA:
		{
			SIMCONNECT_RECV_CLIENT_DATA *pObjData = (SIMCONNECT_RECV_CLIENT_DATA*)pData;
            int	count	= 0;
			BYTE * pD = reinterpret_cast<BYTE*>(&pObjData->dwData);
			while (count < (int) pObjData->dwDefineCount)
			{
				int id = *reinterpret_cast<int*>(pD);
				pD += sizeof(int);
				size_t tsize = sizeof(int);
				if (id = MCPMach)
				{
					double val = *reinterpret_cast<double*>(pD);
					tsize = sizeof(double);
                    mcp->set_mach(val);
				}
                else
				{
					int val = *reinterpret_cast<int*>(pD);
                    switch (id)
                    {
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
                            mcp->set_vs(val);
                            break;
                        case MCPPanelState:
                            mcp->set_panel_state(val);
                            break;
                        case MACH_IAS:
                            mcp->set_mach_ias(val);
                            break;
                        default:
                            break;
                    }
				}
				else
				pD += tsize;
				++count;
			}
			break;
		}

		case SIMCONNECT_RECV_ID_EVENT_FILENAME:
		{
			SIMCONNECT_RECV_EVENT_FILENAME * ev_f = (SIMCONNECT_RECV_EVENT_FILENAME*)pData;
			switch(ev_f->uEventID)
			{
				case EVENT_AIRCRAFT_LOADED:
					
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
                    std::cout << "\nSTART" << std::endl;
                    // Make the call for data every second, but only when it changes and
                    // only that data that has changed

                    break;
				case EVENT_AIRCRAFT_LOADED:

					break;
                default:
                   break;
            }
            break;
		}
		

        case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
        {
            SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;
            
            switch(pObjData->dwRequestID)
            {
                case REQUEST_DATA:
				case REQUEST_VISIBLE:
  				case REQUEST_LED:
				{
					int	count	= 0;
					BYTE * pD = reinterpret_cast<BYTE*>(&pObjData->dwData);
					while (count < (int) pObjData->dwDefineCount)
					{
						int id = *reinterpret_cast<int*>(pD);
						pD += sizeof(int);
					}
                    break;
                }

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
            printf("Unknown dwID: %d\n",pData->dwID);
            break;
    }

}


static BOOL InputCallback( GFDEV_HW_TYPE HWType, unsigned short wParam, LPGFINPUTREPORTTYPE lParam )
{
	BOOL bInputProcessed = FALSE;

	if( NULL != lParam ) // sanity check...
		{
				if( GFDEVHWTYPE_GFMCPPRO == HWType )
				{
                    LPGFMCPPROINPUTREPORT r = reinterpret_cast<LPGFMCPPROINPUTREPORT>(lParam);
					std::cout << "M" << std::setw(8) << r->nADialVal << std::setw(8)<< r->nBDialVal << std::setw(8)<< r->nCDialVal
						<< std::setw(8) << r->nDDialVal << std::setw(8) << r->nEDialVal << std::setw(8) << r->nFDialVal << std::hex<< std::setw(10)<< r->ulSwitchState << std::endl;
                    bInputProcessed = TRUE;
				}
				else if( GFDEVHWTYPE_GFEFIS == HWType )
				{
					LPGFEFISINPUTREPORT r = reinterpret_cast<LPGFEFISINPUTREPORT>(lParam);
					std::cout << "E"<<std::setw(8) << r->nADialVal << std::setw(8)<< r->nBDialVal << std::setw(8)<< std::hex<< int(r->bSelectors)
						<< std::hex<< std::setw(10)<< r->wSwitchState << std::endl;
                    bInputProcessed = TRUE;
				}

		}

	return bInputProcessed;
}
//int _tmain(int argc, _TCHAR* argv[])

int main(int argc, char* argv[])
{
	SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);
	HRESULT hr = NULL;
	GFDEVRESULT result = GFDev_Init( ::GetModuleHandle(NULL) );

	try
	{
		HANDLE hSimConnect;
		std::string config;
        mcppro_display_747 mcppro;
        int devnum = GFMCPPro_GetNumDevices( ) - 1;


		if (SUCCEEDED(SimConnect_Open(&hSimConnect, "PMDG747 GFMcpPro", NULL, 0, 0, 0)))
		{
			printf("\nConnected to Flight Simulator!");
            map_747_vars(hSimConnect);
            map_747_events(hSimConnect);
			SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_START, "SimStart");
			SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_STOP, "SimStop");
			SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_UNPAUSED, "Unpaused");
			SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_AIRCRAFT_LOADED, "AircraftLoaded");

			while( 0 == quit )
			{
				SimConnect_CallDispatch(hSimConnect, MyDispatchProcPDR, mcppro);
                if (devnum >= 0)
                {
                    mcp->update_mcppro(devnum);
                }
				Sleep(1);
			}
			hr = SimConnect_Close(hSimConnect);
		}
		else
		{
			printf("Simconnect connection failed\n");
			getchar();
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	GFDev_Terminate();
	return 0;
}



