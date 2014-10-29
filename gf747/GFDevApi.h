// ***************************************************************************
// *
// * Copyright © 1999-2011 GoFlight, Inc.  All rights reserved.
// *
// * Module Name:
// *   GFDevApi.h
// *
// * 16 May 2012		(GF SDK 2.21)
// *
// * No changes to source code or SDK functionality
// * Added missing TQ6 functions to GFDev.def
// * GFTQ6_GetNumDevices_stdcall
// * GFTQ6_GetNumDevices
// *
// * Abstract:
// *   Header for GoFlight device control library.  Support is included for the
// *   following GoFlight hardware:
// * 
// *      GF-45 Avionics Simulation Unit
// *      GF-45PM Display Panel Module
// *      GF-46 Multi-Mode Display Module
// *      GF-P8 Pushbutton/LED Module
// *      GF-RP48 Rotary/Pushbutton/LED Module
// *      GF-WCP Pro Warning Control Panel
// *      GF-T8 Toggle Switch/LED Module
// *      GF-LGT Landing Gear/Trim control module
// *      GF-LGT II Landing Gear/Trim control module
// *      GF-166 Versatile Radio Panel
// *      GF-MCP Advanced Autopilot Module
// *      GF-MCP Pro Mode Control Panel Module
// *      GF-EFIS Control Panel Module
// *      GF-TQ6 Throttle System
// *      GF-ATC Headset Comms Panel
// *      GF-SECM Single Engine Aircraft Control Module
// *      GF-MESM Multi Engine Start Module
// *      GF-FMC Flight Management Computer Module
// *      GF-TPM Throttle/Prop/Mixture Control Module
// *      GF-DIO Digital Input/Output Module
// *
// * Environment:
// *   Microsoft Windows -- User mode
// *
// ***************************************************************************

#ifndef _GFDEVAPI_H_
#define _GFDEVAPI_H_

// === Data types =============================================================
//
// For use in Mac environments, we define the standard 'loud' datatypes. 
#ifndef _WINDOWS
    typedef unsigned char   BYTE;
    typedef void *          HINSTANCE;
    typedef char            BOOL;
    typedef long            DWORD;
    #define MAX_PATH        512
#define _cdecl
#endif

#pragma pack( push, gfdevapi_h ) // Save current pack value

#pragma pack(1)
//
// Tokens for GoFlight device types
//
typedef enum tagGFDevHWType
{
    GFDEVHWTYPE_NONE    = 0, // Sentinel value (No Hardware.)
    GFDEVHWTYPE_GF45    = 1, // GF-45 Avionics Simulation Unit
    GFDEVHWTYPE_GFP8,        // GF-P8 8 pushbutton/LED control module
    GFDEVHWTYPE_GFT8,        // GF-T8 8 toggle switch/LED control module
    GFDEVHWTYPE_GFLGT,       // GF-LGT Landing Gear/Trim control module
    GFDEVHWTYPE_GF166,       // GF-166 Versatile Radio Panel
    GFDEVHWTYPE_GFMCP,       // GF-MCP Advanced autopilot module
    GFDEVHWTYPE_GFRP48,      // GF-RP48 rotary/pushbutton/LED control module
    GFDEVHWTYPE_GF46,        // GF-46 Multifunction Module
    GFDEVHWTYPE_GFTQ6,       // GF-TQ6 Throttle System
    GFDEVHWTYPE_GFATC,       // GF-ATC Headset Comms Panel
    GFDEVHWTYPE_GFLGT2,      // GF-LGT II Landing Gear/Trim control module
    GFDEVHWTYPE_GFMCPPRO,    // GF-MCP Pro Mode Control Panel module
    GFDEVHWTYPE_GFEFIS,      // GF-EFIS Control Panel Module
    GFDEVHWTYPE_GFSECM,      // GF-SECM Single Engine Aircraft Control Module
    GFDEVHWTYPE_GFFMC,       // GF-FMC Flight Management Computer Module
    GFDEVHWTYPE_GFTPM,       // GF-TPM Throttle/Prop/Mixture Control Module
    GFDEVHWTYPE_GFMESM,      // GF-MESM Multi Engine Start Module    
    GFDEVHWTYPE_GFWCP,       // GF-WCP Pro Warning Control Panel
    GFDEVHWTYPE_GFDIO,       // GF-DIO Digital Input/Output module
    GFDEVHWTYPE_NEXTFREE,    // Not a device, merely a placeholder
    GFDEVHWTYPE_NUM_DEVICES = (GFDEVHWTYPE_NEXTFREE-1)    
} GFDEV_HW_TYPE;

//
// Definitions for encoder types
//
typedef unsigned char ENCODERTYPE, *LPENCODERTYPE; // Data type is equivalent to unsigned char
//
// The following "types" of encoders are defined for use with GoFlight Remote Mount Kit boards.
// These are applicable to those products with support for the Get/SetEncoderTypes() API functions.
// Typical manufacturers are listed for each type of encoder, though just a guideline as most
// manufacturers build devices which could fall into several of the type categories.
// 
// Type 0 ====> 90 degree quadrature, 1/4 cycle per detent output (Grayhill optical, Bourns)
// Type 1 ====> 90 degree quadrature, full cycle per detent, '11' settle output (Panasonic, Bourns)
// Type 2 ====> 90 degree quadrature, full cycle per detent, '10'/'01' settle output (ALPS)
// Type 3 ====> Not yet defined


//
// Device connect/disconnect tokens for use with the Connect callbacks
//
typedef enum tagEventToken
{
    EVENT_DEVICE_CONNECT     = 1,
    EVENT_DEVICE_DISCONNECT

} GFDEV_EVENT_TOKEN;

//
// GFDEVRESULT values are retured from the GFDEV API functions.
//
typedef enum tagGFDevResult
{
    GFDEV_OK = 0,
    GFDEV_ERR_INVALID_DEVICE_INDEX,
    GFDEV_ERR_INSUFFICIENT_MEMORY,
    GFDEV_ERR_DEVICE_NOT_PRESENT,
    GFDEV_ERR_COMMAND_QUEUE,
    GFDEV_ERR_HID_GET_FEATURE,
    GFDEV_ERR_HID_SET_FEATURE,
    GFDEV_ERR_SET_DATA_NOT_VERIFIED,
    GFDEV_ERR_NO_DATA_AVAIL,
    GFDEV_ERR_UNSUPPORTED_OS,
    GFDEV_ERR_DLL_NOT_FOUND,
    GFDEV_ERR_INVALID_ARG,
    GFDEV_ERR_API_NOT_INIT,
    GFDEV_ERR_FUNC_NOT_SUPPORTED

} GFDEVRESULT;

//
// GFDEVICEINFO and GFDEVICEINFO2 structs contain internal information about
// GoFlight devices.
//
// GFDEVICEINFO returns Location ID and Device Version info.  Supported for
// compatibility with older software.  For new applications we recommend
// working with GFDEVICEINFO2 and counterpart function GFDev_GetDeviceInfo2().
//
typedef struct tagGFDeviceInfo
{
    char szLocationID[MAX_PATH];
    unsigned short wDeviceVersion;

} GFDEVICEINFO, *LPGFDEVICEINFO;

//
// GFDEVICEINFO2 returns a superset of data provided by GFDEVICEINFO.
//
// GFDEVICEINFO2 supports a serial number string up to 64 chars in length,
// and is assumed to be zero-terminated
// 
#define MAX_SERNUM   64+1  

typedef struct tagGFDeviceInfo2
{
    unsigned short bLength;
    char szLocationID[MAX_PATH];
    unsigned short wDeviceVersion;
    char szSerialNum[MAX_SERNUM];

} GFDEVICEINFO2, *LPGFDEVICEINFO2;

// Definitions for input reports /////////////////////////////////////////////
//
typedef void* LPGFINPUTREPORTTYPE; // Pointer to some sort of GF<n>INPUTREPORT, but the actual type
                                   // is not known. See GFDev_RegisterInputCallback() function prototype
//
// GF-45 input report
//
typedef struct tagGF45InputReport
{
    short nLDialVal;
    short nRDialVal;

} GF45INPUTREPORT, *LPGF45INPUTREPORT;
//
// GF-46 input report
//

