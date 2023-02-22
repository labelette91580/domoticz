#include "stdafx.h"
#include "EnOceanRawValue.h"
#include "EnOceanProfil.h"

// function number :0x01 : Electronic switches and dimmers with Energy Measurement and Local Control 
// function type :0x00 : Type 0x00 
//{ 0xD2, 0x01, 0x00, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x00                                                                       " },

// TITLE:CMD 0x1 - Actuator Set Output
// DESC :This message is sent to an actuator. It controls switching / dimming of one or all channels of an actuator.
enocean:: T_DATAFIELD D20100_CMD1 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 1 , "ID 01" },}},
{  8 , 3 ,     0 ,     0 ,     0 ,     0 , "DV"       , "Dim value",{{ 0 , "Switch to new output value" },{ 1 , "Dim to new output value - dim timer 1" },{ 2 , "Dim to new output value - dim timer 2" },{ 3 , "Dim to new output value - dim timer 3" },{ 4 , "Stop dimming" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 17 , 7 ,     0 ,     0 ,     0 ,     0 , "OV"       , "Output value",{{ 0 , "Output value 0% or OFF" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE1 ={ D20100_CMD1 , "CMD 0x1 - Actuator Set Output" , "This message is sent to an actuator. It controls switching / dimming of one or all channels of an actuator." } ;

// TITLE:CMD 0x2 - Actuator Set Local
// DESC :This message is sent to an actuator. It configures one or all channels of an actuator.
enocean:: T_DATAFIELD D20100_CMD2 [] = {
{  0 , 1 ,     0 ,     0 ,     0 ,     0 , "d_e"      , "Taught-in devices",{{ 0 , "Disable taught-in devices (with different EEP)" },{ 1 , "Enable taught-in devices (with different EEP)" },}},
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 2 , "ID 02" },}},
{  8 , 1 ,     0 ,     0 ,     0 ,     0 , "OC"       , "Over current shut down",{{ 0 , "Over current shut down: static off" },{ 1 , "Over current shut down: automatic restart" },}},
{  9 , 1 ,     0 ,     0 ,     0 ,     0 , "RO"       , "reset over current shut down",{{ 0 , "Reset over current shut down: not active" },{ 1 , "Reset over current shut down: trigger signal" },}},
{ 10 , 1 ,     0 ,     0 ,     0 ,     0 , "LC"       , "Local control",{{ 0 , "Disable local control" },{ 1 , "Enable local control" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 20 , 4 ,     0 ,     0 ,     0 ,     0 , "DT3"      , "Dim timer 3",{{ 0 , "Not used" },}},
{ 28 , 4 ,     0 ,     0 ,     0 ,     0 , "DT1"      , "Dim timer 1",{{ 0 , "Not used" },}},
{ 16 , 4 ,     0 ,     0 ,     0 ,     0 , "DT2"      , "Dim timer 2",{{ 0 , "Not used" },}},
{ 24 , 1 ,     0 ,     0 ,     0 ,     0 , "d_n"      , "User interface indication",{{ 0 , "User interface indication: day operation" },{ 1 , "User interface indication: night operation" },}},
{ 25 , 1 ,     0 ,     0 ,     0 ,     0 , "PF"       , "Power Failure",{{ 0 , "Disable Power Failure Detection" },{ 1 , "Enable Power Failure Detection" },}},
{ 26 , 2 ,     0 ,     0 ,     0 ,     0 , "DS"       , "Default state",{{ 0 , "Default state: 0% or OFF" },{ 1 , "Default state: 100% or ON" },{ 2 , "Default state: remember previous state" },{ 3 , "Not used" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE2 ={ D20100_CMD2 , "CMD 0x2 - Actuator Set Local" , "This message is sent to an actuator. It configures one or all channels of an actuator." } ;

// TITLE:CMD 0x3 - Actuator Status Query
// DESC :This message is sent to an actuator. It requests the status of one or all channels of an actuator.
enocean:: T_DATAFIELD D20100_CMD3 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 3 , "ID 03" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE3 ={ D20100_CMD3 , "CMD 0x3 - Actuator Status Query" , "This message is sent to an actuator. It requests the status of one or all channels of an actuator." } ;

// TITLE:CMD 0x4 - Actuator Status Response
// DESC :This message is sent by an actuator if one of the following events occurs:
enocean:: T_DATAFIELD D20100_CMD4 [] = {
{  0 , 1 ,     0 ,     0 ,     0 ,     0 , "PF"       , "Power Failure",{{ 0 , "Power Failure Detection disabled/not supported" },{ 1 , "Power Failure Detection enabled" },}},
{  1 , 1 ,     0 ,     0 ,     0 ,     0 , "PFD"      , "Power Failure Detection",{{ 0 , "Power Failure not detected/not supported/disabled" },{ 1 , "Power Failure Detected" },}},
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 4 , "ID 04" },}},
{  8 , 1 ,     0 ,     0 ,     0 ,     0 , "OC"       , "Over current switch off",{{ 0 , "Over current switch off: ready / not supported" },{ 1 , "Over current switch off: executed" },}},
{  9 , 2 ,     0 ,     0 ,     0 ,     0 , "EL"       , "Error level",{{ 0 , "Error level 0: hardware OK" },{ 1 , "Error level 1: hardware warning" },{ 2 , "Error level 2: hardware failure" },{ 3 , "Error level not supported" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 16 , 1 ,     0 ,     0 ,     0 ,     0 , "LC"       , "Local control",{{ 0 , "Local control disabled / not supported" },{ 1 , "Local control enabled" },}},
{ 17 , 7 ,     0 ,     0 ,     0 ,     0 , "OV"       , "Output value",{{ 0 , "Output value 0% or OFF" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE4 ={ D20100_CMD4 , "CMD 0x4 - Actuator Status Response" , "This message is sent by an actuator if one of the following events occurs:" } ;

// TITLE:CMD 0x5 - Actuator Set Measurement
// DESC :The command defines values at offset 32 and at offset 40 which are the limits for the transmission periodicity of messages. MIT must not be set to 0, MAT >= MIT.
enocean:: T_DATAFIELD D20100_CMD5 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 5 , "ID 05" },}},
{  8 , 1 ,     0 ,     0 ,     0 ,     0 , "RM"       , "Report measurement",{{ 0 , "Report measurement: query only" },{ 1 , "Report measurement: query / auto reporting" },}},
{  9 , 1 ,     0 ,     0 ,     0 ,     0 , "RE"       , "Reset measurement",{{ 0 , "Reset measurement: not active" },{ 1 , "Reset measurement: trigger signal" },}},
{ 10 , 1 ,     0 ,     0 ,     0 ,     0 , "e_p"      , "Measurement mode",{{ 0 , "Energy measurement" },{ 1 , "Power measurement" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 16 , 4 ,     0 ,  4095 ,     0 ,  4095 , "MD_LSB"   , "Measurement delta to be reported (LSB)"},
{ 24 , 8 ,     0 ,  4095 ,     0 ,  4095 , "MD_MSB"   , "Measurement delta to be reported (MSB)"},
{ 21 , 3 ,     0 ,     0 ,     0 ,     0 , "UN"       , "Unit",{{ 0 , "Energy [Ws]" },{ 1 , "Energy [Wh]" },{ 2 , "Energy [KWh]" },{ 3 , "Power [W]" },{ 4 , "Power [KW]" },}},
{ 32 , 8 ,     0 ,     0 ,     0 ,     0 , "MAT"      , "Maximum time between two subsequent actuator messages",{}},
{ 40 , 8 ,     0 ,     0 ,     0 ,     0 , "MIT"      , "Minimum time between two subsequent actuator messages",{}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE5 ={ D20100_CMD5 , "CMD 0x5 - Actuator Set Measurement" , "The command defines values at offset 32 and at offset 40 which are the limits for the transmission periodicity of messages. MIT must not be set to 0, MAT >= MIT." } ;

// TITLE:CMD 0x6 - Actuator Measurement Query
// DESC :This message is sent to an actuator. The actuator replies with an Actuator Measurement Response message.
enocean:: T_DATAFIELD D20100_CMD6 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 6 , "ID 06" },}},
{ 10 , 1 ,     0 ,     0 ,     0 ,     0 , "qu"       , "Query",{{ 0 , "Query energy" },{ 1 , "Query power" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE6 ={ D20100_CMD6 , "CMD 0x6 - Actuator Measurement Query" , "This message is sent to an actuator. The actuator replies with an Actuator Measurement Response message." } ;

// TITLE:CMD 0x7 - Actuator Measurement Response
// DESC :This message is sent by an actuator if one of the following events occurs:
enocean:: T_DATAFIELD D20100_CMD7 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 7 , "ID 07" },}},
{  8 , 3 ,     0 ,     0 ,     0 ,     0 , "UN"       , "Unit",{{ 0 , "Energy [Ws]" },{ 1 , "Energy [Wh]" },{ 2 , "Energy [KWh]" },{ 3 , "Power [W]" },{ 4 , "Power [KW]" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 16 ,32 ,     0 , 4294967295 ,     0 ,     0 , "MV"       , "Measurement value (4 bytes)"},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE7 ={ D20100_CMD7 , "CMD 0x7 - Actuator Measurement Response" , "This message is sent by an actuator if one of the following events occurs:" } ;

// TITLE:CMD 0x8 - Actuator Set Pilot Wire Mode
// DESC :
enocean:: T_DATAFIELD D20100_CMD8 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 8 , "ID 08" },}},
{ 13 , 3 ,     0 ,     0 ,     0 ,     0 , "PM"       , "Pilotwire mode",{{ 0 , "Off" },{ 1 , "Comfort" },{ 2 , "Eco" },{ 3 , "Anti-freeze" },{ 4 , "Comfort-1" },{ 5 , "Comfort-2" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE8 ={ D20100_CMD8 , "CMD 0x8 - Actuator Set Pilot Wire Mode" , "" } ;

// TITLE:CMD 0x9 - Actuator Pilot Wire Mode Query
// DESC :
enocean:: T_DATAFIELD D20100_CMD9 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 9 , "ID 09" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE9 ={ D20100_CMD9 , "CMD 0x9 - Actuator Pilot Wire Mode Query" , "" } ;

// TITLE:CMD 0xA - Actuator Pilot Wire Mode Response
// DESC :
enocean:: T_DATAFIELD D20100_CMD10 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 10 , "ID 0A" },}},
{ 13 , 3 ,     0 ,     0 ,     0 ,     0 , "PM"       , "Pilotwire mode",{{ 0 , "Off" },{ 1 , "Comfort" },{ 2 , "Eco" },{ 3 , "Anti-freeze" },{ 4 , "Comfort-1" },{ 5 , "Comfort-2" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE10 ={ D20100_CMD10 , "CMD 0xA - Actuator Pilot Wire Mode Response" , "" } ;

// TITLE:CMD 0xB - Actuator Set External Interface Settings
// DESC :
enocean:: T_DATAFIELD D20100_CMD11 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 11 , "ID 0B" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 16 ,16 ,     0 ,     0 ,     0 ,     0 , "AOT"      , "Auto OFF Timer",{{ 0 , "Timer deactivated" },}},
{ 32 ,16 ,     0 ,     0 ,     0 ,     0 , "DOT"      , "Delay OFF Timer",{{ 0 , "Timer deactivated" },}},
{ 48 , 2 ,     0 ,     0 ,     0 ,     0 , "EBM"      , "External Switch/Push Button",{{ 0 , "Not applicable" },{ 1 , "External Switch" },{ 2 , "External Push Button" },{ 3 , "Auto detect" },}},
{ 50 , 1 ,     0 ,     0 ,     0 ,     0 , "SWT"      , "2-state switch",{{ 0 , "Change of key state sets ON or OFF" },{ 1 , "Specific ON/OFF positions." },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE11 ={ D20100_CMD11 , "CMD 0xB - Actuator Set External Interface Settings" , "" } ;

// TITLE:CMD 0xC - Actuator External Interface Settings Query
// DESC :
enocean:: T_DATAFIELD D20100_CMD12 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 12 , "ID 0C" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE12 ={ D20100_CMD12 , "CMD 0xC - Actuator External Interface Settings Query" , "" } ;

// TITLE:CMD 0xD - Actuator External Interface Settings Response
// DESC :
enocean:: T_DATAFIELD D20100_CMD13 [] = {
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 13 , "ID 0D" },}},
{ 11 , 5 ,     0 ,     0 ,     0 ,     0 , "I_O"      , "I/O channel",{}},
{ 16 ,16 ,     0 ,     0 ,     0 ,     0 , "AOT"      , "Auto OFF Timer",{{ 0 , "Timer deactivated" },}},
{ 32 ,16 ,     0 ,     0 ,     0 ,     0 , "DOT"      , "Delay OFF Timer",{{ 0 , "Timer deactivated" },}},
{ 48 , 2 ,     0 ,     0 ,     0 ,     0 , "EBM"      , "External Switch/Push Button",{{ 0 , "Not applicable" },{ 1 , "External Switch" },{ 2 , "External Push Button" },{ 3 , "Auto detect" },}},
{ 50 , 1 ,     0 ,     0 ,     0 ,     0 , "SWT"      , "2-state switch",{{ 0 , "Change of key state sets ON or OFF" },{ 1 , "Specific ON/OFF positions." },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D20100_CASE13 ={ D20100_CMD13 , "CMD 0xD - Actuator External Interface Settings Response" , "" } ;

enocean:: T_EEP_CASE_* D20100_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x01 : Type 0x01 (description: see table) 
//{ 0xD2, 0x01, 0x01, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x01 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20101_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x02 : Type 0x02 (description: see table) 
//{ 0xD2, 0x01, 0x02, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x02 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20102_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x03 : Type 0x03 (description: see table) 
//{ 0xD2, 0x01, 0x03, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x03 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20103_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x04 : Type 0x04 (description: see table) 
//{ 0xD2, 0x01, 0x04, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x04 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20104_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x05 : Type 0x05 (description: see table) 
//{ 0xD2, 0x01, 0x05, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x05 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20105_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x06 : Type 0x06 (description: see table) 
//{ 0xD2, 0x01, 0x06, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x06 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20106_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x07 : Type 0x07 (description: see table) 
//{ 0xD2, 0x01, 0x07, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x07 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20107_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x08 : Type 0x08 (description: see table) 
//{ 0xD2, 0x01, 0x08, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x08 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20108_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x09 : Type 0x09 (description: see table) 
//{ 0xD2, 0x01, 0x09, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x09 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20109_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x0A : Type 0x0A (description: see table) 
//{ 0xD2, 0x01, 0x0A, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0A (description: see table)                                              " },

enocean:: T_EEP_CASE_* D2010A_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x0B : Type 0x0B (description: see table) 
//{ 0xD2, 0x01, 0x0B, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0B (description: see table)                                              " },

enocean:: T_EEP_CASE_* D2010B_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x0C : Type 0x0C 
//{ 0xD2, 0x01, 0x0C, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0C                                                                       " },

enocean:: T_EEP_CASE_* D2010C_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x0D : Type 0x0D 
//{ 0xD2, 0x01, 0x0D, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0D                                                                       " },

enocean:: T_EEP_CASE_* D2010D_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x0E : Type 0x0E 
//{ 0xD2, 0x01, 0x0E, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0E                                                                       " },

enocean:: T_EEP_CASE_* D2010E_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x0F : Type 0x0F 
//{ 0xD2, 0x01, 0x0F, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0F                                                                       " },

enocean:: T_EEP_CASE_* D2010F_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x10 : Type 0x10 (description: see table) 
//{ 0xD2, 0x01, 0x10, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x10 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20110_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x11 : Type 0x11 (description: see table) 
//{ 0xD2, 0x01, 0x11, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x11 (description: see table)                                              " },

enocean:: T_EEP_CASE_* D20111_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function type :0x12 : Type 0x12 
//{ 0xD2, 0x01, 0x12, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x12                                                                       " },

enocean:: T_EEP_CASE_* D20112_CASES [] = {
&D20100_CASE1 ,
&D20100_CASE2 ,
&D20100_CASE3 ,
&D20100_CASE4 ,
&D20100_CASE5 ,
&D20100_CASE6 ,
&D20100_CASE7 ,
&D20100_CASE8 ,
&D20100_CASE9 ,
&D20100_CASE10 ,
&D20100_CASE11 ,
&D20100_CASE12 ,
&D20100_CASE13 ,
{0 }
};

// function number :0x05 : Blinds Control for Position and Angle 
// function type :0x00 : Type 0x00 
//{ 0xD2, 0x05, 0x00, "Blinds Control for Position and Angle                                           " , "Type 0x00                                                                       " },

// TITLE:CMD 1 - Go to Position and Angle
// DESC :Once the actuator is configured either by the  Set Parameters  command or through manual configuration (using local buttons) the position of the blinds can be controlled with this command.
enocean:: T_DATAFIELD D2050X_CMD1 [] = {
{  1 , 7 ,     0 ,     0 ,     0 ,     0 , "POS"      , "Position",{}},
{  9 , 7 ,     0 ,     0 ,     0 ,     0 , "ANG"      , "Angle",{}},
{ 17 , 3 ,     0 ,     0 ,     0 ,     0 , "REPO"     , "Repositioning",{{ 0 , "Go directly to POS/ANG" },{ 1 , "Go up (0%), then to POS/ANG" },{ 2 , "Go down (100%), then to POS/ANG" },{ 3 , "Reserved" },}},
{ 21 , 3 ,     0 ,     0 ,     0 ,     0 , "LOCK"     , "Locking modes",{{ 0 , "Do not change" },{ 1 , "Set blockage mode" },{ 2 , "Set alarm mode" },{ 3 , "Reserved" },{ 7 , "Deblockage" },}},
{ 24 , 4 ,     0 ,     0 ,     0 ,     0 , "CHN"      , "Channel",{{ 0 , "Channel 1" },}},
{ 28 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 1 , "Goto command" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D2050X_CASE1 ={ D2050X_CMD1 , "CMD 1 - Go to Position and Angle" , "Once the actuator is configured either by the  Set Parameters  command or through manual configuration (using local buttons) the position of the blinds can be controlled with this command." } ;

// TITLE:CMD 2 - Stop
// DESC :This command immediately stops a running blind motor. It has no effect when the actuator is in  blockage  or  alarm  mode, i.e. it will not stop an eventual  go up  or  go down  alarm action.
enocean:: T_DATAFIELD D2050X_CMD2 [] = {
{  0 , 4 ,     0 ,     0 ,     0 ,     0 , "CHN"      , "Channel",{{ 0 , "Channel 1" },}},
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 2 , "Stop command" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D2050X_CASE2 ={ D2050X_CMD2 , "CMD 2 - Stop" , "This command immediately stops a running blind motor. It has no effect when the actuator is in  blockage  or  alarm  mode, i.e. it will not stop an eventual  go up  or  go down  alarm action." } ;

// TITLE:CMD 3 - Query Position and Angle
// DESC :This command requests the actuator to return a  reply  command.
enocean:: T_DATAFIELD D2050X_CMD3 [] = {
{  0 , 4 ,     0 ,     0 ,     0 ,     0 , "CHN"      , "Channel",{{ 0 , "Channel 1" },}},
{  4 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 3 , "Query command" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D2050X_CASE3 ={ D2050X_CMD3 , "CMD 3 - Query Position and Angle" , "This command requests the actuator to return a  reply  command." } ;

// TITLE:CMD 4 - Reply Position and Angle
// DESC :Either upon request ( Query  command) or after an internal trigger (see EEP Properties) the actuator sends this command to inform about its current state.
enocean:: T_DATAFIELD D2050X_CMD4 [] = {
{  1 , 7 ,     0 ,     0 ,     0 ,     0 , "POS"      , "Position",{}},
{  9 , 7 ,     0 ,     0 ,     0 ,     0 , "ANG"      , "Angle",{}},
{ 21 , 3 ,     0 ,     0 ,     0 ,     0 , "LOCK"     , "Locking modes",{{ 0 , "Normal (no lock)" },{ 1 , "Blockage mode" },{ 2 , "Alarm mode" },{ 3 , "Reserved" },}},
{ 24 , 4 ,     0 ,     0 ,     0 ,     0 , "CHN"      , "Channel",{{ 0 , "Channel 1" },}},
{ 28 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 4 , "Reply command" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D2050X_CASE4 ={ D2050X_CMD4 , "CMD 4 - Reply Position and Angle" , "Either upon request ( Query  command) or after an internal trigger (see EEP Properties) the actuator sends this command to inform about its current state." } ;

// TITLE:CMD 5 - Set parameters
// DESC :This command sets one or multiple configuration parameters of the actuator. When a parameter value is set to  -> no change  this parameter will not be modified. The VERT and ROT parameters describe the duration needed by the motor for a full run of the blind, or for a complete turn of the slats, respectively. They have to be measured on site and assigned to the actuator.
enocean:: T_DATAFIELD D2050X_CMD5 [] = {
{  1 ,15 ,     0 ,     0 ,     0 ,     0 , "VERT"     , "Set vertical",{}},
{ 16 , 8 ,     0 ,     0 ,     0 ,     0 , "ROT"      , "Set rotation",{}},
{ 29 , 3 ,     0 ,     0 ,     0 ,     0 , "AA"       , "Set alarm action",{{ 0 , "No action" },{ 1 , "Immediate stop" },{ 2 , "Go up (0%)" },{ 3 , "Go down (100%)" },{ 4 , "Reserved" },{ 7 , "-> No change" },}},
{ 32 , 4 ,     0 ,     0 ,     0 ,     0 , "CHN"      , "Channel",{{ 0 , "Channel 1" },}},
{ 36 , 4 ,     0 ,     0 ,     0 ,     0 , "CMD"      , "Command ID",{{ 5 , "Set parameters command" },}},
{  0 , 0 , 0          , 0           }
};

enocean::T_EEP_CASE_ D2050X_CASE5 ={ D2050X_CMD5 , "CMD 5 - Set parameters" , "This command sets one or multiple configuration parameters of the actuator. When a parameter value is set to  -> no change  this parameter will not be modified. The VERT and ROT parameters describe the duration needed by the motor for a full run of the blind, or for a complete turn of the slats, respectively. They have to be measured on site and assigned to the actuator." } ;

enocean:: T_EEP_CASE_* D2050X_CASES [] = {
&D2050X_CASE1 ,
&D2050X_CASE2 ,
&D2050X_CASE3 ,
&D2050X_CASE4 ,
&D2050X_CASE5 ,
{0 }
};


// Profils list 
T_PROFIL_LIST Profillist [] = {
{ 0xD20100, 0xD2 , 0x01, 0x00, D20100_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x00                                                                       " },
{ 0xD20101, 0xD2 , 0x01, 0x01, D20101_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x01 (description: see table)                                              " },
{ 0xD20102, 0xD2 , 0x01, 0x02, D20102_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x02 (description: see table)                                              " },
{ 0xD20103, 0xD2 , 0x01, 0x03, D20103_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x03 (description: see table)                                              " },
{ 0xD20104, 0xD2 , 0x01, 0x04, D20104_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x04 (description: see table)                                              " },
{ 0xD20105, 0xD2 , 0x01, 0x05, D20105_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x05 (description: see table)                                              " },
{ 0xD20106, 0xD2 , 0x01, 0x06, D20106_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x06 (description: see table)                                              " },
{ 0xD20107, 0xD2 , 0x01, 0x07, D20107_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x07 (description: see table)                                              " },
{ 0xD20108, 0xD2 , 0x01, 0x08, D20108_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x08 (description: see table)                                              " },
{ 0xD20109, 0xD2 , 0x01, 0x09, D20109_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x09 (description: see table)                                              " },
{ 0xD2010A, 0xD2 , 0x01, 0x0A, D2010A_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0A (description: see table)                                              " },
{ 0xD2010B, 0xD2 , 0x01, 0x0B, D2010B_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0B (description: see table)                                              " },
{ 0xD2010C, 0xD2 , 0x01, 0x0C, D2010C_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0C                                                                       " },
{ 0xD2010D, 0xD2 , 0x01, 0x0D, D2010D_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0D                                                                       " },
{ 0xD2010E, 0xD2 , 0x01, 0x0E, D2010E_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0E                                                                       " },
{ 0xD2010F, 0xD2 , 0x01, 0x0F, D2010F_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x0F                                                                       " },
{ 0xD20110, 0xD2 , 0x01, 0x10, D20110_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x10 (description: see table)                                              " },
{ 0xD20111, 0xD2 , 0x01, 0x11, D20111_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x11 (description: see table)                                              " },
{ 0xD20112, 0xD2 , 0x01, 0x12, D20112_CASES ,13, "Electronic switches and dimmers with Energy Measurement and Local Control       " , "Type 0x12                                                                       " },
{ 0xD20500, 0xD2 , 0x05, 0x00, D2050X_CASES ,5, "Blinds Control for Position and Angle                                           " , "Type 0x00                                                                       " },
{0,0,0,0,0,0,"","" }
};

