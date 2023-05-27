//////////////////////////////////////////////////////////////////////////////
//	SC_Constant.h : Defines the Constant for SecsComm class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Saturday, Sep 3, 2005
//	Revision:	1.00
//
//	By:			Xu Zhi jin
//				
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#pragma once
//** For SEC/GEM Constants for CSecsComm station class member fcns
#ifndef _HOSTCOM_H
#define _HOSTCOM_H

#include "GemConstant.h"

// macros for string map file element
#define SG_DATA         _T("sgconfig")
#define SG_HOSTIP       _T("sgHostIP")
#define SG_AUTOSTART    _T("sgAutoStart")
#define SG_EQUIP_ID		_T("EquipId")			//aaa111
#define SG_COMMPORT     _T("sgCommPort")
#define SG_CONNMODE     _T("sgConnMode")
#define SG_CONVTIME     _T("sgConvTime")
#define SG_HSMST3       _T("sgHsmsT3")
#define SG_HSMST5       _T("sgHsmsT5")
#define SG_HSMST6       _T("sgHsmsT6")
#define SG_HSMST7       _T("sgHsmsT7")
#define SG_HSMST8       _T("sgHsmsT8")
#define SG_HSMSLT       _T("sgHsmsLT")

#define SG_SECSONLINE	_T("sgSECSOnline")
#define SG_CTRLMODE		_T("sgControlMode")

#define SG_CYCLIC_REPORT_NOTIFY_INTERVAL	_T("sgCReportNotifyInterval")
#define SG_CYCLIC_REPORT_TIME_HOUR			_T("sgCReportLastNotifyTimeInHour")
#define SG_CYCLIC_REPORT_TIME_MINUTE		_T("sgCReportLastNotifyTimeInMinute")

// macros for command of call back by secs gem library
#define SG_CB_TIME_OUT                          _T("SG_TimeOut")
#define SG_CB_REC_SEL_EQUIP_CONST				_T("SG_ReceiveSelectEquipConst")
#define SG_CB_REC_SEL_EQUIP_STATUS				_T("SG_ReceiveSelectEquipStatus")
#define SG_CB_REC_REQ_STATUS_NM_LIST			_T("SG_ReceiveReqestStatusNameList")
#define SG_CB_REC_REQ_EC_NM_LIST				_T("SG_ReceiveReqestEquipConstNameList")
#define SG_CB_REC_REQ_EQUIP_CONST				_T("SG_ReceiveReqestEquipConst")
#define SG_CB_EQUIP_CONST_UPDATE                _T("SG_UpdateConst")
#define SG_CB_REMOTE_COMMAND_RECEIVE            _T("SG_UpdateRc")
#define SG_CB_EVENT_REPORT_DEFINE               _T("SG_DefineEvent")
#define SG_CB_REQUEST_HOST_TIME                 _T("SG_HostTime")
#define SG_CB_REQUEST_ON_OFF_LINE               _T("SG_ReqOnOffLine")
#define SG_CB_DEFINE_REPORT                     _T("SG_DefineReport")
#define SG_CB_DATE_TIME_CHANGE                  _T("SG_UpdateTime")
#define SG_CB_ENABLE_DISABLE_ALARM              _T("SG_EnableAlarm")
#define SG_CB_ENABLE_DISABLE_EVENT              _T("SG_EnableEvent")
#define SG_CB_TERMINAL_SERVICE_RECEIVE          _T("SG_TerminalMsg")
#define SG_CB_TERMINAL_MB_MSG_RECEIVE           _T("SG_TermMbMsg")
#define SG_CB_EVENT_ACK_RECEIVE                 _T("SG_EventRecAck")
#define SG_CB_SECS_COMM_STATE                   _T("SG_CommState")
#define SG_CB_HOST_COMMAND_RECEIVE              _T("SG_UpdateHc")
#define SG_CB_ALARM_ACK_RECEIVE                 _T("SG_HostReceiveAlarmAck")
#define SG_CB_REC_DOWNLOAD_PP_ACK               _T("SG_ReceiveDownLoadPPAck")
#define SG_CB_UPLOAD_PROCESS_PROG               _T("SG_HostRequestPPG")
#define SG_CB_DELETE_PROCESS_PROG               _T("SG_DeletePP")
#define SG_CB_DIRECTORY_PROCESS_PROG            _T("SG_DirectoryPP")
#define SG_CB_REC_REQ_EPPD                      _T("SG_ReceiveRequestEPPD")
#define SG_CB_REC_DELETE_PPID_ACK               _T("SG_ReceiveDeletePPIDAck")
#define SG_CB_REC_PP_REQ                        _T("SG_ReceivePPRquest")
#define SG_CB_DOWNLOAD_PROCESS_PROG             _T("SG_HostSendPPG")
#define SG_CB_HOST_REQ_CEID_REC                 _T("SG_HostReqEvent")
#define SG_CB_REC_TERMINAL_DISP_ACK             _T("SG_TerminalMsgAck")