typedef struct tagGF46InputReport
{
    short nLgDialVal;
    short nSmDialVal;
    BYTE  bSwitchState; // Function pushbutton state

} GF46INPUTREPORT, *LPGF46INPUTREPORT;

//
// GF-RP48 input report
//
typedef struct tagGFRP48InputReport
{
    short nADialVal;            // encoder "A"
    short nBDialVal;            // encoder "B"
    short nCDialVal;            // encoder "C"
    short nDDialVal;            // encoder "D"
    BYTE  bSwitchState; // Buttons 1-8

} GFRP48INPUTREPORT, *LPGFRP48INPUTREPORT;

//
// GF-LGT input report (returned from InputCallback())
//
typedef struct tagGFLGTInputReport
{
    signed short nWheelVal;
    BYTE         bSwitchState;

} GFLGTINPUTREPORT, *LPGFLGTINPUTREPORT;

//
// GF-LGT II input report (returned from InputCallback())
//
typedef struct tagGFLGT2InputReport
{
    signed short nAWheelVal;     // Elevator trim wheel
    signed short nBWheelVal;     // Aileron trim wheel
    signed short nCWheelVal;     // Rudder trim knob
    BYTE         bSwitchState;

} GFLGT2INPUTREPORT, *LPGFLGT2INPUTREPORT;

//
// GF-166 input report
//
typedef struct tagGF166InputReport
{
    short nLgDialVal;           // Coarse encoder
    short nSmDialVal;           // Fine encoder
    BYTE  bSwitchState; // Function pushbuttons

} GF166INPUTREPORT, *LPGF166INPUTREPORT;

//
// GF-MCP input report
//
typedef struct tagGFMCPInputReport
{
    short nADialVal;        // encoder "A" (leftmost knob)
    short nBDialVal;        // encoder "B"
    short nCDialVal;        // encoder "C"
    short nDDialVal;        // encoder "D"
    short nEDialVal;        // encoder "E" (rightmost knob)
    BYTE  bSwitchState1_8;  // Switches 1-8
    BYTE  bSwitchState9_16; // Switches 9-16

} GFMCPINPUTREPORT, *LPGFMCPINPUTREPORT;

//
// GF-TQ6 input report
//
typedef struct tagGFTQ6InputReport
{
    unsigned char bLeverAVal;       // Control Lever A (leftmost slot)
    unsigned char bLeverBVal;       // Control Lever B
    unsigned char bLeverCVal;       // Control Lever C
    unsigned char bLeverDVal;       // Control Lever D
    unsigned char bLeverEVal;       // Control Lever E
    unsigned char bLeverFVal;       // Control Lever F (rightmost slot)
    unsigned char bSwitchState;     // Control lever switches

} GFTQ6INPUTREPORT, *LPGFTQ6INPUTREPORT;

//
// GF-ATC input report
//
typedef struct tagGFATCInputReport
{
    short nDialVal;      // Selection knob
    BYTE  bSwitchState;  // Selection knob push switch

} GFATCINPUTREPORT, *LPGFATCINPUTREPORT;

//
// GF-MCP Pro input report
//
typedef struct tagGFMCPProInputReport
{
    short nADialVal;                // encoder "A" (leftmost knob)
    short nBDialVal;                // encoder "B"
    short nCDialVal;                // encoder "C"
    short nDDialVal;                // encoder "D"
    short nEDialVal;                // encoder "E"
    short nFDialVal;                // encoder "F" (rightmost knob)
    unsigned long ulSwitchState;    // panel switches

} GFMCPPROINPUTREPORT, *LPGFMCPPROINPUTREPORT;

//
// GF-EFIS input report
//
typedef struct tagGFEFISInputReport
{
    short nADialVal;                // encoder "A" (top left knob)
    short nBDialVal;                // encoder "B" (top right knob)
    unsigned short wSwitchState;    // State of toggles and push buttons 
    unsigned char bSelectors;       // State of range and mode selectors

} GFEFISINPUTREPORT, *LPGFEFISINPUTREPORT;

//
// GF-SECM input report
//
typedef struct tagGFSECMInputReport
{
    unsigned char bSelector;        // Magneto Switch selector states
    unsigned char bGearState;       // Landing gear switch (up/down)
    unsigned char bWingFlapsState;  // Wing flaps switch (up/center/down)
    unsigned char bCowlFlapsState;  // Cowl flaps switch (close/open)
    unsigned short wSwitchState;    // Electrical systems & lights toggles 

} GFSECMINPUTREPORT, *LPGFSECMINPUTREPORT;

//
// GF-MESM input report
//
typedef struct tagGFMESMInputReport
{
    unsigned char bSelector1;       // Left selector knob state
    unsigned char bSelector2;       // Center 3-way toggle state
    unsigned char bSelector3;       // Right selector knob state

} GFMESMINPUTREPORT, *LPGFMESMINPUTREPORT;

//
// GF-DIO input report
//
typedef struct tagGFDIOInputReport
{
    unsigned char bSwitchBank1;     // Switch Bank 1
    unsigned char bSwitchBank2;     // Switch Bank 2
    unsigned char bSwitchBank3;     // Switch Bank 3
    unsigned char bSwitchBank4;     // Switch Bank 4
    unsigned char bSwitchBank5;     // Switch Bank 5
    unsigned char bSwitchBank6;     // Switch Bank 6
    unsigned char bSwitchBank7;     // Switch Bank 7

} GFDIOINPUTREPORT, *LPGFDIOINPUTREPORT;


//
// Bitmasks for access to GF-LGT switch and LED states
//
// GF-LGT switch states
#define GFLGT_SW_LEVER_UP     0x01 // b0 : '1' = lever up
#define GFLGT_SW_SWITCH_DN    0x02 // b1 : '1' = mom. switch down
#define GFLGT_SW_SWITCH_UP    0x04 // b2 : '1' = mom. switch up

//
// GF-LGT gear status LED states
//                                    
#define GFLGT_LED_NOSE_GREEN   0x80 // b7 : '1' Nose gear green
#define GFLGT_LED_NOSE_RED     0x40 // b6 : '1' Nose gear red
#define GFLGT_LED_NOSE_AMBER   (GFLGT_LED_NOSE_RED | GFLGT_LED_NOSE_GREEN)

#define GFLGT_LED_RIGHT_GREEN  0x20 // b5 : '1' Left gear green
#define GFLGT_LED_RIGHT_RED    0x10 // b4 : '1' Left gear red
#define GFLGT_LED_RIGHT_AMBER  (GFLGT_LED_RIGHT_RED | GFLGT_LED_RIGHT_GREEN)

#define GFLGT_LED_LEFT_GREEN   0x08 // b3 : '1' Right gear green
#define GFLGT_LED_LEFT_RED     0x04 // b2 : '1' Right gear red
#define GFLGT_LED_LEFT_AMBER   (GFLGT_LED_LEFT_RED | GFLGT_LED_LEFT_GREEN)

                                    // b1 : ignored     
                                    // b0 : ignored     

#define GFLGT_LED_ALL_OFF      0x00 // All three gear status LEDs off
#define GFLGT_LED_ALL_RED      0x54 // All three gear status LEDs red
#define GFLGT_LED_ALL_GREEN    0xA8 // All three gear status LEDs off
#define GFLGT_LED_ALL_AMBER    (GFLGT_LED_RIGHT_AMBER | GFLGT_LED_LEFT_AMBER | GFLGT_LED_NOSE_AMBER)

//
// Bitmasks for access to GF-LGT II switch and LED states
//
#define GFLGT2_SW_SW1_UP       0x01 // b0 : '1' Switch 1 (gear lever) up
#define GFLGT2_SW_SW1_DN       0x02 // b1 : '1' Switch 1 (gear lever) down
#define GFLGT2_SW_SW2_RT       0x04 // b0 : '1' Switch 2 (fuel) switch to right
#define GFLGT2_SW_SW2_LT       0x08 // b1 : '1' Switch 2 (fuel) switch to left
#define GFLGT2_SW_SW3_UP       0x10 // b2 : '1' Switch 3 (flaps) switch up
#define GFLGT2_SW_SW3_DN       0x20 // b3 : '1' Switch 3 (flaps) switch down

//
// GF-LGT II gear status LED states
//
                                     // b0 : ignored
                                     // b1 : ignored
