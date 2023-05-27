//////////////////////////////////////////////////////////////////////////////
//	MS_HostCommConstant.h : Defines the Constant header
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Wednesday, Nov 16, 2005
//	Revision:	1.00
//
//	By:			Barry Chu
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once


#define MS_HCOM_CMD_RX_MCCHECK			"IdCheck"		//CTM send to App
#define MS_HCOM_CMD_ACK_MCCHECK			"10"			//App send to CTM
#define MS_HCOM_CMD_RX_LOTNO			"11"			//CTM send to App
#define MS_HCOM_CMD_ACK_LOTNO			"13"			//App send to CTM
#define MS_HCOM_CMD_TX_WAFEREND			"12"			//App send to CTM
#define MS_HCOM_CMD_ACK_WAFEREND		"14"			//CTM send to App 
#define MS_HCOM_CMD_TX_BINFULL			"15"			//App send to CTM
#define MS_HCOM_CMD_ACK_BINFULL			"16"			//CTM send to App 



////////////////////////////////////////////
//	Definitions of Config file
////////////////////////////////////////////

#define MS_HCOM_GENERAL					"Setting"
#define MS_HCOM_GENERAL_MCID			"Machine ID"
#define MS_HCOM_GENERAL_IPADDR			"IP Address"
#define MS_HCOM_GENERAL_PORTNO			"Port No"
#define MS_HCOM_GENERAL_LOTID			"Lot ID"
#define MS_HCOM_GENERAL_AUTO_CONNECT	"Auto Connect"
#define MS_HCOM_GENERAL_ENABLE_COMM		"Enable Connection"
#define MS_HCOM_GENERAL_AUTO_RETRY		"Auto Re-Connect"



#define MS_HCOM_DATA					"Data"
#define MS_HCOM_DATA_CONTENT			"Content"
#define MS_HCOM_DATA_SENDOK				"Send OK"
