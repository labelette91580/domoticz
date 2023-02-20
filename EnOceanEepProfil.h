// function number :0x01 : Electronic switches and dimmers with Energy Measurement and Local Control 
// function type :0x00 : Type 0x00 

// TITLE:CMD 0x1 - Actuator Set Output
extern enocean:: T_DATAFIELD D20100_CMD1 [] ;
// Index of field
#define D20100_CMD1_CMD        0
#define D20100_CMD1_DV         1
#define D20100_CMD1_I_O        2
#define D20100_CMD1_OV         3
#define D20100_CMD1_NB_DATA    4
#define D20100_CMD1_DATA_SIZE  3

// TITLE:CMD 0x2 - Actuator Set Local
extern enocean:: T_DATAFIELD D20100_CMD2 [] ;
// Index of field
#define D20100_CMD2_d_e        0
#define D20100_CMD2_CMD        1
#define D20100_CMD2_OC         2
#define D20100_CMD2_RO         3
#define D20100_CMD2_LC         4
#define D20100_CMD2_I_O        5
#define D20100_CMD2_DT3        6
#define D20100_CMD2_DT1        7
#define D20100_CMD2_DT2        8
#define D20100_CMD2_d_n        9
#define D20100_CMD2_PF         10
#define D20100_CMD2_DS         11
#define D20100_CMD2_NB_DATA    12
#define D20100_CMD2_DATA_SIZE  4

// TITLE:CMD 0x3 - Actuator Status Query
extern enocean:: T_DATAFIELD D20100_CMD3 [] ;
// Index of field
#define D20100_CMD3_CMD        0
#define D20100_CMD3_I_O        1
#define D20100_CMD3_NB_DATA    2
#define D20100_CMD3_DATA_SIZE  2

// TITLE:CMD 0x4 - Actuator Status Response
extern enocean:: T_DATAFIELD D20100_CMD4 [] ;
// Index of field
#define D20100_CMD4_PF         0
#define D20100_CMD4_PFD        1
#define D20100_CMD4_CMD        2
#define D20100_CMD4_OC         3
#define D20100_CMD4_EL         4
#define D20100_CMD4_I_O        5
#define D20100_CMD4_LC         6
#define D20100_CMD4_OV         7
#define D20100_CMD4_NB_DATA    8
#define D20100_CMD4_DATA_SIZE  3

// TITLE:CMD 0x5 - Actuator Set Measurement
extern enocean:: T_DATAFIELD D20100_CMD5 [] ;
// Index of field
#define D20100_CMD5_CMD        0
#define D20100_CMD5_RM         1
#define D20100_CMD5_RE         2
#define D20100_CMD5_e_p        3
#define D20100_CMD5_I_O        4
#define D20100_CMD5_MD_LSB     5
#define D20100_CMD5_MD_MSB     6
#define D20100_CMD5_UN         7
#define D20100_CMD5_MAT        8
#define D20100_CMD5_MIT        9
#define D20100_CMD5_NB_DATA    10
#define D20100_CMD5_DATA_SIZE  6

// TITLE:CMD 0x6 - Actuator Measurement Query
extern enocean:: T_DATAFIELD D20100_CMD6 [] ;
// Index of field
#define D20100_CMD6_CMD        0
#define D20100_CMD6_qu         1
#define D20100_CMD6_I_O        2
#define D20100_CMD6_NB_DATA    3
#define D20100_CMD6_DATA_SIZE  2

// TITLE:CMD 0x7 - Actuator Measurement Response
extern enocean:: T_DATAFIELD D20100_CMD7 [] ;
// Index of field
#define D20100_CMD7_CMD        0
#define D20100_CMD7_UN         1
#define D20100_CMD7_I_O        2
#define D20100_CMD7_MV         3
#define D20100_CMD7_NB_DATA    4
#define D20100_CMD7_DATA_SIZE  6

// TITLE:CMD 0x8 - Actuator Set Pilot Wire Mode
extern enocean:: T_DATAFIELD D20100_CMD8 [] ;
// Index of field
#define D20100_CMD8_CMD        0
#define D20100_CMD8_PM         1
#define D20100_CMD8_NB_DATA    2
#define D20100_CMD8_DATA_SIZE  2

// TITLE:CMD 0x9 - Actuator Pilot Wire Mode Query
extern enocean:: T_DATAFIELD D20100_CMD9 [] ;
// Index of field
#define D20100_CMD9_CMD        0
#define D20100_CMD9_NB_DATA    1
#define D20100_CMD9_DATA_SIZE  1

// TITLE:CMD 0xA - Actuator Pilot Wire Mode Response
extern enocean:: T_DATAFIELD D20100_CMD10 [] ;
// Index of field
#define D20100_CMD10_CMD        0
#define D20100_CMD10_PM         1
#define D20100_CMD10_NB_DATA    2
#define D20100_CMD10_DATA_SIZE  2

// TITLE:CMD 0xB - Actuator Set External Interface Settings
extern enocean:: T_DATAFIELD D20100_CMD11 [] ;
// Index of field
#define D20100_CMD11_CMD        0
#define D20100_CMD11_I_O        1
#define D20100_CMD11_AOT        2
#define D20100_CMD11_DOT        3
#define D20100_CMD11_EBM        4
#define D20100_CMD11_SWT        5
#define D20100_CMD11_NB_DATA    6
#define D20100_CMD11_DATA_SIZE  7