#define GFLGT2_LED_RIGHT_GREEN  0x04 // b2 : '1' Right gear green
#define GFLGT2_LED_RIGHT_RED    0x08 // b3 : '1' Right gear red
#define GFLGT2_LED_RIGHT_AMBER  (GFLGT2_LED_RIGHT_RED | GFLGT2_LED_RIGHT_GREEN)

#define GFLGT2_LED_NOSE_GREEN   0x10 // b4 : '1' Nose gear green
#define GFLGT2_LED_NOSE_RED     0x20 // b5 : '1' Nose gear red
#define GFLGT2_LED_NOSE_AMBER   (GFLGT2_LED_NOSE_RED | GFLGT2_LED_NOSE_GREEN)

#define GFLGT2_LED_LEFT_GREEN   0x40 // b6 : '1' Left gear green
#define GFLGT2_LED_LEFT_RED     0x80 // b7 : '1' Left gear red
#define GFLGT2_LED_LEFT_AMBER   (GFLGT2_LED_LEFT_RED | GFLGT2_LED_LEFT_GREEN)

#define GFLGT2_LED_ALL_OFF      0x00 // All three gear status LEDs off
#define GFLGT2_LED_ALL_GREEN    0x54 // All three gear status LEDs green
#define GFLGT2_LED_ALL_RED      0xA8 // All three gear status LEDs red
#define GFLGT2_LED_ALL_AMBER    (GFLGT2_LED_RIGHT_AMBER | GFLGT2_LED_LEFT_AMBER | GFLGT2_LED_NOSE_AMBER)

//
// Bitmask for access to GF-46 function button
//
// GF-46 function button -- '1' = button pressed, '0' = button released.
#define GF46_BTN_FUNC         0x01 // Function button (LSB)
                                   // All other bits will read a constant '0'
//
// Bitmasks for access to GF-166 function buttons and LED states
//
// GF-166 function buttons -- '1' = button pressed, '0' = button released.
#define GF166_BTN_FUNC_C       0x01 // Center function button
#define GF166_BTN_FUNC_L       0x02 // Left function button
#define GF166_BTN_FUNC_R       0x04 // Right function button

// GF-166 LED indicators -- '1' LED is on, '0' = LED is off
#define GF166_LED_FUNC_C       0x01 // Center function indicator (NOTE: on the -166A,
                                    //   there is no physical LED here.  But this bit
                                    //   position is reserved in case one is added
                                    //   on future revs.
#define GF166_LED_FUNC_L       0x02 // Left function indicator
#define GF166_LED_FUNC_R       0x04 // Center function indicator (NOTE: on the -166A,
//
// Bitmasks for access to GF-MCP switch and LED states
//
#define GFMCP_SW_1             0x01 // Switch 1 rectangular pushbutton, leftmost on panel
#define GFMCP_LED_1            0x01 // LED 1 integrated with Switch 1
#define GFMCP_SW_2             0x02 // Switch 2 rectangular pushbutton, right of Switch 1
#define GFMCP_LED_2            0x02 // LED 2 integrated with Switch 2
#define GFMCP_SW_3             0x04 // Switch 3 rectangular pushbutton, right of Switch 2
#define GFMCP_LED_3            0x04 // LED 3 integrated with Switch 3
#define GFMCP_SW_4             0x08 // Switch 4 round pushbutton, below digit 1 of display "C"
#define GFMCP_LED_4            0x08 // LED 4 not present on current rev of MCP (future)
#define GFMCP_SW_5             0x10 // Switch 5 rectangular pushbutton, right of Dial D (wheel control)
#define GFMCP_LED_5            0x10 // LED 5 integrated with Switch 5
#define GFMCP_SW_6             0x20 // Switch 6 rectangular pushbutton, right of Switch 5
#define GFMCP_LED_6            0x20 // LED 6 integrated with Switch 6
#define GFMCP_SW_7             0x40 // Switch 7 rectangular pushbutton, right of Switch 6
#define GFMCP_LED_7            0x40 // LED 7 integrated with Switch 7
#define GFMCP_SW_8             0x80 // Switch 8 rectangular pushbutton, lower left panel
#define GFMCP_LED_8            0x80 // LED 8 integrated with Switch 8

#define GFMCP_SW_9             0x01 // Switch 9 toggle switch connected to "disengage" bar
#define GFMCP_SW_10            0x02 // Switch 10 is HDG knob "push" switch; added in GF-MCP

//
// Bitmasks for access to GF-MCP Pro switch and LED states
//
#define GFMCPPRO_SW_1      0x00000001 // Switch 1 rectangular pushbutton, VNAV
#define GFMCPPRO_LED_1     0x00000001 // LED 1 integrated with Switch 1
#define GFMCPPRO_SW_2      0x00000002 // Switch 2 rectangular pushbutton, LNAV
#define GFMCPPRO_LED_2     0x00000002 // LED 2 integrated with Switch 2
#define GFMCPPRO_SW_3      0x00000004 // Switch 3 rectangular pushbutton, CMD A
#define GFMCPPRO_LED_3     0x00000004 // LED 3 integrated with Switch 3
#define GFMCPPRO_SW_4      0x00000008 // Switch 4 rectangular pushbutton, CMD B
#define GFMCPPRO_LED_4     0x00000008 // LED 4 integrated with Switch 4
#define GFMCPPRO_SW_5      0x00000010 // Switch 5 toggle switch, A/T ARM
#define GFMCPPRO_LED_5     0x00000010 // LED 5 mounted above Switch 5 on panel
#define GFMCPPRO_SW_6      0x00000020 // Switch 6 round pushbutton, C/O
#define GFMCPPRO_LED_6     0x00000020 // LED 6 not installed
#define GFMCPPRO_SW_7      0x00000040 // Switch 7 round pushbutton, SPD INTV
#define GFMCPPRO_LED_7     0x00000040 // LED 7 not installed
#define GFMCPPRO_SW_8      0x00000080 // Switch 8 rectangular pushbutton, VOR LOC
#define GFMCPPRO_LED_8     0x00000080 // LED 8 integrated with Switch 8
#define GFMCPPRO_SW_9      0x00000100 // Switch 9 round pushbutton, ALT INTV
#define GFMCPPRO_LED_9     0x00000100 // LED 9 not installed
#define GFMCPPRO_SW_10     0x00000200 // Switch 10 rectangular pushbutton, CWS A
#define GFMCPPRO_LED_10    0x00000200 // LED 10 integrated with Switch 10
#define GFMCPPRO_SW_11     0x00000400 // Switch 11 rectangular pushbutton, CWS B
#define GFMCPPRO_LED_11    0x00000400 // LED 11 integrated with Switch 11
#define GFMCPPRO_SW_12     0x00000800 // Switch 12 SPD knob "push" switch
#define GFMCPPRO_LED_12    0x00000800 // LED 12 not installed
#define GFMCPPRO_SW_13     0x00001000 // Switch 13 HDG knob "push" switch
#define GFMCPPRO_LED_13    0x00001000 // LED 13 not installed
#define GFMCPPRO_SW_14     0x00002000 // Switch 14 ALT knob "push" switch
#define GFMCPPRO_LED_14    0x00002000 // LED 14 not installed
#define GFMCPPRO_SW_15     0x00004000 // Switch 15 toggle switch, F/D (left)
#define GFMCPPRO_LED_15    0x00004000 // LED 15 mounted above Switch 15 on panel
#define GFMCPPRO_SW_16     0x00008000 // Switch 16 rectangular pushbutton, N1
#define GFMCPPRO_LED_16    0x00008000 // LED 16 integrated with Switch 16
#define GFMCPPRO_SW_17     0x00010000 // Switch 17 rectangular pushbutton, SPEED
#define GFMCPPRO_LED_17    0x00010000 // LED 17 integrated with Switch 17
#define GFMCPPRO_SW_18     0x00020000 // Switch 18 rectangular pushbutton, LVL CHG
#define GFMCPPRO_LED_18    0x00020000 // LED 18 integrated with Switch 18
#define GFMCPPRO_SW_19     0x00040000 // Switch 19 rectangular pushbutton, HDG SEL
#define GFMCPPRO_LED_19    0x00040000 // LED 19 integrated with Switch 19
#define GFMCPPRO_SW_20     0x00080000 // Switch 20 rectangular pushbutton, APP
#define GFMCPPRO_LED_20    0x00080000 // LED 20 integrated with Switch 20
#define GFMCPPRO_SW_21     0x00100000 // Switch 21 rectangular pushbutton, ALT HLD
#define GFMCPPRO_LED_21    0x00100000 // LED 21 integrated with Switch 21
#define GFMCPPRO_SW_22     0x00200000 // Switch 22 rectangular pushbutton, V/S
#define GFMCPPRO_LED_22    0x00200000 // LED 22 integrated with Switch 22
#define GFMCPPRO_SW_23     0x00400000 // Switch 23 is toggle switch, DISENGAGE bar
#define GFMCPPRO_LED_23    0x00400000 // LED 23 is not installed
#define GFMCPPRO_SW_24     0x00800000 // Switch 24 is toggle switch, F/D (right)
#define GFMCPPRO_LED_24    0x00800000 // LED 24 is not installed
#define GFMCPPRO_SW_25     0x01000000 // Switch 25 is not installed
#define GFMCPPRO_LED_25    0x01000000 // LED 25 is not installed
#define GFMCPPRO_SW_26     0x02000000 // Switch 26 is not installed
#define GFMCPPRO_LED_26    0x02000000 // LED 26 is not installed
#define GFMCPPRO_SW_27     0x04000000 // Switch 27 is not installed
#define GFMCPPRO_LED_27    0x04000000 // LED 27 is not installed
#define GFMCPPRO_SW_28     0x08000000 // Switch 28 is not installed
#define GFMCPPRO_LED_28    0x08000000 // LED 28 is not installed
#define GFMCPPRO_SW_29     0x10000000 // Switch 29 is not installed
#define GFMCPPRO_LED_29    0x10000000 // LED 29 is not installed
#define GFMCPPRO_SW_30     0x20000000 // Switch 30 is not installed
#define GFMCPPRO_LED_30    0x20000000 // LED 30 is not installed
#define GFMCPPRO_SW_31     0x40000000 // Switch 31 is not installed
#define GFMCPPRO_LED_31    0x40000000 // LED 31 is not installed
#define GFMCPPRO_SW_32     0x80000000 // Switch 32 not installed
#define GFMCPPRO_LED_32    0x80000000 // LED 32 controls backlighting
#define GFMCPPRO_BACKLIGHT 0x80000000 // Specific define for backlight bitmask

