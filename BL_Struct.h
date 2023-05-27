/////////////////////////////////////////////////////////////////
// BinLoader.h : interface of the CBinLoader class
//
//	Description:
//		
//
//	Date:		12 August 2004
//	Revision:	1.00
//
//	By:			
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., .
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once
#ifndef _BL_STRUCT_H_
#define _BL_STRUCT_H_

// the usage element for magazine
enum
{
    BL_MGZN_USAGE_UNUSE = 0,
    BL_MGZN_USAGE_FULL,
    BL_MGZN_USAGE_EMPTY,
    BL_MGZN_USAGE_ACTIVE,
    BL_MGZN_USAGE_MIXED
};

// magazine state, normal, full, empty
enum
{
    BL_MGZN_STATE_OK=0,
    BL_MGZN_STATE_FULL,
    BL_MGZN_STATE_EMPTY
};

enum // operation mode
{
    BL_MODE_A=0,
    BL_MODE_B,
    BL_MODE_C,
    BL_MODE_D,
    BL_MODE_E,			//125 bin config			//v3.45
	BL_MODE_F,			//MS100 8mag 4x2 150bins config		//v3.82
	BL_MODE_G,			//MS100 8mag 4x2 175bins config		//v3.82
	BL_MODE_H			//MS100Plus 4mag 4x1 100bins config	//v4.31T10	//Yearly MS100Plus
};

enum // magazine id
{
    BL_MGZN_TOP1 = 0,
    BL_MGZN_MID1,
    BL_MGZN_BTM1,
    BL_MGZN_TOP2,
    BL_MGZN_MID2,
    BL_MGZN_BTM2,
	BL_MGZN8_BTM1,		//MS100 8mag 4x2 config		//v3.82
	BL_MGZN8_BTM2		//MS100 8mag 4x2 config		//v3.82
};

//v4.39T7	//Defined in MS896aStn base class instead becuase it may be accessed by BT
/*
// definition for magazine data struct
typedef struct
{
    LONG    m_lTopLevel;
    LONG    m_lTopLevel2;					//New for MS100 9Inch Dual-table design
    LONG    m_lMidPosnY;
    LONG    m_lMidPosnY2;					//New for MS100 9Inch Dual-table design
    LONG    m_lSlotPitch;
    LONG    m_lNoOfSlots;					// not really used indeed, only get/set
    LONG    m_lMgznState;					// state can be ok=0, full=1, empty = 2.
    LONG    m_lMgznUsage;					// this magazine is used as full, empty or active or mixed or unused
    LONG    m_lSlotBlock[MS_BL_MAX_MGZN_SLOT];	// -1, no bin frame, 0 no die bin frame, 1--... bonded 
    LONG    m_lSlotUsage[MS_BL_MAX_MGZN_SLOT];	// full, empty, active(buffer)
    CString m_SlotBCName[MS_BL_MAX_MGZN_SLOT];	// barcode name(with ext)

} BL_MAGAZINE;
*/
//BL as WaferLoader for Yealy MS100Plus		//v4.31T11
typedef struct
{
	LONG	m_lMgznNo;
    LONG    m_lTopLevel_Z;
    LONG    m_lMidPosnY;
    LONG    m_lNoOfSlots;	
    DOUBLE  m_dSlotPitch;
    //CString m_SlotBCName[MS_BL_MAX_MGZN_SLOT];	
   
} BL_WL_MAGAZINE;


typedef struct 
{
	LONG lMgzNo;
	LONG lSlotNo;
	LONG lDieCount;
}FRAME_DETAILS ;

#endif