#define SG_CB_REC_MAP_DATA_ACK					_T("SG_MapDataAck")
#define SG_CB_REC_MAP_DATA_ACK2					_T("SG_MapDataAck2")
#define	SG_CB_REC_MAP_SETUP_DATA				_T("SG_MapSetupData")
#define SG_CB_REC_MAP_DATA_TYPE1                _T("SG_DownLoadType1Map")
#define SG_CB_REC_MAP_DATA_TYPE2                _T("SG_DownLoadType2Map")
#define SG_CB_REC_MAP_DATA_TYPE3                _T("SG_DownLoadType3Map")
#define SG_CB_REQ_GRANT_DN_PP                   _T("SG_ReqGrantDnPp")
#define SG_CB_REC_ENABLE_ALARM_ACK              _T("SG_ReceiveEnableAlarmAck")
#define SG_CB_REC_ENABLE_EVENT_ACK              _T("SG_ReceiveEnableEventAck")
#define SG_CB_HOST_REQ_REPORT_REC               _T("SG_ReceiveHostRequestReport")
#define SG_CB_SPOOL_ACTIVATE                    _T("SG_SpoolActivate")
#define SG_CB_SPOOL_DEACTIVATE                  _T("SG_SpoolDeactivate")
//SECSGEM CallBack commands for Lexter
#define SG_CB_LOAD_UNLOAD_PORT_STATUS_QY		_T("SG_LoadUnloadPortStatusQy")
#define SG_CB_REQ_ONLINE_COM					_T("SG_RequestOnlineCom")
#define SG_CB_ONLINE_SUM_STATUS_RP_REPLY		_T("SG_OnlineSumStatusRpReply")
#define SG_CB_CAS_LOAD_COMPLETE_RP_REPLY		_T("SG_CasLoadCompleteRpReply")
#define SG_CB_CAS_UNLOAD_REQ_COM_RP_REPLY		_T("SG_CasUnloadReqComRpReply")
#define SG_CB_EQU_STATUS_CHANGE_RP_REPLY		_T("SG_EquStatusChangeRpReply")
#define SG_CB_LOT_PROCESS_DATA_TRANSFER			_T("SG_LotProcessDataTransfer")
#define SG_CB_LOT_STATUS_CHANGE_RP_REPLY		_T("SG_LotStatusChangeRpReply")
#define SG_CB_WAFER_PROCESS_DATA_RP_REPLY		_T("SG_WaferProcessDataRpReply")
#define SG_CB_LOT_PROCESS_DATA_RP_REPLY			_T("SG_LotProcessDataRpReply")
#define SG_CB_ALARM_OCC_REM_RP_REPLY			_T("SG_AlarmOccRemRpReply")
#define SG_CB_REPLY_LOT_PROCESS_DATA_REQ		_T("SG_ReplyLotProcessDataReq")
#define SG_CB_LOT_CANCEL_REQ_REPLY				_T("SG_LotCancelReqReply")
#define SG_CB_ONLINE_REPLY						_T("SG_OnlineReply")

// macros for variable of secs gem registe
#define SG_SECS_NAME_ECID				    _T("ECID")
#define SG_SECS_NAME_ALARMID			    _T("AlarmID")				//ULONG		//#12
#define SG_SECS_NAME_ALARMCOUNT			    _T("AlarmCount")			//UCHAR		//#13
#define SG_SECS_NAME_ALARMCLOCK				_T("Alarm Clock")			//STRING	//#18
#define SG_SECS_NAME_ALARMSET				_T("AlarmsSet")				//LIST		//#19
//OsramTrip 8/22
#define SG_SECS_NAME_ALARM_SIGNAL_RED		_T("AlarmSignalRed")		//UCHAR		//#21
#define SG_SECS_NAME_ALARM_SIGNAL_YELLOW	_T("AlarmSignalYellow")		//UCHAR		//#22
#define SG_SECS_NAME_ALARM_SIGNAL_GREEN		_T("AlarmSignalGreen")		//UCHAR		//#23
#define SG_SECS_NAME_ALARM_SIGNAL_BUZZER	_T("AlarmSignalBuzzer")		//UCHAR		//#24