//
// Bitmasks for access to GF-EFIS switch states
//
#define GFEFIS_SW_1            0x0001 // TERR pushbutton
#define GFEFIS_SW_2            0x0002 // POS pushbutton
#define GFEFIS_SW_3            0x0004 // DATA pushbutton
#define GFEFIS_SW_4            0x0008 // ARPT pushbutton
#define GFEFIS_SW_5            0x0010 // WPT pushbutton
#define GFEFIS_SW_6            0x0020 // STA pushbutton
#define GFEFIS_SW_7            0x0040 // WXR pushbutton
#define GFEFIS_SW_8            0x0080 // Not connected
#define GFEFIS_SW_9            0x0100 // Toggle is in ADF1 position 
#define GFEFIS_SW_10           0x0200 // Toggle is in VOR1 position
#define GFEFIS_SW_11           0x0400 // Toggle is in ADF2 position
#define GFEFIS_SW_12           0x0800 // Toggle is in VOR2 position
#define GFEFIS_SW_13           0x1000 // MTRS pushbutton
#define GFEFIS_SW_14           0x2000 // FPV pushbutton
#define GFEFIS_SW_15           0x4000 // L knob push (DH/MDA)
#define GFEFIS_SW_16           0x8000 // R knob push (BARO)

#define GFEFIS_MASK_SEL1       0x0F   // bits 3..0 of bSelectors
#define GFEFIS_MASK_SEL2       0xF0   // bits 7..4 of bSelectors

//
// Bitmasks for access to GF-TQ6 control lever switch states
//
#define GFTQ6_SW_A             0x01  // Control Lever Switch "A" (currently not installed)
#define GFTQ6_SW_B             0x02  // Control Lever Switch "B"
#define GFTQ6_SW_C             0x04  // Control Lever Switch "C"
#define GFTQ6_SW_D             0x08  // Control Lever Switch "D"
#define GFTQ6_SW_E             0x10  // Control Lever Switch "E"
#define GFTQ6_SW_F             0x20  // Control Lever Switch "F" (currently not installed)

//
// Bitmasks for access to GF-ATC switch state
//
#define GFATC_KNOB_PUSH        0x01  // Selection knob push switch

//
// Definitions for data members of GFSECMINPUTREPORT 
//
// bSelector member discrete values ...
#define GFSECM_SEL_OFF            1
#define GFSECM_SEL_MAG_LEFT       2
#define GFSECM_SEL_MAG_RIGHT      3
#define GFSECM_SEL_MAG_BOTH       4
#define GFSECM_SEL_START          5

// Bitmask definitions for switch-related values
// bGearState ...
#define GFSECM_GEAR_DOWN       0x00
#define GFSECM_GEAR_UP         0x01

// bWingFlapsState ...
#define GFSECM_FLAPS_CENTER    0x00
#define GFSECM_FLAPS_DOWN      0x01
#define GFSECM_FLAPS_UP        0x02

// bCowlFlapsState ...
#define GFSECM_COWLFLAPS_OPEN  0x00
#define GFSECM_COWLFLAPS_CLOSE 0x01

// wSwitchState ...
#define GFSECM_MASTER_BAT    0x0001
#define GFSECM_MASTER_ALT    0x0002
#define GFSECM_AVIONICS      0x0004
#define GFSECM_FUEL_PUMP     0x0008
#define GFSECM_CARB_HEAT     0x0010
#define GFSECM_PITOT_HEAT    0x0020
#define GFSECM_PANEL_LTS     0x0040
#define GFSECM_BEACON_LTS    0x0080
#define GFSECM_NAV_LTS       0x0100
#define GFSECM_STROBE_LTS    0x0200
#define GFSECM_TAXI_LTS      0x0400
#define GFSECM_LANDING_LTS   0x0800

//
// Definitions for data members of GFMESMINPUTREPORT 
//
// Left/right selector knob state values
#define GFMESM_SEL_GRD          1
#define GFMESM_SEL_OFF          2
#define GFMESM_SEL_CONT         3
#define GFMESM_SEL_FLT          4
// 3-way toggle switch state values
#define GFMESM_IGN_LEFT         1
#define GFMESM_IGN_BOTH         2
#define GFMESM_IGN_RIGHT        3

//
// Bitmasks for access to GF-FMC LED states
//
#define GFFMC_LED_EXEC      0x01       // EXEC indicator (integrated with key)
#define GFFMC_LED_DSPL      0x02       // DSPL indicator
#define GFFMC_LED_MSG       0x04       // MSG indicator
#define GFFMC_LED_FAIL      0x08       // FAIL indicator
#define GFFMC_LED_OFST      0x10       // OFST indicator

// Bitmasks and constants for working with the GF-WCP module
//
#define GFWCP_BTN_1           0x01 // b0 : '1' = leftmost button pressed
#define GFWCP_BTN_2           0x02 // b1 : '1' = 2nd button (from left) pressed
#define GFWCP_BTN_3           0x04 // b2 : '1' = 3rd button (from left) pressed
#define GFWCP_BTN_4           0x08 // b3 : '1' = 4th button (from left) pressed
#define GFWCP_BTN_5           0x10 // b4 : '1' = 5th button (from left) pressed
#define GFWCP_BTN_6           0x20 // b5 : '1' = rightmost button pressed

// Constants for defining GF-WCP Indicator states and colors
//
// Each indicator on the GF-WCP can be set to one of 1 million distinct colors.
// A color value for a particular indicator is set using the function GFWCP_SetIndColor()
// specifying a red, green, and blue color value for the indicator of interest.
//
#define GFWCP_IND_COLOR_MIN          0 // Minimum color value supported by hardware
#define GFWCP_IND_COLOR_MAX        100 // Maximum color value supported by hardware

#define GFWCP_IND_COLOR_BLACK		0
#define GFWCP_IND_COLOR_WHITE		1
#define GFWCP_IND_COLOR_RED			2
#define GFWCP_IND_COLOR_GREEN		3
#define GFWCP_IND_COLOR_BLUE		4
#define GFWCP_IND_COLOR_MAGENTA     5
#define GFWCP_IND_COLOR_YELLOW      6
#define GFWCP_IND_COLOR_CYAN		7


