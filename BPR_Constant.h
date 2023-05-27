//////////////////////////////////////////////////////////////////////////////
//	BT_Constant.h : Defines the Constant for BinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, August 13, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once

#ifndef _BPR_CONSTANT_H_
#define _BPR_CONSTANT_H_

#define BT_TLH_PATH						0
#define BT_TLV_PATH						1
#define BT_TRH_PATH						2
#define BT_TRV_PATH						3
#define BT_BLH_PATH						4			//v4.40T7
#define BT_BLV_PATH						5			//v4.40T7
#define BT_BRH_PATH						6			//v4.40T7
#define BT_BRV_PATH						7			//v4.40T7

enum BPR_REALIGN_CORNER
{
	REALIGN_UPPER_LEFT_CORNER = 0,
	REALIGN_UPPER_RIGHT_CORNER,
	REALIGN_LOWER_LEFT_CORNER,
	REALIGN_LOWER_RIGHT_CORNER
};

#endif