#define SG_SECS_NAME_COMM_TIMER		        _T("ESTABLISH_COMM")
#define SG_SECS_NAME_SPOOL_START_TM	        _T("SpoolStartTime")
#define SG_SECS_NAME_SPOOL_FULL_TM		    _T("SpoolFullTime")
#define SG_SECS_NAME_MAX_SPOOL_TRANSMIT     _T("MAX_SPOOL_TRANSMIT")
#define SG_SECS_NAME_OVER_WRITE_SPOOL	    _T("OVER_WRITE_SPOOL")
#define SG_SECS_NAME_SPOOL_COUNT_TOTAL      _T("SpoolCountTotal")
#define SG_SECS_NAME_SPOOL_COUNT_ACTUAL     _T("SpoolCountActual")
#define SG_SECS_NAME_PROCESS_STATE		    _T("ProcessState")
#define SG_SECS_NAME_TIME_FORMAT		    _T("TIMEFORMAT")
#define SG_SECS_NAME_ALM_LIST			    _T("AlarmSet")
#define SG_SECS_NAME_EVENT_ENABLE_LIST      _T("EventsEnabled")
#define SG_SECS_NAME_ALM_ENABLE_LIST	    _T("AlarmsEnabled")
#define SG_SECS_NAME_CLOCK				    _T("Clock")
#define SG_SECS_NAME_LIMIT_TRANSITION_TYPE  _T("TransitionType")
#define SG_SECS_NAME_LIMIT_VARIABLE	        _T("LimitVariable")
#define SG_SECS_NAME_LIMIT_LIST		        _T("EventLimit")		
#define SG_SECS_NAME_ALARM_CLOCK		    _T("AlarmClock")
#define SG_SECS_NAME_SPOOL_ENABLE		    _T("SpoolEnable")
#define SG_SECS_NAME_CONTROL_STATE		    _T("ControlState")
//OsramTrip 8/22
#define SG_SECS_NAME_PRE_CONTROL_STATE		_T("PreviousControlState")
#define SG_SECS_NAME_CONTROL_SUBSTATE		_T("ControlSubState")
#define SG_SECS_NAME_PRE_CONTROL_SUBSTATE	_T("PreviousControlSubState")



//Host Command Parameter Error Definition
//1 = Parameter Name (CPNAME) does not exist	
//2 = Illegal Value specified for CPVAL	
//3 = Illegal Format specified for CPVAL	
//4 = Parameter Name is 	not desired for equipment
//5 = Parameter Value is 	not desired for equipment
#define HCMD_PARA_SUCC					0
#define HCMD_PARA_EMPTY					1
#define HCMD_PARA_CPVAL_ILLEGAL_VALUE	2
#define HCMD_PARA_CPVAL_ILLEGAL_FORMAT	3
#define HCMD_PARA_CPNAME_NOT_DESIRED	4
#define HCMD_PARA_CPVAL_NOT_DESIRED		5
#define HCMD_PARA_CPNAME_EMPTY			6
#define HCMD_PARA_OTHER_ERROR			7



//const	UCHAR	HCACK_COMPLETED					= 0x0;
//const	UCHAR	HCACK_COMMAND_NOT_EXIST			= 0x1;
//const	UCHAR	HCACK_CANNOT_PERFORM_NOW		= 0x2;
//const	UCHAR	HCACK_PARA_INVALID				= 0x3; // At least on parameter invalid
//const	UCHAR	HCACK_WAIT_COMPLETION_SIGNAL	= 0x4;
//const	UCHAR	HCACK_REJECT_ALDY_IN_DESIRED	= 0x5;
//const	UCHAR	HCACK_OBJECT_NOT_EXIST			= 0x6;

const UCHAR	HCACK_LOADER_RESPONSE_TIME_OUT			= 0x7;
const UCHAR	HCACK_LOADER_BACK_GATE_NOT_OPENED		= 0x8;
const UCHAR	HCACK_LOADER_BACK_GATE_NOT_CLOSED		= 0x9;
const UCHAR	HCACK_LOADER_PARA_CPVAL_NOT_DESIRED		= 0x0a;
const UCHAR	HCACK_LOADER_BACK_GATE_OTHER_ERR		= 0x0b;
#endif