//
// Bitmasks for access to GF-DIO switch states
//
// First Byte
#define GFDIO_SW_1_1      0x01 // Switch 1
#define GFDIO_SW_1_2      0x02 // Switch 2
#define GFDIO_SW_1_3      0x04 // Switch 3
#define GFDIO_SW_1_4      0x08 // Switch 4
#define GFDIO_SW_1_5      0x10 // Switch 5
#define GFDIO_SW_1_6      0x20 // Switch 6
#define GFDIO_SW_1_7      0x40 // Switch 7
#define GFDIO_SW_1_8      0x80 // Switch 8

// Second Byte
#define GFDIO_SW_2_1      0x01 // Switch 9
#define GFDIO_SW_2_2      0x02 // Switch 10
#define GFDIO_SW_2_3      0x04 // Switch 11
#define GFDIO_SW_2_4      0x08 // Switch 12
#define GFDIO_SW_2_5      0x10 // Switch 13
#define GFDIO_SW_2_6      0x20 // Switch 14
#define GFDIO_SW_2_7      0x40 // Switch 15
#define GFDIO_SW_2_8      0x80 // Switch 16

// Third Byte
#define GFDIO_SW_3_1      0x01 // Switch 17
#define GFDIO_SW_3_2      0x02 // Switch 18
#define GFDIO_SW_3_3      0x04 // Switch 19
#define GFDIO_SW_3_4      0x08 // Switch 20
#define GFDIO_SW_3_5      0x10 // Switch 21
#define GFDIO_SW_3_6      0x20 // Switch 22
#define GFDIO_SW_3_7      0x40 // Switch 23
#define GFDIO_SW_3_8      0x80 // Switch 24

// Fourth Byte
#define GFDIO_SW_4_1      0x01 // Switch 25
#define GFDIO_SW_4_2      0x02 // Switch 26
#define GFDIO_SW_4_3      0x04 // Switch 27
#define GFDIO_SW_4_4      0x08 // Switch 28
#define GFDIO_SW_4_5      0x10 // Switch 29
#define GFDIO_SW_4_6      0x20 // Switch 30
#define GFDIO_SW_4_7      0x40 // Switch 31
#define GFDIO_SW_4_8      0x80 // Switch 32

// Fifth Byte
#define GFDIO_SW_5_1      0x01 // Switch 33
#define GFDIO_SW_5_2      0x02 // Switch 34
#define GFDIO_SW_5_3      0x04 // Switch 35
#define GFDIO_SW_5_4      0x08 // Switch 36
#define GFDIO_SW_5_5      0x10 // Switch 37
#define GFDIO_SW_5_6      0x20 // Switch 38
#define GFDIO_SW_5_7      0x40 // Switch 39
#define GFDIO_SW_5_8      0x80 // Switch 40

// Sixth Byte
#define GFDIO_SW_6_1      0x01 // Switch 41
#define GFDIO_SW_6_2      0x02 // Switch 42
#define GFDIO_SW_6_3      0x04 // Switch 43
#define GFDIO_SW_6_4      0x08 // Switch 44
#define GFDIO_SW_6_5      0x10 // Switch 45
#define GFDIO_SW_6_6      0x20 // Switch 46
#define GFDIO_SW_6_7      0x40 // Switch 47
#define GFDIO_SW_6_8      0x80 // Switch 48

// Seventh Byte
#define GFDIO_SW_7_1      0x01 // Switch 49
#define GFDIO_SW_7_2      0x02 // Switch 50
#define GFDIO_SW_7_3      0x04 // Switch 51
#define GFDIO_SW_7_4      0x08 // Switch 52
#define GFDIO_SW_7_5      0x10 // Switch 53
#define GFDIO_SW_7_6      0x20 // Switch 54
#define GFDIO_SW_7_7      0x40 // Switch 55
#define GFDIO_SW_7_8      0x80 // Switch 56

//
// Bitmasks for access to GF-DIO LED states
//

// First Byte
#define GFDIO_LED_1_1     0x01 // LED 1 
#define GFDIO_LED_1_2     0x02 // LED 2
#define GFDIO_LED_1_3     0x04 // LED 3
#define GFDIO_LED_1_4     0x08 // LED 4
#define GFDIO_LED_1_5     0x10 // LED 5
#define GFDIO_LED_1_6     0x20 // LED 6
#define GFDIO_LED_1_7     0x40 // LED 7
#define GFDIO_LED_1_8     0x80 // LED 8

// Second Byte
#define GFDIO_LED_2_1     0x01 // LED 9
#define GFDIO_LED_2_2     0x02 // LED 10
#define GFDIO_LED_2_3     0x04 // LED 11
#define GFDIO_LED_2_4     0x08 // LED 12
#define GFDIO_LED_2_5     0x10 // LED 13
#define GFDIO_LED_2_6     0x20 // LED 14
#define GFDIO_LED_2_7     0x40 // LED 15
#define GFDIO_LED_2_8     0x80 // LED 16

// Third Byte
#define GFDIO_LED_3_1     0x01 // LED 17 
#define GFDIO_LED_3_2     0x02 // LED 18
#define GFDIO_LED_3_3     0x04 // LED 19
#define GFDIO_LED_3_4     0x08 // LED 20
#define GFDIO_LED_3_5     0x10 // LED 21
#define GFDIO_LED_3_6     0x20 // LED 22
#define GFDIO_LED_3_7     0x40 // LED 23
#define GFDIO_LED_3_8     0x80 // LED 24

// Fourth Byte
#define GFDIO_LED_4_1     0x01 // LED 25 
#define GFDIO_LED_4_2     0x02 // LED 26
#define GFDIO_LED_4_3     0x04 // LED 27
#define GFDIO_LED_4_4     0x08 // LED 28
#define GFDIO_LED_4_5     0x10 // LED 29
#define GFDIO_LED_4_6     0x20 // LED 30
#define GFDIO_LED_4_7     0x40 // LED 31
#define GFDIO_LED_4_8     0x80 // LED 32


