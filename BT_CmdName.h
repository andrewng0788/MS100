//////////////////////////////////////////////////////////////////////////////
//	BT_CmdName.h : Defines the HMI registered command for BinTable class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, Nov 10, 2004
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

/* Define the IPC Service Commands */
// Move
#define	BT_X_MoveToCmd		_T("X_MoveToCmd")
#define	BT_X_MoveCmd		_T("X_MoveCmd")
#define	BT_Y_MoveToCmd		_T("Y_MoveToCmd")
#define	BT_Y_MoveCmd		_T("Y_MoveCmd")
#define	BT_XY_MoveToCmd		_T("XY_MoveToCmd")
#define	BT_XY_MoveCmd		_T("XY_MoveCmd")

// Home
#define	BT_X_HomeCmd		_T("X_HomeCmd")
#define	BT_Y_HomeCmd		_T("Y_HomeCmd")
#define	BT_XY_HomeCmd		_T("XY_HomeCmd")

// Get Encoder
#define	BT_GetEncoderCmd	_T("GetEncoderCmd")

// Joystick
#define	BT_SetJoystickCmd		_T("SetJoystickCmd")
#define	BT_SetJoystickSpeedCmd	_T("SetJoystickSpeedCmd")