// TITLE:CMD 0xC - Actuator External Interface Settings Query
extern enocean:: T_DATAFIELD D20100_CMD12 [] ;
// Index of field
#define D20100_CMD12_CMD        0
#define D20100_CMD12_I_O        1
#define D20100_CMD12_NB_DATA    2
#define D20100_CMD12_DATA_SIZE  2

// TITLE:CMD 0xD - Actuator External Interface Settings Response
extern enocean:: T_DATAFIELD D20100_CMD13 [] ;
// Index of field
#define D20100_CMD13_CMD        0
#define D20100_CMD13_I_O        1
#define D20100_CMD13_AOT        2
#define D20100_CMD13_DOT        3
#define D20100_CMD13_EBM        4
#define D20100_CMD13_SWT        5
#define D20100_CMD13_NB_DATA    6
#define D20100_CMD13_DATA_SIZE  7

extern enocean:: T_EEP_CASE_* D20100_CASES [] ;
// function type :0x01 : Type 0x01 (description: see table) 

extern enocean:: T_EEP_CASE_* D20101_CASES [] ;
// function type :0x02 : Type 0x02 (description: see table) 

extern enocean:: T_EEP_CASE_* D20102_CASES [] ;
// function type :0x03 : Type 0x03 (description: see table) 

extern enocean:: T_EEP_CASE_* D20103_CASES [] ;
// function type :0x04 : Type 0x04 (description: see table) 

extern enocean:: T_EEP_CASE_* D20104_CASES [] ;
// function type :0x05 : Type 0x05 (description: see table) 

extern enocean:: T_EEP_CASE_* D20105_CASES [] ;
// function type :0x06 : Type 0x06 (description: see table) 

extern enocean:: T_EEP_CASE_* D20106_CASES [] ;
// function type :0x07 : Type 0x07 (description: see table) 

extern enocean:: T_EEP_CASE_* D20107_CASES [] ;
// function type :0x08 : Type 0x08 (description: see table) 

extern enocean:: T_EEP_CASE_* D20108_CASES [] ;
// function type :0x09 : Type 0x09 (description: see table) 

extern enocean:: T_EEP_CASE_* D20109_CASES [] ;
// function type :0x0A : Type 0x0A (description: see table) 

extern enocean:: T_EEP_CASE_* D2010A_CASES [] ;
// function type :0x0B : Type 0x0B (description: see table) 

extern enocean:: T_EEP_CASE_* D2010B_CASES [] ;
// function type :0x0C : Type 0x0C 

extern enocean:: T_EEP_CASE_* D2010C_CASES [] ;
// function type :0x0D : Type 0x0D 

extern enocean:: T_EEP_CASE_* D2010D_CASES [] ;
// function type :0x0E : Type 0x0E 

extern enocean:: T_EEP_CASE_* D2010E_CASES [] ;
// function type :0x0F : Type 0x0F 

extern enocean:: T_EEP_CASE_* D2010F_CASES [] ;
// function type :0x10 : Type 0x10 (description: see table) 

extern enocean:: T_EEP_CASE_* D20110_CASES [] ;
// function type :0x11 : Type 0x11 (description: see table) 

extern enocean:: T_EEP_CASE_* D20111_CASES [] ;
// function type :0x12 : Type 0x12 

extern enocean:: T_EEP_CASE_* D20112_CASES [] ;
// function number :0x05 : Blinds Control for Position and Angle 
// function type :0x00 : Type 0x00 

// TITLE:CMD 1 - Go to Position and Angle
extern enocean:: T_DATAFIELD D20500_CMD1 [] ;
// Index of field
#define D20500_CMD1_POS        0
#define D20500_CMD1_ANG        1
#define D20500_CMD1_REPO       2
#define D20500_CMD1_LOCK       3
#define D20500_CMD1_CHN        4
#define D20500_CMD1_CMD        5
#define D20500_CMD1_NB_DATA    6
#define D20500_CMD1_DATA_SIZE  4

// TITLE:CMD 2 - Stop
extern enocean:: T_DATAFIELD D20500_CMD2 [] ;
// Index of field
#define D20500_CMD2_CHN        0
#define D20500_CMD2_CMD        1
#define D20500_CMD2_NB_DATA    2
#define D20500_CMD2_DATA_SIZE  1

// TITLE:CMD 3 - Query Position and Angle
extern enocean:: T_DATAFIELD D20500_CMD3 [] ;
// Index of field
#define D20500_CMD3_CHN        0
#define D20500_CMD3_CMD        1
#define D20500_CMD3_NB_DATA    2
#define D20500_CMD3_DATA_SIZE  1

// TITLE:CMD 4 - Reply Position and Angle
extern enocean:: T_DATAFIELD D20500_CMD4 [] ;
// Index of field
#define D20500_CMD4_POS        0
#define D20500_CMD4_ANG        1
#define D20500_CMD4_LOCK       2
#define D20500_CMD4_CHN        3
#define D20500_CMD4_CMD        4
#define D20500_CMD4_NB_DATA    5
#define D20500_CMD4_DATA_SIZE  4

// TITLE:CMD 5 - Set parameters
extern enocean:: T_DATAFIELD D20500_CMD5 [] ;
// Index of field
#define D20500_CMD5_VERT       0
#define D20500_CMD5_ROT        1
#define D20500_CMD5_AA         2
#define D20500_CMD5_CHN        3
#define D20500_CMD5_CMD        4
#define D20500_CMD5_NB_DATA    5
#define D20500_CMD5_DATA_SIZE  5

extern enocean:: T_EEP_CASE_* D20500_CASES [] ;