// === Function Prototypes ====================================================
//
// Define API functions extern "C" when included in C++ source; by default, _cdecl
//   calling convention will be used 
//
#if defined( __cplusplus)
extern "C" 
{
#endif
    // The main initialization function for the GFDEV API.
    GFDEVRESULT GFDev_Init( HINSTANCE hAppInstance );
    // Functions for getting internal information about a specific GoFlight device.
    GFDEVRESULT GFDev_GetDeviceInfo( GFDEV_HW_TYPE HWType, int nDevIndex, LPGFDEVICEINFO pGFDI );
    GFDEVRESULT GFDev_GetDeviceInfo2( GFDEV_HW_TYPE HWType, int nDevIndex, LPGFDEVICEINFO2 pGFDI2 );

    // The main teardown function for the GFDEV API.
    GFDEVRESULT GFDev_Terminate( void );

    // Input callback is called any time new input data arrives from connected hardware.
    //   A single callback processes data for all instances and types of GoFlight hardware.
    BOOL GFDev_RegisterInputCallback( BOOL (_cdecl *pfnInputCallback)(GFDEV_HW_TYPE, unsigned short, LPGFINPUTREPORTTYPE) );
    BOOL GFDev_UnregisterInputCallback( void );

    // Connect callback is called from the API as the result of a hardware connect or disconnect event.
    //   The API is thus able to notify the client app of device connection and disconnection.
    BOOL GFDev_RegisterConnectCallback( BOOL (_cdecl *pfnConnectCallback)(GFDEV_HW_TYPE, unsigned short, GFDEV_EVENT_TOKEN) );
    BOOL GFDev_UnregisterConnectCallback( void );
    
    // Wakeup callback is called at regular intervals -- useful for general-purpose timing.
    //   Time interval is based on thread sleep time.
    BOOL GFDev_RegisterWakeupCallback( BOOL (_cdecl *pfnWakeupCallback)( void ), DWORD dwCallbackInterval );
    BOOL GFDev_UnregisterWakeupCallback( void );
    
    // GF-45 specific API functions
    unsigned short GF45_GetNumDevices ( void );
    GFDEVRESULT GF45_GetDialVals( int nDevIndex, short *pnLDialVal, short *pnRDialVal );
    GFDEVRESULT GF45_SetBrightness( int nDevIndex, int nBrightLevel );  
    GFDEVRESULT GF45_SetAlpha( int nDevIndex, char* szValue );
    GFDEVRESULT GF45_SetAlphaSegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GF45_SetNumeric( int nDevIndex, char* szValue );
    GFDEVRESULT GF45_SetNumericSegments( int nDevIndex, unsigned char* pszSegData );
    
    // GF-46 specific API functions
    unsigned short GF46_GetNumDevices ( void );
    GFDEVRESULT GF46_GetControlVals( int nDevIndex, short *pnLgDialVal, short *pnSmDialVal, unsigned char* pbSwitchVal );
    GFDEVRESULT GF46_SetAlpha( int nDevIndex, char* szValue );
    GFDEVRESULT GF46_SetAlphaSegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GF46_SetNumeric( int nDevIndex, char* szValue );
    GFDEVRESULT GF46_SetNumericSegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GF46_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GF46_SetBrightness( int nDevIndex, int nBrightLevel );  
    
    // GF-P8 specific API functions
    unsigned short GFP8_GetNumDevices ( void );
    GFDEVRESULT GFP8_GetSwitchVals( int nDevIndex, unsigned char* pbSwitchVals );
    GFDEVRESULT GFP8_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFP8_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFP8_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFP8_SetBrightness( int nDevIndex, int nBrightLevel );

    // GF-RP48 specific API functions
    unsigned short GFRP48_GetNumDevices ( void );
    GFDEVRESULT GFRP48_GetControlVals( int nDevIndex, short* pnADialVal, short* pnBDialVal, short* pnCDialVal, short* pnDDialVal, unsigned char* pbSwitchVals );
    GFDEVRESULT GFRP48_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFRP48_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFRP48_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFRP48_SetBrightness( int nDevIndex, int nBrightLevel );

    // GF-WCP specific API functions
    unsigned short GFWCP_GetNumDevices ( void );
    GFDEVRESULT GFWCP_GetControlVals( int nDevIndex, unsigned char* pbBtnsState );
    GFDEVRESULT GFWCP_GetIndColor( int nDevIndex, unsigned char bIndNum, unsigned char* pbColorRed, unsigned char* pbColorGreen, unsigned char* pbColorBlue );
    GFDEVRESULT GFWCP_SetIndColor( int nDevIndex, unsigned char bIndNum, unsigned char bColorRed, unsigned char bColorGreen, unsigned char bColorBlue );
    GFDEVRESULT GFWCP_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFWCP_SetBrightness( int nDevIndex, int nBrightLevel );
    GFDEVRESULT GFWCP_GetIndColorValue( int nDevIndex, unsigned char bIndNum, unsigned char* pbColor );
    GFDEVRESULT GFWCP_SetIndColorValue( int nDevIndex, unsigned char bIndNum, unsigned char bColor );
    
    // GF-T8 specific API functions
    unsigned short GFT8_GetNumDevices ( void );
    GFDEVRESULT GFT8_GetSwitchVals( int nDevIndex, unsigned char* pbSwitchVals );
    GFDEVRESULT GFT8_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFT8_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFT8_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFT8_SetBrightness( int nDevIndex, int nBrightLevel );

    // GF-SECM specific API functions
    unsigned short GFSECM_GetNumDevices ( void );
    GFDEVRESULT GFSECM_GetControlVals( int nDevIndex,
                                       unsigned char* pbSelector,
                                       unsigned char* pbGearState,
                                       unsigned char* pbWingFlapsState,
                                       unsigned char* pbCowlFlapsState,
                                       unsigned short* pwSwitchState );

    // GF-MESM specific API functions
    unsigned short GFMESM_GetNumDevices ( void );
    GFDEVRESULT GFMESM_GetControlVals( int nDevIndex,
                                       unsigned char* pbSelector1,
                                       unsigned char* pbSelector2,
                                       unsigned char* pbSelector3 );
    
    // GF-LGT specific API functions
    unsigned short GFLGT_GetNumDevices ( void );
    GFDEVRESULT GFLGT_GetControlVals( int nDevIndex, short* pnWheelVal, unsigned char* pbSwitchVals );
    GFDEVRESULT GFLGT_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFLGT_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFLGT_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFLGT_SetBrightness( int nDevIndex, int nBrightLevel );
    
    // GF-LGT II specific API functions
    unsigned short GFLGT2_GetNumDevices ( void );
    GFDEVRESULT GFLGT2_GetControlVals( int nDevIndex, short* pnAWheelVal, short* pnBWheelVal, short* pnCWheelVal, unsigned char* pbSwitchVals );
    GFDEVRESULT GFLGT2_GetEncoderTypes( int nDevIndex, ENCODERTYPE* petAWheel, ENCODERTYPE* petBWheel, ENCODERTYPE* petCWheel );
    GFDEVRESULT GFLGT2_SetEncoderTypes( int nDevIndex, ENCODERTYPE etAWheel, ENCODERTYPE etBWheel, ENCODERTYPE etCWheel );
    GFDEVRESULT GFLGT2_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFLGT2_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFLGT2_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFLGT2_SetBrightness( int nDevIndex, int nBrightLevel );
    
    // GF-166 specific API functions
    unsigned short GF166_GetNumDevices ( void );
    GFDEVRESULT GF166_GetControlVals( int nDevIndex, short *pnLgDialVal, short *pnSmDialVal, unsigned char* pbSwitchVals );
    GFDEVRESULT GF166_GetEncoderTypes( int nDevIndex, ENCODERTYPE* petLgDial, ENCODERTYPE* petSmDial );
    GFDEVRESULT GF166_SetEncoderTypes( int nDevIndex, ENCODERTYPE etLgDial, ENCODERTYPE etSmDial );
    GFDEVRESULT GF166_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GF166_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GF166_SetLDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GF166_SetLDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GF166_SetRDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GF166_SetRDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GF166_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GF166_SetBrightness( int nDevIndex, int nBrightLevel ); 
    
    // GF-MCP specific API functions
    unsigned short GFMCP_GetNumDevices ( void );
    GFDEVRESULT GFMCP_GetControlVals( int nDevIndex,
                                      short* pnADialVal, short* pnBDialVal, short* pnCDialVal, short* pnDDialVal, short* pnEDialVal,
                                      unsigned char* pbSwitchVals_1_8, unsigned char* pbSwitchVals_9_16 );
    GFDEVRESULT GFMCP_GetEncoderTypes( int nDevIndex, ENCODERTYPE* petADial, ENCODERTYPE* petBDial, ENCODERTYPE* petCDial, ENCODERTYPE* petDDial, ENCODERTYPE* petEDial );
    GFDEVRESULT GFMCP_SetEncoderTypes( int nDevIndex, ENCODERTYPE etADial, ENCODERTYPE etBDial, ENCODERTYPE etCDial, ENCODERTYPE etDDial, ENCODERTYPE etEDial );
    GFDEVRESULT GFMCP_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFMCP_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFMCP_SetADisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCP_SetADisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCP_SetBDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCP_SetBDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCP_SetCDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCP_SetCDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCP_SetDDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCP_SetDDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCP_SetEDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCP_SetEDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCP_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFMCP_SetBrightness( int nDevIndex, int nBrightLevel ); 
    
    // GF-MCP Pro specific API functions
    unsigned short GFMCPPro_GetNumDevices( void );
    GFDEVRESULT GFMCPPro_GetControlVals( int nDevIndex, short *pnADialVal, short *pnBDialVal, short* pnCDialVal, short* pnDDialVal, short* pnEDialVal, short* pnFDialVal, unsigned long* pulSwitchVals );
    GFDEVRESULT GFMCPPro_GetIndicators( int nDevIndex, unsigned long* pulIndicatorState );
    GFDEVRESULT GFMCPPro_SetIndicators( int nDevIndex, unsigned long ulIndicatorState );
    GFDEVRESULT GFMCPPro_SetADisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCPPro_SetADisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCPPro_SetBDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCPPro_SetBDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCPPro_SetCDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCPPro_SetCDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCPPro_SetDDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCPPro_SetDDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCPPro_SetEDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCPPro_SetEDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCPPro_SetFDisplayText( int nDevIndex, char* pszText );
    GFDEVRESULT GFMCPPro_SetFDisplaySegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFMCPPro_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFMCPPro_SetBrightness( int nDevIndex, int nBrightLevel );  

    // GF-EFIS specific API functions
    unsigned short GFEFIS_GetNumDevices( void );
    GFDEVRESULT GFEFIS_GetControlVals( int nDevIndex, short *pnADialVal, short* pnBDialVal, unsigned short* pwSwitchVals, unsigned char* pbSelectors );
    
    // GF-ATC specific API functions
    unsigned short GFATC_GetNumDevices ( void );
    GFDEVRESULT GFATC_GetControlVals( int nDevIndex, short *pnDialVal, unsigned char* pbSwitchVals );
    GFDEVRESULT GFATC_SetDigit( int nDevIndex, char* szValue );
    GFDEVRESULT GFATC_SetDigitSegments( int nDevIndex, unsigned char* pszSegData );
    GFDEVRESULT GFATC_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFATC_SetBrightness( int nDevIndex, int nBrightLevel ); 
    
    // GF-TQ6 specific API functions
    unsigned short GFTQ6_GetNumDevices( void );
    GFDEVRESULT GFTQ6_GetControlVals( int nDevIndex,
                                      unsigned char* pnLeverAVal, unsigned char* pnLeverBVal, unsigned char* pnLeverCVal,
                                      unsigned char* pnLeverDVal, unsigned char* pnLeverEVal, unsigned char* pnLeverFVal,
                                      unsigned char* pbSwitchVals );

    // GF-FMC specific API functions
    unsigned short GFFMC_GetNumDevices ( void );
    GFDEVRESULT GFFMC_GetIndicators( int nDevIndex, unsigned char* pbIndicatorState );
    GFDEVRESULT GFFMC_SetIndicators( int nDevIndex, unsigned char bIndicatorState );
    GFDEVRESULT GFFMC_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFFMC_SetBrightness( int nDevIndex, int nBrightLevel ); 
    GFDEVRESULT GFFMC_GetKeycodes( int nDevIndex, unsigned char* pbKeycodes, unsigned int* pnCount );

    // GF-DIO specific API functions
    unsigned short GFDIO_GetNumDevices ( void );
    GFDEVRESULT GFDIO_GetSwitchVals( int nDevIndex, unsigned char* pbSwitchBank1, unsigned char* pbSwitchBank2, unsigned char* pbSwitchBank3, unsigned char* pbSwitchBank4, 
													unsigned char* pbSwitchBank5, unsigned char* pbSwitchBank6, unsigned char* pbSwitchBank7);
	// GFDIO_GetIndicators not implemented in the current firmware
	GFDEVRESULT GFDIO_GetIndicators( int nDevIndex, unsigned char* pbIndicatorBank1, unsigned char* pbIndicatorBank2, unsigned char* pbIndicatorBank3, unsigned char* pbIndicatorBank4 );
    GFDEVRESULT GFDIO_SetIndicators( int nDevIndex, unsigned char bIndicatorBank1, unsigned char bIndicatorBank2,  unsigned char bIndicatorBank3,  unsigned char bIndicatorBank4 );
	
	// No Brightness support in the current firmware so these functions do nothing
    GFDEVRESULT GFDIO_GetBrightness( int nDevIndex, int* pnBrightLevel );
    GFDEVRESULT GFDIO_SetBrightness( int nDevIndex, int nBrightLevel );


#if defined( __cplusplus)
} // extern "C"
#endif

#ifdef _WINDOWS // Windows only - no _stdcall on Mac.

// _stdcall versions of GFDEV API functions.  These are for use by apps built using non-C
//   languages (Visual Basic, Delphi, etc.)  _stdcall is equivalent to _far _pascal.
//
// GFDEV API management functions
GFDEVRESULT _stdcall GFDev_Init_stdcall( HINSTANCE hAppInstance );
GFDEVRESULT _stdcall GFDev_GetDeviceInfo_stdcall( GFDEV_HW_TYPE HWType, int nDevIndex, LPGFDEVICEINFO pGFDI );
GFDEVRESULT _stdcall GFDev_GetDeviceInfo2_stdcall( GFDEV_HW_TYPE HWType, int nDevIndex, LPGFDEVICEINFO2 pGFDI2 );

GFDEVRESULT _stdcall GFDev_Terminate_stdcall( void );
BOOL _stdcall GFDev_RegisterInputCallback_stdcall( BOOL (_stdcall *pfnInputCallback)(GFDEV_HW_TYPE, unsigned short, LPGFINPUTREPORTTYPE) );
BOOL _stdcall GFDev_UnregisterInputCallback_stdcall( void );
BOOL _stdcall GFDev_RegisterConnectCallback_stdcall( BOOL (_stdcall *pfnConnectCallback)(GFDEV_HW_TYPE, unsigned short, GFDEV_EVENT_TOKEN) );
BOOL _stdcall GFDev_UnregisterConnectCallback_stdcall( void );
BOOL _stdcall GFDev_RegisterWakeupCallback_stdcall( BOOL (_stdcall *pfnWakeupCallback)( void ), DWORD dwCallbackInterval );
BOOL _stdcall GFDev_UnregisterWakeupCallback_stdcall( void );

// GF-45 specific API functions
unsigned short _stdcall GF45_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GF45_GetDialVals_stdcall( int nDevIndex, short *pnLDialVal, short *pnRDialVal );
GFDEVRESULT _stdcall GF45_SetBrightness_stdcall( int nDevIndex, int nBrightLevel ); 
GFDEVRESULT _stdcall GF45_SetAlpha_stdcall( int nDevIndex, char* szValue );
GFDEVRESULT _stdcall GF45_SetAlphaSegments_stdcall( int nDevIndex, char* pszSegData );
GFDEVRESULT _stdcall GF45_SetNumeric_stdcall( int nDevIndex, char* szValue );
GFDEVRESULT _stdcall GF45_SetNumericSegments_stdcall( int nDevIndex, char* pszSegData );

// GF-46 specific API functions
unsigned short _stdcall GF46_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GF46_GetControlVals_stdcall( int nDevIndex, short *pnLgDialVal, short *pnSmDialVal, unsigned char* pbSwitchVal );
GFDEVRESULT _stdcall GF46_SetAlpha_stdcall( int nDevIndex, char* szValue );
GFDEVRESULT _stdcall GF46_SetAlphaSegments_stdcall( int nDevIndex, char* pszSegData );
GFDEVRESULT _stdcall GF46_SetNumeric_stdcall( int nDevIndex, char* szValue );
GFDEVRESULT _stdcall GF46_SetNumericSegments_stdcall( int nDevIndex, char* pszSegData );
GFDEVRESULT _stdcall GF46_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GF46_SetBrightness_stdcall( int nDevIndex, int nBrightLevel ); 

// GF-P8 specific API functions
unsigned short _stdcall GFP8_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFP8_GetSwitchVals_stdcall( int nDevIndex, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GFP8_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GFP8_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GFP8_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFP8_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

// GF-RP48 specific API functions
unsigned short _stdcall GFRP48_GetNumDevices_stdcall ( void );
GFDEVRESULT _stdcall GFRP48_GetControlVals_stdcall( int nDevIndex, short* pnADialVal, short* pnBDialVal, short* pnCDialVal, short* pnDDialVal, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GFRP48_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GFRP48_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GFRP48_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFRP48_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

// GF-WCP specific API functions
unsigned short _stdcall GFWCP_GetNumDevices_stdcall ( void );
GFDEVRESULT _stdcall GFWCP_GetControlVals_stdcall( int nDevIndex, short* pnADialVal, short* pnADialPush, short* pnBDialVal, short* pnBDialPush, short* pnToggleA, short* pnToggleB, short* pnToggleC, short* pnToggleD, unsigned char* pbBtnsState );
GFDEVRESULT _stdcall GFWCP_GetIndColor_stdcall( int nDevIndex, unsigned char bIndNum, unsigned char* pbColorRed, unsigned char* pbColorGreen, unsigned char* pbColorBlue );
GFDEVRESULT _stdcall GFWCP_SetIndColor_stdcall( int nDevIndex, unsigned char bIndNum, unsigned char bColorRed, unsigned char bColorGreen, unsigned char bColorBlue );
GFDEVRESULT _stdcall GFWCP_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFWCP_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );
GFDEVRESULT _stdcall GFWCP_GetIndColorValue_stdcall( int nDevIndex, unsigned char bIndNum, unsigned char* pbColor );
GFDEVRESULT _stdcall GFWCP_SetIndColorValue_stdcall( int nDevIndex, unsigned char bIndNum, unsigned char bColor );

// GF-T8 specific API functions
unsigned short _stdcall GFT8_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFT8_GetSwitchVals_stdcall( int nDevIndex, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GFT8_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GFT8_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GFT8_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFT8_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

// GF-SECM specific API functions
unsigned short _stdcall GFSECM_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFSECM_GetControlVals_stdcall( int nDevIndex, unsigned char* pbSelector, unsigned char* pbGearState, unsigned char* pbWingFlapsState, unsigned char* pbCowlFlapsState, unsigned short* pwSwitchState );
GFDEVRESULT _stdcall GFSECM_GetSwitchVals_stdcall( int nDevIndex, unsigned long* pulSwitchVals );

// GF-MESM specific API functions
unsigned short _stdcall GFMESM_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFMESM_GetControlVals_stdcall( int nDevIndex, unsigned char* pbSelector1, unsigned char* pbSelector2, unsigned char* pbSelector3 );

// GF-LGT specific API functions
unsigned short _stdcall GFLGT_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFLGT_GetControlVals_stdcall( int nDevIndex, short* pnWheelVal, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GFLGT_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GFLGT_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GFLGT_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFLGT_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

// GF-LGT II specific API functions
unsigned short _stdcall GFLGT2_GetNumDevices_stdcall ( void );
GFDEVRESULT _stdcall GFLGT2_GetControlVals_stdcall( int nDevIndex, short* pnAWheelVal, short* pnBWheelVal, short* pnCWheelVal, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GFLGT2_GetEncoderTypes_stdcall( int nDevIndex, ENCODERTYPE* petAWheel, ENCODERTYPE* petBWheel, ENCODERTYPE* petCWheel );
GFDEVRESULT _stdcall GFLGT2_SetEncoderTypes_stdcall( int nDevIndex, ENCODERTYPE etAWheel, ENCODERTYPE etBWheel, ENCODERTYPE etCWheel );
GFDEVRESULT _stdcall GFLGT2_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GFLGT2_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GFLGT2_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFLGT2_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

// GF-166 specific API functions
unsigned short _stdcall GF166_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GF166_GetControlVals_stdcall( int nDevIndex, short *pnLgDialVal, short *pnSmDialVal, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GF166_GetEncoderTypes_stdcall( int nDevIndex, ENCODERTYPE* petLgDial, ENCODERTYPE* petSmDial );
GFDEVRESULT _stdcall GF166_SetEncoderTypes_stdcall( int nDevIndex, ENCODERTYPE etLgDial, ENCODERTYPE etSmDial );
GFDEVRESULT _stdcall GF166_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GF166_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GF166_SetLDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GF166_SetLDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GF166_SetRDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GF166_SetRDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GF166_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GF166_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

// GF-MCP specific API functions
unsigned short _stdcall GFMCP_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFMCP_GetControlVals_stdcall( int nDevIndex, short *pnADialVal, short *pnBDialVal, short* pnCDialVal, short* pnDDialVal, short* pnEDialVal, unsigned char* pbSwitchVals_1_8, unsigned char* pbSwitchVals_9_16 );
GFDEVRESULT _stdcall GFMCP_GetEncoderTypes_stdcall( int nDevIndex, ENCODERTYPE* petADial, ENCODERTYPE* petBDial, ENCODERTYPE* petCDial, ENCODERTYPE* petDDial, ENCODERTYPE* petEDial );
GFDEVRESULT _stdcall GFMCP_SetEncoderTypes_stdcall( int nDevIndex, ENCODERTYPE etADial, ENCODERTYPE etBDial, ENCODERTYPE etCDial, ENCODERTYPE etDDial, ENCODERTYPE etEDial );
GFDEVRESULT _stdcall GFMCP_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorState );
GFDEVRESULT _stdcall GFMCP_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorState );
GFDEVRESULT _stdcall GFMCP_SetADisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCP_SetADisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCP_SetBDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCP_SetBDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCP_SetCDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCP_SetCDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCP_SetDDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCP_SetDDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCP_SetEDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCP_SetEDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCP_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFMCP_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );    

// GF-MCP Pro specific API functions
unsigned short _stdcall GFMCPPro_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFMCPPro_GetControlVals_stdcall( int nDevIndex, short *pnADialVal, short *pnBDialVal, short* pnCDialVal, short* pnDDialVal, short* pnEDialVal, short* pnFDialVal, unsigned long* pulSwitchVals );
GFDEVRESULT _stdcall GFMCPPro_GetIndicators_stdcall( int nDevIndex, unsigned long* pulIndicatorState );
GFDEVRESULT _stdcall GFMCPPro_SetIndicators_stdcall( int nDevIndex, unsigned long ulIndicatorState );
GFDEVRESULT _stdcall GFMCPPro_SetADisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCPPro_SetADisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCPPro_SetBDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCPPro_SetBDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCPPro_SetCDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCPPro_SetCDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCPPro_SetDDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCPPro_SetDDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCPPro_SetEDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCPPro_SetEDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCPPro_SetFDisplayText_stdcall( int nDevIndex, char* pszText );
GFDEVRESULT _stdcall GFMCPPro_SetFDisplaySegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFMCPPro_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFMCPPro_SetBrightness_stdcall( int nDevIndex, int nBrightLevel ); 

// GF-EFIS specific API functions
unsigned short _stdcall GFEFIS_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFEFIS_GetControlVals_stdcall( int nDevIndex, short *pnADialVal, short* pnBDialVal, unsigned short* pwSwitchVals, unsigned char* pbSelectors );

// GF-ATC specific API functions
unsigned short _stdcall GFATC_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFATC_GetControlVals_stdcall( int nDevIndex, short *pnDialVal, unsigned char* pbSwitchVals );
GFDEVRESULT _stdcall GFATC_SetDigit_stdcall( int nDevIndex, char* szValue );
GFDEVRESULT _stdcall GFATC_SetDigitSegments_stdcall( int nDevIndex, unsigned char* pszSegData );
GFDEVRESULT _stdcall GFATC_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFATC_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );    

// GF-TQ6 specific API functions
unsigned short _stdcall GFTQ6_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFTQ6_GetControlVals_stdcall( int nDevIndex, short *pnLeverAVal, short *pnLeverBVal, short* pnLeverCVal, short* pnLeverDVal, short* pnLeverEVal, short* pnLeverFVal, unsigned char* pbSwitchVals );

// GF-DIO specific API functions
unsigned short _stdcall GFDIO_GetNumDevices_stdcall( void );
GFDEVRESULT _stdcall GFDIO_GetSwitchVals_stdcall( int nDevIndex, unsigned char* pbSwitchBank1, unsigned char* pbSwitchBank2, unsigned char* pbSwitchBank3, unsigned char* pbSwitchBank4, 
				  												 unsigned char* pbSwitchBank5, unsigned char* pbSwitchBank6, unsigned char* pbSwitchBank7);

// GFDIO_GetIndicators not implemented in the current firmware
GFDEVRESULT _stdcall GFDIO_GetIndicators_stdcall( int nDevIndex, unsigned char* pbIndicatorBank1, unsigned char* pbIndicatorBank2, unsigned char* pbIndicatorBank3, unsigned char* pbIndicatorBank4 );
GFDEVRESULT _stdcall GFDIO_SetIndicators_stdcall( int nDevIndex, unsigned char bIndicatorBank1, unsigned char bIndicatorBank2,  unsigned char bIndicatorBank3,  unsigned char bIndicatorBank4);

// No Brightness support in the current firmware so these functions do nothing
GFDEVRESULT _stdcall GFDIO_GetBrightness_stdcall( int nDevIndex, int* pnBrightLevel );
GFDEVRESULT _stdcall GFDIO_SetBrightness_stdcall( int nDevIndex, int nBrightLevel );

#endif  // End Windows-only

#pragma pack( pop, gfdevapi_h ) // Ensure previous pack value is restored

#endif // #ifdef GFDEVAPI_H_

//
// *** End of File: GFDevApi.h ***
//
