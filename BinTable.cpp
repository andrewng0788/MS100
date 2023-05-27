/////////////////////////////////////////////////////////////////
// BinTable.cpp : interface of the CBinTable class
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
//	Copyright @ ASM Assembly Automation Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "HmiDataManager.h"
#include "BinTable.h"
#include "FileUtil.h"
#include "WaferEndFileFactory.h"
#include "WaferEndFactoryMap.h"
#include "WaferEndFileInterface.h"
#include "FlushMessageThread.h"
#include "MS_SecCommConstant.h"
#include "PrescanInfo.h"
#include "Bondhead.h"
#include "Utility.H"
#include "WaferMap.H"
#include "NGGrade.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBinTable, CMS896AStn)

CBinTable::CBinTable(): m_evDieReadyForBT(FALSE, TRUE,				"WtDieReadyForBTEvt"),
						m_evDieInfoRead(FALSE, TRUE,				"WtDieInfoReadEvt"),
						m_evBPRLatched(FALSE, TRUE,					"BprLatchedEvt"),
						m_evDieBonded(FALSE, TRUE,					"BhDieBondedEvt"),
						m_evBTReady(FALSE, TRUE,					"BtReadyEvt"),
						m_evBTStable(FALSE, TRUE,					"BtStableEvt"),
						m_evBHInit(FALSE, TRUE,						"BhInitEvt"),
						m_evBhTReady(FALSE, TRUE,					"BhTReadyEvt"),
						m_evBLOperate(FALSE, TRUE,					"BlOperateEvt"),
						m_evBLReady(FALSE, TRUE,					"BlReadyEvt"),
						m_evBTStartMoveWithoutDelay(FALSE, TRUE,	"BtStartMoveWithoutDelayEvt"),
						m_evBTStartMove(FALSE, TRUE,				"BtStartMoveEvt"),
						m_evBLAOperate(FALSE, TRUE,					"BlAOperateEvt"),
						m_evBLPreOperate(FALSE, TRUE,				"BlPreOperateEvt"),
						m_evBLPreChangeGrade(FALSE, TRUE,			"BlPreChangeGrade"),
						m_evBTCompensate(FALSE, TRUE,				"BTCompensate"),			//v4.52A16
						m_evWTStartToMoveForBT(FALSE, TRUE,			"WTStartToMoveForBT"),
						m_evNLOperate(FALSE, TRUE,					"NlOperateEvt"),
						m_evNLReady(FALSE, TRUE,					"NlReadyEvt"),
						m_evNLPreOperate(FALSE, TRUE,				"NlPreOperateEvt")
{
	InitVariable();
} //end CBinTable constructor

CBinTable::~CBinTable()
{
} //end CBinTable destructor


BOOL CBinTable::InitInstance()
{
	LONG lDynamicControlX	= 0;
	LONG lStaticControlX	= 0;
	LONG lDynamicControlY	= 0;
	LONG lStaticControlY	= 0;
	LONG lVelFilterGainX	= 0;
	LONG lVelFilterWeightX	= 0;
	LONG lVelFilterGainY	= 0;
	LONG lVelFilterWeightY	= 0;
	LONG lMotorDirectionX	= 0;
	LONG lMotorDirectionY	= 0;

	CMS896AStn::InitInstance();
	
	m_bUseLargeBinArea		= (BOOL)GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_LARGE_TRAVEL);
	m_bUseDualTablesOption	= FALSE;		

	// Get path from MS896A station
	m_bUseBackupPath		= FALSE;
	m_szOutputFilePath		= (*m_psmfSRam)["MS896A"]["OutputFilePath"];
	m_szBinOutputFileFormat = (*m_psmfSRam)["MS896A"]["OutputFileFormat"];
	m_szOutputFileBlueTapeNo = (*m_psmfSRam)["MS896A"]["OutputFileBlueTapeNo"]; // v4.51D10 Dicon

	m_szWaferEndPath		= (*m_psmfSRam)["MS896A"]["WaferEndFilePath"];
	m_szWaferEndFileFormat	= (*m_psmfSRam)["MS896A"]["WaferEndFileFormat"];

	m_bIfGenLabel			= CMS896AStn::m_bIfGenLabel;

	m_lX_ProfileType		= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X,	MS896A_CFG_CH_PROFILE_TYPE);
	m_lY_ProfileType		= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y,	MS896A_CFG_CH_PROFILE_TYPE);
	(*m_psmfSRam)["MS896A"]["BinTableX_ProfileType"] = m_lX_ProfileType;
	(*m_psmfSRam)["MS896A"]["BinTableY_ProfileType"] = m_lY_ProfileType;

	m_bEnable_T				= (BOOL) GetChannelInformation(MS896A_CFG_CH_BINTABLE_T, MS896A_CFG_CH_ENABLE);
	//m_bUseTEncoder		= (BOOL) GetChannelInformation(MS896A_CFG_CH_BINTABLE_T, MS896A_CFG_CH_ENABLE_ENCODER);	//v4.59A42
	m_bUseTEncoder			= TRUE;
	
	if (m_bEnable_T)
	{
		m_bSel_T = TRUE;
		(*m_psmfSRam)["BinTable"]["EnableT"] = TRUE;
	}
	else
	{
		(*m_psmfSRam)["BinTable"]["EnableT"] = FALSE;
	}

	InitOutputFile();		// Init output file related objects

	try
	{
		GetAxisInformation();	
		if (IsBinTableTHighResolution())
		{
			SetAxisMoveSrchProfileRatio(m_stBTAxis_T, 2);
		}
	
		if (m_fHardware && !m_bDisableBT)
		{
			/*if (CMS896AStn::m_ulJoyStickMode != MS899_JS_MODE_PR)
			{
				CMS896AStn::MotionSetJoystickConfig("Module0", "jsBinTableX", "iJoyLeft", "iJoyRight");
				CMS896AStn::MotionSetJoystickConfig("Module0", "jsBinTableY", "iJoyUp", "iJoyDown");

				CMS896AStn::MotionSetIncrementParam(BT_AXIS_X, "BT_JoySlow", 0x20, 0x10, 0x10, 0x10, &m_stBTAxis_X);
				CMS896AStn::MotionSetIncrementParam(BT_AXIS_Y, "BT_JoySlow", 0x20, 0x10, 0x10, 0x10, &m_stBTAxis_Y);

				CMS896AStn::MotionSetIncrementParam(BT_AXIS_X, "BT_JoyNormal", 0x100, 0x50, 0x50, 0x50, &m_stBTAxis_X);
				CMS896AStn::MotionSetIncrementParam(BT_AXIS_Y, "BT_JoyNormal", 0x100, 0x50, 0x50, 0x50, &m_stBTAxis_Y);

				CMS896AStn::MotionSetIncrementParam(BT_AXIS_X, "BT_JoyFast", 0x400, 0x200, 0x200, 0x200, &m_stBTAxis_X);
				CMS896AStn::MotionSetIncrementParam(BT_AXIS_Y, "BT_JoyFast", 0x400, 0x200, 0x200, 0x200, &m_stBTAxis_Y);

				CMS896AStn::MotionSelectJoystickConfig(BT_AXIS_X, "jsBinTableX", &m_stBTAxis_X);
				CMS896AStn::MotionSelectJoystickFactor(BT_AXIS_X, 20, &m_stBTAxis_X);
				CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X, -1000000, 1000000, &m_stBTAxis_X);		
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_X, "BT_JoyNormal", &m_stBTAxis_X);

				CMS896AStn::MotionSelectJoystickConfig(BT_AXIS_Y, "jsBinTableY", &m_stBTAxis_Y);
				CMS896AStn::MotionSelectJoystickFactor(BT_AXIS_Y, 20, &m_stBTAxis_Y);
				CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_Y, -1000000, 1000000, &m_stBTAxis_Y);
				CMS896AStn::MotionSelectIncrementParam(BT_AXIS_Y, "BT_JoyNormal", &m_stBTAxis_Y); 
			}*/

			//Get Channel attribute from config file
			/*lDynamicControlX	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_DYNA_CONTROL);				
			lStaticControlX		= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_STAT_CONTROL);				
			lVelFilterGainX		= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_VELF_GAIN);				
			lVelFilterWeightX	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_VELF_WEIGHT);				
			lDynamicControlY	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_DYNA_CONTROL);				
			lStaticControlY		= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_STAT_CONTROL);				
			lVelFilterGainY		= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_VELF_GAIN);				
			lVelFilterWeightY	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_VELF_WEIGHT);				
			lMotorDirectionX	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X, MS896A_CFG_CH_MOTOR_DIRECTION);				
			lMotorDirectionY	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y, MS896A_CFG_CH_MOTOR_DIRECTION);				
			*/

			//Setup Static, dynamic control type & velocity filter
			//if (lDynamicControlX != 0)
			//{
			//	CMS896AStn::MotionSelectControlType(BT_AXIS_X, lDynamicControlX, 2, &m_stBTAxis_X);
			//}

			//if (lStaticControlX != 0)
			//{
			//	CMS896AStn::MotionSelectControlType(BT_AXIS_X, lStaticControlX, 1, &m_stBTAxis_X);
			//}

			//if ((lVelFilterGainX != 0) && (lVelFilterWeightX != 0))
			//{
			//	CMS896AStn::MotionSetVelocityFilter(BT_AXIS_X, (SHORT)lVelFilterGainX, (SHORT)lVelFilterWeightX, &m_stBTAxis_X);
			//}

			//if (lDynamicControlY != 0)
			//{
			//	CMS896AStn::MotionSelectControlType(BT_AXIS_Y, lDynamicControlY, 2, &m_stBTAxis_Y);
			//}

			//if (lStaticControlY != 0)
			//{
			//	CMS896AStn::MotionSelectControlType(BT_AXIS_Y, lStaticControlY, 1, &m_stBTAxis_Y);
			//}

			//if ((lVelFilterGainY != 0) && (lVelFilterWeightY != 0))
			//{
			//	CMS896AStn::MotionSetVelocityFilter(BT_AXIS_Y, (SHORT)lVelFilterGainY, (SHORT)lVelFilterWeightY, &m_stBTAxis_Y);
			//}


			// Reverse motor direction if needed
			/*if (lMotorDirectionX == -1)
			{
				CMS896AStn::MotionSetEncoderDirection(BT_AXIS_X, HP_NEGATIVE_DIR, &m_stBTAxis_X);
				CMS896AStn::MotionReverseMotorDirection(BT_AXIS_X, HP_ENABLE, &m_stBTAxis_X);
			}

			if (lMotorDirectionY == -1)
			{
				CMS896AStn::MotionSetEncoderDirection(BT_AXIS_Y, HP_NEGATIVE_DIR, &m_stBTAxis_Y);
				CMS896AStn::MotionReverseMotorDirection(BT_AXIS_Y, HP_ENABLE, &m_stBTAxis_Y);
			}*/

			X_Profile(NORMAL_PROF);
			Y_Profile(NORMAL_PROF);

			if ( WaitBHInit(120000) == FALSE )
			{
				DisplayMessage("BinTable - Timeout when wait BondHead init ");
				return FALSE;
			}

			m_lTableXNegLimit	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X,  MS896A_CFG_CH_MIN_DISTANCE);
			m_lTableXPosLimit	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_X,  MS896A_CFG_CH_MAX_DISTANCE);
			m_lTableYNegLimit	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y,  MS896A_CFG_CH_MIN_DISTANCE);
			m_lTableYPosLimit	= GetChannelInformation(MS896A_CFG_CH_BINTABLE_Y,  MS896A_CFG_CH_MAX_DISTANCE);

			//For HMI display graphics
			//variable for bin table operation => convert into um
			//X Limit
			m_lXNegLimit = 0 - _round(((DOUBLE) m_lTableXPosLimit / m_dXResolution));
			m_lXPosLimit = 0 - _round(((DOUBLE) m_lTableXNegLimit / m_dXResolution));
			//Y Limit
			m_lYNegLimit = 0 - _round(((DOUBLE) m_lTableYPosLimit / m_dYResolution));
			m_lYPosLimit = 0 - _round(((DOUBLE) m_lTableYNegLimit / m_dYResolution));
			//CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, 999999, -999999, &m_stBTAxis_X);
			//CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, 999999, -999999, &m_stBTAxis_Y);

			X_Home();
			if (m_bHome_X)
			{
				Y_Home();
			}

			if (m_bEnable_T)
			{
				CMS896AStn::MotionEnableEncoder(BT_AXIS_T, &m_stBTAxis_T);
				//CMS896AStn::MotionSelectProfile(BT_AXIS_T, "mpfBinTableTNormal", &m_stBTAxis_T);
				//===========================================2018.5.4====================================
				T_Profile(LOW_PROF);
				//=======================================================================================
				T_Home();
			}

			//CMS896AStn::MotionEnableProtection(BT_AXIS_X, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_X);
			//CMS896AStn::MotionEnableProtection(BT_AXIS_Y, HP_LIMIT_SENSOR, FALSE, TRUE, &m_stBTAxis_Y);
			//CMS896AStn::MotionSelectSettlingControl(BT_AXIS_Y, &m_stBTAxis_Y, PL_SETTLING, 10);
			
			// Clear the last warning
			CMS896AStn::MotionSetLastWarning(BT_AXIS_X, HP_SUCCESS, &m_stBTAxis_X);
			CMS896AStn::MotionSetLastWarning(BT_AXIS_Y, HP_SUCCESS, &m_stBTAxis_Y);
		
		}
		else if (m_fHardware && m_bDisableBT)
		{
			m_bSel_X	= FALSE;	// Select X flag
			m_bSel_Y	= FALSE;	// Select Y flag
			m_bSel_T	= FALSE;	// Select T flag
			m_bSel_X2	= FALSE;	// Select X flag
			m_bSel_Y2	= FALSE;	// Select Y flag
		}

		m_oBinBlkMain.SetBinTable(this);	//	should pass when init instance
		m_WaferMapEvent.SetBinTable(this);
		m_bHardwareReady = TRUE;
		m_dBTStartTime = GetTime();
		m_lBTMoveDelayTime = 0;

		m_lCurXPosn = 0;
		m_lCurYPosn = 0;
		m_lCurX2Posn = 0;
		m_lCurY2Posn = 0;
	}
	catch (CAsmException e)
	{
		DisplayException(e);
	}
	return TRUE;
} //end InitInstance


INT CBinTable::ExitInstance()
{
	//SaveBinTableData();		// Save data to file before exit
	if (m_fHardware && !m_bDisableBT)
	{
		try 
		{
			CMS896AStn::MotionSync(BT_AXIS_X, 10000, &m_stBTAxis_X);
			CMS896AStn::MotionJoyStickOn(BT_AXIS_X, FALSE, &m_stBTAxis_X);
			CMS896AStn::MotionPowerOff(BT_AXIS_X, &m_stBTAxis_X);
			CMS896AStn::MotionClearError(BT_AXIS_X, HP_MOTION_ABORT, &m_stBTAxis_X);
		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		try 
		{
			CMS896AStn::MotionSync(BT_AXIS_Y, 10000, &m_stBTAxis_Y);
			CMS896AStn::MotionJoyStickOn(BT_AXIS_Y, FALSE, &m_stBTAxis_Y);
			CMS896AStn::MotionPowerOff(BT_AXIS_Y, &m_stBTAxis_Y);
			CMS896AStn::MotionClearError(BT_AXIS_Y, HP_MOTION_ABORT, &m_stBTAxis_Y);
		}
		catch (CAsmException e)
		{
			// Nothing can do ...
		}

		if (m_bEnable_T)
		{
			try 
			{
				CMS896AStn::MotionSync(BT_AXIS_T, 10000, &m_stBTAxis_T);
				CMS896AStn::MotionJoyStickOn(BT_AXIS_T, FALSE, &m_stBTAxis_T);
				CMS896AStn::MotionPowerOff(BT_AXIS_T, &m_stBTAxis_T);
				CMS896AStn::MotionClearError(BT_AXIS_T, HP_MOTION_ABORT, &m_stBTAxis_T);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}

		if (m_bUseDualTablesOption)	
		{
			try 
			{
				CMS896AStn::MotionSync(BT_AXIS_X2, 10000, &m_stBTAxis_X2);
				CMS896AStn::MotionJoyStickOn(BT_AXIS_X2, FALSE, &m_stBTAxis_X2);
				CMS896AStn::MotionPowerOff(BT_AXIS_X2, &m_stBTAxis_X2);
				CMS896AStn::MotionClearError(BT_AXIS_X2, HP_MOTION_ABORT, &m_stBTAxis_X2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}

			try 
			{
				CMS896AStn::MotionSync(BT_AXIS_Y2, 10000, &m_stBTAxis_Y2);
				CMS896AStn::MotionJoyStickOn(BT_AXIS_Y2, FALSE, &m_stBTAxis_Y2);
				CMS896AStn::MotionPowerOff(BT_AXIS_Y2, &m_stBTAxis_Y2);
				CMS896AStn::MotionClearError(BT_AXIS_Y2, HP_MOTION_ABORT, &m_stBTAxis_Y2);
			}
			catch (CAsmException e)
			{
				// Nothing can do ...
			}
		}
	}

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "CyOptics"|| pApp->GetCustomerName() == "FiberOptics")
	{
		m_oBinBlkMain.SaveLeaveEmptyRowData();						//v4.30T4	//CyOptics US
	}

	m_oBinBlkMain.SetBinTable(this);	//v4.51A17	//XM SanAn
	//AfxMessageBox("Exit", MB_SYSTEMMODAL);
	
	return CMS896AStn::ExitInstance();

} //end ExitInstance


BOOL CBinTable::InitData()
{
	CString szData;
	CString szBinBlkId;
	CString szTempFilename;
	UCHAR ucGrade = '0';
	CString szMsg;

	BOOL bBinLoader = IsNLEnable();	//IsBLEnable();		//andrewng //2020-1007
	BOOL bResetSerialNo = (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Reset Bin Serial No"];
	
	if (m_bMachineFirstStart == TRUE)
	{
		bResetSerialNo = TRUE;
		m_bMachineFirstStart = FALSE;
	}

	//if (bBinLoader) //1 = Standard; 2 = Loader; 3 = Dual-Buffer
	//{
		//BOOL bDualBufferConfig = (BOOL)(LONG)(*m_psmfSRam)["BinLoaderStn"]["DualBufferExist"];
		//if (bDualBufferConfig)
	m_ulMachineType = BT_MACHTYPE_DBUFFER;
		//else
		//	m_ulMachineType = BT_MACHTYPE_DL_DLA;

		//v4.16T5	//MS100 9Inch
		//BOOL bUseBT2 = (BOOL) GetChannelInformation(MS896A_CFG_CH_BINTABLE2_Y, MS896A_CFG_CH_ENABLE);
		//if (bUseBT2 && m_bUseDualTablesOption)
		//{
		//	m_ulMachineType = BT_MACHTYPE_DTABLE;
		//}
	//}
	//else
	//	m_ulMachineType = BT_MACHTYPE_STD;


	//get the BT config file handle

	ULONG i;

	/////////// Non-volatile memory ///////////
	m_oBinBlkMain.SetNVRamPtr(m_pvNVRAM);
	m_oBinBlkMain.SetNVRamBinTableStart(glNVRAM_BinTable_Start);
	m_oBinBlkMain.SetNVRamTempDataStart(m_pvNVRAM_HW, glNVRAM_TEMPDATA_Start);	//v4.65A1
	m_oBinBlkMain.InitNVBTDataPtr();

	/////////// Physical Block Structures ///////////
	if (IsLoadingPKGFile() == TRUE && IsLoadingPortablePKGFile() == TRUE)
	{
		// update backup current bin table setup data to pkg file one
		SaveBinTableSetupData();

		// load all bin table data
		InitBinTableData();
		
		// Setup the phy blk again and save it
		m_oPhyBlkMain.SaveAllPhyBlkSetupData(m_oPhyBlkMain.GetNoOfBlk(),m_oPhyBlkMain.GetBlkPitchX(), 
			m_oPhyBlkMain.GetBlkPitchY());
	}
	else
	{
		/////////// BinTable XY Limit & unload Position //////////
		InitBinTableRunTimeData();
		InitBinTableData();
		InitBinTableSetupData();

		m_oPhyBlkMain.InitPhyBlkMainData();

		//v4.56A2
		CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
		for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
		{
			i = min(i, BT_MAX_PHYBLK_SIZE-1);	//Klocwork	//v4.02T5
			m_oPhyBlkMain.InitPhyBlkData(i, pBTfile);
		}
		CMSFileUtility::Instance()->CloseBTConfig();
	}	

	/////////// Bin Block Structures ///////////
	m_oBinBlkMain.InitBinBlkMainData(bResetSerialNo);

	szData.Format("Init Bin Blk Data - SerialNo: %d", bResetSerialNo);
	CMSLogFileUtility::Instance()->BT_BinSerialLog(szData);

	// load all bin block data
/*
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		m_oBinBlkMain.InitBinBlkData(i, bResetSerialNo);
		m_oBinBlkMain.InitRandomHoleData(i);
		m_oBinBlkMain.LoadCurHoleDieNum(i);
		m_oBinBlkMain.SetEmptyHoleDieNum(i, m_ulHoleDieNum);
	}
*/
	//v4.46T15	//WarmStart speedup by reducing no. of MSD OPEN/CLOSE fcn calls
	m_oBinBlkMain.InitAllBinBlkData(bResetSerialNo);
	for (i=1; i<=m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		i = min(i, BT_MAX_BINBLK_SIZE-1);	//v4.51A20	//Klocwork
		m_oBinBlkMain.SetEmptyHoleDieNum(i, m_ulHoleDieNum);
	}
	m_oBinBlkMain.InitAndLoadAllRandomHoleData();

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == CTM_SANAN)
	{
		m_oBinBlkMain.SetBackupTempFilePath(m_szOutputFilePath3);	//v4.48A10	//WH Sanan
	}

	// reset setup the bin block for portable
	PortableBinBlockSetup();

	m_ulBinSerialNoFormat	= m_oBinBlkMain.GrabSerialNoFormat();
	m_ulBinSerialNoMaxLimit = m_oBinBlkMain.GrabSerialNoMaxLimit();

	m_ulBinClearFormat		= m_oBinBlkMain.GrabBinClearFormat();
	m_ulBinClearMaxLimit	= m_oBinBlkMain.GrabBinClearMaxLimit();
	m_ulBinClearInitCount	= m_oBinBlkMain.GrabBinClearInitCount();

	m_bSaveBinBondedCount	= m_oBinBlkMain.GrabAccCountOption();
	m_bOptimizeBinGrade		= m_oBinBlkMain.GrabAutoAssignGrade();
	m_bLSBondPattern		= m_oBinBlkMain.GrabLSBondPattern();

	//Force disable Optimize grade &  function if DL machine
	//if ( (m_ulMachineType == 2) || (m_ulMachineType == 3) )		//if DL/DLA (2) or DBuffer (3)
	if ( m_ulMachineType >= BT_MACHTYPE_DL_DLA )					//v4.16T1
	{
		m_bOptimizeBinGrade	= FALSE;
		m_oBinBlkMain.SetAutoAssignGrade(m_bOptimizeBinGrade);

		//m_bLSBondPattern = FALSE;		//v3.96T1
		m_oBinBlkMain.SetLSBondPattern(m_bLSBondPattern);
	}

	m_oBinBlkMain.SetBinSerialDefault(CMS896AStn::m_bDisableBinSNRFormat , CMS896AStn::m_ulDefaultBinSNRFormat);
	m_oBinBlkMain.SetClearCountDefault(CMS896AStn::m_bDisableClearCountFormat, CMS896AStn::m_ulDefaultClearCountFormat);
	
	/*
	//Check Bin Serial no format is disabled for user select
	if ( CMS896AStn::m_bDisableBinSNRFormat == TRUE )
	{
		m_ulBinSerialNoFormat = CMS896AStn::m_ulDefaultBinSNRFormat;
		m_oBinBlkMain.SetSerialNoFormat(m_ulBinSerialNoFormat);
	}

	if (CMS896AStn::m_bDisableClearCountFormat == TRUE)
	{
		//m_ulBinClearFormat = 
		m_ulBinClearFormat = CMS896AStn::m_ulDefaultClearCountFormat;
		m_oBinBlkMain.SetBinClearFormat(m_ulBinClearFormat);
	}
	*/

	/////////// Bin Grade Data Structures ///////////
	m_oBinBlkMain.InitBinGradeData();
	m_oBinBlkMain.InitOptimizeBinCountData();
	m_oBinBlkMain.InitOptimizeBinCountRunTimeData();
	
	
	/////////// Wafer Statistics ///////////
	LoadWaferStatistics();
	LoadWaferDieCount();
	
	BT_LoadLastState();
	SetNotGenerateWaferEndFile(m_bNotGenerateWaferEndFile);

	// Update bin blk summary time
	UpdateGenerateBinBlkSummaryTime();


	/////////// Grade Rank ID ///////////
	LoadGradeRankID();

	if ( (m_fHardware == TRUE) && !m_bDisableBT )
	{
#ifndef NU_MOTION
		LogProfileTime();		//v3.61T1	//Log to SRam for BH LogProfileTime logging
#endif
	}

	if (IsLoadingPortablePKGFile() == TRUE)
	{
		UpdatedPKGDisplayRecord();
	}

	UpdateAmiFileChecking();

	// reset all run time status if it is loading pkg
	if (IsLoadingPKGFile() == TRUE)
	{
		SetNotGenerateWaferEndFile(FALSE);
		ResetOptimizeBinCountStatus();
	}


	//v4.03		//PLLM/PLSG bin map fcn
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (bEnableBinMapFcn)	//v4.47T8	//WH SanAn
	{
		UINT nBinMapOption = pApp->GetEnableBMapBondAreaFromRegistry();
		if (nBinMapOption != 0)
		{
			if ((nBinMapOption == 2) && !m_bEnableBinMapBondArea)
			{
				SetErrorMessage("BT: BINMAP option is detected to be disabled");
				HmiMessage_Red_Yellow("BINMAP option is disabled; please check");
			}
		}
	}
	if (bEnableBinMapFcn && m_bEnableBinMapBondArea)
	{
		//Update nBinMap[][] array to bond index checking
		BOOL bCreateBinMap	= CreateBinMap2(0, 0, 0, FALSE);		//v4.36
		ULONG ulBinCount = 0;
		BOOL bLoadBinMap = LoadBinMap(TRUE, ulBinCount);	//TRUE for Initial data
		if (!bLoadBinMap)
		{
			CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map from CurBinMap.dat fail and start to Load Bin Map from the input path");
			bLoadBinMap	= LoadBinMap(FALSE, ulBinCount);	//Load Bin Map from the default path
			if (bLoadBinMap)
			{
				CMSLogFileUtility::Instance()->MS_LogOperation("Warm Start Load Bin Map from the input path success");
				BackupBinMap();
			}
		}

		if (!bLoadBinMap)
		{
			//v4.40T9
			HmiMessage("Fails to load bin map file at WARMSTART.", "Load Bin Map");
			SetErrorMessage("BT: WarmStart fails to load BinMap");
			CMSLogFileUtility::Instance()->MS_LogOperation("Load Bin Map from Both Path fail");
			SaveBinTableData();
			//CreateBinMap2(TRUE);			//Create full dummy map instead
		}
		else if (bLoadBinMap && (IsLoadingPKGFile()))
		{
			UpdateBinMapCapacity(ulBinCount);
		}

		LoadBinMapData(1);	//v4.42T6
	}
	else if (bEnableBinMapFcn && (pApp->GetCustomerName() == CTM_NICHIA))		//v4.42T12
	{
		BOOL bCreateBinMap	= CreateBinMap2(0, 0, 0, FALSE);
		ULONG ulBinCount = 0;
		BOOL bLoadBinMap	= LoadBinMap(TRUE, ulBinCount); //TRUE for Initial data
		if (!bLoadBinMap)
		{
			bLoadBinMap	= LoadBinMap(FALSE, ulBinCount);	//Load Bin Map from the default path
			if (bLoadBinMap)
			{
				BackupBinMap();
			}
		}
		if (!bLoadBinMap)
		{
			//HmiMessage("Nichia: Fails to load bin map file at WARMSTART", "Load Bin Map");
		}
		else
		{
			//HmiMessage("Nichia: Load bin map OK at WARMSTART", "Load Bin Map");
			LoadBinMapData(1);
		}
	}
	else if (bEnableBinMapFcn && m_bEnableOsramBinMixMap)		//v4.47A6
	{
		BOOL bCreateBinMap	= CreateOsramBinMixMap(FALSE);
		BOOL bLoadBinMap	= LoadOsramBinMixMap();
	}


	//v4.24T8	//Avoid BT1 & BT2 for collision with DBH in MS100 9Inch config
	if (m_bUseDualTablesOption)
	{
		if (Y_IsPowerOn())
		{
			Y_Profile(LOW_PROF1);
			if (m_lTableYNegLimit < -100000)
			{
				Y_MoveTo(m_lTableYNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_Y);		//v4.24T8	//Avoid collision with DBH
			}
			else
			{
				Y_MoveTo(BT_DUAL_TABLE_PARKING_DEFAULT_POSY, FALSE);					//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
			}
			Y_Profile(NORMAL_PROF);
		}

		if (Y2_IsPowerOn())
		{
			Y2_Profile(LOW_PROF1);
			if (m_lTableY2PosLimit > 100000)
			{
				Y2_MoveTo(m_lTableY2PosLimit - BT_DUAL_TABLE_PARKING_OFFSET_Y);		//v4.24T8	//Avoid collision with DBH
			}
			else
			{
				Y2_MoveTo(BT_DUAL_TABLE2_PARKING_DEFAULT_POSY, FALSE);					//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
			}

			Y2_Profile(NORMAL_PROF);
		}

		Y_Sync();
		Y2_Sync();

		if (X_IsPowerOn())
		{
			X_Profile(LOW_PROF1);
			if (m_lTableXNegLimit != 0)
			{
				X_MoveTo(m_lTableXNegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
			}
			else
			{
				X_MoveTo(BT_DUAL_TABLE_PARKING_DEFAULT_POSX, FALSE);					//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
			}
			X_Profile(NORMAL_PROF);
		}

		if (X2_IsPowerOn())
		{
			X2_Profile(LOW_PROF1);
			if (m_lTableX2NegLimit != 0)
			{
				X2_MoveTo(m_lTableX2NegLimit + BT_DUAL_TABLE_PARKING_OFFSET_X);		//v4.24T8	//Avoid collision with DBH
			}
			else
			{
				X2_MoveTo(BT_DUAL_TABLE2_PARKING_DEFAULT_POSX, FALSE);					//v4.24T8	//DEFAULT parking pos to avoid collision with DBH
			}
			X2_Profile(NORMAL_PROF);
		}
	}

	//v4.39T7
	if (pApp->GetCustomerName() == "Knowles")
	{
		//Enable Pick & Place, Do not generate output file
		m_bIfGenOutputFile = FALSE;
	}

	szMsg.Format("BT Init Data Done,Loading PKG, %d,PPKG,%d,Enable BinMap Func,%d,BM Bond Area,%d",
		IsLoadingPKGFile(),IsLoadingPortablePKGFile(),bEnableBinMapFcn,m_bEnableBinMapBondArea);
	CMSLogFileUtility::Instance()->MS_LogOperation(szMsg);
	/////////// backup machine BT logging ///////////
	CMSLogFileUtility::Instance()->BT_BackupReAlignBinFrameLog();
	CMSLogFileUtility::Instance()->BT_BackupTableIndexLog();
	CMSLogFileUtility::Instance()->BT_BackupGenTempLog();
	CMSLogFileUtility::Instance()->BT_BackupLogStatus();
	CMSLogFileUtility::Instance()->BT_BackupOptimizeGradeLog();
	CMSLogFileUtility::Instance()->BT_BackupDLALogStatus();
	CMSLogFileUtility::Instance()->BT_BackupClearBinCounterLog();
	CMSLogFileUtility::Instance()->BT_BackupExchangeFrameLog();			//v3.34 //Cree
	CMSLogFileUtility::Instance()->BT_BackupBinRemaingCountLog();
	CMSLogFileUtility::Instance()->BT_BackupBinSerialLog();
	CMSLogFileUtility::Instance()->BT_BackupPostBondLog();
	CMSLogFileUtility::Instance()->BT_BackupThetaCorrectionLog();

	(*m_psmfSRam)["BinTable"]["MS90"]["Rotate180"] = FALSE;				//v4.58A1

	GetWafflePadSettings();
	CheckIfNeedToRestoreNVRAMData();	//v4.65A3

	m_ulBondedBlk = GetBTCurrentBlock();
	return TRUE;
}

VOID CBinTable::ClearData()
{
	if ( m_pvNVRAM != NULL )
	{
		m_oBinBlkMain.ClearNVRAM(m_pvNVRAM, glNVRAM_BinTable_Start);
	}
}

BOOL CBinTable::GetUseFilenameAsWaferId()
{
	return m_bFilenameAsWaferID;
}

/////////////////////////////////////////////////////////////////
//Measurement Unit Conversion Functions
/////////////////////////////////////////////////////////////////
LONG CBinTable::ConvertXEncoderValueForDisplay(LONG lXEncoderValue)
{
	LONG lConvertedValue = 0;	//Klocwork

	if (GetMeasurementUnit() == "um") //cnt -> um
	{
		if (lXEncoderValue > 0)
			lConvertedValue = (LONG)(((DOUBLE)lXEncoderValue / m_dXResolution) + 0.5);
		else if (lXEncoderValue < 0)
			lConvertedValue = (LONG)(((DOUBLE)lXEncoderValue / m_dXResolution) - 0.5);
		else 
			lConvertedValue = 0;
	}
	else if (GetMeasurementUnit() == "mil") //cnt -> mil
	{
		if (lXEncoderValue > 0)
			lConvertedValue = (LONG)(((DOUBLE)lXEncoderValue / m_dXResolution / 25.4) + 0.5);
		else if (lXEncoderValue < 0)
			lConvertedValue = (LONG)(((DOUBLE)lXEncoderValue / m_dXResolution / 25.4) - 0.5);
		else
			lConvertedValue = 0;
	}    

	return lConvertedValue;
} //end ConvertXEncoderValueForDisplay


LONG CBinTable::ConvertYEncoderValueForDisplay(LONG lYEncoderValue)
{
	LONG lConvertedValue = 0;	//Klocwork

	if (GetMeasurementUnit() == "um") //cnt -> um
	{
		if (lYEncoderValue > 0)
			lConvertedValue = (LONG)(((DOUBLE)lYEncoderValue / m_dYResolution) + 0.5);
		else if (lYEncoderValue < 0)
			lConvertedValue = (LONG)(((DOUBLE)lYEncoderValue / m_dYResolution) - 0.5);
		else
			lConvertedValue = 0;
	}
	else if (GetMeasurementUnit() == "mil") //cnt -> mil
	{
		if (lYEncoderValue > 0)
			lConvertedValue = (LONG)(((DOUBLE)lYEncoderValue / m_dYResolution / 25.4) + 0.5);
		else if (lYEncoderValue < 0)
			lConvertedValue = (LONG)(((DOUBLE)lYEncoderValue / m_dYResolution / 25.4) - 0.5);
		else
			lConvertedValue = 0;
	}

	return lConvertedValue;
} //end ConvertYEncoderValueForDisplay


LONG CBinTable::ConvertFileUnitToXEncoderValue(DOUBLE dXFileValue)
{
	LONG lConvertedValue = 0;	//Klocwork

	if (dXFileValue > 0)
		lConvertedValue = (LONG)((dXFileValue * m_dXResolution) + 0.5);
	else if (dXFileValue < 0)
		lConvertedValue = (LONG)((dXFileValue * m_dXResolution) - 0.5);
	else 
		lConvertedValue = 0;

	lConvertedValue = 0 - lConvertedValue; //new added

	return lConvertedValue;
} //end ConvertFileUnitToXEncoderValue


LONG CBinTable::ConvertFileUnitToYEncoderValue(DOUBLE dYFileValue)
{
	LONG lConvertedValue;

	if (dYFileValue > 0)
		lConvertedValue = (LONG)((dYFileValue * m_dYResolution) + 0.5);
	else if (dYFileValue < 0)
		lConvertedValue = (LONG)((dYFileValue * m_dYResolution) - 0.5);
	else
		lConvertedValue = 0;

	lConvertedValue = 0 - lConvertedValue; //new added

	return lConvertedValue;
} //end ConvertFileUnitToYEncoderValue


LONG CBinTable::ConvertMilUnitToXEncoderValue(DOUBLE dXFileValue)		//v3.62
{
	LONG lConvertedValue;

	if (dXFileValue > 0)
		lConvertedValue = (LONG)((dXFileValue * m_dXResolution * 25.4) + 0.5);
	else if (dXFileValue < 0)
		lConvertedValue = (LONG)((dXFileValue * m_dXResolution * 25.4) - 0.5);
	else 
		lConvertedValue = 0;

	lConvertedValue = 0 - lConvertedValue; //new added
	return lConvertedValue;
}


LONG CBinTable::ConvertMilUnitToYEncoderValue(DOUBLE dYFileValue)		//v3.62
{
	LONG lConvertedValue;

	if (dYFileValue > 0)
		lConvertedValue = (LONG)((dYFileValue * m_dYResolution * 25.4) + 0.5);
	else if (dYFileValue < 0)
		lConvertedValue = (LONG)((dYFileValue * m_dYResolution * 25.4) - 0.5);
	else
		lConvertedValue = 0;

	lConvertedValue = 0 - lConvertedValue; //new added
	return lConvertedValue;
}


VOID CBinTable::GetBinBlkNoOfRowsAndCols(ULONG ulBinBlkId, ULONG &ulNoOfRows, ULONG &ulNoOfCols)
{
	ulNoOfRows = m_oBinBlkMain.GrabNoOfDiePerRow(ulBinBlkId);
	ulNoOfCols = m_oBinBlkMain.GrabNoOfDiePerCol(ulBinBlkId);
} //end GetBinBlkNoOfRowsAndCols



//--------- Function to clear bin by grade ------------//
BOOL CBinTable::ClrBinCntCmdSubTasksByGrade(UCHAR ucGrade, CString szClrBinCntMode, CString szBinOutputFileFormat, CDWordArray& dwaBinBlkIds,
											BOOL bShowMessage, BOOL bAutoClear, BOOL bResetDieCount, BOOL bForSummary)
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szGrade, szTempFilename;
	ULONG i=0;
	LONG lFirstBlkId;
	LONG lGenDatabaseStatus;
	LONG lGenStdOutputFileOk;
	LONG lMagzID, lSlotID; 
	BOOL bIfFilesAreToBeGenerated;
	BOOL bPrintLabel;
	CString szCustomFilename, szMachineNo, szLotNumber, szBinLotDirectory;
	CString szTempPath, szOutputLogFilePath, szOutputBinFilePath, szLineData;
	CString szWorkNo, szPrefix, szLoadPKGCount;
	CString szBinParameter, szMapSpecVersion;
	CString szType, szSortBin, szDevice;
	CString szWaferBin, szModelNo, szLotID;
	CString szProduct, szMachineType;
	CString szSlotID, szMapLotNo;
	CString szOperatorId, szSoftwareVersion, szPKGFilename;
	CString szProberMachineNo, szProberDateTime;
	CString szBinTableSerialNo;
	CString szMESOutgoingPath;
	CString szInternalProductNo, szMode, szSpecification;	
	CString szMapOther;
	CString szSortBinFilename;
	CString szWaferLotFilename;
	CString szMapBinTable;
	CString szMapESDVoltage;
	CString szTestTime = "";
	CString szWO = "";
	CString szSubstarteID = "";
	CString szCSTID = "";
	CString szRecipeID = "";
	CString szRecipeName = "";
	CString szMapFileOperator = "";
	CString szMapFileDate = "";
	CString szRemark1 = "", szRemark2 = "";
	CString szPartNo = "";
	CString szBinDieType = "";
	CString szDN = "";
	CString szWN = "";
	CString szCM1 = "";
	CString	szCM2 = "";
	CString	szCM3 = "";
	CString szUN	= "";
	CString szCOND	= "";
	CString szLO	= "";
	CString szUP	= "";
	CString szFT	= "";
	CString szLIM	= "";
	CString szCOEF1 = "";
	CString szCOEF2 = "";
	CString szCOEF3 = "";
	CString szFullMagSlot = "";
	CString szUseBarcode = "0";
	CString szPLLMDieFabMESLot = "";
	CString szMapSerialNumber = "";
	CString szBIBatchID, szBIBatchNo, szBIProductName, szBIModelNo, szBIPartNo;
	CString szBISortBinFilename, szBIType, szBISortBinItem;
	CString szMachineModel;
	CString szPRDeviceID;

	CString szWaferMapCenterPoint = "", szWaferMapLifeTime = "", szWaferMapESD = "";
	CString szWaferMapPadColor = "", szWaferMapBinCode = "";
		
	CStringArray szaPopUpMsg;
	BOOL bReturn = FALSE;
	BOOL bIsBlkNeedClear = FALSE;
	BOOL bUseBarcodeAsFileName = FALSE;
	BOOL bUseBarcode = FALSE;

	BOOL	bFileExist;
	BOOL	bContinueToGenerateOutputFile;
	
	CString szDuplicateFileRetryCount;
	LONG	lDuplicateFileRetryCount;

	CString szRetryCount;
	LONG	lRetryCount;
	
	szGrade.Format("%d", ucGrade);

	CMS896AApp::m_bIsFirstBondCycle = TRUE;	//reset the flag

	bIfFilesAreToBeGenerated = m_bGenOFileIfClrByGrade;
	
	if ( IsBurnIn() || (szBinOutputFileFormat.IsEmpty() == TRUE) )
	{
		bIfFilesAreToBeGenerated = FALSE;
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by grade <== burn in or format null:" + szBinOutputFileFormat);
	}

//#ifdef NU_MOTION
//	// disable output file generation if not normal sim
//	if (CMS896AApp::m_ucNuSimCardType != GMODE_NORMAL_SUITE)
//	{
//		bIfFilesAreToBeGenerated = FALSE;
//		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by grade <== NUMOTION abnormal sute");
//	}
//#endif

	//--- Check Whether Bin Need To Clear & found the first ---//
	//--- bin blk id for tmp ouput of the barcode & the slot no ---//
	
	BOOL bBlkHasDie = FALSE;
	bIsBlkNeedClear = FALSE;
	for (i =1 ; i<= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabGrade(i) == ucGrade)
		{
			if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
			{
				lFirstBlkId = i;
				bIsBlkNeedClear = TRUE;
				bBlkHasDie = TRUE;
				break;
			}
			else	// update the NVRam and remove temp file
			{
				lFirstBlkId = i;
				bIsBlkNeedClear = TRUE;
				bIfFilesAreToBeGenerated = FALSE;
				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by grade <== block die counter is 0");
				break;
			}
		}
	}

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap() || IsEnablePNP() )
	{
		if( ucGrade!=m_ucDummyPrescanPNPGrade )	// die counter is 0, return true, else false
		{
			CString szLogMsg;

			if( bBlkHasDie )
				szLogMsg.Format("BT: clr bin block id by grade %lu has die, PNP grade %d", ucGrade, m_ucDummyPrescanPNPGrade);
			else
				szLogMsg.Format("BT: clr bin block id by grade %lu no  die, PNP grade %d", ucGrade, m_ucDummyPrescanPNPGrade);
			CMSLogFileUtility::Instance()->BL_LogStatus(szLogMsg);
			if( bBlkHasDie )
				return FALSE;
			return TRUE;
		}
		if( CMS896AStn::m_bEnablePickNPlaceOutputFile==FALSE )
		{
			bIfFilesAreToBeGenerated = FALSE;
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by grade <== resort mode and disable output file");
		}
	}

	if (bIsBlkNeedClear == FALSE)
		bReturn = TRUE;

	for (i =1 ; i<= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabGrade(i) == ucGrade)
		{
			m_oBinBlkMain.BackupTempFile(i);
		}
	}

	// Reset all status
	(*m_psmfSRam)["BinTable"]["ClearBin"]["Grade"] = 0;
	(*m_psmfSRam)["BinTable"]["ClearBin"]["Bonded"]= 0;
	(*m_psmfSRam)["BinTable"]["ClearBin"]["SNR No"]= 0;


	CString szLog;
	CString m_szOutputBarcode = GetBLBarcodeData(lFirstBlkId);
	szLog.Format("Start ClrBinCntCmd SubTasksByGrade (B%lu) (Auto=%d) (OFile=%d %d) - ", ucGrade, bAutoClear, m_bGenOFileIfClrByGrade, bIfFilesAreToBeGenerated);	//v4.17T1
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szLog + szClrBinCntMode);

	if ( (bIfFilesAreToBeGenerated == TRUE) )		//v2.82T3
	{
		if ( bIsBlkNeedClear == TRUE )
		{
			//Initialize values
			lGenDatabaseStatus = 1;
			lGenStdOutputFileOk = 1;
		
			if (bForSummary == FALSE)
			{
				//Update Serial No
				//m_oBinBlkMain.UpdateSerialNoGivenPhyBlkId(ulPhyBlkId);
				m_oBinBlkMain.UpdateSerialNo(0, ucGrade , CLEAR_BIN_BY_GRADE , m_bUseMultipleSerialCounter);
				CMSLogFileUtility::Instance()->BT_BinSerialLog("UpdateSerialNo - ClrBinCntCmd SubTasksByGrade");
			}

			//Unload Date
			CTime theTime = CTime::GetCurrentTime();
			
			int nUnloadYear = theTime.GetYear();
			int nUnloadMonth = theTime.GetMonth();
			int nUnloadDay = theTime.GetDay();
			CString szUnloadDate;
			szUnloadDate.Format("%d-%d-%d", nUnloadYear, nUnloadMonth, nUnloadDay);
			
			//Unload Time
			int nUnloadHour = theTime.GetHour();
			int nUnloadMinute = theTime.GetMinute();
			int nUnloadSecond = theTime.GetSecond();
			CString szUnloadHour, szUnloadMinute, szUnloadSecond, szUnloadTime;

			szUnloadHour.Format("%d", nUnloadHour);
			if (nUnloadHour < 10)
				szUnloadHour = "0" + szUnloadHour;

			szUnloadMinute.Format("%d", nUnloadMinute);
			if (nUnloadMinute < 10)
				szUnloadMinute = "0" + szUnloadMinute;

			szUnloadSecond.Format("%d", nUnloadSecond);
			if (nUnloadSecond < 10)
				szUnloadSecond = "0" + szUnloadSecond;

			szUnloadTime = szUnloadHour + ":" + szUnloadMinute + ":" + szUnloadSecond;

			// tmp to use lFirstBlkId to get the barcode ans slot no
			//Output Barcode (if empty, set as BinNo)
			CString m_szOutputBarcode = GetBLBarcodeData(lFirstBlkId);

			//v4.41T5	//SanAn problem logging here
			CString szLog;
			szLog.Format("clr bin by grade BC = %s; bUseBarcodeAsFileName = %d", (LPCTSTR) m_szOutputBarcode, m_bUseBarcodeAsFileName);
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szLog);

			if ( m_szOutputBarcode.IsEmpty() == TRUE )
			{
				m_szOutputBarcode.Format("%d", lFirstBlkId);
				
				//v4.50A5	//Cree HZ DynamicGradeMapping uses OriginalGrade
				if ( IsMapDetectSkipMode() )
				{
					USHORT usOrigGradeMap = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(ucGrade + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()) 
												- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
					m_szOutputBarcode.Format("%d", usOrigGradeMap);
				}
			}

			//Get this binblk in DL elevator slot position
			szSlotID.Empty();
			if ( GetFrameSlotID(lFirstBlkId, lMagzID, lSlotID) == TRUE )
			{
				szSlotID.Format("%d/%d", lSlotID, lMagzID);
			}
			
			//v2.78T2
			//For PLLM DLA offline print label feature
			//Use this inside () below
			if (m_bDisableClearAllPrintLabel)
				CMS896AStn::m_bOfflinePrintLabel = TRUE;
			else
				CMS896AStn::m_bOfflinePrintLabel = FALSE;

			//Update SRAM data for other stn use
			(*m_psmfSRam)["BinTable"]["ClearBin"]["Grade"]	= ucGrade;
			(*m_psmfSRam)["BinTable"]["ClearBin"]["Bonded"] = m_oBinBlkMain.GrabNVNoOfBondedDie(lFirstBlkId);
			(*m_psmfSRam)["BinTable"]["ClearBin"]["SNR No"]	= m_oBinBlkMain.GrabSerialNo(lFirstBlkId);

			//Get Info from MapSorter 
			szOperatorId		= (*m_psmfSRam)["MS896A"]["Operator Id"];

			//v4.40T7	//DIsabled for Nichia
			szMachineNo		= GetMachineNo();
			szLotNumber			= (*m_psmfSRam)["MS896A"]["LotNumber"];
			szBinLotDirectory	= (*m_psmfSRam)["MS896A"]["Bin Lot Directory"];
			szWorkNo			= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
			szPrefix			= (*m_psmfSRam)["MS896A"]["Bin FileName Prefix"];
			szLoadPKGCount		= (*m_psmfSRam)["MS896A"]["DevFileCount"];
			szSoftwareVersion	= (*m_psmfSRam)["MS896A"]["Software Version"];
			szPKGFilename		= (*m_psmfSRam)["MS896A"]["PKG Filename"];
			szWaferLotFilename	= (*m_psmfSRam)["MS896A"]["WaferLot Filename"];
			szBinDieType		= (*m_psmfSRam)["MS896A"]["UserDefineDieType"];
		
			szPLLMDieFabMESLot	= (*m_psmfSRam)["MS896A"]["PLLMDieFabMESLot"];

			szPRDeviceID = GetWaferPrDeviceID();

			//Get Wafermap Header Info
			szMapSpecVersion	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPEC];
			szBinParameter		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINPARA];
			szType				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TYPE];
			szSortBin			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBIN];
			szDevice			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE];
			szWaferBin			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFERBIN];
			szModelNo			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODELNO];
			szLotID				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];
			szProduct			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PRODUCT];
			szMapLotNo			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER];
			szMapESDVoltage		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD_VOLTAGE];
			szProberMachineNo	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_MACHINE_NO];
			szProberDateTime	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_DATE_TIME];
			szMapFileDate		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_DATE_IN_MAP_FILE];
			szBinTableSerialNo	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_TABLE_SERIAL_NO];
			szInternalProductNo	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_INTERNAL_PRODUCT_NO];	
			szMode				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODE];					
			szSpecification		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPECIFICATION];
			szMapOther			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MAP_OTHER];
			szSortBinFilename	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBINFNAME];
			szRemark1			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK1];
			szRemark2			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK2];
			szMapBinTable		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINTABLE];

			szTestTime			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TEST_TIME];
			szWO				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WO];
			szSubstarteID		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SUBSTRATE_ID];
			szCSTID				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CST_ID];
			szRecipeID			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_ID];
			szRecipeName		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_NAME];
			szMapFileOperator	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_OPERATOR];
			szPartNo			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PART_NO];
			szMapSerialNumber	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_SERIAL_NUMBER];

			szDN				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DN];
			szWN				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WN];
			szCM1				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM1];
			szCM2				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM2];
			szCM3				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM3];

			szUN				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UN];
			szCOND				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COND];
			szLO				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LO];
			szUP				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UP];
			szFT				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_FT];			
			szLIM				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIM];
			szCOEF1				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF1];
			szCOEF2				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF2];			
			szCOEF3				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF3];

			szWaferMapCenterPoint	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CENTERPOINT];
			szWaferMapLifeTime		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIFETIME];
			szWaferMapESD			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD];
			szWaferMapPadColor		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PAD_COLOR];
			szWaferMapBinCode		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_CODE];

			szMachineModel			= (*m_psmfSRam)["MS896A"]["Machine Model"];

			szFullMagSlot			= (*m_psmfSRam)["BinLoader"]["CurFullSlot"];
			bUseBarcode				= (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["UseBracode"];

			if (bUseBarcode == TRUE)
			{
				szUseBarcode = "1";
			}
			
			CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
			BOOL bOsramOutput = FALSE;
			bUseBarcodeAsFileName = m_bUseBarcodeAsFileName;

			szMachineType.Format("%d", m_ulMachineType);
			
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Start Save Clear Bin Counter Data For Output File");

			szMESOutgoingPath = CMESConnector::Instance()->GetMESOutgoingPath();

			lDuplicateFileRetryCount = 0;
			lRetryCount = 0;

			CString szOutputFiePath2 = " ";
			if( m_bValidOutputFilePath2 && m_bEnableBinOutputFilePath2 )
			{
				szOutputFiePath2 = m_szOutputFilePath2;
				//v4.44T2	//Silan
				(*m_psmfSRam)["BinTable"]["Output File Path 3"]	= m_szOutputFilePath3;
				(*m_psmfSRam)["BinTable"]["Output File Path 4"]	= m_szOutputFilePath4;
			}

			do
			{
				bFileExist = FALSE;
				bContinueToGenerateOutputFile = FALSE;
				szDuplicateFileRetryCount.Format("%d", lDuplicateFileRetryCount);
				szRetryCount.Format("%d", lRetryCount);

				//Save all input data
				m_oBinBlkMain.SaveClearBinCountDataForOutputFile(0,ucGrade,m_bGenOFileIfClrByGrade, bUseBarcodeAsFileName,
							szMachineNo, szLotNumber, szBinLotDirectory, m_szOutputFileSummaryPath , m_szFileNo, m_szBlueTapeNo, 
							szMapSpecVersion, szBinParameter, szBinOutputFileFormat, m_szOutputFilename, 
							szUnloadDate, szUnloadTime, m_szOutputBarcode, m_szLabelFilePath,
							szWorkNo, szPrefix, szLoadPKGCount, 
							szType, szSortBin, szDevice, 
							szWaferBin, szModelNo, szLotID,
							szProduct, m_szSpecVersion, szMachineType,
							szSlotID, szMapLotNo, szOperatorId, CLEAR_BIN_BY_PHY_BLK, m_szaExtraBinInfoField, 
							m_szaExtraBinInfo , szSoftwareVersion, szPKGFilename, szProberMachineNo, szProberDateTime, 
							szBinTableSerialNo, szMESOutgoingPath, szInternalProductNo, szMode, szSpecification, szMapOther,
							szSortBinFilename, szWaferLotFilename, szMapBinTable, szDuplicateFileRetryCount, szMapESDVoltage,
							szTestTime, szWO, szSubstarteID, szCSTID, szRecipeID, szRecipeName, szMapFileOperator, 
							szMapFileDate, szRemark1, szRemark2, szPartNo, szBinDieType, m_lBinBarcodeMinDigit,
							szDN, szWN, szCM1, szCM2, szCM3, szUN, szCOND, szLO, szUP, szFT, szLIM, szCOEF1, szCOEF2, szCOEF3,
							szPLLMDieFabMESLot, szFullMagSlot, m_szLabelPrinterSel, szMapSerialNumber, szRetryCount, szWaferMapCenterPoint,
							szWaferMapLifeTime, szWaferMapESD, szWaferMapPadColor, szWaferMapBinCode, szMachineModel, szPRDeviceID,
							szUseBarcode, szOutputFiePath2, bForSummary, m_psmfSRam);	//v4.13T1
				
				
				//Database generating
				szaPopUpMsg.RemoveAll();

				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Start Generate Output File Database");

				lGenDatabaseStatus = m_oBinBlkMain.GenOutputFileDatabase(0,ucGrade,szaPopUpMsg, CLEAR_BIN_BY_GRADE);

				//Retrun value: 1 = OK; 0 = 0 Die count; -1 = Gen Database error; -2 = Load temp file error
				if (lGenDatabaseStatus != 1) 
				{
					//Print error messages
					switch(lGenDatabaseStatus)
					{
					default:
						SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_TO_GEN_OTF, szGrade);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("BT No Die To Gen OTF");
						break;

					case -1:
						SetAlert_Msg_Red_Yellow(IDS_BT_FAIL_TO_GEN_DATABASE, szGrade);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("BT Fail To Gen Database");
						break;

					case -2:
						SetAlert_Msg_Red_Yellow(IDS_BT_FAIL_TO_LOAD_TEMP_FILE, szGrade);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail To Load Temp File");
						break;
					}

					if (lGenDatabaseStatus == 0)
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}

				//Check Output file exist or not

				//Get SPC data & saved in temp directory (Removed! Since SPC data already generated when stop)

				//Standard Output File generating
				if (m_bGenOFileIfClrByGrade==FALSE)
				{
					m_oBinBlkMain.CleanupGenAndQueryDB(0, ucGrade, CLEAR_BIN_BY_GRADE);
					break;
				}
				else
				if (m_bGenOFileIfClrByGrade)
				{
					//Backup Temp & database file
					CTime theTime = CTime::GetCurrentTime();
					CString szTimeStamp;

					szTimeStamp = theTime.Format("%y%m%d%H%M%S");
					
					m_oBinBlkMain.BackupClearBinFiles(0, ucGrade, TRUE, szTimeStamp, CLEAR_BIN_BY_GRADE);


					CString szClrBinOutPath = m_szOutputFilePath;

					if( bOsramOutput && m_bAutoUploadOutputFile==FALSE )
					{
						if (!IsPathExist(szClrBinOutPath))
						{
							CString szTitle, szContent;
							szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
							szTitle = szTitle + " grade " + szGrade;
							szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
							HmiMessage_Red_Back(szContent, szTitle);
							CMSLogFileUtility::Instance()->BL_LogStatus("BT: osram grade need to use BACKUP path");
							m_bUseBackupPath = TRUE;
						}
					}

					if (m_bUseBackupPath)
					{
						szClrBinOutPath = gszUSER_DIRECTORY + "\\OutputFile\\ClearBin";
					}

					if( m_bAutoUploadOutputFile )
					{
						szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;
					}	//	temp local for auto upload


					szaPopUpMsg.RemoveAll();
					if( pApp->GetCustomerName()==CTM_SEMITEK )
					{
						pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\OutputFile\\ClearBin", 60);
					}

					//StartLoadingAlert();
					CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
					((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

					bPrintLabel = m_bIfPrintLabel && m_bIfGenLabel;

					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Start Generate Standard Output File");

					LONG lAlarmStatus = GetAlarmLamp_Status();	//	old->new
					if( bOsramOutput )
					{
						SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);		//	old->new
					}
					lGenStdOutputFileOk = m_oBinBlkMain.GenStandardOutputFile(0, ucGrade,
												szClrBinOutPath, szBinOutputFileFormat, bPrintLabel, szaPopUpMsg, CLEAR_BIN_BY_GRADE);
					if( bOsramOutput )
					{
						SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);	//	old->new
					}

					//CloseLoadingAlert();
					pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

					if (CMS896AStn::m_bKeepOutputFileLog == FALSE)
					{
						//Remove Temp & database file
						if (lGenStdOutputFileOk == 1)		//v2.78T2
							m_oBinBlkMain.BackupClearBinFiles(0, ucGrade, FALSE, szTimeStamp, CLEAR_BIN_BY_GRADE);		//v4.48A10
					}

					//0 = Cannot create object; 1 = OK; -1 = fail to gen output file; -2 = print label error; -3 = output format not define
					if ( (lGenStdOutputFileOk == 1) && bShowMessage )
					{
						/*
						if (szClrBinCntMode == "Single Block")
						{
							HmiMessage("Standard output file is generated for Physical Block " +
										szPhyBlkId + "!");
						}
						*/
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Standard output file is generated successfully for Grade " +szGrade + "!");
					}
					else
					{
						if( (lGenStdOutputFileOk!=1) && (szClrBinCntMode=="Single Grade" || bOsramOutput) )
						{
							UINT uiErrorCode;
							switch(lGenStdOutputFileOk)
							{
							case 0:	
								uiErrorCode = IDS_BT_FAIL_TO_CREATE_OBJ;
								SetErrorMessage("Fail to create OBJ for Grade " + szGrade);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to create OBJ for Grade " + szGrade);
								break;

							case -1:
								uiErrorCode = IDS_BT_FAIL_TO_GEN_OTF;
								SetErrorMessage("Fail to gen output file for Grade " + szGrade);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to gen output file for Grade " + szGrade);
								break;

							case -2:
								uiErrorCode = IDS_BT_FAIL_TO_PRN_LABEL;
								SetErrorMessage("Fail to print label for Grade " + szGrade);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to print label for Grade " + szGrade);
								break;

							case -3:
								uiErrorCode = IDS_BT_NO_OTF_FORMAT;
								SetErrorMessage("No output file format file");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to print label for Grade " + szGrade);
								break;

							case -4:
								bFileExist = TRUE;
								szaPopUpMsg.RemoveAll();
								uiErrorCode = IDS_BT_FILE_ALREDAY_EXIST;
								SetErrorMessage("File already exist");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("File already exist");
								lDuplicateFileRetryCount = lDuplicateFileRetryCount + 1;
								break;
							
							case -5:
								uiErrorCode = IDS_BT_CP_BARCODE_LENGTH_FAILED;
								SetErrorMessage("Compare barcode length failed");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Compare barcode length failed");
								break;
							
							case -6: // without retry one
								uiErrorCode = IDS_BT_FILE_ALREDAY_EXIST;
								SetErrorMessage("File already exist");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("File already exist");
								break;

							case -8:
								bContinueToGenerateOutputFile = TRUE;
								uiErrorCode = IDS_BT_MANUAL_INPUT_BC;
								lDuplicateFileRetryCount = lDuplicateFileRetryCount + 1;
								SetErrorMessage("Manual input barcode");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Manual input barcode");
								break;
							/*
							case -7:
								uiErrorCode = IDS_BT_NO_EXTRA_FILE_DATA_ERR;
								bContinueToGenerateOutputFile = TRUE;
								lRetryCount = lRetryCount + 1;
								SetErrorMessage("No Info File Data Err");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("No Info File Data Err");
								break;
							*/
							}

							SetAlert_Msg_Red_Yellow(uiErrorCode, szGrade);
						}
					}
				}
			}while(bFileExist == TRUE || bContinueToGenerateOutputFile == TRUE);

			if ( lGenStdOutputFileOk == 1 )
			{
				//CDWordArray dwaBinBlkIds;
				if (bResetDieCount == TRUE)
				{
					//Send Clear Bin info to Host
					//SendClearBinInfoToHost(ulPhyBlkId);
					if (m_ulEnableOptimizeBinCount >= TRUE)
					{
						m_oBinBlkMain.UpdateLotRemainCountByGrade(ucGrade);

						m_oBinBlkMain.OptimizeBinFrameCountByGrade(ucGrade, m_ucOptimizeBinCountMode, m_ulEnableOptimizeBinCount);
					}
					//Clear Bin Counter by grade
					m_oBinBlkMain.ClearBinCounterByGrade(ucGrade,dwaBinBlkIds);

					//Reset flag for CyOptics	//v4.30T4
					for (INT j=0; j<dwaBinBlkIds.GetSize(); j++)
					{
						m_oBinBlkMain.SetLeaveEmptyRow(dwaBinBlkIds.GetAt(j), FALSE);	
						m_oBinBlkMain.ResetEmptyRow1stRowIndex(dwaBinBlkIds.GetAt(j));	//v4.40T2
										
						ResetNichiaWafIDList(dwaBinBlkIds.GetAt(j));		//v4.40T6	//Nichia
					}

					if (bShowMessage && szClrBinCntMode == "Single Grade")
					{
						CString szTitle, szContent, szBlockId;

						szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
						szContent.LoadString(HMB_BT_BLK_COUNT_CLEARED);
						//szContent.LoadString("Grade counter is cleared!");
						szContent = szContent + "\n (Block Id: ";

						for (INT j=0; j<dwaBinBlkIds.GetSize(); j++)
						{
							if (j == dwaBinBlkIds.GetSize() -1)
								szBlockId.Format("%d )", dwaBinBlkIds.GetAt(j));
							else
								szBlockId.Format("%d,", dwaBinBlkIds.GetAt(j));

							szContent = szContent + szBlockId;
						}
						
						HmiMessage(szContent, szTitle);
					}
				}

				bReturn = TRUE;
			}
		}
	}
	else 
	{
		if ( bIsBlkNeedClear == TRUE )
		{		
			//CDWordArray dwaBinBlkIds;
			//Send Clear Bin info to Host (Temp to disable this for clear bin by grade)
			//SendClearBinInfoToHost(ulPhyBlkId);

			if (bResetDieCount == TRUE)
			{
				//Clear Bin Counter by grade
				m_oBinBlkMain.ClearBinCounterByGrade(ucGrade,dwaBinBlkIds);

				//Reset flag for CyOptics	//v4.30T4
				for (INT j=0; j<dwaBinBlkIds.GetSize(); j++)
				{
					m_oBinBlkMain.SetLeaveEmptyRow(dwaBinBlkIds.GetAt(j), FALSE);	
					m_oBinBlkMain.ResetEmptyRow1stRowIndex(dwaBinBlkIds.GetAt(j));	//v4.40T2

					ResetNichiaWafIDList(dwaBinBlkIds.GetAt(j));		//v4.40T6	//Nichia
				}

				//Update Serial No
				//m_oBinBlkMain.UpdateSerialNoGivenPhyBlkId(ulPhyBlkId);

				if (bShowMessage && szClrBinCntMode == "Single Grade")
				{
					CString szTitle, szContent, szBlockId;

					szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
					szContent.LoadString(HMB_BT_BLK_COUNT_CLEARED);
					szContent = szContent + "\n (Block Id: ";

					for (INT j=0; j<dwaBinBlkIds.GetSize(); j++)
					{
						if (j == dwaBinBlkIds.GetSize() -1)
							szBlockId.Format("%d )", dwaBinBlkIds.GetAt(j));
						else
							szBlockId.Format("%d,", dwaBinBlkIds.GetAt(j));

						szContent = szContent + szBlockId;
					}
					
					HmiMessage(szContent, szTitle);
				}
			}
			bReturn = TRUE;
		}
	}

	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("End Of ClrBinCntCmd SubTasksByGrade\n");
	return bReturn;
}

BOOL CBinTable::ClrBinCntCmdSubTasks(ULONG ulPhyBlkId, CString szClrBinCntMode, 
									 CString szBinOutputFileFormat, BOOL bShowMessage, BOOL bAutoClear,
									 BOOL& bNeedToClear, BOOL bResetDieCount, BOOL bForSummary)		//v4.14T1
{
	CString szPhyBlkId, szTempFilename;
	LONG lGenDatabaseStatus;
	LONG lGenStdOutputFileOk;
	LONG lMagzID, lSlotID; 
	BOOL bIfFilesAreToBeGenerated;
	BOOL bPrintLabel;
	CString szCustomFilename, szMachineNo, szLotNumber, szBinLotDirectory;
	CString szTempPath, szOutputLogFilePath, szOutputBinFilePath, szLineData;
	CString szWorkNo, szPrefix, szLoadPKGCount;
	CString szBinParameter, szMapSpecVersion;
	CString szType, szSortBin, szDevice;
	CString szWaferBin, szModelNo, szLotID;
	CString szProduct, szMachineType;
	CString szSlotID, szMapLotNo;
	CString szOperatorId, szPKGFilename, szSoftwareVersion;
	CString szProberMachineNo, szProberDateTime;
	CString szBinTableSerialNo, szMESOutgoingPath;
	CString szInternalProductNo, szMode, szSpecification;
	CString szMapOther;
	CString szSortBinFilename;
	CString szWaferLotFilename;
	CString szMapBinTable;
	CString szMapESDVoltage;
	CString szTestTime = "";
	CString szWO = "";
	CString szSubstarteID = "";
	CString szCSTID = "";
	CString szRecipeID = "";
	CString szRecipeName = "";
	CString szMapFileOperator = "";
	CString szMapFileDate = "";
	CString szRemark1 = "", szRemark2 = "";
	CString szPartNo = "";
	CString szBinDieType = "";
	CString szDN = "";
	CString szWN = "";
	CString szCM1 = "";
	CString	szCM2 = "";
	CString	szCM3 = "";
	CString szUN = "";
	CString szCOND = "";
	CString szLO = "";
	CString szUP = "";
	CString szFT = "";
	CString szLIM = "";
	CString szCOEF1 = "";
	CString szCOEF2 = "";
	CString szCOEF3 = "";
	CString szFullMagSlot = "";
	CString szUseBarcode = "0";
	CString szMapSerialNumber = "";
	CString szMachineModel = "";
	CString szWaferMapCenterPoint = "", szWaferMapLifeTime = "", szWaferMapESD = "";
	CString szWaferMapPadColor = "", szWaferMapBinCode = "";

	CString szPLLMDieFabMESLot = "";
	CString szPRDeviceID = "";

	CString szLogMsg;

	BOOL	bContinueToGenerateOutputFile;
	BOOL	bFileExist;

	CString szDuplicateFileRetryCount;
	LONG	lDuplicateFileRetryCount;

	CString szRetryCount;
	LONG lRetryCount;
	
	CString szBIBatchID, szBIBatchNo, szBIProductName, szBIModelNo, szBIPartNo;
	CString szBISortBinFilename, szBIType, szBISortBinItem;

	CStringArray szaPopUpMsg;
	BOOL bReturn = FALSE;
	BOOL bIsBlkNeedClear = FALSE;
	BOOL bUseBarcodeAsFileName = FALSE;
	BOOL bUseBarcode = FALSE;
	szPhyBlkId.Format("%d", ulPhyBlkId);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	CMS896AApp::m_bIsFirstBondCycle = TRUE;	//reset the flag

	bIfFilesAreToBeGenerated = m_bIfGenOutputFile;
	
	SaveBondedCountsByBlock(ulPhyBlkId);

	if ( IsBurnIn() || (szBinOutputFileFormat.IsEmpty() == TRUE) )
	{
		bIfFilesAreToBeGenerated = FALSE;
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by block <== burn in or format null:" + szBinOutputFileFormat);
	}

	BOOL bBlkHasDie = FALSE;
	if ( m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId) > 0 )
	{
		bBlkHasDie = TRUE;
	}
	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if( pUtl->GetPrescanDummyMap() || IsEnablePNP() )
	{
		if( ulPhyBlkId!=m_ucDummyPrescanPNPGrade )	// die counter is 0, return true, else false
		{
			if( bBlkHasDie )
				szLogMsg.Format("BT: clr bin block id %lu has die, PNP grade %d", ulPhyBlkId, m_ucDummyPrescanPNPGrade);
			else
				szLogMsg.Format("BT: clr bin block id %lu no  die, PNP grade %d", ulPhyBlkId, m_ucDummyPrescanPNPGrade);
			CMSLogFileUtility::Instance()->BL_LogStatus(szLogMsg);
			if( bBlkHasDie )
				return FALSE;

			return TRUE;
		}
		if( CMS896AStn::m_bEnablePickNPlaceOutputFile == FALSE )
		{
			bIfFilesAreToBeGenerated = FALSE;
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by block <== resort mode and disable output file");
		}
	}

	if ( m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId) > 0 )
	{
		m_bBinStatus[ulPhyBlkId] = TRUE;
		bNeedToClear	= TRUE;		//v4.15T8
		bIsBlkNeedClear = TRUE;
	}
	else
	{
		m_bBinStatus[ulPhyBlkId] = FALSE;
		bIfFilesAreToBeGenerated = FALSE;
		bIsBlkNeedClear = TRUE;
		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by block <== bonded die count is 0");
	}


//#ifdef NU_MOTION
//	if (CMS896AApp::m_ucNuSimCardType != GMODE_NORMAL_SUITE)
//	{
//		bIfFilesAreToBeGenerated = FALSE;
//		CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("no output file by block <== NUMOTION abnormal sute");
//	}
//#endif

	BackupTempFile(ulPhyBlkId);

	// Reset all status
	(*m_psmfSRam)["BinTable"]["ClearBin"]["Grade"] = 0;
	(*m_psmfSRam)["BinTable"]["ClearBin"]["Bonded"]= 0;
	(*m_psmfSRam)["BinTable"]["ClearBin"]["SNR No"]= 0;

	//MES Clear-Bin message for Testar	//v4.42T6
	CMESConnector::Instance()->SendMessage(MES_CHANGE_MACHINE_STATUS_ID, "C");

	//v4.12
	CString szLog;
	CString m_szOutputBarcode = GetBLBarcodeData(ulPhyBlkId);		//v4.51A17
	szLog.Format("Start ClrBinCntCmd SubTasks (B%lu) (Auto=%d) (OFile=%d %d) - ", ulPhyBlkId, bAutoClear, m_bIfGenOutputFile, bIfFilesAreToBeGenerated);	//v4.17T1
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szLog + szClrBinCntMode);

	if ( (bIfFilesAreToBeGenerated == TRUE))
	{
		if ( bIsBlkNeedClear == TRUE )
		{
			//Initialize values
			lGenDatabaseStatus = 1;
			lGenStdOutputFileOk = 1;

			if (bForSummary == FALSE && szBinOutputFileFormat != "Semitek2")
			{
				//Update Serial No
				m_oBinBlkMain.UpdateSerialNo(ulPhyBlkId, '0' , CLEAR_BIN_BY_PHY_BLK, m_bUseMultipleSerialCounter, m_bUseUniqueSerialNum);
				CMSLogFileUtility::Instance()->BT_BinSerialLog("UpdateSerialNo - ClrBinCntCmd SubTasks");
			}

			CString szClrBinOutPath = m_szOutputFilePath;
			if (m_bAutoUploadOutputFile)
			{
				szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;
			}
			//v4.45T3	//Electech 3E DL
			if((m_bAutoUploadOutputFile==FALSE) && !IsPathExist(szClrBinOutPath))
			{
				if (!m_bUseBackupPath)
				{
					BOOL bAccess = FALSE;
					if (m_bEnableBinOutputFilePath2)
					{
						bAccess = TRUE;
						szClrBinOutPath = m_szOutputFilePath2;
						if (!IsPathExist(szClrBinOutPath))
						{
							bAccess = FALSE;
						}
					}

					if (!bAccess)
					{
						CString szTitle, szContent;
						szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
						szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
						LONG lHmiMsgReply = HmiMessage_Red_Back(szContent, szTitle, 103, 3);
						if( lHmiMsgReply != glHMI_YES )
						{
							CString szMsg = "Fail to access OutputFile Path at: " + szClrBinOutPath;
							SetErrorMessage(szMsg + "\n");
							CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg + "\n");
							SetAlert_Msg(IDS_BT_NO_DIE_TO_GEN_OTF, szMsg);
							return FALSE;
						}
						m_bUseBackupPath = TRUE;
					}
				}
			}

			//Unload Date
			CTime theTime = CTime::GetCurrentTime();
			
			int nUnloadYear = theTime.GetYear();
			int nUnloadMonth = theTime.GetMonth();
			int nUnloadDay = theTime.GetDay();
			CString szUnloadDate;
			szUnloadDate.Format("%d-%d-%d", nUnloadYear, nUnloadMonth, nUnloadDay);
			
			//Unload Time
			int nUnloadHour = theTime.GetHour();
			int nUnloadMinute = theTime.GetMinute();
			int nUnloadSecond = theTime.GetSecond();
			CString szUnloadHour, szUnloadMinute, szUnloadSecond, szUnloadTime;

			szUnloadHour.Format("%d", nUnloadHour);
			if (nUnloadHour < 10)
				szUnloadHour = "0" + szUnloadHour;

			szUnloadMinute.Format("%d", nUnloadMinute);
			if (nUnloadMinute < 10)
				szUnloadMinute = "0" + szUnloadMinute;

			szUnloadSecond.Format("%d", nUnloadSecond);
			if (nUnloadSecond < 10)
				szUnloadSecond = "0" + szUnloadSecond;

			szUnloadTime = szUnloadHour + ":" + szUnloadMinute + ":" + szUnloadSecond;

			//Output Barcode (if empty, set as BinNo)
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("clr bin sub tasks BC = " + m_szOutputBarcode);		//v4.12

			//Get this binblk in DL elevator slot position
			szSlotID.Empty();
			if ( GetFrameSlotID(ulPhyBlkId, lMagzID, lSlotID) == TRUE )
			{
				szSlotID.Format("%d/%d", lSlotID, lMagzID);
			}

			if ( m_szOutputBarcode.IsEmpty() == TRUE )
			{
				m_szOutputBarcode.Format("%d", ulPhyBlkId);
				
				//v4.50A5	//Cree HZ DynamicGradeMapping uses OriginalGrade
				if ( IsMapDetectSkipMode() )
				{
					USHORT usOrigGradeMap = CMS896AStn::m_WaferMapWrapper.GetOriginalGrade(m_oBinBlkMain.GrabGrade(ulPhyBlkId) + CMS896AStn::m_WaferMapWrapper.GetGradeOffset()) 
												- CMS896AStn::m_WaferMapWrapper.GetGradeOffset();
					m_szOutputBarcode.Format("%d", usOrigGradeMap);
				}
			}

			//v2.78T2
			//For PLLM DLA offline print label feature
			//Use this inside () below
			if (m_bDisableClearAllPrintLabel)
				CMS896AStn::m_bOfflinePrintLabel = TRUE;
			else
				CMS896AStn::m_bOfflinePrintLabel = FALSE;

			//Update SRAM data for other stn use
			(*m_psmfSRam)["BinTable"]["ClearBin"]["Grade"]	= m_oBinBlkMain.GrabGrade(ulPhyBlkId);
			(*m_psmfSRam)["BinTable"]["ClearBin"]["Bonded"] = m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId);
			(*m_psmfSRam)["BinTable"]["ClearBin"]["SNR No"]	= m_oBinBlkMain.GrabSerialNo(ulPhyBlkId);
			(*m_psmfSRam)["BinTable"]["ClearBin"]["BMRadius"] = m_dBinMapCircleRadiusInMm;

			//Get Info from MapSorter 
			szOperatorId		= (*m_psmfSRam)["MS896A"]["Operator Id"];
			
			//v4.40T7
			szMachineNo		= GetMachineNo();
			szLotNumber			= (*m_psmfSRam)["MS896A"]["LotNumber"];
			szBinLotDirectory	= (*m_psmfSRam)["MS896A"]["Bin Lot Directory"];
			szWorkNo			= (*m_psmfSRam)["MS896A"]["Bin WorkNo"];
			szPrefix			= (*m_psmfSRam)["MS896A"]["Bin FileName Prefix"];
			szLoadPKGCount		= (*m_psmfSRam)["MS896A"]["DevFileCount"];
			szSoftwareVersion  = (*m_psmfSRam)["MS896A"]["Software Version"];
			szPKGFilename		= (*m_psmfSRam)["MS896A"]["PKG Filename"];
			szWaferLotFilename = (*m_psmfSRam)["MS896A"]["WaferLot Filename"];
			szBinDieType		= (*m_psmfSRam)["MS896A"]["UserDefineDieType"];

			szPLLMDieFabMESLot	= (*m_psmfSRam)["MS896A"]["PLLMDieFabMESLot"];

			szPRDeviceID =  GetWaferPrDeviceID();

			//Get Wafermap Header Info
			szMapSpecVersion	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPEC];
			szBinParameter		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINPARA];
			szType				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TYPE];
			szSortBin			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBIN];
			szDevice			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE];
			szWaferBin			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WAFERBIN];
			szModelNo			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODELNO];
			szLotID				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];
			szProduct			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PRODUCT];
			szMapLotNo			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER];
			szMapESDVoltage		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD_VOLTAGE];

			szProberMachineNo	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_MACHINE_NO];
			szProberDateTime	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PROBER_DATE_TIME];
			szMapFileDate		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_DATE_IN_MAP_FILE];
			szBinTableSerialNo	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_TABLE_SERIAL_NO];
			szInternalProductNo	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_INTERNAL_PRODUCT_NO];	
			szMode				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MODE];					
			szSpecification		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SPECIFICATION];
			szMapOther			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_MAP_OTHER];
			szSortBinFilename	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SORTBINFNAME];
			szRemark1			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK1];
			szRemark2			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_REMARK2];
			szMapBinTable		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BINTABLE];

			szTestTime			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_TEST_TIME];
			szWO				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WO];
			szSubstarteID		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_SUBSTRATE_ID];
			szCSTID				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CST_ID];
			szRecipeID			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_ID];
			szRecipeName		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RECIPE_NAME];
			szMapFileOperator	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_OPERATOR];

			szPartNo			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PART_NO];
			szMapSerialNumber	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_SERIAL_NUMBER];

			szDN				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DN];
			szWN				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_WN];
			szCM1				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM1];
			szCM2				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM2];
			szCM3				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CM3];

			szUN				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UN];
			szCOND				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COND];
			szLO				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LO];
			szUP				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_UP];
			szFT				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_FT];			
			szLIM				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIM];
			szCOEF1				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF1];
			szCOEF2				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF2];			
			szCOEF3				= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_COEF3];

			szWaferMapCenterPoint	= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_CENTERPOINT];
			szWaferMapLifeTime		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LIFETIME];
			szWaferMapESD			= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_ESD];
			szWaferMapPadColor		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_PAD_COLOR];
			szWaferMapBinCode		= (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_BIN_CODE];

			szMachineModel			= (*m_psmfSRam)["MS896A"]["Machine Model"];

			szFullMagSlot		= (*m_psmfSRam)["BinLoader"]["CurFullSlot"];
			bUseBarcode			= (BOOL)(LONG)(*m_psmfSRam)["BinLoader"]["UseBracode"];

			if (bUseBarcode == TRUE)
			{
				szUseBarcode = "1";
			}

			BOOL bOsramOutput = FALSE;
			bUseBarcodeAsFileName = FALSE;
			if ( bAutoClear == TRUE )
			{
				bUseBarcodeAsFileName = m_bUseBarcodeAsFileName;				
			}

			szMachineType.Format("%d", m_ulMachineType);
			
			CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Start Save Clear Bin Counter Data For Output File sub task");

			szMESOutgoingPath = CMESConnector::Instance()->GetMESOutgoingPath();
			
			lDuplicateFileRetryCount = 0;
			lRetryCount = 0;

			CString szOutputFiePath2 = " ";
			if( m_bValidOutputFilePath2 && m_bEnableBinOutputFilePath2 )
			{
				szOutputFiePath2 = m_szOutputFilePath2;
				//v4.44T2	//Silan
				(*m_psmfSRam)["BinTable"]["Output File Path 3"]	= m_szOutputFilePath3;
				(*m_psmfSRam)["BinTable"]["Output File Path 4"]	= m_szOutputFilePath4;
			}

			(*m_psmfSRam)["BinTable"]["Check OutputFile Repeat"] = m_bCheckOutputFileRepeat; // 4.51D6 SanAn

			do
			{
				bFileExist = FALSE;
				bContinueToGenerateOutputFile = FALSE;
				szDuplicateFileRetryCount.Format("%d", lDuplicateFileRetryCount);
				szRetryCount.Format("%d", lRetryCount);

				//Save all input data
				m_oBinBlkMain.SaveClearBinCountDataForOutputFile(ulPhyBlkId,'0',m_bIfGenOutputFile, bUseBarcodeAsFileName,
							szMachineNo, szLotNumber, szBinLotDirectory, m_szOutputFileSummaryPath , m_szFileNo, m_szBlueTapeNo, 
							szMapSpecVersion, szBinParameter, szBinOutputFileFormat, m_szOutputFilename, 
							szUnloadDate, szUnloadTime, m_szOutputBarcode, m_szLabelFilePath,
							szWorkNo, szPrefix, szLoadPKGCount, 
							szType, szSortBin, szDevice, 
							szWaferBin, szModelNo, szLotID,
							szProduct, m_szSpecVersion, szMachineType,
							szSlotID, szMapLotNo, szOperatorId, CLEAR_BIN_BY_PHY_BLK, m_szaExtraBinInfoField, 
							m_szaExtraBinInfo , szSoftwareVersion, szPKGFilename, szProberMachineNo, szProberDateTime, 
							szBinTableSerialNo, szMESOutgoingPath, szInternalProductNo, szMode, szSpecification, szMapOther,
							szSortBinFilename, szWaferLotFilename, szMapBinTable, szDuplicateFileRetryCount, szMapESDVoltage,
							szTestTime, szWO, szSubstarteID, szCSTID, szRecipeID, szRecipeName, szMapFileOperator, 
							szMapFileDate, szRemark1, szRemark2, szPartNo, szBinDieType, m_lBinBarcodeMinDigit,
							szDN, szWN, szCM1, szCM2, szCM3, szUN, szCOND, szLO, szUP, szFT, szLIM, szCOEF1, szCOEF2, szCOEF3,
							szPLLMDieFabMESLot, szFullMagSlot, m_szLabelPrinterSel, szMapSerialNumber, szRetryCount, szWaferMapCenterPoint,
							szWaferMapLifeTime, szWaferMapESD, szWaferMapPadColor, szWaferMapBinCode, szMachineModel, szPRDeviceID, 
							szUseBarcode, szOutputFiePath2, bForSummary, m_psmfSRam);	//v4.13T1
				
				//Database generating
				szaPopUpMsg.RemoveAll();

				CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Start Generate Output File Database sub task");


				lGenDatabaseStatus = m_oBinBlkMain.GenOutputFileDatabase(ulPhyBlkId,'0',szaPopUpMsg, CLEAR_BIN_BY_PHY_BLK);

				//Retrun value: 1 = OK; 0 = 0 Die count; -1 = Gen Database error; -2 = Load temp file error
				if (lGenDatabaseStatus != 1) 
				{
					//Print error messages
					switch(lGenDatabaseStatus)
					{
					default:
						SetAlert_Msg_Red_Yellow(IDS_BT_NO_DIE_TO_GEN_OTF, szPhyBlkId);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("BT No Die To Gen OTF");
						break;

					case -1:
						SetAlert_Msg_Red_Yellow(IDS_BT_FAIL_TO_GEN_DATABASE, szPhyBlkId);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("BT Fail To Gen Database");
						break;

					case -2:
						SetAlert_Msg_Red_Yellow(IDS_BT_FAIL_TO_LOAD_TEMP_FILE, szPhyBlkId);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail To Load Temp File");
						break;
					}

					for (INT m=0; m<szaPopUpMsg.GetSize(); m++)
					{
						CString szMsg = "BT: ClrBinCntCmd SubTasks Gen OutputFileDatabase err -> " + szaPopUpMsg.GetAt(m);
						SetErrorMessage(szMsg);
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
					}

					if (lGenDatabaseStatus == 0)
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}

				//Get SPC data & saved in temp directory (Removed! Since SPC data already generated when stop)

				//Standard Output File generating
				if( m_bIfGenOutputFile==FALSE )
				{
					m_oBinBlkMain.CleanupGenAndQueryDB(ulPhyBlkId, '0', CLEAR_BIN_BY_PHY_BLK);
					break;
				}
				else
				if (m_bIfGenOutputFile)
				{
					//Backup Temp & database file
					CTime theTime = CTime::GetCurrentTime();
					CString szTimeStamp;

					szTimeStamp = theTime.Format("%y%m%d%H%M%S");
					m_oBinBlkMain.BackupClearBinFiles(ulPhyBlkId, '0', TRUE, szTimeStamp, 
						CLEAR_BIN_BY_PHY_BLK);

					CString szClrBinOutPath = m_szOutputFilePath;
					if (!m_bUseBackupPath && !IsPathExist(szClrBinOutPath))
					{
						if (m_bEnableBinOutputFilePath2)
						{
							szClrBinOutPath = m_szOutputFilePath2;
						}
					}

					if( bOsramOutput && m_bAutoUploadOutputFile==FALSE )
					{
						if (!IsPathExist(szClrBinOutPath))
						{
							CString szTitle, szContent;
							szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
							szTitle = szTitle + " Bin " + szPhyBlkId;
							szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);
							HmiMessage_Red_Back(szContent, szTitle);
							CMSLogFileUtility::Instance()->BL_LogStatus("BT: osram bin need to use BACKUP path");
							m_bUseBackupPath = TRUE;
						}
					}

					if (m_bUseBackupPath)
					{
						szClrBinOutPath = gszUSER_DIRECTORY + "\\OutputFile\\ClearBin";
					}

					if( m_bAutoUploadOutputFile )
					{
						szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;
					}	//	temp local for auto upload

					szaPopUpMsg.RemoveAll();

					if( pApp->GetCustomerName()==CTM_SEMITEK )
					{
						pApp->SearchAndRemoveFiles(gszUSER_DIRECTORY + "\\OutputFile\\ClearBin", 60);
					}

					//StartLoadingAlert();
					CWinThread* pThread = AfxBeginThread(RUNTIME_CLASS(CFlushMessageThread), NULL, THREAD_PRIORITY_NORMAL); 
					((CFlushMessageThread *)pThread)->m_pStation = dynamic_cast<CMS896AStn*> (this);

					
					bPrintLabel = m_bIfPrintLabel && m_bIfGenLabel;

					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Start Generate Standard Output File");

					LONG lAlarmStatus = GetAlarmLamp_Status();	//	old->new
					if( bOsramOutput )
					{
						SetAlarmLamp_Red_Only(m_bEnableAlarmLampBlink, FALSE);		//	old->new
					}
					lGenStdOutputFileOk = m_oBinBlkMain.GenStandardOutputFile(ulPhyBlkId, '0',
						szClrBinOutPath, szBinOutputFileFormat, bPrintLabel, szaPopUpMsg, CLEAR_BIN_BY_PHY_BLK);
					if( bOsramOutput )
					{
						SetAlarmLamp_Back(lAlarmStatus, FALSE, FALSE);	//	old->new
					}
					
					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("End Generate Standard Output File");

					//CloseLoadingAlert();
					pThread->PostThreadMessage(IDC_END_FLUSH_MESSAGE_THREAD, 0, 0);

					//Remove Temp & database file
					if (CMS896AStn::m_bKeepOutputFileLog == FALSE)
					{
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("1:Donot keep the Back-Up Clear-Bin Files");

						if (lGenStdOutputFileOk == 1)		//v2.78T2
						{
							m_oBinBlkMain.BackupClearBinFiles(ulPhyBlkId,'0', FALSE, szTimeStamp, CLEAR_BIN_BY_PHY_BLK);
						}
					}
					//0 = Cannot create object; 1 = OK; -1 = fail to gen output file; -2 = print label error; -3 = output format not define
					if ( (lGenStdOutputFileOk == 1) && bShowMessage )
					{
						for (INT m=0; m<szaPopUpMsg.GetSize(); m++)
						{
							SetErrorMessage("BT: ClrBinCntCmd SubTasks Gen Standard OutputFile Warning -> " + szaPopUpMsg.GetAt(m));
							CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Gen Standard OutputFile Warning Message: " + szaPopUpMsg.GetAt(m));
						}
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Standard output file is generated successfully for Physical Block " +szPhyBlkId + "!");
					}
					else
					{
						if( (lGenStdOutputFileOk!=1) && (szClrBinCntMode=="Single Block" || bOsramOutput) )
						{
							UINT uiErrorCode;
							switch(lGenStdOutputFileOk)
							{
							case 0:	
								uiErrorCode = IDS_BT_FAIL_TO_CREATE_OBJ;
								SetErrorMessage("Fail to create OBJ for blk " + szPhyBlkId);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to create OBJ for blk " + szPhyBlkId);
								break;

							case -1:
								uiErrorCode = IDS_BT_FAIL_TO_GEN_OTF;
								SetErrorMessage("Fail to gen output file for blk " + szPhyBlkId);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to create OBJ for blk " + szPhyBlkId);
								break;

							case -2:
								uiErrorCode = IDS_BT_FAIL_TO_PRN_LABEL;
								SetErrorMessage("Fail to print label for blk " + szPhyBlkId);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to print label for blk " + szPhyBlkId);
								break;

							case -3:
								uiErrorCode = IDS_BT_NO_OTF_FORMAT;
								SetErrorMessage("No output file format file");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Fail to print label for blk " + szPhyBlkId);
								break;

							case -4:
								bFileExist = TRUE;
								szaPopUpMsg.RemoveAll();
								uiErrorCode = IDS_BT_FILE_ALREDAY_EXIST;
								SetErrorMessage("File already exist");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("File already exist");
								lDuplicateFileRetryCount = lDuplicateFileRetryCount + 1;
								break;
							
							case -5:
								uiErrorCode = IDS_BT_CP_BARCODE_LENGTH_FAILED;
								SetErrorMessage("Compare barcode length failed");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Compare barcode length failed");
								break;
							
							case -6: // without retry one
								uiErrorCode = IDS_BT_FILE_ALREDAY_EXIST ;
								SetErrorMessage("File already exist");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("File already exist");
								break;

							case -8:
								bContinueToGenerateOutputFile = TRUE;
								uiErrorCode = IDS_BT_MANUAL_INPUT_BC;
								SetErrorMessage("Manual input barcode");
								lDuplicateFileRetryCount = lDuplicateFileRetryCount + 1;
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Manual input barcode");
								break;

							//4.55T08
							case -9:
								lGenStdOutputFileOk = 1; //final checking after generateing output file
								uiErrorCode = IDS_BT_DIE_COUNT_MATCH_ERR;
								SetErrorMessage("Physical Die Count mathch Temp file Error");
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("Physical Die Count mathch Temp file Error");		
								break;
							
										
							}

							//v4.12T1
							for (INT m=0; m<szaPopUpMsg.GetSize(); m++)
							{
								CString szMsg = "BT: ClrBinCntCmd SubTasks Gen Standard OutputFile Err -> " + szaPopUpMsg.GetAt(m);
								SetErrorMessage(szMsg);
								CMSLogFileUtility::Instance()->BT_ClearBinCounterLog(szMsg);
							}

							SetAlert_Msg_Red_Yellow(uiErrorCode, szPhyBlkId);
						}
					}
				}

			}while(bFileExist == TRUE || bContinueToGenerateOutputFile == TRUE);


			if ( lGenStdOutputFileOk == 1 )
			{
				//Send Clear Bin info to Host
				if (bResetDieCount == TRUE)
				{
					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("2: Reset Die Count");

					SendClearBinInfoToHost(ulPhyBlkId);

					if (m_ulEnableOptimizeBinCount >= TRUE)
					{
						CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("3: Enable Optimize Bin Count");
						m_oBinBlkMain.UpdateLotRemainCountByPhyBlk(ulPhyBlkId);
						m_oBinBlkMain.OptimizeBinFrameCountByPhyBlk(ulPhyBlkId, m_ucOptimizeBinCountMode, m_ulEnableOptimizeBinCount);
					}
					
					//Clear Bin Counter
					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("4: Clear Bin Counter");
					m_oBinBlkMain.ClearBinCounterByPhyBlk(ulPhyBlkId);
					m_oBinBlkMain.SetLeaveEmptyRow(ulPhyBlkId, FALSE);		//Reset flag for CyOptics	//v4.30T4
					m_oBinBlkMain.ResetEmptyRow1stRowIndex(ulPhyBlkId);		//v4.40T2
					ResetNichiaWafIDList(ulPhyBlkId);						//v4.40T6	//Nichia
					CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("5: Has been reset row index");

					CMSLogFileUtility::Instance()->BT_BinTableMapIndexLogDelete();

					if (bShowMessage && szClrBinCntMode == "Single Block")
					{
						CString szTitle, szContent;

						szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
						szContent.LoadString(HMB_BT_PHY_BLK_COUNT_CLEARED);
						
						HmiMessage(szContent, szTitle);
					}
				}
				if (szBinOutputFileFormat != "Semitek2")
					OpSemitekCheckEmptyFrame(ulPhyBlkId, m_szOutputBarcode);	//v4.51A19	//Matt
				bReturn = TRUE;
			}
		}
	}
	else
	{
		if ( bIsBlkNeedClear == TRUE )
		{
			if (bResetDieCount == TRUE)
			{
				//Send Clear Bin info to Host
				SendClearBinInfoToHost(ulPhyBlkId);

				//Clear Bin Counter
				m_oBinBlkMain.ClearBinCounterByPhyBlk(ulPhyBlkId);
				m_oBinBlkMain.SetLeaveEmptyRow(ulPhyBlkId, FALSE);		//Reset flag for CyOptics	//v4.30T4
				m_oBinBlkMain.ResetEmptyRow1stRowIndex(ulPhyBlkId);		//v4.40T2
				ResetNichiaWafIDList(ulPhyBlkId);						//v4.40T6	//Nichia

				CMSLogFileUtility::Instance()->BT_BinTableMapIndexLogDelete();

				//Update Serial No
				//m_oBinBlkMain.UpdateSerialNoGivenPhyBlkId(ulPhyBlkId);

				if (bShowMessage && szClrBinCntMode == "Single Block")
				{
					CString szTitle, szContent;

					szTitle.LoadString(HMB_BT_CLEAR_BIN_COUNTER);
					szContent.LoadString(HMB_BT_PHY_BLK_COUNT_CLEARED);
						
					HmiMessage(szContent, szTitle);
				}
			}

			bReturn = TRUE;
		}
	}

	if (pApp->GetCustomerName() == "DeLi")
	{
		CMSLogFileUtility::Instance()->MS_LogOperation("DeLi:Clear Bin and delete BinGrade Info");
		remove("C:\\MapSorter\\UserData\\OutputFile\\ClearBin\\BinGrade");
	}

	if (pApp->GetCustomerName() == "NSS")
	{
		remove("C:\\MapSorter\\UserData\\OutputFile\\ClearBin\\DeviceNumber");
	}

	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("End Of ClrBinCntCmd SubTasks\n");
	return bReturn;
} //end ClrBinCntCmd SubTasks

BOOL CBinTable::SaveBondedCountsByBlock(ULONG ulBondedBlkId)
{
	//m_oBinBlk[i].GetNVNoOfBondedDie()
	CStringMapFile *pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile == NULL)
	{
		//AfxMessageBox("FALSE");
		return FALSE;
	}
	else
	{
		//AfxMessageBox("TRUE");
		CTime theTime = CTime::GetCurrentTime();
		CString szDate;

		int nYear	= theTime.GetYear();
		int nMonth	= theTime.GetMonth();
		int nDay	= theTime.GetDay();
		int nHour	= theTime.GetHour();
		int nMinute = theTime.GetMinute();
		int nSecond = theTime.GetSecond();

		szDate.Format("%d-%d-%d,%d:%d:%d", nYear, nMonth, nDay, nHour, nMinute, nSecond);

		(*pBTfile)["BinBlock"][ulBondedBlkId]["ClearBinDate"] = szDate;
		//(*pBTfile)["BinBlock"][ulBondedBlkId]["BondedDieCount"] = m_oBinBlkMain.GrabNVNoOfBondedDie(ulBondedBlkId);
		for ( ULONG i = 1 ; i <= m_oBinBlkMain.GetNoOfBlk() ; i++ )
		{
			(*pBTfile)["BinBlock"][i]["BondedDieCount"] = m_oBinBlkMain.GrabNVNoOfBondedDie(i);
		}

		CMSFileUtility::Instance()->SaveBTConfig();
		return TRUE;
	}
}

//v4.57A7
ULONG CBinTable::GrabBondXYPosn(UCHAR ucGrade, DOUBLE& dXPosn, DOUBLE& dYPosn, LONG& lRow, LONG& lCol, LONG& lSortDir)
{
	//v4.52A6
	ULONG ulBinBlkInGrade = 0;
	ULONG ulPhyBlkInUse = 0, ulBinBlkInUse = 0;

	if( m_bDisableBT )
	{
		ulBinBlkInUse =  ulPhyBlkInUse = ulBinBlkInGrade = ucGrade;
	}
	else
	{
		ulBinBlkInUse =  m_oBinBlkMain.FindBondXYPosn(ucGrade, dXPosn, dYPosn, lRow, lCol, lSortDir);
		if( m_bEnableOsramBinMixMap)
		{
			m_ucBinMixCurrentGrade = m_oBinBlkMain.GetBinMixCurrentGrade();
		}

		//ulPhyBlkInUse = m_oBinBlkMain.GrabPhyBlkId(ulBinBlkInUse);
		ulBinBlkInGrade = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);
		ulPhyBlkInUse = m_oBinBlkMain.GrabPhyBlkId(ulBinBlkInGrade);
	}

	(*m_psmfSRam)["BinTable"]["PhyBlkInUse"] = ulPhyBlkInUse;

	return ulBinBlkInUse;
} //end GrabBondXYPosn


BOOL CBinTable::IfBondOK(ULONG ulBondedBlk, LONG lWaferX, LONG lWaferY, 
							LONG lWafEncX, LONG lWafEncY, LONG lBtEncX, LONG lBtEncY)
{
	BOOL bReturn = TRUE;
	INT nStatus = 0;

	//Update Share memory Current Index 
	ULONG ulCurrIndex = m_oBinBlkMain.GrabNVCurrBondIndex(ulBondedBlk);
	ULONG ulOriIndex = m_oBinBlkMain.GrabOriginalIndex(ulBondedBlk,ulCurrIndex + 1);
	(*m_psmfSRam)["BinTable"][ulBondedBlk]["CurrIndex"] = ulCurrIndex;
	(*m_psmfSRam)["BinTable"]["CurrIndex"]		= ulOriIndex;

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

//	SanAn TJ 2017
	UCHAR ucBondGrade = m_oBinBlkMain.GrabGrade(ulBondedBlk);
	ucBondGrade = min(ucBondGrade, BT_MAX_BINBLK_SIZE - 1);
	if( m_oBinBlkMain.GrabNVNoOfBondedDie(ulBondedBlk)==0 || 
		m_oBinBlkMain.GrabNVNoOfSortedDie(ucBondGrade)==0 )
	{
		CString szBlkId;
		szBlkId.Format("%02d", ulBondedBlk);
		CString szTempFilename = gszROOT_DIRECTORY + "\\Exe\\" + "Blk" + szBlkId + "TempFile.csv";
		for (int mm = 0; mm < 3; mm++)
		{
			if (_access(szTempFilename, 0) != -1)
			{
				SetErrorMessage("BT die counter is 0, but exist, so delete " + szTempFilename);
				DeleteFile(szTempFilename);
			}
			else
			{
				break;
			}
		}

		// ChangeLight request 1: If new bin, record the bond time into a txt file 
		if ( (pApp->GetCustomerName() == "ChangeLight(XM)") || 
			 (pApp->GetCustomerName() == CTM_SANAN) )
		{
			CString szFileSavedPath = gszROOT_DIRECTORY + "\\Exe\\NewBinTime";
			CreateDirectory(szFileSavedPath, NULL);

			CString szNewBinTimeFile = gszROOT_DIRECTORY + "\\Exe\\NewBinTime\\" + szBlkId + ".txt";
			RecordNewBinTimeToFile(szNewBinTimeFile);
		}
	}


	nStatus = m_oBinBlkMain.BondOK(ulBondedBlk, lWaferX, lWaferY, lWafEncX, lWafEncY, lBtEncX, lBtEncY);	//andrewng //2020-0630

	if (nStatus == FALSE)
	{
		return FALSE;
	}
	else if (nStatus == BT_BACKUP_NVRAM)
	{
		CString szLogMsg = "";
		CTime theTime = CTime::GetCurrentTime(); 

		szLogMsg.Format("%d-%2d-%2d %2d:%2d:%2d - Start Backup NVRunTimeData\n",
			theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);
		
		// Backup to NVRam 
		BackupToNVRAM();
	
		szLogMsg.Format("%d-%2d-%2d %2d:%2d:%2d - Complete Backup NVRunTimeData\n", 
			theTime.GetYear(), theTime.GetMonth(), theTime.GetDay(), theTime.GetHour(), theTime.GetMinute(), theTime.GetSecond());
		CMSLogFileUtility::Instance()->BT_GenTempLog(szLogMsg);
		if( pApp->GetCustomerName()=="Genesis" )
		{
			CMSLogFileUtility::Instance()->BT_GenTempLog("Run Time backup map file for Genesis");
			OpBackupMap();
		}

		m_bNVRamUpdatedInIfBondOK = TRUE;	//v4.59A4
	}
	else if (nStatus == BT_BACKUP_NVRAM_ERROR)
	{
		//SetAlert_Red_Yellow(IDS_BT_GEN_TMP_FILE_ERROR);
		//return FALSE;
	}

	//v4.57A13	//Optional: for ChangeLight only
	//Used by BPR PostBond LF to search upper-row die to check myler-residue problem
	//Requested by Alex Yu
	if (m_bEnableBinMapBondArea)
	{
		(*m_psmfSRam)["BinTable"]["BinMapLastRowDie"] = m_oBinBlkMain.m_bIsBinMapLastRowDieExist;
	}
	return TRUE;
} //end IfBondOK


BOOL CBinTable::SaveBondResult(UCHAR ucGrade, ULONG ulBondedBlk, 
							   LONG lBinTableX, LONG lBinTableY, 
							   LONG lWaferX, LONG lWaferY)
{
	LONG lBTXInDisplayUnit = 0 - ConvertXEncoderValueForDisplay(lBinTableX);
	LONG lBTYInDisplayUnit = 0 - ConvertYEncoderValueForDisplay(lBinTableY);

	return m_oBinBlkMain.SaveDieInfoToFile(ucGrade, ulBondedBlk, 
			lBTXInDisplayUnit,lBTYInDisplayUnit, 
			lBinTableX, lBinTableY, 
			lWaferX, lWaferY);
} //end SaveBondResult


BOOL CBinTable::InitBinTableRunTimeData()
{
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	
	if ( pBTfile == NULL )
		return FALSE;

	//v4.24
	m_nBTInUse = (INT)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_IN_USE]);
	if (!m_bUseDualTablesOption)
		m_nBTInUse = 0;		//0=DEFAULT, 1=BT2

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_FILENAME])) == FALSE)
	{
		m_szPKGFilenameDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_FILENAME];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIEPITCH_X_DISPLAY])) == FALSE)
	{
		m_ulPKGDiePitchXDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIEPITCH_X_DISPLAY];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIEPITCH_Y_DISPLAY])) == FALSE)
	{
		m_ulPKGDiePitchYDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIEPITCH_Y_DISPLAY];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_INPUT_COUNT_DISPLAY])) == FALSE)
	{
		m_ulPKGInputCountDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_INPUT_COUNT_DISPLAY];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_PER_ROW_DISPLAY])) == FALSE)
	{
		m_ulPKGDiePerRowDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_PER_ROW_DISPLAY];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_PER_COL_DISPLAY])) == FALSE)
	{
		m_ulPKGDiePerColDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_PER_COL_DISPLAY];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_CAPACITY_DISPLAY])) == FALSE)
	{
		m_ulPKGBlkCapacityDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_CAPACITY_DISPLAY];
	}

	if ((!((*pBTfile)[BT_PKG_DISPLAY][BT_PKG_BIN_BLK_NO_DISPLAY])) == FALSE)
	{
		m_ulPKGNoOfBinBlkDisplay = (*pBTfile)[BT_PKG_DISPLAY][BT_PKG_BIN_BLK_NO_DISPLAY];
	}

	CMSFileUtility::Instance()->CloseBTConfig();

	return TRUE;

}


BOOL CBinTable::InitBinTableData()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	BOOL bManualLoadPkg			= (BOOL)(LONG)(*m_psmfSRam)["MS896A"]["Manual Load Pkg"];
	if ( pBTfile == NULL )
		return FALSE;


	//Load ouptut file format & path
	m_szBinOutputFileFormat = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FORMAT_NAME])) == FALSE)
	{
		m_szBinOutputFileFormat = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FORMAT_NAME];
	}

	m_szOutputFileBlueTapeNo = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_BLUETAPE_NO])) == FALSE)
	{
		m_szOutputFileBlueTapeNo = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_BLUETAPE_NO]; // v4.51D10 Dicon

	} 

	m_szLabelPrinterSel = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_LABEL_PRINTER_SEL])) == FALSE)
	{
		m_szLabelPrinterSel = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_LABEL_PRINTER_SEL];
		
	}
			
	m_szOutputFilePath = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH])) == FALSE)
	{
		m_szOutputFilePath = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH];
	}

	m_szOutputFilePath2 = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH2])) == FALSE)
	{
		m_szOutputFilePath2 = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH2];
	}
	(*m_psmfSRam)["MS896A"]["OutputFilePath2"] = m_szOutputFilePath2;
	m_szOutputFilePath3 = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH3];
	m_szOutputFilePath4 = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH4];

	//Load waferend file option, format & path
	m_bIfGenWaferEndFile = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FILE_ENABLE]);

	m_szWaferEndFileFormat = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FORMAT_NAME])) == FALSE)
	{
		m_szWaferEndFileFormat = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FORMAT_NAME];
	}

	m_szWaferEndPath = " ";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FILE_PATH])) == FALSE)
	{
		m_szWaferEndPath = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FILE_PATH];
	}

	m_dWaferEndSortingYield = (DOUBLE)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_YIELD];

	m_szOutputFileSummaryPath = "";
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_SUMMARY_PATH])) == FALSE)
	{
		m_szOutputFileSummaryPath = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_SUMMARY_PATH];
	}

	m_szWaferEndSummaryPath	=  "";

	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFER_END_SUMMARY_PATH])) == FALSE)
	{
		m_szWaferEndSummaryPath = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFER_END_SUMMARY_PATH];
	}

	m_bUseUniqueSerialNum = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_UNIQUE_SERIAL_NUM]);		//v3.23T1	
	m_bUseMultipleSerialCounter = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_MULTIPLE_SERIAL_COUNTER]);		//v3.23T1	

	//Load Bin Summary Output File Path
	m_szBinSummaryOutputPath = "";	
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_BIN_SUMMARY_OUTPUT_PATH])) == FALSE)
	{
		m_szBinSummaryOutputPath = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_BIN_SUMMARY_OUTPUT_PATH];
	}

	//Check format name
	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	if ( pAppMod != NULL )
	{
		pAppMod->CheckOutputFileFormatName(m_szBinOutputFileFormat);
		pAppMod->CheckWaferEndFileFormatName(m_szWaferEndFileFormat);
	}

	//Use Filename as WaferID option
	m_bFilenameAsWaferID = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILENAME_AS_WAFERID]);

	//Use BL barcode as output file name
	m_bUseBarcodeAsFileName = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_USE_BARCODE_NAME]);

	//Print label	
	m_bIfPrintLabel = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_PRINT_LABEL]);
	if (CMS896AStn::m_bIsAlwaysPrintLabel == TRUE)
	{
		m_bIfPrintLabel = TRUE;
	}
	m_bAutoClearBin = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_AUTO_CLEAR_BIN]);		//v2.93T2

	//Load label file path
	m_szLabelFilePath = gszROOT_DIRECTORY + _T("\\OutputFiles");
	if ((!((*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_LABEL_FILEPATH])) == FALSE)
	{
		m_szLabelFilePath = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_LABEL_FILEPATH];
	}
	
	//Nichia
	m_bEnableNichiaOutputFile		= (BOOL)(LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_NICHIA_ENABLE_OUTPUTFILE];
	m_bEnableNichiaOTriggerFile		= (BOOL)(LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_NICHIA_ENABLE_OTRIGGERFILE];
	m_bEnableNichiaRPTFile			= (BOOL)(LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_NICHIA_ENABLE_RPTFILE];

	//Realignment delay & sameview option
	//m_lAlignBinDelay = (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_REALIGN_DELAY];
	m_bUseSameView = (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_REALIGN_SAMEVIEW]);

	//Change grade option
	m_ulGradeChangeLimit = (*pBTfile)[BT_GRADECHANGE_OPTION][BT_GRADE_COUNT];

	//Display binblk image when binfull
	m_bShowBinBlkImage = (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_SHOW_BINBLK_IMAGE]);

	//BT Spec Version
	m_szSpecVersion = (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_SPEC_VERSION];

	//v2.63
	//Realign Last Die Offset
	m_dAlignLastDieOffsetX		= (*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_OFFSET_X];
	m_dAlignLastDieOffsetY		= (*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_OFFSET_Y];
	m_dAlignLastDieRelOffsetX	= (*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_REL_OFFSET_X];
	m_dAlignLastDieRelOffsetY	= (*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_REL_OFFSET_Y];
	m_bDisableClearAllPrintLabel = (BOOL)((LONG)(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_OFFLINE_PRINT_LABEL]);		//v2.70

	m_bUseFrameCrossAlignment	= (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_USE_FRAME_CROSS]);	

	//-------- Extra Information for clear bin & output file ------------//
	for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		szTemp.Format("%s %d",BT_EXTRA_BIN_INFO_ITEM,i+1);
		m_szaExtraBinInfo[i] = (*pBTfile)[BT_OUTPUTFILE_OPTION][BT_EXTRA_BIN_INFO][szTemp];
	}

	LoadExtraClearBinInfo();

	m_bWaferEndUploadMapMpd	= (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_WAFEREND_UPLOAD_MAP_MPD]);
	m_bEnableBinOutputFile	= (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_ENABLE_OUTPUT_FILE]);
	m_bFinForBinOutputFile	= (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BIN_OUTPUT_FILE_FIN]);
	m_bEnableBinMapFile		= (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_ENABLE_BIN_MAP_FILE]);
	m_bFin2ForBinMapFile	= (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BIN_MAP_FILE_FIN2]);
	m_bEnable2DBarcodeOutput = FALSE;	//v3.33T3
	if (pApp->GetCustomerName() == "ChangeLight(XM)")
	{
		//Default setting by Matthew 20190415
		if ((!((*pBTfile)[BT_TABLE_OPTION][BT_TABLE_ENABLE_2DBARCODE_OUTPUT])) == FALSE)
		{
			m_bEnable2DBarcodeOutput = (BOOL)((LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_ENABLE_2DBARCODE_OUTPUT]);
		}
	}

	m_ulHoleDieNum = 3;					//v3.33T3
	if ((!((*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NUM_OF_HOLE_DIE])) == FALSE)
	{
	m_ulHoleDieNum = (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NUM_OF_HOLE_DIE];
	}

	m_lRealignBinFrameOption = (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_FRAME_OPTION];		//v3.70T3
	(*m_psmfSRam)["BinTable"]["RealignFrameOption"] = m_lRealignBinFrameOption;					//v3.71T5

	m_bPrPreBondAlignment		= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNMENT];		//v3.79
	m_lCollet1PadOffsetX		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C1_PAD_OFFSET_X];
	m_lCollet1PadOffsetY		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C1_PAD_OFFSET_Y];
	m_lCollet2PadOffsetX		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C2_PAD_OFFSET_X];
	m_lCollet2PadOffsetY		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C2_PAD_OFFSET_Y];

	m_bEnableAutoLoadRankFile	= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_ENABLE_AUTO_LOAD_RANK_FILE];
	m_bLoadRankIDFromMap		= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_LOAD_RANK_FILE_FROM_MAP];
	m_bAutoBinBlkSetupWithTempl = (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_AUTO_SETUP_BINBLK_WITH_TEMPL];
	m_szRankIDFilePath			= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_RANK_FILE_PATH];
	m_b1ptRealignComp			= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_1PT_COMP];			//v3.86	
	m_bClearBinInCopyTempFileMode  = (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_SAVE_TEMP_FILE];	//v4.44T6

	m_dSpeedModeOffsetX		= (*pBTfile)[BT_TABLE_OPTION][BT_SPEEDMODE_OFFSET_X];	//v3.62
	m_dSpeedModeOffsetY		= (*pBTfile)[BT_TABLE_OPTION][BT_SPEEDMODE_OFFSET_Y];	//v3.62

	if( IsLoadingPortablePKGFile() && IsLoadingPKGFile() )
	{
	}
	else
	{
		if (!bManualLoadPkg || pApp->GetProductLine() != "Rebel")
		{
			m_lBHZ1BondPosOffsetX	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_BOND_OFFSET_X];
			m_lBHZ1BondPosOffsetY	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_BOND_OFFSET_Y];

			m_lBHZ2BondPosOffsetX	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_BOND_OFFSET_X];
			m_lBHZ2BondPosOffsetY	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_BOND_OFFSET_Y];
		}
	}

	if (!bManualLoadPkg || pApp->GetProductLine() != "Rebel")
	{
		SaveBTCollet1Offset(m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY, m_dXResolution_UM_CNT);
		SaveBTCollet2Offset(m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY, m_dXResolution_UM_CNT);
	}
	
	//MS100 9Inch dual-table BT2 offset XY	//v4.17T3	//v4.37
	//m_lBT2OffsetX	= (*pBTfile)[BT_TABLE2_OPTION][BT2_OFFSET_X];
	//m_lBT2OffsetY	= (*pBTfile)[BT_TABLE2_OPTION][BT2_OFFSET_Y];
	
	//v4.55A8
	m_lBHZ1PrePickToBondOffsetX	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_PREPICK_OFFSET_X];
	m_lBHZ1PrePickToBondOffsetY	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_PREPICK_OFFSET_Y];
	m_lBHZ2PrePickToBondOffsetX	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_PREPICK_OFFSET_X];
	m_lBHZ2PrePickToBondOffsetY	= (*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_PREPICK_OFFSET_Y];

	m_dBond180DegOffsetX_um		= (DOUBLE)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BOND_180_DEGREE_OFFSET_X_UM];
	m_dBond180DegOffsetY_um		= (DOUBLE)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BOND_180_DEGREE_OFFSET_Y_UM];
	SaveBTBond180DegOffset(m_dBond180DegOffsetX_um, m_dBond180DegOffsetY_um);
	//v4.43T8	//Jenoptic
	m_lMagSummaryYear			= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_MAG_SUM_YEAR];
	m_lMagSummaryMonth			= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_MAG_SUM_MONTH];
	m_lMagSummaryDay			= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_MAG_SUM_DAY];

	//v4.03		//PLLM REBEL
	m_bEnableBinMapBondArea		= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP];
	//m_bEnableBinMapCDieOffset	= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CDIE_OFFSET];
	m_dBinMapCircleRadiusInMm	= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CIRCLE_RADIUS];
	m_lBinMapEdgeSize			= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_EDGE_SIZE];
	m_lBinMapBEdgeSize			= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_BEDGE_SIZE];
	m_lBinMapLEdgeSize			= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_LEDGE_SIZE];
	m_lBinMapREdgeSize			= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_REDGE_SIZE];
	//m_lBinMapCDieRowOffset	= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CDIE_ROW_OFFSET];
	//m_lBinMapCDieColOffset	= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CDIE_COL_OFFSET];
	m_bLoadRankIDFile			= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_RANKID_FILE];	//v4.21T3	//TongFang
	m_ucSpGrade1ToByPassBinMap	= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_BYPASS_SP_GRADE1];	//v4.48A2
	m_ucBinMixTypeAGrade		= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEA_DIE];
	m_ucBinMixTypeBGrade		= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEB_DIE];
	m_lBinMixTypeAQty			= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEA_QTY];
	m_lBinMixTypeBQty			= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEB_QTY];
	m_lBinMixTypeAStopPoint		= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEA_STOPPOINT];
	m_lBinMixTypeBStopPoint		= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEB_STOPPOINT];

	m_bEnableOsramBinMixMap		= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_BINMAP];
	m_ulBinMixPatternType		= (*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_PATTERN_TYPE];

	m_bIfEnableResortDie		= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_IS_RESORT_MODE];
	m_szBinLotSummaryPath		= (*pBTfile)[BT_TABLE_OPTION][BT_BIN_LOT_SUM_PATH];
	m_szBinLotSummaryFilename	= (*pBTfile)[BT_TABLE_OPTION][BT_BIN_LOT_SUN_FILENAME];
	m_ucOptimizeBinCountMode	= (UCHAR)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_OPTIMIZE_BIN_COUNT_MODE];
	m_ulEnableOptimizeBinCount	= (ULONG)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_OPTIMIZE_BIN_COUNT];
	m_bEnableBinSumWaferLotCheck = (ULONG)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_BIN_SUMMARY_WAFER_LOT_CHECK];
	m_ulMinLotCount				=  (ULONG)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_MIN_LOT_COUNT];
	m_szInputCountSetupFilename = (*pBTfile)[BT_TABLE_OPTION][BT_BIN_INPUT_COUNT_SETUP_FILENAME];
	m_szInputCountSetupFilePath	= (*pBTfile)[BT_TABLE_OPTION][BT_BIN_INPUT_COUNT_SETUP_FILEPATH];
	m_lBinBarcodeMinDigit		= (*pBTfile)[BT_TABLE_OPTION][BT_MIN_BAR_DIGIT];	
	m_szBinMapFilePath			= (*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_FILE_PATH];
	
	CMS896AApp::m_bNGWithBinMap	= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION]["NG With Bin Map"];
	CMS896AApp::m_bEnableErrMap	= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION]["EnableErrMap"];		//andrewng //2020-0806

	//subbin
	m_ulSubBinSRowA				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Row A"];
	m_ulSubBinSRowB				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Row B"];
	m_ulSubBinSRowC				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Row C"];
	m_ulSubBinSRowD				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Row D"];

	m_ulSubBinERowA				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Row A"];
	m_ulSubBinERowB				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Row B"];
	m_ulSubBinERowC				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Row C"];
	m_ulSubBinERowD				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Row D"];

	m_ulSubBinEColA				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Col A"];
	m_ulSubBinEColB				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Col B"];
	m_ulSubBinEColC				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Col C"];
	m_ulSubBinEColD				= (*pBTfile)[BT_TABLE_OPTION]["SubBin E Col D"];

	m_ulSubBinSColA				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Col A"];
	m_ulSubBinSColB				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Col B"];
	m_ulSubBinSColC				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Col C"];
	m_ulSubBinSColD				= (*pBTfile)[BT_TABLE_OPTION]["SubBin S Col D"];

	//4.53D91 load
	m_ulFullDieCountInput		= (*pBTfile)[BT_TABLE_OPTION]["Full Die Count Input Per Wafer"];
	m_ulMinDieCountInput		= (*pBTfile)[BT_TABLE_OPTION]["Min Die Count Input Per Wafer"];
	m_bEnableDieCountInput		= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION]["Enable Die Count Input PerWf"];

	//m_bBTIsMoveCollet			= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION]["Bin Table is move collet offset"];
	// Auto Clean Collet
	if (pAppMod->GetCustomerName() != "Testar")
	{
		m_lACCRangeX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_RANGE_X];
		m_lACCRangeY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_RANGE_Y];
		m_lACCCycleCount= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_CYCLE_COUNT];
		m_lACCMatrixRow	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_MATRIX_ROW];
		m_lACCMatrixCol	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_MATRIX_COL];
		m_lACCAreaSizeX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_SIZEX];
		m_lACCAreaSizeY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_SIZEY];
		m_lACCAreaLimit	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_LIMIT];
		m_lACCReplaceLimit = m_lACCMatrixRow * m_lACCMatrixCol * m_lACCAreaLimit;
		if( m_lACCAreaSizeX>m_lACCRangeX )
		{
			m_lACCAreaSizeX = m_lACCRangeX;
		}
		if( m_lACCAreaSizeY>m_lACCRangeY )
		{
			m_lACCAreaSizeY = m_lACCRangeY;
		}
		if( m_lACCCycleCount<=0 )
		{
			m_lACCCycleCount = 5;
		}
	}

	//andrewng //2020-0806
	m_ulErrMapNoOfRow	= (ULONG)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map No Of Row"]);
	m_ulErrMapNoOfCol	= (ULONG)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map No Of Col"]);
	m_dErrMapPitchX		= (DOUBLE)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Pitch X"]);
	m_dErrMapPitchY		= (DOUBLE)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Pitch Y"]);
	m_ulErrMapDelay		= (ULONG)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Delay"]);
	m_ulErrMapCount		= (ULONG)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Count"]);
	m_dErrMapSlope		= (DOUBLE)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Slope"]);
	m_lErrMapRefX1		= (LONG)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefX1"]);
	m_lErrMapRefY1		= (LONG)	((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefY1"]);
	m_dErrMapSamplingTolX		= (DOUBLE)((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Sampling Err Tol X"]);
	m_dErrMapSamplingTolY		= (DOUBLE)((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Sampling Err Tol Y"]);
	m_ulErrMapSamplingTolLimit	= (ULONG)((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Sampling Tol Limit"]);
	//m_lErrMapRefX2			= (LONG) ((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefX2"]);
	//m_lErrMapRefY2			= (LONG) ((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefY2"]);
	m_lErrMapOffsetLimitX		= (LONG) ((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Offset Limit X"]);
	m_lErrMapOffsetLimitY		= (LONG) ((*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Offset Limit Y"]);
	m_ulMarksPrAccLimit			= (ULONG)((*pBTfile)[BT_TABLE_OPTION]["Marks fail Acc Limit"]);
	m_ulMarksPrConLimit			= (ULONG)((*pBTfile)[BT_TABLE_OPTION]["Marks fail Con Limit"]);

	m_lCleanDirtMatrixRow		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_MATRIX_ROW];
	m_lCleanDirtMatrixCol		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_MATRIX_COL];

	m_bACCToggleBHVacuum	= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_ATUO_CC_TOGGLE_BH_VACUUM];
	m_lACCAreaCount			= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_COUNT];
	m_lACCAreaIndex			= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_INDEX];
	if (m_lACCMatrixRow <= 0)
	{
		m_lACCMatrixRow = 1;
	}
	if (m_lACCMatrixCol <= 0)
	{
		m_lACCMatrixCol = 1;
	}

	m_lACCReplaceCount	= (m_lACCAreaIndex - 1) * m_lACCAreaLimit + m_lACCAreaCount;

	//if ( (m_ulMachineType == 2) || (m_ulMachineType == 3) )
	if ( m_ulMachineType >= BT_MACHTYPE_DL_DLA )
	{	
		//Disable on DL machine
		m_bShowBinBlkImage = FALSE;			
	}
	else
	{
		//Disable on STD machine
		m_bUseBarcodeAsFileName = FALSE;
	}

	for ( ULONG i = 0 ; i < m_oBinBlkMain.GetNoOfBlk() ; i++ )
	{
		//Load and Save the Rank Name in SRAM
		(*m_psmfSRam)["MS896A"][WT_MAP_HEADER]["Rank Name"][i] = (*pBTfile)["BinBlock"][i]["Bin Rank Name"];
	}

	CMSFileUtility::Instance()->CloseBTConfig();


	//v4.42T9	//Re-arrange to bottom of this fcn
	BOOL bEnableBinMapFcn = pApp->GetFeatureStatus(MS896A_FUNC_WAFERMAP_ENABLE_BINMAP);
	if (!bEnableBinMapFcn)
	{
		m_bEnableBinMapBondArea		= FALSE;
		m_dBinMapCircleRadiusInMm	= 0.00;
		m_lBinMapEdgeSize			= 0;
		m_lBinMapBEdgeSize			= 0;
		m_lBinMapLEdgeSize			= 0;
		m_lBinMapREdgeSize			= 0;
		m_lBinMapCDieRowOffset		= 0;
		m_lBinMapCDieColOffset		= 0;
		//CMSLogFileUtility::Instance()->MS_LogOperation("BT MSD: binmap fcn is forced OFF");		//v4.36T1
	}

	m_oBinBlkMain.SetUseBinMapBondArea(m_bEnableBinMapBondArea);
	m_oBinBlkMain.SetSpGradeToByPassBinMap(1, m_ucSpGrade1ToByPassBinMap);

	m_oBinBlkMain.SetOSRAMBinMixA(1,		m_ucBinMixTypeAGrade);
	m_oBinBlkMain.SetOSRAMBinMixB(1,		m_ucBinMixTypeBGrade);
	m_oBinBlkMain.EnableOSRAMBinMixMap(1,	m_bEnableOsramBinMixMap);
	m_oBinBlkMain.SetBinMixPatternType(1,	m_ulBinMixPatternType);
	//m_oBinBlkMain.SetBinMixOtherValue(1,	m_lBinMixTypeAQty,m_lBinMixTypeBQty,m_lBinMixTypeAStopPoint,m_lBinMixTypeBStopPoint);
	
	//Update SRAM contents
	(*m_psmfSRam)["MS896A"]["OutputFileFormat"]					= m_szBinOutputFileFormat;
	(*m_psmfSRam)["MS896A"]["OutputFilePath"]					= m_szOutputFilePath;
	(*m_psmfSRam)["MS896A"]["OutputFileBlueTapeNo"]				= m_szOutputFileBlueTapeNo; // v4.51D10 Dicon
	(*m_psmfSRam)["MS896A"]["WaferEndFileFormat"]				= m_szWaferEndFileFormat;
	(*m_psmfSRam)["MS896A"]["WaferEndFilePath"]					= m_szWaferEndPath;
	(*m_psmfSRam)["BinTable"]["EnableOptimizeBinCount"]			= m_ulEnableOptimizeBinCount;
	(*m_psmfSRam)["BinTable"]["PreBondAlignment"]				= m_bPrPreBondAlignment;
	(*m_psmfSRam)["BinTable"]["RealignRelOffsetX"]				= m_dAlignLastDieRelOffsetX;
	(*m_psmfSRam)["BinTable"]["RealignRelOffsetY"]				= m_dAlignLastDieRelOffsetY;
	(*m_psmfSRam)["BinTable"][BT_IS_RESORT_MODE]				= m_bIfEnableResortDie;
	(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"]	= m_bEnableOsramBinMixMap;
	//Nichia//v4.43T7
	(*m_psmfSRam)["BinOutputFile"]["Enable Nichia OutputFile"]		= m_bEnableNichiaOutputFile;
	(*m_psmfSRam)["BinOutputFile"]["Enable Nichia OTriggerFile"]	= m_bEnableNichiaOTriggerFile;
	(*m_psmfSRam)["BinOutputFile"]["Enable Nichia RPTFile"]			= m_bEnableNichiaRPTFile;
	(*m_psmfSRam)["BinTable"]["Output File Path 3"]	= m_szOutputFilePath3;
	//ScoulSemi //v4.44
	
	//Update BinBlkMain class	
	m_oBinBlkMain.SetFileNameAsWaferID(m_bFilenameAsWaferID);
	//pass the info to BinBlkMain class							
	m_oBinBlkMain.SubmitRandomHoleDieInfo(m_bEnable2DBarcodeOutput, m_ulHoleDieNum);	
	//andrewng //2020-0820
	m_lErrMapRefX1InUm = ConvertXEncoderValueForDisplay(m_lErrMapRefX1);
	m_lErrMapRefY1InUm = ConvertYEncoderValueForDisplay(m_lErrMapRefY1);

	return TRUE;
}


BOOL CBinTable::InitBinTableSetupData()//Only PKG
{
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	
	if ( pBTfile == NULL )
		return FALSE;

	if (!IsMS50())
	{
		//v4.59A45
		// MS50 will retrieve table limit from Machine MSD instead

		//BinTable Limit
		if ((!((*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X_POS_LIMIT])) == FALSE)
		{
			m_lTableXPosLimit = (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X_POS_LIMIT];
			m_lXNegLimit = 0 - (LONG)(((DOUBLE)m_lTableXPosLimit / m_dXResolution) + 0.5);
							//variable for bin table operation => convert into um
		}
		else
		{
			m_lTableXPosLimit = 0;
			m_lXNegLimit = 0;
		}

		if ((!((*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X_NEG_LIMIT])) == FALSE)
		{
			m_lTableXNegLimit	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X_NEG_LIMIT];
			m_lXPosLimit = 0 - (LONG)(((DOUBLE)m_lTableXNegLimit / m_dXResolution) - 0.5);
		}
		else
		{
			m_lTableXNegLimit = 0;
			m_lXPosLimit = 0;
		}

		if ((!((*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y_POS_LIMIT])) == FALSE)
		{
			m_lTableYPosLimit	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y_POS_LIMIT];
			m_lYNegLimit = 0 - (LONG)(((DOUBLE)m_lTableYPosLimit / m_dYResolution) + 0.5);
		}
		else
		{
			m_lTableYPosLimit = 0;
			m_lYNegLimit = 0;
		}


		if ((!((*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y_NEG_LIMIT])) == FALSE)
		{
			if (!m_bUseDualTablesOption)	//v4.35T4
				m_lTableYNegLimit	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y_NEG_LIMIT];
			m_lYPosLimit = 0 - (LONG)(((DOUBLE)m_lTableYNegLimit / m_dYResolution) - 0.5);
		}
		else
		{
			m_lTableYNegLimit = 0;
			m_lYPosLimit = 0;
		}
	}

	m_lBinCalibX			= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COR_X];
	m_lBinCalibY			= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COR_Y];

	//m_dThetaOffsetByCross	= (DOUBLE)( (*pBTfile)[BT_TABLE_OPTION][BT_2CROSS_THETA_OFFSET]);
	m_dBinTwoCrossXDistance = (DOUBLE)( (*pBTfile)[BT_TABLE_OPTION][BT_TWO_CROSS_DISTANCE_X]);
	m_dBinTwoCrossYDistance = (DOUBLE)( (*pBTfile)[BT_TABLE_OPTION][BT_TWO_CROSS_DISTANCE_Y]);
	m_lEncRefCross_X		= (LONG)( (*pBTfile)[BT_TABLE_OPTION]["Reference Cross X Coor."]);
	m_lEncRefCross_Y		= (LONG)( (*pBTfile)[BT_TABLE_OPTION]["Reference Cross Y Coor."]);

	m_lTableX2PosLimit		= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X2_POS_LIMIT];
	m_lTableX2NegLimit		= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X2_NEG_LIMIT];
	if (!m_bUseDualTablesOption)		//v4.35T4
		m_lTableY2PosLimit	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y2_POS_LIMIT];
	m_lTableY2NegLimit		= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y2_NEG_LIMIT];
		
	m_lX2NegLimit		= 0 - (LONG)(((DOUBLE)m_lTableX2PosLimit / m_dXResolution) + 0.5);
	m_lX2PosLimit		= 0 - (LONG)(((DOUBLE)m_lTableX2NegLimit / m_dXResolution) - 0.5);
	m_lY2NegLimit		= 0 - (LONG)(((DOUBLE)m_lTableY2PosLimit / m_dYResolution) + 0.5);
	m_lY2PosLimit		= 0 - (LONG)(((DOUBLE)m_lTableY2NegLimit / m_dYResolution) - 0.5);


	if (m_fHardware && !m_bDisableBT)
	{
		CString szLog;

		//Update Bintable joystick limit
		if ( (m_lTableXNegLimit < 0) && (m_lTableXPosLimit > 0) )
		{
			CMS896AStn::MotionSetJoystickPositionLimit("BinTableXAxis", m_lTableXNegLimit, m_lTableXPosLimit, &m_stBTAxis_X);
#ifdef NU_MOTION
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, m_lTableXPosLimit, m_lTableXNegLimit, &m_stBTAxis_X);
			szLog.Format("BTX Limit: U=%ld, L=%ld", m_lTableXPosLimit, m_lTableXNegLimit);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
#endif
		}
		else
		{
#ifdef NU_MOTION
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X, 999999, -999999, &m_stBTAxis_X);
			CMSLogFileUtility::Instance()->MS_LogOperation("BTX Limit : DEFAULT");
#endif
		}

		if ( (m_lTableYNegLimit < 0) && (m_lTableYPosLimit > 0) )
		{
			CMS896AStn::MotionSetJoystickPositionLimit("BinTableYAxis", m_lTableYNegLimit, m_lTableYPosLimit, &m_stBTAxis_Y);
#ifdef NU_MOTION
			CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y, m_lTableYPosLimit, m_lTableYNegLimit-2000, &m_stBTAxis_Y);
			szLog.Format("BTY Limit: U=%ld, L=%ld", m_lTableYPosLimit, m_lTableYNegLimit);
			CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
#endif
		}

		if (m_bUseDualTablesOption)	//v4.16T5	//MS100 9Inch
		{
			if ( (m_lTableX2NegLimit < 0) && (m_lTableX2PosLimit > 0) )
			{
				CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_X2, m_lTableX2NegLimit, m_lTableX2PosLimit,	&m_stBTAxis_X2);
				CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, m_lTableX2PosLimit, m_lTableX2NegLimit,			&m_stBTAxis_X2);			
				szLog.Format("BT2X Limit: U=%ld, L=%ld", m_lTableX2PosLimit, m_lTableX2NegLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
			else
			{
				CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_X2, 999999, -999999, &m_stBTAxis_X2);
				CMSLogFileUtility::Instance()->MS_LogOperation("BTX2 Limit : DEFAULT");
			}

			if ( (m_lTableY2NegLimit < 0) && (m_lTableY2PosLimit > 0) )
			{
				CMS896AStn::MotionSetJoystickPositionLimit(BT_AXIS_Y2, m_lTableY2NegLimit, m_lTableY2PosLimit,	&m_stBTAxis_Y2);
				CMS896AStn::MotionSetSoftwareLimit(BT_AXIS_Y2, m_lTableY2PosLimit, m_lTableY2NegLimit,			&m_stBTAxis_Y2);
				szLog.Format("BT2Y Limit: U=%ld, L=%ld", m_lTableY2PosLimit, m_lTableY2NegLimit);
				CMSLogFileUtility::Instance()->MS_LogOperation(szLog);
			}
		}
	}


	//Load/Unload positions
	if( m_bDisableBL )
	{
		m_lBTUnloadPos_X	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_UNLOAD_X_POS];
		m_lBTUnloadPos_Y	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_UNLOAD_Y_POS];
	}
		
	(*m_psmfSRam)["BinTable"]["Limit"]["X Pos"]			= m_lTableXPosLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["X Neg"]			= m_lTableXNegLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["Y Pos"]			= m_lTableYPosLimit;
	(*m_psmfSRam)["BinTable"]["Limit"]["Y Neg"]			= m_lTableYNegLimit;

	(*m_psmfSRam)["BinTable2"]["Limit"]["X Pos"]		= m_lTableX2PosLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["X Neg"]		= m_lTableX2NegLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["Y Pos"]		= m_lTableY2PosLimit;
	(*m_psmfSRam)["BinTable2"]["Limit"]["Y Neg"]		= m_lTableY2NegLimit;

	m_lColletOffsetX	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COLLET_OFFSET_X1];
	m_lColletOffsetY	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COLLET_OFFSET_Y1];
	
	m_lACCLiquidX		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_LIQUID_X];
	m_lACCLiquidY		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_LIQUID_Y];
	m_lACCBrushX		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_BRUSH_X];
	m_lACCBrushY		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_BRUSH_Y];

	m_lNGPickPocketX	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NGPICK_POCKET_X];
	m_lNGPickPocketY	= (*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NGPICK_POCKET_Y];
	m_lMS90MCCGarbageBinX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_MANUAL_CC_GBIN_X];	//v4.59A7
	m_lMS90MCCGarbageBinY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_MANUAL_CC_GBIN_Y];	//v4.59A7
	//shiraishi02
	m_bCheckFrameOrientation= (BOOL)(LONG)(*pBTfile)[BT_TABLE_OPTION][BT_CHK_FRAME_ORIENT];
	(*m_psmfSRam)["BinTable"]["CheckFrameOrientation"]	= m_bCheckFrameOrientation;
	m_lFrameOrientCheckX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_CHK_FRAME_ORIENT][BT_ORIENT_X];
	m_lFrameOrientCheckY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_CHK_FRAME_ORIENT][BT_ORIENT_Y];

	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == "Testar")
	{
		m_lACCRangeX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_RANGE_X];
		m_lACCRangeY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_RANGE_Y];
		m_lACCAreaSizeX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_SIZEX];
		m_lACCAreaSizeY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_SIZEY];
		m_lACCMatrixRow	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_MATRIX_ROW];
		m_lACCMatrixCol	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_MATRIX_COL];
		m_lACCAreaLimit	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_LIMIT];
		m_lACCReplaceLimit = m_lACCMatrixRow * m_lACCMatrixCol * m_lACCAreaLimit;
		m_lACCCycleCount= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_CYCLE_COUNT];
		
		if ( m_lACCAreaSizeX>m_lACCRangeX )
		{
			m_lACCAreaSizeX = m_lACCRangeX;
		}
		if ( m_lACCAreaSizeY>m_lACCRangeY )
		{
			m_lACCAreaSizeY = m_lACCRangeY;
		}
		if ( m_lACCCycleCount<=0 )
		{
			m_lACCCycleCount = 5;
		}
	}

	m_lBTNoDiePosX				= (*pBTfile)[BT_TABLE_OPTION][BT_No_Die_COOR_X];
	m_lBTNoDiePosY				= (*pBTfile)[BT_TABLE_OPTION][BT_No_Die_COOR_Y];

	m_lCleanDirtMatrixRow		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_MATRIX_ROW];
	m_lCleanDirtMatrixCol		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_MATRIX_COL];
	m_lCleanDirtUpleftPosX		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_UP_LEFT_X];
	m_lCleanDirtUpleftPosY		= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_UP_LEFT_Y];
	m_lCleanDirtLowerRightPosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_LOWER_RIGHT_X];
	m_lCleanDirtLowerRightPosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_LOWER_RIGHT_Y];
	m_lUnloadPhyPosX			= (LONG)(*pBTfile)[BT_TABLE_OPTION]["Unload Position X"];
	m_lUnloadPhyPosY			= (LONG)(*pBTfile)[BT_TABLE_OPTION]["Unload Position Y"];
	m_lUnloadPhyPosT			= (LONG)(*pBTfile)[BT_TABLE_OPTION]["Unload Position T"];

	//v4.50A5
	if (CMS896AStn::m_bAutoChangeCollet)
	{
		m_lChgColletClampPosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_CLAMP_POS_X];
		m_lChgColletClampPosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_CLAMP_POS_Y];
		m_lChgColletHolderInstallPosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_INSTALL_POS_X];
		m_lChgColletHolderInstallPosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_INSTALL_POS_Y];
		m_lChgColletHolderUploadPosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_UPLOAD_POS_X];
		m_lChgColletHolderUploadPosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_UPLOAD_POS_Y];
		m_lChgColletPusher3PosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_PUSHER3_POS_X];
		m_lChgColletPusher3PosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_PUSHER3_POS_Y];
		m_lChgColletUpLookPosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_POS_X];
		m_lChgColletUpLookPosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_POS_Y];
		m_lChgColletUPLUploadPosX	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_UPLOAD_POS_X];
		m_lChgColletUPLUploadPosY	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_UPLOAD_POS_Y];
	}	//	auto change collet hardware relative settings.

	m_lAlignBinDelay	= (LONG)(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_REALIGN_DELAY];

	CMSFileUtility::Instance()->CloseBTConfig();

	return TRUE;
}

BOOL CBinTable::LoadExtraClearBinInfo()
{
	CString szTemp;
	CStringMapFile* psmf;
	CMSFileUtility::Instance()->LoadAppFeatureConfig();			//v4.17T5
	psmf = CMSFileUtility::Instance()->GetAppFeatureFile();

	if (psmf == NULL)
		return FALSE;

	for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		szTemp.Format("%s %d",BT_EXTRA_BIN_INFO_FIELD_NAME ,i+1);
		m_szaExtraBinInfoField[i] = (*psmf)[MS896A_FUNC_OUTFILES][MS896A_EXTRA_BIN_INFO][szTemp];
		m_bEnableExtraBinInfo[i] = TRUE;

		if (m_szaExtraBinInfoField[i] == "")
		{
			m_szaExtraBinInfoField[i].Format("%s %d", "Info" , i+1);
			m_bEnableExtraBinInfo[i] = FALSE;
		}
	}

	psmf->Close();

	return TRUE;
}

BOOL CBinTable::SaveBinTableData(VOID)
{
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//Check Load/Save Data
	if (pBTfile == NULL)
		return FALSE;

//	xuzhijin_gpk
	(*pBTfile)[BT_TABLE_OPTION][BT_WAFEREND_UPLOAD_MAP_MPD]		= m_bWaferEndUploadMapMpd;
	(*pBTfile)[BT_TABLE_OPTION][BT_ENABLE_OUTPUT_FILE]			= m_bEnableBinOutputFile;
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_OUTPUT_FILE_FIN]			= m_bFinForBinOutputFile;
	(*pBTfile)[BT_TABLE_OPTION][BT_ENABLE_BIN_MAP_FILE]			= m_bEnableBinMapFile;
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_MAP_FILE_FIN2]			= m_bFin2ForBinMapFile;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_REALIGN_DELAY]			= m_lAlignBinDelay;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_REALIGN_SAMEVIEW]		= m_bUseSameView;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_IN_USE]				= m_nBTInUse;					//v4.24
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_SHOW_BINBLK_IMAGE]		= m_bShowBinBlkImage;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_SPEC_VERSION]			= m_szSpecVersion;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_ENABLE_2DBARCODE_OUTPUT] = m_bEnable2DBarcodeOutput;	//v3.33T3
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NUM_OF_HOLE_DIE] = m_ulHoleDieNum;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_AUTO_SETUP_BINBLK_WITH_TEMPL]	= m_bAutoBinBlkSetupWithTempl;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_ENABLE_AUTO_LOAD_RANK_FILE] = m_bEnableAutoLoadRankFile;
	(*pBTfile)[BT_TABLE_OPTION][BT_LOAD_RANK_FILE_FROM_MAP]		= m_bLoadRankIDFromMap;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_RANK_FILE_PATH]		 = m_szRankIDFilePath;
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_LOT_SUM_PATH]			= m_szBinLotSummaryPath;
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_LOT_SUN_FILENAME]		= m_szBinLotSummaryFilename;
	
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_INPUT_COUNT_SETUP_FILENAME] = m_szInputCountSetupFilename;
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_INPUT_COUNT_SETUP_FILEPATH] = m_szInputCountSetupFilePath;

	//4.53D91 Save Data
	(*pBTfile)[BT_TABLE_OPTION]["Full Die Count Input Per Wafer"] = m_ulFullDieCountInput;
	(*pBTfile)[BT_TABLE_OPTION]["Min Die Count Input Per Wafer"]  = m_ulMinDieCountInput;
	(*pBTfile)[BT_TABLE_OPTION]["Enable Die Count Input PerWf"]	  = m_bEnableDieCountInput;

	(*pBTfile)[BT_TABLE_OPTION][BT_SPEEDMODE_OFFSET_X]			= m_dSpeedModeOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_SPEEDMODE_OFFSET_Y]			= m_dSpeedModeOffsetY;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_BOND_OFFSET_X]			= m_lBHZ1BondPosOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_BOND_OFFSET_Y]			= m_lBHZ1BondPosOffsetY;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_BOND_OFFSET_X]			= m_lBHZ2BondPosOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_BOND_OFFSET_Y]			= m_lBHZ2BondPosOffsetY;

	//v4.55A8
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_PREPICK_OFFSET_X]		= m_lBHZ1PrePickToBondOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ1_PREPICK_OFFSET_Y]		= m_lBHZ1PrePickToBondOffsetY;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_PREPICK_OFFSET_X]		= m_lBHZ2PrePickToBondOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_BHZ2_PREPICK_OFFSET_Y]		= m_lBHZ2PrePickToBondOffsetY;
	
	(*pBTfile)[BT_TABLE_OPTION][BT_BOND_180_DEGREE_OFFSET_X_UM] = m_dBond180DegOffsetX_um;
	(*pBTfile)[BT_TABLE_OPTION][BT_BOND_180_DEGREE_OFFSET_Y_UM] = m_dBond180DegOffsetY_um;

	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_FRAME_OPTION]		= m_lRealignBinFrameOption;		//v3.70T3
	(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNMENT]		= m_bPrPreBondAlignment;		//v3.79
	(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C1_PAD_OFFSET_X]		= m_lCollet1PadOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C1_PAD_OFFSET_Y]		= m_lCollet1PadOffsetY;		//CSP
	(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C2_PAD_OFFSET_X]		= m_lCollet2PadOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_PR_PREBOND_ALIGNPAD][BT_C2_PAD_OFFSET_Y]		= m_lCollet2PadOffsetY;

	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_1PT_COMP]			= m_b1ptRealignComp;			//v3.86	
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP]						= m_bEnableBinMapBondArea;		//v4.03		//PLLM REBEL
	//(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CDIE_OFFSET]		= m_bEnableBinMapCDieOffset;	//v4.42T1
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CIRCLE_RADIUS]		= m_dBinMapCircleRadiusInMm;	//v4.36
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_EDGE_SIZE]			= m_lBinMapEdgeSize;			//v4.36T9
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_BEDGE_SIZE]			= m_lBinMapBEdgeSize;			//v4.36T10
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_LEDGE_SIZE]			= m_lBinMapLEdgeSize;			//v4.36T10
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_REDGE_SIZE]			= m_lBinMapREdgeSize;			//v4.36T10
//4.48 OSRAM	
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_BINMAP]				= m_bEnableOsramBinMixMap;		//v4.47A6
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_PATTERN_TYPE]			= m_ulBinMixPatternType;
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEA_QTY]				= m_lBinMixTypeAQty;
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEB_QTY]				= m_lBinMixTypeBQty;
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEA_STOPPOINT]				= m_lBinMixTypeAStopPoint;
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEB_STOPPOINT]				= m_lBinMixTypeBStopPoint;
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEA_DIE]				= m_ucBinMixTypeAGrade;
	(*pBTfile)[BT_TABLE_OPTION][BT_OSRAM_TYPEB_DIE]				= m_ucBinMixTypeBGrade;
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_BYPASS_SP_GRADE1]		= m_ucSpGrade1ToByPassBinMap;	//v4.48A2

	(*pBTfile)[BT_TABLE_OPTION][BT_No_Die_COOR_X]				= m_lBTNoDiePosX;
	(*pBTfile)[BT_TABLE_OPTION][BT_No_Die_COOR_Y]				= m_lBTNoDiePosY;

	//(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CDIE_ROW_OFFSET]	= m_lBinMapCDieRowOffset;
	//(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_CDIE_COL_OFFSET]	= m_lBinMapCDieColOffset;
	(*pBTfile)[BT_TABLE_OPTION][BT_RANKID_FILE]					= m_bLoadRankIDFile;			//v4.21T3	//TongFang
	(*pBTfile)[BT_TABLE_OPTION][BT_BINMAP_FILE_PATH]			= m_szBinMapFilePath;			//v4.37T11
	
	//andrewng //2020-0806
	(*pBTfile)[BT_TABLE_OPTION]["EnableErrMap"]					= CMS896AApp::m_bEnableErrMap;

	//(*pBTfile)[BT_TABLE_OPTION][BT_2CROSS_THETA_OFFSET]			= m_dThetaOffsetByCross;
	(*pBTfile)[BT_TABLE_OPTION][BT_TWO_CROSS_DISTANCE_X]		= m_dBinTwoCrossXDistance;
	(*pBTfile)[BT_TABLE_OPTION][BT_TWO_CROSS_DISTANCE_Y]		= m_dBinTwoCrossYDistance;
	(*pBTfile)[BT_TABLE_OPTION]["Reference Cross X Coor."]		= m_lEncRefCross_X;
	(*pBTfile)[BT_TABLE_OPTION]["Reference Cross Y Coor."]		= m_lEncRefCross_Y;
	
	//Jenoptic	//v4.43T8
	(*pBTfile)[BT_TABLE_OPTION][BT_MAG_SUM_YEAR]				= m_lMagSummaryYear;
	(*pBTfile)[BT_TABLE_OPTION][BT_MAG_SUM_MONTH]				= m_lMagSummaryMonth;
	(*pBTfile)[BT_TABLE_OPTION][BT_MAG_SUM_DAY]					= m_lMagSummaryDay;

	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FORMAT_NAME]		= m_szBinOutputFileFormat;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_BLUETAPE_NO]		= m_szOutputFileBlueTapeNo; // v4.51D10 Dicon  
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_LABEL_PRINTER_SEL]		= m_szLabelPrinterSel;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH]		= m_szOutputFilePath;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH2]		= m_szOutputFilePath2;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH3]		= m_szOutputFilePath3;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_PATH4]		= m_szOutputFilePath4;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FORMAT_NAME]	= m_szWaferEndFileFormat;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FILE_PATH]		= m_szWaferEndPath;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_YIELD]			= m_dWaferEndSortingYield;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFEREND_FILE_ENABLE]	= m_bIfGenWaferEndFile;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_BIN_SUMMARY_OUTPUT_PATH]= m_szBinSummaryOutputPath;	
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILENAME_AS_WAFERID]	= m_bFilenameAsWaferID;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_USE_BARCODE_NAME]	= m_bUseBarcodeAsFileName;	
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_LABEL_FILEPATH]	= m_szLabelFilePath;
	//Nichia//v4.43T7
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_NICHIA_ENABLE_OUTPUTFILE]	= m_bEnableNichiaOutputFile;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_NICHIA_ENABLE_OTRIGGERFILE]	= m_bEnableNichiaOTriggerFile;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_NICHIA_ENABLE_RPTFILE]		= m_bEnableNichiaRPTFile;
	
	if (CMS896AStn::m_bIsAlwaysPrintLabel == TRUE)
	{
		m_bIfPrintLabel = TRUE;
	}
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_PRINT_LABEL]		= m_bIfPrintLabel;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_AUTO_CLEAR_BIN]			= m_bAutoClearBin;				//v2.93T2
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_FILE_SUMMARY_PATH]	= m_szOutputFileSummaryPath;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_WAFER_END_SUMMARY_PATH]	= m_szWaferEndSummaryPath;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_UNIQUE_SERIAL_NUM]		= m_bUseUniqueSerialNum;	
	//v3.23T1	
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_MULTIPLE_SERIAL_COUNTER] = m_bUseMultipleSerialCounter;

	//-------- Extra Information for clear bin & output file ------------//
	for (INT i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		szTemp.Format("%s %d",BT_EXTRA_BIN_INFO_ITEM,i+1);
		(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_EXTRA_BIN_INFO][szTemp] = m_szaExtraBinInfo[i];
	}

	(*pBTfile)[BT_GRADECHANGE_OPTION][BT_GRADE_COUNT]			= m_ulGradeChangeLimit;

	//v2.63
	//Realign Last Die Offset
	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_OFFSET_X]		= m_dAlignLastDieOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_OFFSET_Y]		= m_dAlignLastDieOffsetY;
	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_REL_OFFSET_X]	= m_dAlignLastDieRelOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_LASTDIE_REL_OFFSET_Y]	= m_dAlignLastDieRelOffsetY;
	(*pBTfile)[BT_OUTPUTFILE_OPTION][BT_OUTPUT_OFFLINE_PRINT_LABEL] = m_bDisableClearAllPrintLabel;		//v2.70

	(*pBTfile)[BT_TABLE_OPTION][BT_REALIGN_USE_FRAME_CROSS]	= m_bUseFrameCrossAlignment;	//v4.59A1

	(*pBTfile)[BT_TABLE_OPTION][BT_OPTIMIZE_BIN_COUNT_MODE]		= m_ucOptimizeBinCountMode;
	(*pBTfile)[BT_TABLE_OPTION][BT_OPTIMIZE_BIN_COUNT]			= m_ulEnableOptimizeBinCount;
	(*pBTfile)[BT_TABLE_OPTION][BT_BIN_SUMMARY_WAFER_LOT_CHECK] = m_bEnableBinSumWaferLotCheck;
	(*pBTfile)[BT_TABLE_OPTION][BT_MIN_LOT_COUNT]				= m_ulMinLotCount;
		
	(*pBTfile)[BT_TABLE_OPTION][BT_MIN_BAR_DIGIT]				= m_lBinBarcodeMinDigit;	

	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_FILENAME]	= m_szPKGFilenameDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIEPITCH_X_DISPLAY] = m_ulPKGDiePitchXDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIEPITCH_Y_DISPLAY] = m_ulPKGDiePitchYDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_INPUT_COUNT_DISPLAY] = m_ulPKGInputCountDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_PER_ROW_DISPLAY] = m_ulPKGDiePerRowDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_PER_COL_DISPLAY] = m_ulPKGDiePerColDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_DIE_CAPACITY_DISPLAY] = m_ulPKGBlkCapacityDisplay;
	(*pBTfile)[BT_PKG_DISPLAY][BT_PKG_BIN_BLK_NO_DISPLAY]	= m_ulPKGNoOfBinBlkDisplay;

	//v4.51A19	//Silan MS90 NGPick
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NGPICK_POCKET_X]		= m_lNGPickPocketX;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_NGPICK_POCKET_Y]		= m_lNGPickPocketY;

	// Auto Clean Collet
	if( m_lACCCycleCount<=0 )
	{
		m_lACCCycleCount = 5;
	}
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_LIQUID_X]	= m_lACCLiquidX;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_LIQUID_Y]	= m_lACCLiquidY;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_BRUSH_X]		= m_lACCBrushX;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_BRUSH_Y]		= m_lACCBrushY;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_RANGE_X]		= m_lACCRangeX;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_RANGE_Y]		= m_lACCRangeY;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_CYCLE_COUNT]	= m_lACCCycleCount;

	if( m_lACCAreaSizeX>m_lACCRangeX )
	{
		m_lACCAreaSizeX = m_lACCRangeX;
	}
	if( m_lACCAreaSizeY>m_lACCRangeY )
	{
		m_lACCAreaSizeY = m_lACCRangeY;
	}
	if( m_lACCMatrixRow<=0 )
	{
		m_lACCMatrixRow = 1;
	}
	if( m_lACCMatrixCol<=0 )
	{
		m_lACCMatrixCol = 1;
	}
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_MATRIX_ROW]	= m_lACCMatrixRow;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_MATRIX_COL]	= m_lACCMatrixCol;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_SIZEX]	= m_lACCAreaSizeX;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_SIZEY]	= m_lACCAreaSizeY;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_LIMIT]	= m_lACCAreaLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_COUNT]	= m_lACCAreaCount;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CC_AREA_INDEX]	= m_lACCAreaIndex;
	(*pBTfile)[BT_TABLE_OPTION][BT_ATUO_CC_TOGGLE_BH_VACUUM]	= m_bACCToggleBHVacuum;
	(*pBTfile)[BT_TABLE_OPTION][BT_MANUAL_CC_GBIN_X]	= m_lMS90MCCGarbageBinX;	//v4.59A7
	(*pBTfile)[BT_TABLE_OPTION][BT_MANUAL_CC_GBIN_Y]	= m_lMS90MCCGarbageBinY;	//v4.59A7
	//andrewng //2020-0902
	(*pBTfile)[BT_TABLE_OPTION]["Unload Position X"]	= m_lUnloadPhyPosX;
	(*pBTfile)[BT_TABLE_OPTION]["Unload Position Y"]	= m_lUnloadPhyPosY;
	(*pBTfile)[BT_TABLE_OPTION]["Unload Position T"]	= m_lUnloadPhyPosT;

	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_MATRIX_ROW]	= m_lCleanDirtMatrixRow;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_MATRIX_COL]	= m_lCleanDirtMatrixCol;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_UP_LEFT_X]	= m_lCleanDirtUpleftPosX;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_UP_LEFT_Y]	= m_lCleanDirtUpleftPosY;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_LOWER_RIGHT_X] = m_lCleanDirtLowerRightPosX;
	(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CLEAN_DIRT_LOWER_RIGHT_Y] = m_lCleanDirtLowerRightPosY;

	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map No Of Row"]	= m_ulErrMapNoOfRow;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map No Of Col"]	= m_ulErrMapNoOfCol;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Pitch X"]	= m_dErrMapPitchX;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Pitch Y"]	= m_dErrMapPitchY;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Delay"]		= m_ulErrMapDelay;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Count"]		= m_ulErrMapCount;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Slope"]		= m_dErrMapSlope;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Sampling Err Tol X"]	= m_dErrMapSamplingTolX;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Sampling Err Tol Y"]	= m_dErrMapSamplingTolY;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Sampling Tol Limit"] = m_ulErrMapSamplingTolLimit;

	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefX1"]		= m_lErrMapRefX1;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefY1"]		= m_lErrMapRefY1;
	//(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefX2"]	= m_lErrMapRefX2;
	//(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map RefY2"]	= m_lErrMapRefY2;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Offset Limit X"]	= m_lErrMapOffsetLimitX;
	(*pBTfile)[BT_TABLE_OPTION][BT_ERROR_MAP_FCN]["Err Map Offset Limit Y"]	= m_lErrMapOffsetLimitY;
	(*pBTfile)[BT_TABLE_OPTION]["Marks fail Acc Limit"]	= m_ulMarksPrAccLimit;
	(*pBTfile)[BT_TABLE_OPTION]["Marks fail Con Limit"]	= m_ulMarksPrConLimit;

	//shiraishi02
	(*m_psmfSRam)["BinTable"]["CheckFrameOrientation"]	= m_bCheckFrameOrientation;
	(*pBTfile)[BT_TABLE_OPTION][BT_CHK_FRAME_ORIENT]	= m_bCheckFrameOrientation;
	(*pBTfile)[BT_TABLE_OPTION][BT_CHK_FRAME_ORIENT][BT_ORIENT_X]	= m_lFrameOrientCheckX;
	(*pBTfile)[BT_TABLE_OPTION][BT_CHK_FRAME_ORIENT][BT_ORIENT_Y]	= m_lFrameOrientCheckY;
	(*pBTfile)[BT_TABLE_OPTION]["NG With Bin Map"]	= CMS896AApp::m_bNGWithBinMap;

	//Subbin
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Row A"]=m_ulSubBinSRowA;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Row B"]=m_ulSubBinSRowB;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Row C"]=m_ulSubBinSRowC;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Row D"]=m_ulSubBinSRowD;

	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Row A"]=m_ulSubBinERowA;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Row B"]=m_ulSubBinERowB;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Row C"]=m_ulSubBinERowC;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Row D"]=m_ulSubBinERowD;

	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Col A"]=m_ulSubBinEColA;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Col B"]=m_ulSubBinEColB;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Col C"]=m_ulSubBinEColC;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin E Col D"]=m_ulSubBinEColD;

	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Col A"]=m_ulSubBinSColA;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Col B"]=m_ulSubBinSColB;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Col C"]=m_ulSubBinSColC;
	(*pBTfile)[BT_TABLE_OPTION]["SubBin S Col D"]=m_ulSubBinSColD;
	//v4.50A5
	if (CMS896AStn::m_bAutoChangeCollet)
	{
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_CLAMP_POS_X]	= m_lChgColletClampPosX;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_CLAMP_POS_Y]	= m_lChgColletClampPosY;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_INSTALL_POS_X]	= m_lChgColletHolderInstallPosX;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_INSTALL_POS_Y]	= m_lChgColletHolderInstallPosY;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_UPLOAD_POS_X]	= m_lChgColletHolderUploadPosX;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_HOLDER_UPLOAD_POS_Y]	= m_lChgColletHolderUploadPosY;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_PUSHER3_POS_X]	= m_lChgColletPusher3PosX;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_PUSHER3_POS_Y]	= m_lChgColletPusher3PosY;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_POS_X]	= m_lChgColletUpLookPosX;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_POS_Y]	= m_lChgColletUpLookPosY;

		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_UPLOAD_POS_X] = m_lChgColletUPLUploadPosX;
		(*pBTfile)[BT_TABLE_OPTION][BT_AUTO_CHG_COLLET][AGC_UPLOOK_UPLOAD_POS_Y] = m_lChgColletUPLUploadPosY;
	}

	(*pBTfile)[BT_TABLE_OPTION][BT_IS_RESORT_MODE]		= m_bIfEnableResortDie;
	//TesterEpister //v4.44T6
	(*pBTfile)[BT_TABLE_OPTION][BT_SAVE_TEMP_FILE]		= m_bClearBinInCopyTempFileMode;	

	m_lACCReplaceCount	= (m_lACCAreaIndex-1)*m_lACCAreaLimit + m_lACCAreaCount;
	m_lACCReplaceLimit = m_lACCMatrixRow * m_lACCMatrixCol * m_lACCAreaLimit;
	CMSFileUtility::Instance()->SaveBTConfig();

	//Update BinBlkMain class	
	m_oBinBlkMain.SetFileNameAsWaferID(m_bFilenameAsWaferID);
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetCustomerName() == CTM_SANAN)
	{
		m_oBinBlkMain.SetBackupTempFilePath(m_szOutputFilePath3);	//v4.48A10	//WH Sanan
	}

	//pass the info to BinBlkMain class													//v3.33T3
	m_oBinBlkMain.SubmitRandomHoleDieInfo(m_bEnable2DBarcodeOutput, m_ulHoleDieNum);
	m_oBinBlkMain.SetUseBinMapBondArea(m_bEnableBinMapBondArea);						//v4.03		//PLLM Bin map fcn
	
	
	m_oBinBlkMain.SetOSRAMBinMixA(1,m_ucBinMixTypeAGrade);
	m_oBinBlkMain.SetOSRAMBinMixB(1,m_ucBinMixTypeBGrade);
	m_oBinBlkMain.EnableOSRAMBinMixMap(1,m_bEnableOsramBinMixMap);
	m_oBinBlkMain.SetBinMixPatternType(1,m_ulBinMixPatternType);
	//m_oBinBlkMain.SetBinMixOtherValue(1,m_lBinMixTypeAQty,m_lBinMixTypeBQty,m_lBinMixTypeAStopPoint,m_lBinMixTypeBStopPoint);
	//pBTfile->Close();
	(*m_psmfSRam)["BinTable"]["RealignFrameOption"]		= m_lRealignBinFrameOption;		//v3.71T5
	(*m_psmfSRam)["BinTable"]["EnableOptimizeBinCount"] = m_ulEnableOptimizeBinCount;
	(*m_psmfSRam)["BinTable"]["PreBondAlignment"]		= m_bPrPreBondAlignment;		//v3.80
	SaveBTCollet1Offset(m_lBHZ1BondPosOffsetX, m_lBHZ1BondPosOffsetY, m_dXResolution_UM_CNT);
	SaveBTCollet2Offset(m_lBHZ2BondPosOffsetX, m_lBHZ2BondPosOffsetY, m_dXResolution_UM_CNT);

	(*m_psmfSRam)["BinTable"]["RealignRelOffsetX"]		= m_dAlignLastDieRelOffsetX;
	(*m_psmfSRam)["BinTable"]["RealignRelOffsetY"]		= m_dAlignLastDieRelOffsetY;
	(*m_psmfSRam)["BinTable"][BT_IS_RESORT_MODE]		= m_bIfEnableResortDie;
	//Nichia//v4.43T7
	(*m_psmfSRam)["BinOutputFile"]["Enable Nichia OutputFile"]		= m_bEnableNichiaOutputFile;
	(*m_psmfSRam)["BinOutputFile"]["Enable Nichia OTriggerFile"]	= m_bEnableNichiaOTriggerFile;
	(*m_psmfSRam)["BinOutputFile"]["Enable Nichia RPTFile"]			= m_bEnableNichiaRPTFile;
	(*m_psmfSRam)["BinOutputFile"]["Enable Osram Bin Mix Map"]		= m_bEnableOsramBinMixMap;
	(*m_psmfSRam)["BinTable"]["Output File Path 3"]					= m_szOutputFilePath3;
	
	//v4.47T8	//WH SanAn
	if (m_bEnableBinMapBondArea)
		pApp->SetEnableBMapBondAreaToRegistry(2);
	else
		pApp->SetEnableBMapBondAreaToRegistry(1);

	return TRUE;
}

BOOL CBinTable::SaveBinGradeWithIndex(BOOL bCheckAll)
{
	CString szTemp;

	// check is bondong and have bin count
	if (bCheckAll)		//v4.46T10
	{
		if ( m_oBinBlkMain.IsBondingInProgress()==TRUE || m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkToSetup)>0 )
		{
			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			return FALSE;
		}
	}
	else
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkToSetup) > 0)
		{
			SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
			return FALSE;
		}
	}
	

	// check all bin block have same grade
	for ( ULONG i = 1 ; i<= m_oBinBlkMain.GetNoOfBlk() ; i++ ) 
	{
		if ( m_oBinBlkMain.GrabGrade(i) == m_ucGrade )
		{
			if ( i != m_ulBinBlkToSetup )
			{
				CString szMsg;
				szMsg.Format("Set Grade %d Error, allready set in Bin Block %d!", m_ucGrade, i);
				HmiMessage(szMsg, "Change Bin Grade Error");

				CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
				if (pBTfile != NULL)	//v4.46T20	//Klocwork
					m_ucGrade = (*pBTfile)["BinBlock"][m_ulBinBlkToSetup]["Grade"];
				CMSFileUtility::Instance()->SaveBTConfig();
				return FALSE;
			}
		}
	}

	m_oBinBlkMain.ReassignBlkGrade(m_ulBinBlkToSetup,m_ucGrade,FALSE,TRUE);
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();
	if (pBTfile != NULL)	//Klocwork	//v4.46
	{
		(*pBTfile)["BinBlock"][m_ulBinBlkToSetup]["Grade"] = m_ucGrade;
		CMSFileUtility::Instance()->SaveBTConfig();
	}

	//BinBlksDrawing();
	BinBlksDrawing_BL(m_ulBinBlkToSetup);
	WriteGradeLegend();
	return TRUE;
}

BOOL CBinTable::SaveByPassBinMap(BOOL bEnable)
{
	CString szTemp;

	if (m_oBinBlkMain.GrabNVNoOfBondedDie(m_ulBinBlkToSetup) > 0)
	{
		SetAlert(IDS_BT_DENYBLKSETTINGSEDIT2);
		return FALSE;
	}
	
	m_oBinBlkMain.SetUseByPassBinMap(m_ulBinBlkToSetup, bEnable);
	return TRUE;
}

BOOL CBinTable::SaveBinTableSetupData()
{
	CString szTemp;
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//Check Load/Save Data
	if (pBTfile == NULL)
		return FALSE;

	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X_POS_LIMIT]			= m_lTableXPosLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X_NEG_LIMIT]			= m_lTableXNegLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y_POS_LIMIT]			= m_lTableYPosLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y_NEG_LIMIT]			= m_lTableYNegLimit;
	if( m_bDisableBL )
	{
		(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_UNLOAD_X_POS]			= m_lBTUnloadPos_X;
		(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_UNLOAD_Y_POS]			= m_lBTUnloadPos_Y;
	}

	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X2_POS_LIMIT]			= m_lTableX2PosLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_X2_NEG_LIMIT]			= m_lTableX2NegLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y2_POS_LIMIT]			= m_lTableY2PosLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_Y2_NEG_LIMIT]			= m_lTableY2NegLimit;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_UNLOAD_X2_POS]			= m_lTableUnloadX2Pos;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_UNLOAD_Y2_POS]			= m_lTableUnloadY2Pos;

	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COLLET_OFFSET_X1]		= m_lColletOffsetX;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COLLET_OFFSET_Y1]		= m_lColletOffsetY;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COR_X]					= m_lBinCalibX;
	(*pBTfile)[BT_TABLE_OPTION][BT_TABLE_COR_Y]					= m_lBinCalibY;

	//Variables for Bin Table operation => Convert into um
	//BinTable Limit
	m_lXNegLimit	= (LONG)(((DOUBLE)m_lTableXNegLimit / m_dXResolution) - 0.5);
	m_lXPosLimit	= (LONG)(((DOUBLE)m_lTableXPosLimit / m_dXResolution) + 0.5);
	m_lYNegLimit	= (LONG)(((DOUBLE)m_lTableYNegLimit / m_dYResolution) - 0.5);
	m_lYPosLimit	= (LONG)(((DOUBLE)m_lTableYPosLimit / m_dYResolution) + 0.5);

	m_lX2NegLimit	= (LONG)(((DOUBLE)m_lTableX2NegLimit / m_dXResolution) - 0.5);
	m_lX2PosLimit	= (LONG)(((DOUBLE)m_lTableX2PosLimit / m_dXResolution) + 0.5);
	m_lY2NegLimit	= (LONG)(((DOUBLE)m_lTableY2NegLimit / m_dYResolution) - 0.5);
	m_lY2PosLimit	= (LONG)(((DOUBLE)m_lTableY2PosLimit / m_dYResolution) + 0.5);

	CMSFileUtility::Instance()->SaveBTConfig();

	return TRUE;
}

BOOL CBinTable::SaveReferenceCross(DOUBLE dBinTwoCrossXDistance, DOUBLE dBinTwoCrossYDistance, LONG lEncRefCross_X, LONG lEncRefCross_Y)
{
	CStringMapFile* pBTfile = CMSFileUtility::Instance()->GetBTConfigFile();

	//Check Load/Save Data
	if (pBTfile == NULL)
		return FALSE;

	//HmiMessage("SAVE!");

	(*pBTfile)[BT_TABLE_OPTION][BT_TWO_CROSS_DISTANCE_X]		= dBinTwoCrossXDistance;
	(*pBTfile)[BT_TABLE_OPTION][BT_TWO_CROSS_DISTANCE_Y]		= dBinTwoCrossYDistance;
	(*pBTfile)[BT_TABLE_OPTION]["Reference Cross X Coor."]		= lEncRefCross_X;
	(*pBTfile)[BT_TABLE_OPTION]["Reference Cross Y Coor."]		= lEncRefCross_Y;
	//(*pBTfile)[BT_TABLE_OPTION][BT_2CROSS_THETA_OFFSET]			= m_dThetaOffsetByCross;

	CMSFileUtility::Instance()->SaveBTConfig();	

	return TRUE;
}

//v3.86
BOOL CBinTable::GenerateConfigData()
{
	CString szConfigFile = (*m_psmfSRam)["MS896A"]["ParameterListPath"];
		//_T("c:\\MapSorter\\UserData\\Parameters.csv");
	CStdioFile oFile;
	CString szLine;

	if (oFile.Open(szConfigFile, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeText) == FALSE)
	{
		return FALSE;
	}

	oFile.SeekToEnd();

	//BOnding Delays
	oFile.WriteString("\n[BIN BLOCK / FRAME]\n");
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();

	szLine.Format("Die Pitch X,%d\n",			_round(m_oBinBlkMain.GrabDDiePitchX(1)));	//v4.37T10
	oFile.WriteString(szLine);
	szLine.Format("Die Pitch Y,%d\n",			_round(m_oBinBlkMain.GrabDDiePitchY(1)));	//v4.37T10
	oFile.WriteString(szLine);

	if (pApp->GetCustomerName()=="EverVision")
	{
		oFile.WriteString("\n[Bin Block Setup]\n");

		szLine.Format("Bin Block No. OF Row,%d\n",			m_ulDiePerRow);
		oFile.WriteString(szLine);

		szLine.Format("Bin Block No. OF Col,%d\n",			m_ulDiePerCol);
		oFile.WriteString(szLine);

		//szLine.Format("Bin Block Walk Path,m_szWalkPath\n",			m_ulDiePerCol);
		oFile.WriteString("Bin Block Walk Path," + m_szWalkPath  + "\n");

		szLine.Format("BEnable Centrallizing Bond Area ON/OFF,%d\n",			m_bCentralizedBondArea);
		oFile.WriteString(szLine);

		oFile.WriteString("[Bond->Counts]\n");
	
		LONG lNumber1 = m_oBinBlkMain.GrabGradeCapacity(1);
		LONG lNumber2 = m_oBinBlkMain.GrabInputCount(1);

		szLine.Format("Grade Capacity,%d\n",			lNumber1 );
		oFile.WriteString(szLine);

		szLine.Format("Input Count,%d\n",				lNumber2);
		oFile.WriteString(szLine);
	}

	if (m_bUseDualTablesOption)	//v4.36T1
	{
		LONG lBT2OffsetZ = (*m_psmfSRam)["BinTable"]["BT2OffsetZ"];
		oFile.WriteString("\n[BIN TABLE]\n");

		szLine.Format("Table 2 Offset (X/Y)(um),%ld / %ld\n", m_lBT2OffsetX, m_lBT2OffsetY);
		oFile.WriteString(szLine);
		szLine.Format("Table 2 Z Offset,%ld\n",	lBT2OffsetZ);
		oFile.WriteString(szLine);

		szLine.Format("Re-Align Delay(ms),%d\n",			m_lAlignBinDelay);
		oFile.WriteString(szLine);

		szLine.Format("Last Die Offset(X mil),%d\n",			m_dAlignLastDieOffsetX);
		oFile.WriteString(szLine);

		szLine.Format("Last Die Offset(Y mil),%d\n",			m_dAlignLastDieOffsetY);
		oFile.WriteString(szLine);

		szLine.Format("Relative Die Offset X (FOV),%d\n",			m_dAlignLastDieRelOffsetX);
		oFile.WriteString(szLine);

		szLine.Format("Relative Die Offset Y (FOV),%d\n",			m_dAlignLastDieRelOffsetY);
		oFile.WriteString(szLine);

		szLine.Format("Realign Method DEFAULT/1-PT,%d\n",			m_lRealignBinFrameOption);
		oFile.WriteString(szLine);
	}

	oFile.Close();
	return TRUE;
}


VOID CBinTable::PhyBlksDrawing()
{
	ULONG ulBTXOffset=0, ulBTYOffset=0, ulBTXRange=0, ulBTYRange=0;		//Klocwork
	ULONG ulBinTblULX, ulBinTblLRX, ulBinTblULY, ulBinTblLRY;
	ULONG ulUpperLeftX, ulUpperLeftY, ulLowerRightX, ulLowerRightY;
	ULONG k, ulWidth, ulHeight;

	m_PhySetupWrapper.Initialize(_T("PhyBlkSetup"));
	m_PhySetupWrapper.ResetPhysicalBin();
	m_PhySetupWrapper.ResetLogicalBin();

	//Bin table
	ulBTXRange = m_lXPosLimit + abs(m_lXNegLimit);
	ulBTYRange = m_lYPosLimit + abs(m_lYNegLimit);

	//BT shifts on x or y axis for better looking
	if (ulBTYRange > ulBTXRange)
	{
		ulBTXOffset = 
			(ULONG)((DOUBLE)(ulBTYRange - ulBTXRange) / 8 / m_lBlocksDisplayResol);
		ulBTYOffset = 0;
	}
	else if (ulBTYRange < ulBTXRange)
	{
		ulBTXOffset = 0;
		ulBTYOffset = 
			(ULONG)((DOUBLE)(ulBTXRange - ulBTYRange) / 8 / m_lBlocksDisplayResol);
	}	

	ulBinTblULY = (ULONG)((m_lYNegLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblLRY = (ULONG)((m_lYPosLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblULX = (ULONG)((m_lXNegLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;
	ulBinTblLRX = (ULONG)((m_lXPosLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;

	ulWidth = ulBinTblLRX - ulBinTblULX;
	ulHeight = ulBinTblLRY - ulBinTblULY;

	m_PhySetupWrapper.SetBinTable(ulBinTblULX, ulBinTblULY, ulWidth, ulHeight);
	
	//Physical blocks    
	for (k = 1; k <= m_ulNoOfPhyBlk; k++)
	{
		k = min(k, BT_MAX_PHYBLK_SIZE-1);	//Klocwork	//v4.02T5
		
		if (m_oPhyBlkMain.GrabIsSetup(k))
		{
			ulUpperLeftX = m_oPhyBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulUpperLeftY = m_oPhyBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightX = m_oPhyBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightY = m_oPhyBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);

			ulWidth = ulLowerRightX - ulUpperLeftX;
			ulHeight = ulLowerRightY - ulUpperLeftY;

			m_PhySetupWrapper.AddPhysicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight);
		}
	}

	m_PhySetupWrapper.UpdateBin();
} //end PhyBlksDrawing


VOID CBinTable::PhyBlksDrawing_BL(ULONG ulBlkId)
{
	ULONG ulBTXOffset=0, ulBTYOffset=0, ulBTXRange=0, ulBTYRange=0;		//Klocwork
	ULONG ulBinTblULX, ulBinTblLRX, ulBinTblULY, ulBinTblLRY;
	ULONG ulUpperLeftX, ulUpperLeftY, ulLowerRightX, ulLowerRightY;
	ULONG ulWidth, ulHeight;

	m_PhySetupWrapper.Initialize(_T("PhyBlkSetup"));
	m_PhySetupWrapper.ResetPhysicalBin();
	m_PhySetupWrapper.ResetLogicalBin();

	//Bin table
	ulBTXRange = m_lXPosLimit + abs(m_lXNegLimit);
	ulBTYRange = m_lYPosLimit + abs(m_lYNegLimit);

	//BT shifts on x or y axis for better looking
	if (ulBTYRange > ulBTXRange)
	{
		ulBTXOffset = 
			(ULONG)((DOUBLE)(ulBTYRange - ulBTXRange) / 8 / m_lBlocksDisplayResol);
		ulBTYOffset = 0;
	}
	else if (ulBTYRange < ulBTXRange)
	{
		ulBTXOffset = 0;
		ulBTYOffset = 
			(ULONG)((DOUBLE)(ulBTXRange - ulBTYRange) / 8 / m_lBlocksDisplayResol);
	}	

	ulBinTblULY = (ULONG)((m_lYNegLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblLRY = (ULONG)((m_lYPosLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblULX = (ULONG)((m_lXNegLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;
	ulBinTblLRX = (ULONG)((m_lXPosLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;

	ulWidth = ulBinTblLRX - ulBinTblULX;
	ulHeight = ulBinTblLRY - ulBinTblULY;

	m_PhySetupWrapper.SetBinTable(ulBinTblULX, ulBinTblULY, ulWidth, ulHeight);
	//Physical blocks    
	if (m_oPhyBlkMain.GrabIsSetup(ulBlkId))
	{
		ulUpperLeftX = m_oPhyBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
			ulBlkId, m_lBlocksDisplayResol);
		ulUpperLeftY = m_oPhyBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
			ulBlkId, m_lBlocksDisplayResol);
		ulLowerRightX = m_oPhyBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
			ulBlkId, m_lBlocksDisplayResol);
		ulLowerRightY = m_oPhyBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
			ulBlkId, m_lBlocksDisplayResol);

		ulWidth = ulLowerRightX - ulUpperLeftX;
		ulHeight = ulLowerRightY - ulUpperLeftY;

		m_PhySetupWrapper.AddPhysicalBin(ulBlkId, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight);
	}

	m_PhySetupWrapper.UpdateBin();
} //end PhyBlksDrawing_BL


VOID CBinTable::BinBlksDrawing()
{
	ULONG ulBTXOffset=0, ulBTYOffset=0, ulBTXRange=0, ulBTYRange=0;		//Klocwork
	ULONG ulBinTblULX, ulBinTblLRX, ulBinTblULY, ulBinTblLRY;
	ULONG ulUpperLeftX, ulUpperLeftY, ulLowerRightX, ulLowerRightY;
	ULONG k, ulWidth, ulHeight;
	UCHAR ucGrade;

	m_BinSetupWrapper.Initialize(_T("BinBlkSetup"));
	m_BinSetupWrapper.ResetPhysicalBin();
	m_BinSetupWrapper.ResetLogicalBin();

	//Bin table
	ulBTXRange = m_lXPosLimit + abs(m_lXNegLimit);
	ulBTYRange = m_lYPosLimit + abs(m_lYNegLimit);

	//BT shifts on x or y axis for better looking
	if (ulBTYRange > ulBTXRange)
	{
		ulBTXOffset = 
			(ULONG)((DOUBLE)(ulBTYRange - ulBTXRange) / 8 / m_lBlocksDisplayResol);
		ulBTYOffset = 0;
	}
	else if (ulBTYRange < ulBTXRange)
	{
		ulBTXOffset = 0;
		ulBTYOffset = 
			(ULONG)((DOUBLE)(ulBTXRange - ulBTYRange) / 8 / m_lBlocksDisplayResol);
	}	

	ulBinTblULY = (ULONG)((m_lYNegLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblLRY = (ULONG)((m_lYPosLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblULX = (ULONG)((m_lXNegLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;
	ulBinTblLRX = (ULONG)((m_lXPosLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;

	ulWidth = ulBinTblLRX - ulBinTblULX;
	ulHeight = ulBinTblLRY - ulBinTblULY;

	m_BinSetupWrapper.SetBinTable(ulBinTblULX, ulBinTblULY, ulWidth, ulHeight);
	
	//Physical blocks    
	for (k = 1; k <= m_oPhyBlkMain.GetNoOfBlk(); k++)
	{
		k = min(k, BT_MAX_PHYBLK_SIZE-1);	//Klocwork	//v4.02T5

		if (m_oPhyBlkMain.GrabIsSetup(k))
		{
			ulUpperLeftX = m_oPhyBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulUpperLeftY = m_oPhyBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightX = m_oPhyBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightY = m_oPhyBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);

			ulWidth = ulLowerRightX - ulUpperLeftX;
			ulHeight = ulLowerRightY - ulUpperLeftY;

			m_BinSetupWrapper.AddPhysicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight);
		}
	}

	//Bin Blocks
	for (k = 1; k <= m_ulNoOfBinBlk; k++)
	{
		if ((m_oBinBlkMain.GrabIsSetup(k)) && (!m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(k)))
		{
			ulUpperLeftX = m_oBinBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulUpperLeftY = m_oBinBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightX = m_oBinBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightY = m_oBinBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);

			ulWidth = ulLowerRightX - ulUpperLeftX;
			ulHeight = ulLowerRightY - ulUpperLeftY;

			ucGrade = m_oBinBlkMain.GrabGrade(k);

			//Force display grade 0 if this binblk is temp disabled
			if ( m_oBinBlkMain.GrabStatus(k) == BT_BLK_DISABLE )
			{
				ucGrade = 0;
			}

			//m_BinSetupWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade + m_WaferMapWrapper.GetGradeOffset());
			m_BinSetupWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade);
		}
	}

	m_BinSetupWrapper.UpdateBin();

} //end BinBlksDrawing


VOID CBinTable::BinBlksDrawing_BL(ULONG ulBlkId)
{
	ULONG ulBTXOffset=0, ulBTYOffset=0, ulBTXRange=0, ulBTYRange=0;		//Klocwork
	ULONG ulBinTblULX, ulBinTblLRX, ulBinTblULY, ulBinTblLRY;
	ULONG ulUpperLeftX, ulUpperLeftY, ulLowerRightX, ulLowerRightY;
	ULONG k, ulWidth, ulHeight, ulPhyBlkItBelongsTo;
	UCHAR ucGrade;

	m_BinSetupWrapper.Initialize(_T("BinBlkSetup"));
	m_BinSetupWrapper.ResetPhysicalBin();
	m_BinSetupWrapper.ResetLogicalBin();

	//Bin table
	ulBTXRange = m_lXPosLimit + abs(m_lXNegLimit);
	ulBTYRange = m_lYPosLimit + abs(m_lYNegLimit);

	//BT shifts on x or y axis for better looking
	if (ulBTYRange > ulBTXRange)
	{
		ulBTXOffset = 
			(ULONG)((DOUBLE)(ulBTYRange - ulBTXRange) / 8 / m_lBlocksDisplayResol);
		ulBTYOffset = 0;
	}
	else if (ulBTYRange < ulBTXRange)
	{
		ulBTXOffset = 0;
		ulBTYOffset = 
			(ULONG)((DOUBLE)(ulBTXRange - ulBTYRange) / 8 / m_lBlocksDisplayResol);
	}	

	ulBinTblULY = (ULONG)((m_lYNegLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblLRY = (ULONG)((m_lYPosLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
	ulBinTblULX = (ULONG)((m_lXNegLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;
	ulBinTblLRX = (ULONG)((m_lXPosLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;

	ulWidth = ulBinTblLRX - ulBinTblULX;
	ulHeight = ulBinTblLRY - ulBinTblULY;

	m_BinSetupWrapper.SetBinTable(ulBinTblULX, ulBinTblULY, ulWidth, ulHeight);

	ulPhyBlkItBelongsTo = m_oBinBlkMain.GrabPhyBlkId(ulBlkId);
	
	//Physical block  
	if (m_oPhyBlkMain.GrabIsSetup(ulPhyBlkItBelongsTo))
	{
		ulUpperLeftX = m_oPhyBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
			ulPhyBlkItBelongsTo, m_lBlocksDisplayResol);
		ulUpperLeftY = m_oPhyBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
			ulPhyBlkItBelongsTo, m_lBlocksDisplayResol);
		ulLowerRightX = m_oPhyBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
			ulPhyBlkItBelongsTo, m_lBlocksDisplayResol);
		ulLowerRightY = m_oPhyBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
			ulPhyBlkItBelongsTo, m_lBlocksDisplayResol);

		ulWidth = ulLowerRightX - ulUpperLeftX;
		ulHeight = ulLowerRightY - ulUpperLeftY;

		m_BinSetupWrapper.AddPhysicalBin(ulPhyBlkItBelongsTo, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight);
	}

	//Bin Blocks
	for (k = 1; k <= m_ulNoOfBinBlk; k++)
	{
		if (m_oBinBlkMain.GrabPhyBlkId(k) == ulPhyBlkItBelongsTo)
		{
			if (m_oBinBlkMain.GrabIsSetup(k))
			{
				ulUpperLeftX = m_oBinBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
					k, m_lBlocksDisplayResol);
				ulUpperLeftY = m_oBinBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
					k, m_lBlocksDisplayResol);
				ulLowerRightX = m_oBinBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
					k, m_lBlocksDisplayResol);
				ulLowerRightY = m_oBinBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
					k, m_lBlocksDisplayResol);

				ulWidth = ulLowerRightX - ulUpperLeftX;
				ulHeight = ulLowerRightY - ulUpperLeftY;

				ucGrade = m_oBinBlkMain.GrabGrade(k);

				//m_BinSetupWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade + m_WaferMapWrapper.GetGradeOffset());
				m_BinSetupWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade);
			}
		}
	}

	m_BinSetupWrapper.UpdateBin();
} //end BinBlksDrawing_BL


VOID CBinTable::UpdateBinDisplay(ULONG ulBlkId, UCHAR ucMode)
{
	//Mode: 0 = Show Bin Full; 1 = Show original; 2 = Show active bonding Blk 
	ULONG ulBTXOffset=0, ulBTYOffset=0, ulBTXRange=0, ulBTYRange=0;
	ULONG ulBinTblULX, ulBinTblLRX, ulBinTblULY, ulBinTblLRY;
	ULONG ulUpperLeftX, ulUpperLeftY, ulLowerRightX, ulLowerRightY;
	ULONG k, ulWidth, ulHeight;
	UCHAR ucGrade;


	//Only initial if mode = 0 or 1
	if ( ucMode < 2 )	
	{
		m_BinDisplayWrapper.Initialize(_T("BinBlkDisplay"));
		m_BinDisplayWrapper.ResetPhysicalBin();
		m_BinDisplayWrapper.ResetLogicalBin();
	}

	//Bin table
	ulBTXRange = m_lXPosLimit + abs(m_lXNegLimit);
	ulBTYRange = m_lYPosLimit + abs(m_lYNegLimit);

	//BT shifts on x or y axis for better looking
	if (ulBTYRange > ulBTXRange)
	{
		ulBTXOffset = 
			(ULONG)((DOUBLE)(ulBTYRange - ulBTXRange) / 8 / m_lBlocksDisplayResol);
		ulBTYOffset = 0;
	}
	else if (ulBTYRange < ulBTXRange)
	{
		ulBTXOffset = 0;
		ulBTYOffset = 
			(ULONG)((DOUBLE)(ulBTXRange - ulBTYRange) / 8 / m_lBlocksDisplayResol);
	}	

	//Only draw physical blk if mode = 0 or 1
	if ( ucMode < 2 )
	{
		ulBinTblULY = (ULONG)((m_lYNegLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
		ulBinTblLRY = (ULONG)((m_lYPosLimit + abs(m_lYNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTYOffset;
		ulBinTblULX = (ULONG)((m_lXNegLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;
		ulBinTblLRX = (ULONG)((m_lXPosLimit + abs(m_lXNegLimit))/m_lBlocksDisplayResol + 0.5) + 15 + ulBTXOffset;

		ulWidth = ulBinTblLRX - ulBinTblULX;
		ulHeight = ulBinTblLRY - ulBinTblULY;

		m_BinDisplayWrapper.SetBinTable(ulBinTblULX, ulBinTblULY, ulWidth, ulHeight);
		
		//Physical blocks    
		for (k = 1; k <= m_oPhyBlkMain.GetNoOfBlk(); k++)
		{
			k = min(k, BT_MAX_PHYBLK_SIZE-1);	//Klocwork	//v4.02T5

			if (m_oPhyBlkMain.GrabIsSetup(k))
			{
				ulUpperLeftX = m_oPhyBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
					k, m_lBlocksDisplayResol);
				ulUpperLeftY = m_oPhyBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
					k, m_lBlocksDisplayResol);
				ulLowerRightX = m_oPhyBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
					k, m_lBlocksDisplayResol);
				ulLowerRightY = m_oPhyBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
					k, m_lBlocksDisplayResol);

				ulWidth = ulLowerRightX - ulUpperLeftX;
				ulHeight = ulLowerRightY - ulUpperLeftY;

				m_BinDisplayWrapper.AddPhysicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight);
			}
		}
	}

	//Bin Blocks
	for (k = 1; k <= m_oBinBlkMain.GetNoOfBlk(); k++)
	{
		if ((m_oBinBlkMain.GrabIsSetup(k)) && (!m_oBinBlkMain.GrabIsDisableFromSameGradeMerge(k)))
		{
			ulUpperLeftX = m_oBinBlkMain.ConvertUpperLeftX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulUpperLeftY = m_oBinBlkMain.ConvertUpperLeftY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightX = m_oBinBlkMain.ConvertLowerRightX(ulBTXRange/2, ulBTXOffset, 
				k, m_lBlocksDisplayResol);
			ulLowerRightY = m_oBinBlkMain.ConvertLowerRightY(ulBTYRange/2, ulBTYOffset, 
				k, m_lBlocksDisplayResol);

			ulWidth = ulLowerRightX - ulUpperLeftX;
			ulHeight = ulLowerRightY - ulUpperLeftY;

			ucGrade = m_oBinBlkMain.GrabGrade(k);

			if ( ucMode == 0 )
			{
				COLORREF crUserColor = RGB(150,150,150); //(white color)

				//Show other binblk is fulled if their grade is equal
				if ( m_oBinBlkMain.GrabGrade(k) == m_oBinBlkMain.GrabGrade(ulBlkId) )
				{
					crUserColor = RGB(255,5,5); //(in Red color)
				}

				m_BinDisplayWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade, k, crUserColor);
			}
			else if ( ucMode == 1 )
			{
				//Force display grade 0 if this binblk is temp disabled
				if ( m_oBinBlkMain.GrabStatus(k) == BT_BLK_DISABLE )
				{
					ucGrade = 0;
					m_BinDisplayWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade);
				}
				else if ( m_oBinBlkMain.GrabNVIsFull(k) == TRUE )
				{
					COLORREF crUserColor = RGB(255,5,5); //(in Red color)
					m_BinDisplayWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade, k, crUserColor);
				}
				else
				{
					m_BinDisplayWrapper.AddLogicalBin(k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade);
				}
			}
			else 
			{
				if ( k == ulBlkId )
				{
					COLORREF crUserColor = RGB(200,200,200); //(grey color)
					m_BinDisplayWrapper.ModifyLogicalBin(k-1, k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade, k, crUserColor);
				}
				else
				{
					//Force display grade 0 if this binblk is temp disabled
					if ( m_oBinBlkMain.GrabStatus(k) == BT_BLK_DISABLE )
					{
						ucGrade = 0;
						m_BinDisplayWrapper.ModifyLogicalBin(k-1, k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade);
					}
					else if ( m_oBinBlkMain.GrabNVIsFull(k) == TRUE )
					{
						COLORREF crUserColor = RGB(255,5,5); //(in Red color)
						m_BinDisplayWrapper.ModifyLogicalBin(k-1, k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade, k, crUserColor);
					}
					else
					{
						m_BinDisplayWrapper.ModifyLogicalBin(k-1, k, ulUpperLeftX, ulUpperLeftY, ulWidth, ulHeight, ucGrade);
					}
				}
			}
		}
	}

	m_BinDisplayWrapper.UpdateBin();
}


VOID CBinTable::WriteGradeLegend()
{
	ULONG i;
	UCHAR ucGrade;
	CString szGrade;

	m_GradeLegendWrapper.Initialize("GradeLegend");
	m_GradeLegendWrapper.ResetGrade();

	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			ucGrade = m_oBinBlkMain.GrabGrade(i);
			szGrade.Format("%d", ucGrade + m_WaferMapWrapper.GetGradeOffset());

			m_GradeLegendWrapper.AddGrade(ucGrade);
		}
	}

	m_GradeLegendWrapper.UpdateGrade();
} //end WriteGradeLegend


VOID CBinTable::LoadWaferStatistics()
{
	CStringArray szaHeader, szaData, szaDataSum;
	CString szNoOfSortedDie, szGrade, szBinBlk, szBinBlkList;
	CString szTotalSortedFromAllGrades, szCapacity, szInputCount, szBlkInUse;
	CString szOrgGradeString;
	ULONG ulNoOfSortedDie, i;
	ULONG j;
	ULONG ulCapacity, ulInputCount, ulBlkInUse, ulRowIndex;
	UCHAR ucGrade;
	int nFontSize = 17;

	m_BinTotalWrapper.Initialize("WaferStat");
	m_BinTotalWrapper.SetScrollBarSize(25);
	m_BinTotalWrapper.RemoveAllData();

	//Statistics Header
	szaHeader.Add("Capacity");
	szaHeader.Add("Input Count");
	szaHeader.Add("Total Sorted");
	szaHeader.Add("Bin Blks(s)");
	szaHeader.Add("Blk In Use");
	szaHeader.Add("Org. Grade");

	m_BinTotalWrapper.SetHeader(8404992,10987412,"Grade", nFontSize, szaHeader);


	//Get wafermap selected grade
	int k;
	BOOL bGradeMatch = FALSE;	//Klocwork
	CString szOrgGrade;
	CUIntArray aulSelectedGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);


	//Statistics Data
	m_ulTotalSortedFromAllGrades = 0;
	ulRowIndex = 0;
	BOOL bHaveSameGrade = FALSE;
	
/*	commented for correct display
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			BOOL bHaveSameGrade = FALSE;
			for (j = 1; j <i; j++)
			{
				if (m_oBinBlkMain.GrabGrade(j) == m_oBinBlkMain.GrabGrade(i))
				{
					bHaveSameGrade = TRUE;
					break;
				}
			}
			
			if ( bHaveSameGrade == FALSE )
			{
				ucGrade = m_oBinBlkMain.GrabGrade(i);
			}
			else
			{
				ucGrade = m_oBinBlkMain.GrabOriginalGrade(i);
			}

			szGrade.Format("%d", ucGrade);

			m_oBinBlkMain.SetStatRowIndexPerGrade(ucGrade, i-1);
		}
	}
*/
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			BOOL bHaveSameGrade = FALSE;
			for (j = 1; j <i; j++)
			{
				if (m_oBinBlkMain.GrabGrade(j) == m_oBinBlkMain.GrabGrade(i))
				{
					bHaveSameGrade = TRUE;
					break;
				}
			}
			
			if ( bHaveSameGrade == FALSE )
			{
				ucGrade = m_oBinBlkMain.GrabGrade(i);
			}
			else
			{
				ucGrade = m_oBinBlkMain.GrabOriginalGrade(i);
			}

			//ucGrade = m_oBinBlkMain.GrabGrade(i);
			szGrade.Format("%d", ucGrade);

			m_oBinBlkMain.SetStatRowIndexPerGrade(ucGrade, ulRowIndex);		//block2
			ulRowIndex++;

			//Capacity
			ulCapacity = m_oBinBlkMain.GrabGradeCapacity(ucGrade);
			szCapacity.Format("%d", ulCapacity);
			szaData.Add(szCapacity);

			//Input Count
			ulInputCount = m_oBinBlkMain.GrabInputCount(ucGrade);
			szInputCount.Format("%d", ulInputCount);
			szaData.Add(szInputCount);

			
			//Total No of Sorted Die
			// Check if whether it is a row required to update for specific grade
			if (i-1 == m_oBinBlkMain.GrabStatRowIndex(ucGrade))
			{
				ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
			}
			else
			{
				ulNoOfSortedDie = 0;
			}
			
			szNoOfSortedDie.Format("%d", ulNoOfSortedDie);
			m_ulTotalSortedFromAllGrades += m_oBinBlkMain.GrabNVNoOfBondedDie(i);

		
			szaData.Add(szNoOfSortedDie);

			//Bin Block(s)
			szBinBlkList.Format("%d", i);
			for (j = i + 1; j <= m_oBinBlkMain.GetNoOfBlk(); j++)
			{
				if (m_oBinBlkMain.GrabIsSetup(j))
				{
					if (m_oBinBlkMain.GrabGrade(j) == ucGrade)
					{
						szBinBlk.Format("%d", j);
						szBinBlkList = szBinBlkList + ";" + szBinBlk;
					}
				}
			}
			if ( ulCapacity == 0 )
			{
				szBinBlkList = "0";
			}
			szaData.Add(szBinBlkList);

			//Block In Use
			//ulBlkInUse = m_oBinBlkMain.GrabBlkInUse(ucGrade);
			ulBlkInUse = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);
			szBlkInUse.Format("%d", ulBlkInUse);
			if ( ulCapacity == 0 )
			{
				szBlkInUse = "0";
			}
			szaData.Add(szBlkInUse);
	

			//Change row color for grade is selected on wafermap or not
			for (k = 0; k < aulSelectedGradeList.GetSize(); k++)
			{
				if ( (ucGrade == (UCHAR)(aulSelectedGradeList.GetAt(k) - m_WaferMapWrapper.GetGradeOffset())) ) 
				{
					bGradeMatch = TRUE;
				}
			}

			szOrgGrade.Format("%d", m_WaferMapWrapper.GetOriginalGrade(ucGrade + m_WaferMapWrapper.GetGradeOffset()) - m_WaferMapWrapper.GetGradeOffset());
			
			/*
			if (m_WaferMapWrapper.GetReader() != NULL)
			{
				szOrgGradeString = m_WaferMapWrapper.GetReader()->GetOriginalGradeString(ucGrade + m_WaferMapWrapper.GetGradeOffset());

				if (szOrgGradeString != "")
				{
					szOrgGrade = szOrgGradeString;
				}
			}
			*/

			szOrgGradeString = m_WaferMapWrapper.GetOriginalGradeString(ucGrade + m_WaferMapWrapper.GetGradeOffset());

			if (szOrgGradeString != "")
			{
				szOrgGrade = szOrgGradeString;
			}

			szaData.Add(szOrgGrade);

			if ( (bGradeMatch == TRUE)  && (i-1 == m_oBinBlkMain.GrabStatRowIndex(ucGrade)))
			{
				m_BinTotalWrapper.AddData(13158600, 8404992, ucGrade, nFontSize, szaData);
			}
			else
			{
				m_BinTotalWrapper.AddData(8404992, 10987412, ucGrade, nFontSize, szaData);
			}

			szaData.RemoveAll();
		}
	}

	szaDataSum.Add(" ");
	szaDataSum.Add(" ");
	
	szTotalSortedFromAllGrades.Format("%d", m_ulTotalSortedFromAllGrades);
	szaDataSum.Add(szTotalSortedFromAllGrades);
	m_ulTotalSorted = m_ulTotalSortedFromAllGrades;
	m_BinTotalWrapper.SetSum(8404992, 10987412, "Total", nFontSize, szaDataSum);

	m_BinTotalWrapper.UpdateTotal();
} //end LoadWaferStatistics


VOID CBinTable::UpdateWaferStatistics(UCHAR ucGrade)
{
	ULONG ulRowIndex, ulCapacity, ulInputCount, ulNoOfSortedDie, ulBlkInUse;
	CString szCapacity, szInputCount, szNoOfSortedDie, szBinBlkList, szBinBlk, szBlkInUse;
	CString szTotalSortedFromAllGrades;
	CStringArray szaData, szaDataSum;
	CString szOrgGradeString;
	BOOL bFirstElement;
	ULONG i;
	int nFontSize = 17;

	m_ulTotalSortedFromAllGrades ++;

	ulRowIndex = m_oBinBlkMain.GrabStatRowIndex(ucGrade);

	//Capacity
	ulCapacity = m_oBinBlkMain.GrabGradeCapacity(ucGrade);
	szCapacity.Format("%d", ulCapacity);
	szaData.Add(szCapacity);

	//Input Count
	ulInputCount = m_oBinBlkMain.GrabInputCount(ucGrade);
	szInputCount.Format("%d", ulInputCount);
	szaData.Add(szInputCount);

	//Total No of Sorted Die
	ulNoOfSortedDie = m_oBinBlkMain.GrabNVNoOfSortedDie(ucGrade);
	szNoOfSortedDie.Format("%d", ulNoOfSortedDie);
	szaData.Add(szNoOfSortedDie);


	//Bin Block(s)
	bFirstElement = TRUE;
	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabGrade(i) == ucGrade)
		{
			szBinBlk.Format("%d", i);
			if (bFirstElement)
			{
				szBinBlkList = szBinBlk;
				bFirstElement = FALSE;
			}
			else
			{
				szBinBlkList = szBinBlkList + ";" + szBinBlk;
			}
		}
	}
	szaData.Add(szBinBlkList);

	//Block In Use
	ulBlkInUse = m_oBinBlkMain.GrabNVBlkInUse(ucGrade);
	szBlkInUse.Format("%d", ulBlkInUse);
	szaData.Add(szBlkInUse);

	//Get wafermap selected grade
	int k;
	BOOL bGradeMatch = FALSE;	//Klocwork
	CString szOrgGrade;
	CUIntArray aulSelectedGradeList;
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

	//Change row color for grade is selected on wafermap or not
	for (k = 0; k < aulSelectedGradeList.GetSize(); k++)
	{
		if ( (ucGrade == (UCHAR)(aulSelectedGradeList.GetAt(k) - m_WaferMapWrapper.GetGradeOffset())) ) 
		{
			bGradeMatch = TRUE;
		}
	}

	szOrgGrade.Format("%d", m_WaferMapWrapper.GetOriginalGrade(ucGrade + m_WaferMapWrapper.GetGradeOffset()) - m_WaferMapWrapper.GetGradeOffset());

	/*
	if (m_WaferMapWrapper.GetReader() != NULL)
	{
		szOrgGradeString = m_WaferMapWrapper.GetReader()->GetOriginalGradeString(ucGrade + m_WaferMapWrapper.GetGradeOffset());

		if (szOrgGradeString != "")
		{
			szOrgGrade = szOrgGradeString;
		}
	}
	*/
	szOrgGradeString = m_WaferMapWrapper.GetOriginalGradeString(ucGrade + m_WaferMapWrapper.GetGradeOffset());

	if (szOrgGradeString != "")
	{
		szOrgGrade = szOrgGradeString;
	}

	szaData.Add(szOrgGrade);

	if ( bGradeMatch == TRUE )
	{
		m_BinTotalWrapper.ModifyData(ulRowIndex, 13158600, 8404992, ucGrade, nFontSize, szaData);
	}
	else
	{
		m_BinTotalWrapper.ModifyData(ulRowIndex, 8404992, 10987412, ucGrade, nFontSize, szaData);
	}

	szaDataSum.Add(" ");
	szaDataSum.Add(" ");
	szTotalSortedFromAllGrades.Format("%d", m_ulTotalSortedFromAllGrades);
	szaDataSum.Add(szTotalSortedFromAllGrades);
	m_ulTotalSorted = m_ulTotalSortedFromAllGrades;
	m_BinTotalWrapper.SetSum(8404992, 10987412, "Total", nFontSize, szaDataSum);
	//m_BinTotalWrapper.UpdateSum();

	m_BinTotalWrapper.UpdateTotal();

} //end UpdateWaferStatistics


BOOL CBinTable::GetBinBlksContainedInPhyBlk(ULONG ulPhyBlkId, CDWordArray &dwaBinBlkIdsList)
{
	ULONG i;

	if (ulPhyBlkId > m_oPhyBlkMain.GetNoOfBlk())
	{
		return FALSE;
	}
	else
	{
		for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
		{
			if (m_oBinBlkMain.GrabPhyBlkId(i) == ulPhyBlkId)
			{
				dwaBinBlkIdsList.Add(i);
			}
		}
		return TRUE;
	}

} //end GetBinBlksContainedInPhyBlk

// huga
BOOL CBinTable::CheckWaferIsPicked()
{
	CUIntArray aulavilGradeList;
	UCHAR ucGrade=0, ucCurrGrade=0;
	ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;
	LONG lTotalDie = 0;

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		return FALSE;
	}

	m_WaferMapWrapper.GetAvailableGradeList(aulavilGradeList);
	
	for (INT i=0; i<aulavilGradeList.GetSize(); i++)
	{
		ucGrade = aulavilGradeList.GetAt(i);
		m_WaferMapWrapper.GetStatistics(ucGrade, ulLeft, ulPicked, ulTotal);
		lTotalDie = lTotalDie + ulPicked; 	
	}

	if (lTotalDie > 0)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CBinTable::WaferEndYieldCheck()
{
	CString szMsg;
	ULONG ulGradeLeft, ulGradePicked, ulGradeTotal;
	ULONG ulTotalPicked, ulTotal;
	DOUBLE dYield;
	CUIntArray unaAvaGradeList,aulSelectedGradeList;
	BOOL bInSelectedList;
	int i, j;

	if (m_dWaferEndSortingYield == 0.0)
	{
		return TRUE;
	}

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		return TRUE;
	}
	
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);

	ulTotalPicked = 0;
	ulTotal = 0;

	for (i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		bInSelectedList = FALSE;
		m_WaferMapWrapper.GetStatistics(unaAvaGradeList.GetAt(i), ulGradeLeft, ulGradePicked, ulGradeTotal);
		
		for (j=0; j< aulSelectedGradeList.GetSize(); j++)
		{
			if (unaAvaGradeList.GetAt(i) == aulSelectedGradeList.GetAt(j))
			{
				bInSelectedList = TRUE;
				break;
			}
		}

		/*
		if (bInSelectedList == FALSE)
		{
			aulNGGradeList.Add(unaAvaGradeList.GetAt(i));
		}
		*/
		if (bInSelectedList == TRUE)
		{
			ulTotalPicked =  ulTotalPicked + ulGradePicked;
			ulTotal	= ulTotal + ulGradeTotal;
		}
	}

	if (ulTotal != 0)
	{
		dYield = ((DOUBLE)(ulTotalPicked)/ (DOUBLE)(ulTotal));
	}
	else
	{
		dYield = 0.0;
	}

	if (dYield < m_dWaferEndSortingYield/100)
	{	
		szMsg.Format("Total: %d Yield: %.2f", ulTotalPicked, dYield*100);
		szMsg = szMsg + "%";
		SetAlert_Msg_Red_Back(IDS_BT_WAFER_YIELD_ALARM, szMsg);

		szMsg.Format("Wafer yield alarm. Current wafer yield:%.2f", dYield*100);
		szMsg = szMsg + "%";
		SetStatusMessage(szMsg);

		return FALSE;
	}

	return TRUE;
}

BOOL CBinTable::WaferEndFileGenerating(CString szMsg, BOOL bCheckYield)
{
	INT i=0, j=0;
	LONG lWaferEndYear, lWaferEndMonth, lWaferEndDay;
	LONG lWaferEndHour, lWaferEndMinute, lWaferEndSecond;
	LONG lMapTime;
	ULONG ulPrescanGoodNum = 0, ulPrescanDefectNum = 0, ulPrescanBadCutNum = 0, ulPrescanExtraNum = 0, ulPrescanEmptyNum = 0;
	CString szTotalNoOfBinBlks, szMapDate, szMapTime, szTempFilePath;
	CString szWaferEndDate, szWaferEndHour, szWaferEndMinute, szWaferEndSecond, szWaferEndTime;
	CString szEndMonth, szEndDay;
	CString szLotNo, szBinLotDir;
	CString szTitle, szContent, szWaferDownTime;
	CString szMachineNo, szOperatorID;
	CString szMapFileDate;
	CString szMapFilePath;
	CString szPKGFilename = "";
	ULONG ulDieState;
	BOOL bIfGenWaferEndFileOk = FALSE;
	CString szMacUpTime = "", szMacIdleTime = "", szMacSetupTime = "";
	CString szMacRunTime = "", szMacAlarmTime = "", szMacAssitTime = "", szMacDownTime = "";
	CString szPSMachineNo = "";
	CString szMinGradeCountGrade = "";
	CString szMinGradeCountDieNo = "";
	LONG ulMinGradeCount;
	CString szMinGradeCount;
	CString szPrescanGoodNum, szPrescanDefectNum, szPrescanBadCutNum, szPrescanExtraNum, szPrescanEmptyNum;
	CString szBIBatchId;
	CString szDiePitchX, szDiePitchY, szBinBlockDiePerRow, szBinBlockDiePerCol, szBinBlockFullCount;
	CString szMapEPID;
	CString szWaferMapDevice;
	CString szMapReSortingBin;
	CString szWaferMapDeviceNumber;
	CString szWaferMapAt;
	CString szWaferMapLotNo;
	CString szIsCentralized;
	CString szGradeOneInputCount;
	CString szLog;
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	if (pApp == NULL)
	{
		return FALSE;
	}

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		SetNotGenerateWaferEndFile(FALSE);
		return TRUE;
	}
	CString szWaferId;
	LONG lPickTotal = 0, lMapTotal = 0;
	m_oBinBlkMain.GenBinBlkFile(lPickTotal, lMapTotal);
	CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferId);
	szWaferId.Remove('\t');
	szWaferId.Remove(',');
	szWaferId.Remove('\"');

	if (!CheckIfGenerateWaferEnd(szWaferId, lPickTotal, lMapTotal))
	{
		SetNotGenerateWaferEndFile(FALSE);
		szLog.Format("(Not Generate)P,%d,M,%d,%s",lPickTotal,lMapTotal,szMsg);
		SaveWaferEndInfo(szLog, szWaferId);
		SetErrorMessage(szLog);
		return TRUE;
	}

	BOOL bIsNGGrade = FALSE;

	CString szUnSelectedGrade, szUnSelectedGradeUnPick;
	CString szUnSelectedTotal, szMinGradeDieTotal;
	ULONG	ulUnSelectedTotal = 0, ulMinGradeTotal = 0;
	CWaferEndFileInterface *pWaferEndFileInterface;

	szTitle.LoadString(HMB_BT_GEN_WAFEREND_FILE);

	if( pApp->GetCustomerName()=="FATC" )
	{
		OpWaferEndBackupMap();
	}

	szTempFilePath = m_szWaferEndPath;
	if (!IsPathExist(m_szWaferEndPath))
	{
		szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);

		if (HmiMessage(szContent, szTitle, 103, 3) != glHMI_YES)
		{
			//HmiMessage("WaferEnd File is not generated!");
			SetAlert(IDS_BT_FAIL_TO_GEN_WEF);
			return FALSE;
		}

		szTempFilePath = gszUSER_DIRECTORY + "\\OutputFile\\WaferEnd";
	}

	szDiePitchX.Format("%d", _round(m_oBinBlkMain.GrabDDiePitchX(1)));
	szDiePitchY.Format("%d", _round(m_oBinBlkMain.GrabDDiePitchY(1)));
	szBinBlockDiePerRow.Format("%d",  m_oBinBlkMain.GrabNoOfDiePerRow(1));
	szBinBlockDiePerCol.Format("%d",  m_oBinBlkMain.GrabNoOfDiePerCol(1));

	if (m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1)) < m_oBinBlkMain.GrabNoOfDiePerBlk(1))
	{
		szBinBlockFullCount.Format("%d", m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1)));
	}
	else
	{
		szBinBlockFullCount.Format("%d", m_oBinBlkMain.GrabNoOfDiePerBlk(1));
	}

	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Pitch X",		szDiePitchX);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Pitch Y",		szDiePitchY);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Per Row",		szBinBlockDiePerRow);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Per Col",		szBinBlockDiePerCol);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Full Bin Count",		szBinBlockFullCount);

	pApp->GetMachineTimes(szMacUpTime, szMacIdleTime, szMacSetupTime, szMacRunTime, 
		szMacAlarmTime, szMacAssitTime, szMacDownTime);

	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Up Time",		szMacUpTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Idle Time",	szMacIdleTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Setup Time",	szMacSetupTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Run Time",		szMacRunTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Alarm Time",	szMacAlarmTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Assit Time",	szMacAssitTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Down Time",	szMacDownTime);

	//Evervision : the point check list
	CString szCoorChain = (*m_psmfSRam)["MS896A"]["PointCheckPosition"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("Point Check Position", szCoorChain);

	//Map Base Num
	CString szMapBaseNum;
	szMapBaseNum.Format("%d",m_ulMapBaseNum);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Map Base Num", szMapBaseNum);  //4.51D9 CL_YZ

	//Evervision : Grade 1 count
	szGradeOneInputCount.Format("%d", m_oBinBlkMain.GrabInputCount(1));
	m_GenerateWaferDatabase.SetGeneralInfoItems("Grade One Input Count", szGradeOneInputCount);

	//BinBlock Info
	szTotalNoOfBinBlks.Format("%d", m_oBinBlkMain.GetNoOfBlk());
	m_GenerateWaferDatabase.SetGeneralInfoItems("Total No of Bin Blks", szTotalNoOfBinBlks);

	//Wafer end path
	//szTempFilePath.Replace("\\", "\\\\");
	m_GenerateWaferDatabase.SetGeneralInfoItems("Wafer End File Path", szTempFilePath);
	szMapFilePath = (*m_psmfSRam)["MS896A"]["MapFilePath"];

	m_GenerateWaferDatabase.SetGeneralInfoItems("Wafer Map Path", szMapFilePath);

	//Lot No & Lot directory
	szLotNo		= (*m_psmfSRam)["MS896A"]["LotNumber"];
	szBinLotDir = (*m_psmfSRam)["MS896A"]["Bin Lot Directory"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Lot No", szLotNo);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Lot Directory", szBinLotDir);

	//Load Map time & date
	szMapDate = (*m_psmfSRam)["WaferTable"]["Load Map Date"];
	szMapTime = (*m_psmfSRam)["WaferTable"]["Load Map Time"];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Load Map Date", szMapDate);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Load Map Time", szMapTime);

	//Map start time & date
	szMapDate = (*m_psmfSRam)["WaferTable"]["Map Start Date"];
	szMapTime = (*m_psmfSRam)["WaferTable"]["Map Start Time"];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Start Date", szMapDate);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Start Time", szMapTime);

	//Map run time
	lMapTime = (*m_psmfSRam)["WaferTable"]["Map Run Time"];
	szMapTime.Format("%d", lMapTime);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Run Time", szMapTime);

	//Wafer End Date
	lWaferEndYear = (*m_psmfSRam)["WaferTable"]["Wafer End Year"];
	lWaferEndMonth = (*m_psmfSRam)["WaferTable"]["Wafer End Month"];

	szEndMonth.Format("%d", lWaferEndMonth);
	if (lWaferEndMonth < 10)
		szEndMonth = "0" + szEndMonth;

	lWaferEndDay = (*m_psmfSRam)["WaferTable"]["Wafer End Day"];
	szEndDay.Format("%d", lWaferEndDay);
	if (lWaferEndDay < 10)
		szEndDay = "0" + szEndDay;

	szWaferEndDate.Format("%d", lWaferEndYear);
	szWaferEndDate = szWaferEndDate + "-" + szEndMonth + "-" + szEndDay;

	//Wafer End Time
	lWaferEndHour = (*m_psmfSRam)["WaferTable"]["Wafer End Hour"];
	szWaferEndHour.Format("%d", lWaferEndHour);
	if (lWaferEndHour < 10)
		szWaferEndHour = "0" + szWaferEndHour;

	lWaferEndMinute = (*m_psmfSRam)["WaferTable"]["Wafer End Minute"];
	szWaferEndMinute.Format("%d", lWaferEndMinute);
	if (lWaferEndMinute < 10)
		szWaferEndMinute = "0" + szWaferEndMinute;

	lWaferEndSecond = (*m_psmfSRam)["WaferTable"]["Wafer End Second"];
	szWaferEndSecond.Format("%d", lWaferEndSecond);
	if (lWaferEndSecond < 10)
		szWaferEndSecond = "0" + szWaferEndSecond;

	szWaferEndTime = szWaferEndHour + ":" + szWaferEndMinute + ":" + szWaferEndSecond;

	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer End Date", szWaferEndDate);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer End Time", szWaferEndTime);

	for (i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		m_GenerateWaferDatabase.SetWaferSummaryItems(m_szaExtraBinInfoField[i], m_szaExtraBinInfo[i]);
	}

	ulMinGradeCount =	(LONG)(*m_psmfSRam)["Wafer Table Options"]["Min Grade Count"];
	szMinGradeCount.Format("%d", ulMinGradeCount);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Grade Count", szMinGradeCount);

	// write map header items
	szMapEPID = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_EPIID];
	szMapReSortingBin = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RESORTINGBIN];

	CString szMapDevice = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE];
	szMapDevice.Remove('"');
	szMapDevice.Remove(',');
	//AfxMessageBox(szMapDevice , MB_SYSTEMMODAL);

	//For LatticePower
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Device", szMapDevice);

	m_GenerateWaferDatabase.SetWaferSummaryItems("WaferMap EPID", szMapEPID);
	m_GenerateWaferDatabase.SetWaferSummaryItems("WaferMap ResortingBin", szMapReSortingBin);

	szWaferMapLotNo = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];
	m_GenerateWaferDatabase.SetWaferSummaryItems("WaferMap LotNo", szWaferMapLotNo);

	
	//Die counts
	CUIntArray unaAvaGradeList,aulSelectedGradeList, aulNGGradeList;
	CUIntArray aulRow, aulCol;
	BOOL bInSelectedList = FALSE;
	ULONG ulLeft, ulPicked, ulTotal;
	ULONG ulAccumulatedLeft, ulAccumulatedPicked, ulAccumulatedTotal = 0, ulAccumulatedFailGradeTotal = 0;
	ULONG ulNGEdgeDieNo;
	CString szAccumulatedLeft, szAccumulatedPicked, szAccumulatedTotal, szAccumulatedFailGradeTotal = "";
	CString szEdgeDieNo, szNGEdgeDieNo;
	CString szAvailableGradeList = "", szAvailableGradeTotal = "", szAvailableGradePick = "", szAvailableGradeLeft = "";
	CString szGrade, szTotal, szPick, szLeft;
	CString szRankIdName, szRankID, szRankBlkId;
	CString szBlkNo;

	ulNGEdgeDieNo	= 0;
	ulAccumulatedLeft = 0;
	ulAccumulatedPicked = 0;

	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);

	//v4.46T15	//NanoJoin
	BOOL bSaveBin151And152 = FALSE;
	if (pApp->GetCustomerName() == "NanoJoin")
		bSaveBin151And152 = TRUE;
	// 4.51D9 //CL_YZ
	BOOL bSaveBin150And100 = FALSE;
	if (pApp->GetCustomerName() == "ChanGeLight")
	{
		bSaveBin150And100 = TRUE;
	}
		
	ULONG ulMaxRow, ulMaxCol, ulRow, ulCol;
	ulMaxRow = ulMaxCol = 0;
	if (!WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol))
	{
		return FALSE;
	}

	for (i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		bInSelectedList = FALSE;
		m_WaferMapWrapper.GetStatistics(unaAvaGradeList.GetAt(i), ulLeft, ulPicked, ulTotal);

		for (j=0; j< aulSelectedGradeList.GetSize(); j++)
		{
			if (unaAvaGradeList.GetAt(i) == aulSelectedGradeList.GetAt(j))
			{
				bInSelectedList = TRUE;
				break;
			}
		}

		szGrade.Format("%d",unaAvaGradeList.GetAt(i)- m_WaferMapWrapper.GetGradeOffset());
		szPick.Format("%d", ulPicked);
		szLeft.Format("%d", ulLeft);
		szTotal.Format("%d",ulTotal);

		if (bSaveBin151And152)		//NanoJoin //v4.46T15	//Matt
		{
			if (szGrade == "122")
			{
				CString szBin151 = szTotal;
				m_GenerateWaferDatabase.SetWaferSummaryItems("Bin151", szBin151);
			}
			else if (szGrade == "123")
			{
				CString szBin152 = szTotal;
				m_GenerateWaferDatabase.SetWaferSummaryItems("Bin152", szBin152);
			}
		}

		if(bSaveBin150And100)
		{
			if (szGrade == "150")  // 4.51D9 //CL_YZ
			{
				CString szBin150 = szTotal;
				m_GenerateWaferDatabase.SetWaferSummaryItems("Bin150", szBin150);
			}
			else if (szGrade == "100")
			{
				CString szBin100 = szTotal;
				m_GenerateWaferDatabase.SetWaferSummaryItems("Bin100", szBin100);
			}
		}

		szAvailableGradeList = szAvailableGradeList + szGrade + ",";
		szAvailableGradeLeft = szAvailableGradeLeft + szLeft + ",";
		szAvailableGradePick = szAvailableGradePick + szPick + ",";
		szAvailableGradeTotal= szAvailableGradeTotal + szTotal + ",";

		if (bInSelectedList == FALSE)
		{
			// Previous Version use left, V3.26 updated to use Total no
			CString szDieTotal,szUnSelected;
			szUnSelected.Format("%d",unaAvaGradeList.GetAt(i)- m_WaferMapWrapper.GetGradeOffset());
			szUnSelectedGrade = szUnSelectedGrade + szUnSelected+ ",";
			szDieTotal.Format("%d",ulTotal);
			szUnSelectedGradeUnPick = szUnSelectedGradeUnPick + szDieTotal + ",";
			aulNGGradeList.Add(unaAvaGradeList.GetAt(i));
			ulUnSelectedTotal += ulTotal;
		}
		if ( (((ULONG) atoi(szMinGradeCount)) >= ulTotal) )
		{
			CString szMinTotal,szMinGrade;
			szMinGrade.Format("%d",unaAvaGradeList.GetAt(i)- m_WaferMapWrapper.GetGradeOffset());
			szMinGradeCountGrade = szMinGradeCountGrade + szMinGrade + ",";
			szMinTotal.Format("%d",ulTotal);
			szMinGradeCountDieNo = szMinGradeCountDieNo + szMinTotal + ",";
			ulMinGradeTotal += ulTotal;
		}

		ulAccumulatedLeft += ulLeft;
		ulAccumulatedPicked += ulPicked;
		ulAccumulatedTotal += ulTotal;
		CString szOriginalGrade = m_WaferMapWrapper.GetOriginalGradeString(unaAvaGradeList.GetAt(i));
		if( szOriginalGrade.Find("Fail")!=-1 )
		{
			ulAccumulatedFailGradeTotal += ulTotal;
		}
	}
	if( pApp->GetCustomerName()=="TongHui" )
	{
		szUnSelectedTotal.Format("%d", CPreBondEvent::m_ulMapOrgUnselTotal);
		szMinGradeDieTotal.Format("%d", CPreBondEvent::m_ulMapOrgMinTotal);
	}
	else
	{
		szUnSelectedTotal.Format("%d", ulUnSelectedTotal);
		szMinGradeDieTotal.Format("%d", ulMinGradeTotal);
	}

	//Remove comma at the back
	m_GenerateWaferDatabase.SetWaferSummaryItems("UnSelected Grade",			szUnSelectedGrade);
	m_GenerateWaferDatabase.SetWaferSummaryItems("UnSelected Grade Die Left",	szUnSelectedGradeUnPick);
	m_GenerateWaferDatabase.SetWaferSummaryItems("UnSelected Grade Die Total",	szUnSelectedTotal);

	// available grade list information
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade",				szAvailableGradeList);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade Die Left",	szAvailableGradeLeft);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade Die Pick",	szAvailableGradePick);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade Die Total",	szAvailableGradeTotal);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Count Grade",		szMinGradeCountGrade);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Count Die Left",	szMinGradeCountDieNo);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Count Die Total",	szMinGradeDieTotal);

	szAccumulatedLeft.Format("%d",		ulAccumulatedLeft);
	szAccumulatedPicked.Format("%d",	ulAccumulatedPicked);
	szAccumulatedTotal.Format("%d",		ulAccumulatedTotal);
	szAccumulatedFailGradeTotal.Format("%d", ulAccumulatedFailGradeTotal);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Die",				szAccumulatedTotal);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Sorted Die",		szAccumulatedPicked);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Unsorted Die",	szAccumulatedLeft);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total Fail device",			szAccumulatedFailGradeTotal);

	//AoYang
	if (pApp->GetCustomerName() == "AoYang")
	{
		CString szTotalTested, szOKChip, szNGChip, szModel;
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("TotalTested", szTotalTested);
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("OK CHIP", szOKChip);
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("NG CHIP", szNGChip);
		CMS896AStn::m_WaferMapWrapper.GetHeaderInfo()->Lookup("Model", szModel);

		m_GenerateWaferDatabase.SetWaferSummaryItems("AoYang TotalTested", szTotalTested);	
		m_GenerateWaferDatabase.SetWaferSummaryItems("AoYang OK CHIP", szOKChip);
		m_GenerateWaferDatabase.SetWaferSummaryItems("AoYang NG CHIP", szNGChip);
		m_GenerateWaferDatabase.SetWaferSummaryItems("AoYang Model", szModel);
	}

	//Get the number of edge die set in the map						//v3.33T4
	m_WaferMapWrapper.GetReader()->GetEdgeDice(aulRow, aulCol);
	szEdgeDieNo.Format("%d", aulRow.GetSize());
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Edge Die", szEdgeDieNo);

	if (aulRow.GetSize() == aulCol.GetSize())
	{
		for (INT i=0; i<aulRow.GetSize(); i++)
		{
			for (INT j = 0; j<aulNGGradeList.GetSize(); j++)
			{
				if (m_WaferMapWrapper.GetGrade(aulRow.GetAt(i), aulCol.GetAt(i)) == 
					aulNGGradeList.GetAt(j))
				{
					ulNGEdgeDieNo = ulNGEdgeDieNo + 1;
				}
			}
		}
	}

	for (INT i = 1; i <= (LONG)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		szRankID = m_oBinBlkMain.GrabRankID(m_oBinBlkMain.GrabGrade(i));
		szBlkNo.Format("%d", i);

		if (i==1)
		{
			szRankBlkId = szBlkNo;
			szRankIdName = szRankID;
		}
		else
		{
			szRankBlkId = szRankBlkId + "," + szBlkNo;
			szRankIdName = szRankIdName + "," + szRankID;
		}
	}

	m_GenerateWaferDatabase.SetWaferSummaryItems("RankID BlkId",	szRankBlkId);
	m_GenerateWaferDatabase.SetWaferSummaryItems("RankID Name",		szRankIdName);
	
	szNGEdgeDieNo.Format("%d", ulNGEdgeDieNo);
	m_GenerateWaferDatabase.SetWaferSummaryItems("NG Edge Die Size", szNGEdgeDieNo);

	//Map Filename
	m_GenerateWaferDatabase.SetWaferSummaryItems("Original Map Filename", GetMapFileName());

	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Filename", GetMapNameOnly());
	
	szMapFileDate = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_DATE_IN_MAP_FILE];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Date In Map File", szMapFileDate);

	//Output Filename
	CString szClrBinOutPath = m_szOutputFilePath;
	if( m_bAutoUploadOutputFile )
		szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;
	m_GenerateWaferDatabase.SetWaferSummaryItems("Output Filename", szClrBinOutPath);

	//Is centralized bond area ?
	//szIsCentralized = m_bCentralizedBondArea?"YES":"NO";
	szIsCentralized = m_oBinBlkMain.GrabIsCentralizedBondArea(1)? "YES":"NO";
	//HmiMessage("program: " + szIsCentralized);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Is Centralized Bond Area", szIsCentralized);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Id", szWaferId);

	CString szWaferName;

	szWaferName = GetMapFileName();
	m_GenerateWaferDatabase.SetWaferSummaryItems("MapFileName", szWaferName);

	//Wafer map lot ID
	CString szMapLotID, szLotNumber;
	szMapLotID = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map LotID", szMapLotID);

	szLotNumber = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map LotNo", szLotNumber);

	szWaferMapDeviceNumber = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map DeviceNumber", szWaferMapDeviceNumber);

	szWaferMapAt = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_AT];
	szWaferMapAt.Replace('\n','*');
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map At", szWaferMapAt);

	//Machine No	//v4.40T7
	szMachineNo = GetMachineNo();
	m_GenerateWaferDatabase.SetGeneralInfoItems("Machine No", szMachineNo);

	// Operator ID
	szOperatorID = (*m_psmfSRam)["MS896A"]["Operator Id"];

	CString szWaferLotFilePath = (*m_psmfSRam)["WaferTable"]["WaferLotInfoFilePath"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("WaferLotInfoFilePath", szWaferLotFilePath);

	m_GenerateWaferDatabase.SetGeneralInfoItems("Wafer End Summary Path", m_szWaferEndSummaryPath);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Operator ID", szOperatorID);

	szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("PKG Filename", szPKGFilename);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pUtl != NULL)
	{
		szPSMachineNo = pUtl->GetPsmMapMachineNo();
		m_GenerateWaferDatabase.SetGeneralInfoItems("PS MachineNo", szPSMachineNo);
	}

	
	//Chip, bad-cut, defect, ink, empty, rotate die counts
	//ULONG ulMissingDie;
	ULONG ulTotalNoOfChipDie, ulTotalNoOfBadCutDie, ulTotalNoOfDefectDie;
	ULONG ulTotalNoOfInkDie, ulTotalNoOfEmptyDie, ulTotalNoOfRotateDie;
	ULONG ulTotalNoOfMissingDie;
	ULONG ulTotalNoOfUpLookFailDie = 0;
	ULONG ulPSTotalNoOfEmptyDie, ulPSTotalNoOfDefectDie, ulPSTotalNoOfBadCutDie;
	ULONG ulPSNGTotalNoOfEmptyDie, ulPSNGTotalNoOfDefectDie, ulPSNGTotalNoOfBadCutDie;

	ulTotalNoOfInkDie = 0;
	ulTotalNoOfEmptyDie = 0;
	ulTotalNoOfRotateDie = 0;
	ulTotalNoOfChipDie = 0;
	ulTotalNoOfBadCutDie = 0;
	ulTotalNoOfDefectDie = 0;
	ulTotalNoOfMissingDie = 0;

	ulPSTotalNoOfEmptyDie = 0;
	ulPSTotalNoOfDefectDie = 0;
	ulPSTotalNoOfBadCutDie = 0;

	ulPSNGTotalNoOfEmptyDie = 0;
	ulPSNGTotalNoOfDefectDie = 0;
	ulPSNGTotalNoOfBadCutDie = 0;


	for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
	{
		for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
		{
			ulDieState = GetMapDieState(ulRow, ulCol);
			
			// Missing Die Check Since it has no grade so check first
			if( ulDieState>WT_MAP_DS_BH_UPLOOK_FAIL )
			{
				ulTotalNoOfUpLookFailDie++;
				continue;
			}

			if (ulDieState > WT_MAP_DIESTATE_MISSING)
			{
				ulTotalNoOfMissingDie++;
				continue;
			}

			if (ulDieState == WT_MAP_DIESTATE_DEFECT)
				ulTotalNoOfDefectDie++;
			else if (ulDieState == WT_MAP_DIESTATE_INK)
				ulTotalNoOfInkDie++;
			else if (ulDieState == WT_MAP_DIESTATE_CHIP)
				ulTotalNoOfChipDie++;
			else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
				ulTotalNoOfBadCutDie++;
			else if (ulDieState == WT_MAP_DIESTATE_EMPTY)
				ulTotalNoOfEmptyDie++;
			else if (ulDieState == WT_MAP_DIESTATE_ROTATE)
				ulTotalNoOfRotateDie++;
			else if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY)
				ulPSTotalNoOfEmptyDie++;
			else if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT)
				ulPSTotalNoOfDefectDie++;
			else if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT)
				ulPSTotalNoOfBadCutDie++;
		}
	}

	CString szTotalNoOfChipDie, szTotalNoOfBadCutDie, szTotalNoOfDefectDie, szTotalNoOfMissingDie;
	CString szTotalNoOfUpLookFail = "0";
	//CString	szMissingDie;
	CString szTotalNoOfInkDie, szTotalNoOfEmptyDie, szTotalNoOfRotateDie;
	CString szPSTotalNoOfEmptyDie, szPSTotalNoOfDefectDie, szPSTotalNoOfBadCutDie;
	CString szPSNGTotalNoOfEmptyDie, szPSNGTotalNoOfDefectDie, szPSNGTotalNoOfBadCutDie;
	CString szMDCycleCountBHZ1, szMDCycleCountBHZ2, szColletJamBHZ1, szColletJamBHZ2;
	ULONG ulMDCycleCountBHZ1, ulMDCycleCountBHZ2, ulColletJamBHZ1, ulColletJamBHZ2;
	//4.54T15 Wafer End Gen MD and CJ of BHZ1 and BHZ2
	ulMDCycleCountBHZ1 = (*m_psmfSRam)["BondHead"]["MissingDieBHZ1"]; 
	ulMDCycleCountBHZ2 = (*m_psmfSRam)["BondHead"]["MissingDieBHZ2"]; 
	ulColletJamBHZ1	   = (*m_psmfSRam)["BondHead"]["ColletJamBHZ1"];
	ulColletJamBHZ2	   = (*m_psmfSRam)["BondHead"]["ColletJamBHZ2"];
	
	szTotalNoOfChipDie.Format("%d",		ulTotalNoOfChipDie);
	szTotalNoOfBadCutDie.Format("%d",	ulTotalNoOfBadCutDie);
	szTotalNoOfDefectDie.Format("%d",	ulTotalNoOfDefectDie);
	szTotalNoOfInkDie.Format("%d",		ulTotalNoOfInkDie);
	szTotalNoOfEmptyDie.Format("%d",	ulTotalNoOfEmptyDie);
	szTotalNoOfRotateDie.Format("%d",	ulTotalNoOfRotateDie);
	szTotalNoOfMissingDie.Format("%d",	ulTotalNoOfMissingDie);
	szTotalNoOfUpLookFail.Format("%d",  ulTotalNoOfUpLookFailDie);
	szMDCycleCountBHZ1.Format("%d",		ulMDCycleCountBHZ1);
	szMDCycleCountBHZ2.Format("%d",		ulMDCycleCountBHZ2);
	szColletJamBHZ1.Format("%d",		ulColletJamBHZ1);
	szColletJamBHZ2.Format("%d",		ulColletJamBHZ2);
	
	szPSTotalNoOfEmptyDie.Format("%d", ulPSTotalNoOfEmptyDie);
	szPSTotalNoOfDefectDie.Format("%d", ulPSTotalNoOfDefectDie);
	szPSTotalNoOfBadCutDie.Format("%d", ulPSTotalNoOfBadCutDie);

	szPSNGTotalNoOfEmptyDie.Format("%d", ulPSNGTotalNoOfEmptyDie);
	szPSNGTotalNoOfDefectDie.Format("%d", ulPSNGTotalNoOfDefectDie);
	szPSNGTotalNoOfBadCutDie.Format("%d", ulPSNGTotalNoOfBadCutDie);

	// Using the wafer map missing die count instead of
	//ulMissingDie = (ULONG)(*m_psmfSRam)["BondHead"]["CurrentWafer"]["MissingDie"];
	//szMissingDie.Format("%d", ulMissingDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Chip Die",	szTotalNoOfChipDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Bad-cut Die", szTotalNoOfBadCutDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Defect Die",	szTotalNoOfDefectDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Missing Die", szTotalNoOfMissingDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Up Look Fail", szTotalNoOfUpLookFail);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Ink Die",		szTotalNoOfInkDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Empty Die",	szTotalNoOfEmptyDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Rotate Die",	szTotalNoOfRotateDie);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Missing Die Cycle Count of BHZ1", szMDCycleCountBHZ1); //4.54T15
	m_GenerateWaferDatabase.SetWaferSummaryItems("Missing Die Cycle Count of BHZ2", szMDCycleCountBHZ2);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Collet Jam Cycle Count of BHZ1", szColletJamBHZ1);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Collet Jam Cycle Count of BHZ2", szColletJamBHZ2);


	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Total No of Empty Die", szPSTotalNoOfEmptyDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Total No of Defect Die", szPSTotalNoOfDefectDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Total No of Bad-cut Die",	 szPSTotalNoOfBadCutDie);

	m_GenerateWaferDatabase.SetWaferSummaryItems("PS NG Total No of Empty Die", szPSNGTotalNoOfEmptyDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS NG Total No of Defect Die", szPSNGTotalNoOfDefectDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS NG Total No of Bad-cut Die", szPSNGTotalNoOfBadCutDie);

	ulPrescanGoodNum   = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
    ulPrescanDefectNum = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
	ulPrescanBadCutNum = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
	ulPrescanExtraNum  = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA];
    ulPrescanEmptyNum  = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];

	szPrescanGoodNum.Format("%d",	ulPrescanGoodNum);
	szPrescanDefectNum.Format("%d", ulPrescanDefectNum);
	szPrescanBadCutNum.Format("%d", ulPrescanBadCutNum);
	szPrescanExtraNum.Format("%d",	ulPrescanExtraNum);
	szPrescanEmptyNum.Format("%d",	ulPrescanEmptyNum);



	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Good Num",		szPrescanGoodNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Defect Num",	szPrescanDefectNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS BadCut Num",	szPrescanBadCutNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Extra Num",	szPrescanExtraNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Empty Num",	szPrescanEmptyNum);
	
	szWaferDownTime = CalculateWaferEndDownTime();
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Down Time", szWaferDownTime);

	CString szWaferTotalTime;
	szWaferTotalTime = CalculateWaferTotalTime();
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Total Time", szWaferTotalTime);

	//Write the no of die in the wafer before wafer scn
	CString szOrginalCount;
	szOrginalCount.Format("%d",CPreBondEvent::m_nMapOrginalCount);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Die before SCN",szOrginalCount);

	ULONG ulSelectedTotal;
	CString szSelectedTotal;

	ulSelectedTotal = m_WaferMapWrapper.GetSelectedTotalDice();
	szSelectedTotal.Format("%d", ulSelectedTotal);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Selected Total", szSelectedTotal);

	// Prescan End
	CString szPrescanEndMonth;
	CString szPrescanEndDay;
	CString szPrescanEndHour;
	CString szPrescanEndMinute;
	CString szPrescanEndSecond;
	szPrescanEndMonth.Format("%d", m_stScanEndCTime.GetMonth());
	szPrescanEndDay.Format("%d", m_stScanEndCTime.GetDay());
	szPrescanEndHour.Format("%d", m_stScanEndCTime.GetHour());
	szPrescanEndMinute.Format("%d", m_stScanEndCTime.GetMinute());
	szPrescanEndSecond.Format("%d", m_stScanEndCTime.GetSecond());

	// Prescan Start
	CString szPrescanStartMonth;
	CString szPrescanStartDay;
	CString szPrescanStartHour;
	CString szPrescanStartMinute;
	CString szPrescanStartSecond;
	szPrescanStartMonth.Format("%d", m_stScanStartCTime.GetMonth());
	szPrescanStartDay.Format("%d", m_stScanStartCTime.GetDay());
	szPrescanStartHour.Format("%d", m_stScanStartCTime.GetHour());
	szPrescanStartMinute.Format("%d", m_stScanStartCTime.GetMinute());
	szPrescanStartSecond.Format("%d", m_stScanStartCTime.GetSecond());

	// Scan Time
	CString szWaferScanTime;
	CTimeSpan stScanTimeDiff = m_stScanStartCTime - m_stScanEndCTime;
	szWaferScanTime.Format("%d:%d:%d",stScanTimeDiff.GetHours(),stScanTimeDiff.GetMinutes(),stScanTimeDiff.GetSeconds());
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Scan Time", szWaferDownTime);

	CMS896AStn::m_GenerateWaferDatabase.GenEntireDatabase();

	CMS896AStn::m_QueryWaferDatabase.LoadDatabase();

	CString szWaferEndFileFormat = (*m_psmfSRam)["MS896A"]["WaferEndFileFormat"];
	pWaferEndFileInterface = CWaferEndFactoryMap::GetInstance()->CreateInstance(szWaferEndFileFormat);

	if (pWaferEndFileInterface == NULL)
	{
		//HmiMessage("Error: Wafer End File Object can't be created");
		SetAlert(IDS_BT_FAIL_TO_CREATE_OBJ);
	}
	else
	{
		bIfGenWaferEndFileOk = pWaferEndFileInterface->GenWaferEndFile(&CMS896AStn::m_QueryWaferDatabase);

		pWaferEndFileInterface->CleanUp();
		delete pWaferEndFileInterface;
	}

	CMS896AStn::m_QueryWaferDatabase.CleanUp();
	CMS896AStn::m_GenerateWaferDatabase.CleanUp();

	if (bIfGenWaferEndFileOk == FALSE)
	{
		//HmiMessage("Error generating wafer end file! Directory invalid!");
		SetAlert(IDS_BT_GEN_WAFERENDFILE_FAILED);
		return FALSE;
	}

	szLog.Format("#P,%d,#M,%d,%s",lPickTotal,lMapTotal,szMsg);
	SaveWaferEndInfo(szLog, szWaferId);
	SetNotGenerateWaferEndFile(FALSE);

	BOOL bIsEnableWaferLoader = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];

	if (bIsEnableWaferLoader == FALSE)
		return TRUE;

	LONG lSlotNo;
	lSlotNo = (*m_psmfSRam)["WaferTable"]["WT1LoadSlot"];
	
	SetGemValue(MS_SECS_SV_WAFER_ID, szWaferId);							//3003
	SetGemValue(MS_SECS_SV_SLOT_NO, lSlotNo);								//3004
	SetGemValue(MS_SECS_SV_CASSETTE_POS, "WFR");							//3005
	SetGemValue(MS_SECS_SV_TOTAL_BIN_TABLE_DIE_ON_WAFER, ulSelectedTotal);	//3040
	SetGemValue(MS_SECS_SV_TOTAL_DIE_BINED_FROM_WAFER, ulAccumulatedPicked);//3041
	SetGemValue(MS_SECS_SV_MAP_LOAD_TIME,				szMapTime);			//3007		//v4.36T1	//3E EquipMgr
    SendEvent(SG_CEID_WaferCompleted, TRUE);						//8017

	if( pApp->GetCustomerName()=="ZVision" )	// ZVision
	{
		CString szMapImagePath = szTempFilePath + "\\" + GetMapNameOnly() + ".JPG";
		SaveMapBitmapImage(szMapImagePath);
	}

	if ( pApp->GetCustomerName() == "AoYang" )
	{
		SaveTheWaferEndTime();		
	}

	return TRUE;
} //end WaferEnd FileGenerating

BOOL CBinTable::ScanSummaryGenerating()
{
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	if( m_lOutputScanSummary!=1 && m_lOutputScanSummary!=3 )
	{
		return FALSE;
	}

	ULONG ulTotalDie = GetMapTotalDie();
	ULONG ulEmptyDie = GetEmptySize();
	ULONG ulDefectDie= GetDefectSize();
	ULONG ulBadCutDie= GetBadCutSize();
	if( IsMS90HalfSortMode() )
	{
		ulEmptyDie  = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];
		ulDefectDie = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
		ulBadCutDie = (*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
	//x	(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD]	= ulUpGood   + ulGoodIdx;
	}
	ULONG ulScanGood = ulTotalDie;
	ULONG ulDiePicked = 0;

	CUIntArray unaAvaGradeList;
	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);
	for(INT i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		ULONG ulLeft, ulPick, ulTotal;
		m_WaferMapWrapper.GetStatistics(unaAvaGradeList.GetAt(i), ulLeft, ulPick, ulTotal);
		ulDiePicked += ulPick;
	}

	ulScanGood -= ulEmptyDie;
	ulScanGood -= ulDefectDie;
	ulScanGood -= ulBadCutDie;

	CStdioFile pSummaryFile;
	CTime stTime = CTime::GetCurrentTime();
	CString szMcNo = GetMachineNo();
	CString szSummaryName = stTime.Format("%Y-%m-") + szMcNo + ".csv";
	CString szSummaryPath = PRESCAN_RESULT_FULL_PATH + szSummaryName;
	CString szTargetPath  = m_szOutputScanSummaryPath + _T("\\") +  szSummaryName;

	ULONG ulTotalHeader		= 0;	//Klocwork
	ULONG ulPBGoodHeader	= 0;	//Klocwork

	double dResult = 0;
	if (ulTotalDie != 0 )
	{
		dResult = ((1.0*ulDiePicked)/(1.0*ulTotalDie));
	}

	CreateDirectory(m_szOutputScanSummaryPath, NULL);
	if( pSummaryFile.Open(szSummaryPath, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::typeText) )
	{
		if( pSummaryFile.GetLength()==0 )
		{
			pSummaryFile.WriteString("Load,Time,Device,WaferNo,Bad Cut,Defect,Empty,NG Bin,Scan Good,PB Good,Total,Yield%,Bin Count\n");
		}
		CString szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
		szPKGFilename.Trim(".pkg");
		pSummaryFile.SeekToEnd();

		//Load Map time & date
		CString szMapDate = (*m_psmfSRam)["WaferTable"]["Load Map Date"];
		CString szMapTime = (*m_psmfSRam)["WaferTable"]["Load Map Time"];
		// scan begin time (create map time).	13-01-04 01:27, end time
		CString szTime = szMapDate + "," + szMapTime + "-" + stTime.Format("%H:%M:%S,");
		pSummaryFile.WriteString(szTime);
		// PKG name, wafer name, badcut, defect, empty
		CString szName;
		szName.Format("%s,%s,", szPKGFilename, GetMapNameOnly());
		pSummaryFile.WriteString(szName);

		ULONG ulNgGradeDie = 0;
		if( m_bPrescanSkipNgGrade )
		{
			CUIntArray unaAvaGradeList;
			m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);
			for (int i = 0; i < unaAvaGradeList.GetSize(); i++)
			{
				UCHAR ucNgGrade = unaAvaGradeList.GetAt(i);
				if( IsScanMapNgGrade(ucNgGrade - m_WaferMapWrapper.GetGradeOffset()) )
				{
					ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;
					m_WaferMapWrapper.GetStatistics(ucNgGrade, ulLeft, ulPicked, ulTotal);
					ulNgGradeDie += ulTotal;
				}
			}
		}


		CString szCount;
		szCount.Format("%lu,%lu,%lu,%lu,%lu,%lu,%lu,%.4f,%lu", ulBadCutDie, ulDefectDie, ulEmptyDie, ulNgGradeDie, ulScanGood, ulTotalDie-ulNgGradeDie, ulTotalDie, dResult, ulDiePicked);
		pSummaryFile.WriteString(szCount);
		pSummaryFile.WriteString("\n");
		pSummaryFile.Close();

		// copy summary file to target folder
		if( szSummaryPath.CompareNoCase(szTargetPath)!=0 )
		{
			CopyFile(szSummaryPath, szTargetPath, FALSE);
		}
	}

	return TRUE;
}

CString CBinTable::CalculateWaferEndDownTime()
{
	CString szText;
	LONG lWaferStartYear, lWaferStartMonth, lWaferStartDay;
	LONG lWaferStartHour, lWaferStartMinute, lWaferStartSecond;

	LONG lWaferEndYear, lWaferEndMonth, lWaferEndDay;
	LONG lWaferEndHour, lWaferEndMinute, lWaferEndSecond;

	LONG lMapTime;
	int nCol;

	CString szMapStartDate, szMapStartTime, szMapRunTime, szMapDownTime, szMapTotalTime;
	CTime StartTime, EndTime;
	CTimeSpan TimeDiff;

	//Get Map start time & date
	szMapStartDate = (*m_psmfSRam)["WaferTable"]["Map Start Date"];
	nCol = szMapStartDate.Find('-');
	if( nCol==-1 )
	{
		return "00";
	}
	lWaferStartYear = atoi((LPCTSTR)szMapStartDate.Left(nCol));	//Get year

	szText = szMapStartDate.Mid(nCol+1);
	nCol = szText.Find('-');
	if( nCol==-1 )
	{
		return "00";
	}

	lWaferStartMonth = atoi((LPCTSTR)szText.Left(nCol));		//Get month
	lWaferStartDay = atoi((LPCTSTR)szText.Mid(nCol+1));			//Get day

	szMapStartTime = (*m_psmfSRam)["WaferTable"]["Map Start Time"];
	nCol = szMapStartTime.Find(':');
	if( nCol==-1 )
	{
		return "00";
	}

	lWaferStartHour = atoi((LPCTSTR)szMapStartTime.Left(nCol));	//Get hour
	szText = szMapStartTime.Mid(nCol+1);
	nCol = szText.Find(':');
	if( nCol==-1 )
	{
		return "00";
	}

	lWaferStartMinute = atoi((LPCTSTR)szText.Left(nCol));		//Get min	
	lWaferStartSecond = atoi((LPCTSTR)szText.Mid(nCol+1));		//Get sec

	szMapStartDate.Remove('-');
	szMapStartTime.Remove(':');

	//Get Wafer End Date & Time
	//Get Date
	lWaferEndYear = (*m_psmfSRam)["WaferTable"]["Wafer End Year"];
	lWaferEndMonth = (*m_psmfSRam)["WaferTable"]["Wafer End Month"];
	lWaferEndDay = (*m_psmfSRam)["WaferTable"]["Wafer End Day"];

	//Get Time
	lWaferEndHour = (*m_psmfSRam)["WaferTable"]["Wafer End Hour"];
	lWaferEndMinute = (*m_psmfSRam)["WaferTable"]["Wafer End Minute"];
	lWaferEndSecond = (*m_psmfSRam)["WaferTable"]["Wafer End Second"];


	//Get Wafer total time , down time & run time
	lMapTime	= (*m_psmfSRam)["WaferTable"]["Map Run Time"];
	StartTime	= CTime::CTime(lWaferStartYear, lWaferStartMonth, lWaferStartDay, lWaferStartHour, lWaferStartMinute, lWaferStartSecond, 0);
	EndTime		= CTime::CTime(lWaferEndYear, lWaferEndMonth, lWaferEndDay, lWaferEndHour, lWaferEndMinute, lWaferEndSecond, 0);
	TimeDiff	= EndTime - StartTime;

	szMapRunTime.Format("%d", lMapTime);
	szMapDownTime.Format("%d", TimeDiff.GetTotalSeconds() - (LONGLONG)lMapTime);

	return szMapDownTime;
}


VOID CBinTable::CheckSlotAssignment()
{
	int nConvID = 0;
	IPC_CServiceMessage stMsg;

	ULONG i, j;
	BOOL bIsAssignedWithSlot;


	typedef struct
	{
		BOOL bAssigned[MS_MAX_BIN+1];
	} BIN_ASS;

 	BIN_ASS stPos;


	nConvID = m_comClient.SendRequest(BIN_LOADER_STN, _T("IsSlotAssignedWithBlock"), stMsg);

	// Get the reply
	while (1)
	{
		if( m_comClient.ScanReplyForConvID(nConvID,10*1000) == TRUE ) // 10 second
		{
			m_comClient.ReadReplyForConvID(nConvID,stMsg);
			stMsg.GetMsg(sizeof(BIN_ASS), &stPos);
			break;
		}
		else
		{
			Sleep(10);
		}
	}

	for (i = 1; i <= m_oPhyBlkMain.GetNoOfBlk(); i++)
	{
		bIsAssignedWithSlot = stPos.bAssigned[i];

		for (j = 1; j <= m_oBinBlkMain.GetNoOfBlk(); j++)
		{
			if (m_oBinBlkMain.GrabPhyBlkId(j) == i)
			{
				m_oBinBlkMain.SetIsBinBlkAssignedWithSlot(j, bIsAssignedWithSlot);
			}
		}
	}
} //end CheckSlotAssignment


BOOL CBinTable::GetIsBinBlkSetup(ULONG ulBinBlkId)
{
	return m_oBinBlkMain.GrabIsSetup(ulBinBlkId);
} //end GetIsBinBlkSetup


UCHAR CBinTable::GetBinBlkGrade(ULONG ulBinBlkId)
{
	return m_oBinBlkMain.GrabGrade(ulBinBlkId);
} //end GetBinBlkGrade


BOOL CBinTable::GetIfGradeIsAssigned(UCHAR ucGrade)
{
	if( m_bDisableBT )
		return TRUE;
	return m_oBinBlkMain.GrabIsAssigned(ucGrade);
} //end GetIfGradeIsAssigned


unsigned long CBinTable::GetNoOfBinBlks()
{
	return m_oBinBlkMain.GetNoOfBlk();
} //end GetNoOfBinBlks


BOOL CBinTable::IfAllBinCountsAreCleared()
{
	ULONG i;

	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) > 0)
		{
			return FALSE;
		}
	}

	return TRUE;
} //end IfAllBinCountsAreCleared


BOOL CBinTable::IfNGBinCountsAreCleared()
{
	CUIntArray aulNGGradeBlockList;
	LONG lBinBlock;
	CNGGrade *pNGGrade = CNGGrade::Instance();
	
	pNGGrade->GetNGGradeBlockList(aulNGGradeBlockList);
	for (LONG i = 0; i < aulNGGradeBlockList.GetSize(); i++)
	{
		lBinBlock = (LONG)aulNGGradeBlockList[i];
		if (m_oBinBlkMain.GrabNVNoOfBondedDie(lBinBlock) > 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}


UCHAR CBinTable::IfAllBinCountsAreNotFull()
{
	UCHAR i=0;

	//v2.97T4
	BOOL bBinLoader = IsBLEnable();
	if (!bBinLoader)
		return 0;

	//v4.52A6	//CYOptics, Inari
	BOOL bCheckEmptyRowFcn = FALSE;
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetFeatureStatus(MS896A_FUNC_BINBLK_NEWLOT_EMPTYROW)) ||
		(pApp->GetCustomerName() == "CyOptics")		|| 
		(pApp->GetCustomerName() == "FiberOptics")	||		// = Dicon
		(pApp->GetCustomerName() == "Inari")	)	
	{
		bCheckEmptyRowFcn = TRUE;
	}

	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			if (bCheckEmptyRowFcn)	//v4.52A6
			{
				UCHAR ucGrade = m_oBinBlkMain.GrabGrade(i);
				ULONG ulIndex = m_oBinBlkMain.GrabNVCurrBondIndex(i);
				ULONG ulOrigIndex = ulIndex;

				if (!m_oBinBlkMain.CheckIfNeedToLeaveEmptyRow(i, ulIndex))
				{
					return i;		//BIN i FULL!
				}

				CString szLog;
				szLog.Format("BT PreStart - Bin #%d, Grade = %d, Index = %lu (%lu), InputCnt = %ld", 
					i, ucGrade, ulIndex, ulOrigIndex, m_oBinBlkMain.GrabInputCount(ucGrade));
CMSLogFileUtility::Instance()->MS_LogOperation(szLog);

				if (m_oBinBlkMain.GrabInputCount(ucGrade) <= ulIndex)
				{
					szLog.Format("BT PreStart - Bin #%d CheckFull fail", i);
					SetErrorMessage(szLog);
					return i;		//BIN i FULL!
				}
			}
			else
			{
				if (m_oBinBlkMain.GrabNVNoOfBondedDie(i) >= m_oBinBlkMain.GrabNoOfDiePerBlk(i))
					return i;
			}
		}
	}

	return 0;	// 0-> OK (no bin is FULL), can start bonding
}


VOID CBinTable::SetIsAllBinCntCleared(BOOL bIsAllBinCntCleared)
{
	m_bIsAllBinCntCleared = bIsAllBinCntCleared;
} //end SetIsAllBinCntCleared


VOID CBinTable::LoadGradeRankID()
{
	CStringArray szaHeader, szaData, szaDataSum;
	ULONG i;
	UCHAR ucGrade;

	m_GradeRankWrapper.Initialize("GradeRank");
	m_GradeRankWrapper.SetScrollBarSize(15);
	m_GradeRankWrapper.RemoveAllData();

	//Statistics Header
	szaHeader.Add("Name");
	m_GradeRankWrapper.SetHeader(8404992,10987412,"Grade", 15, szaHeader);

	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			ucGrade = m_oBinBlkMain.GrabGrade(i);
			szaData.Add(m_oBinBlkMain.GrabRankID(ucGrade));
	
			m_GradeRankWrapper.AddData(8404992, 10987412, ucGrade, 15, szaData);
			szaData.RemoveAll();
		}
	}

	szaDataSum.Add(" ");
	m_GradeRankWrapper.SetSum(8404992, 10987412, "", 15, szaDataSum);
	m_GradeRankWrapper.UpdateTotal();

} //end LoadGradeRankID


VOID CBinTable::UpdateGradeRankID()
{
	CStringArray szaData, szaDataSum;

	ULONG i;
	UCHAR ucGrade;

	for (i = 1; i <= m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		if (m_oBinBlkMain.GrabIsSetup(i))
		{
			ucGrade = m_oBinBlkMain.GrabGrade(i);
			szaData.Add(m_oBinBlkMain.GrabRankID(ucGrade));

			m_GradeRankWrapper.ModifyData(i-1, 8404992, 10987412, ucGrade, 15, szaData);
			szaData.RemoveAll();
		}
	}

	szaDataSum.Add(" ");
	m_GradeRankWrapper.SetSum(8404992, 10987412, "", 15, szaDataSum);
	m_GradeRankWrapper.UpdateTotal();

} //end UpdateGradeRankID


//Function for TCP/IP communication
BOOL CBinTable::SendWaferEndInfoToHost()
{
	if (m_bUseTcpIp == FALSE)
	{
		return TRUE;
	}

	LONG lWaferStartYear, lWaferStartMonth, lWaferStartDay;
	LONG lWaferStartHour, lWaferStartMinute, lWaferStartSecond;

	LONG lWaferEndYear, lWaferEndMonth, lWaferEndDay;
	LONG lWaferEndHour, lWaferEndMinute, lWaferEndSecond;

	LONG lMapTime;
	int nCol;

	CString szMapStartDate, szMapStartTime, szMapRunTime, szMapDownTime, szMapTotalTime;
	CString szWaferEndDate, szWaferEndHour, szWaferEndMinute, szWaferEndSecond, szWaferEndTime;
	CString szTempA, szTempB, szText;

	CTime StartTime, EndTime;
	CTimeSpan TimeDiff;


	//Get Map start time & date
	szMapStartDate = (*m_psmfSRam)["WaferTable"]["Map Start Date"];
	nCol = szMapStartDate.Find('-');
	if( nCol==-1 )
	{
		return FALSE;
	}
	lWaferStartYear = atoi((LPCTSTR)szMapStartDate.Left(nCol));	//Get year
	szText = szMapStartDate.Mid(nCol+1);
	nCol = szText.Find('-');
	if( nCol==-1 )
	{
		return FALSE;
	}
	lWaferStartMonth = atoi((LPCTSTR)szText.Left(nCol));		//Get month
	lWaferStartDay = atoi((LPCTSTR)szText.Mid(nCol+1));			//Get day

	szMapStartTime = (*m_psmfSRam)["WaferTable"]["Map Start Time"];
	nCol = szMapStartTime.Find(':');
	if( nCol==-1 )
	{
		return FALSE;
	}
	lWaferStartHour = atoi((LPCTSTR)szMapStartTime.Left(nCol));	//Get hour
	szText = szMapStartTime.Mid(nCol+1);
	nCol = szText.Find(':');
	if( nCol==-1 )
	{
		return FALSE;
	}
	lWaferStartMinute = atoi((LPCTSTR)szText.Left(nCol));		//Get min	
	lWaferStartSecond = atoi((LPCTSTR)szText.Mid(nCol+1));		//Get sec

	szMapStartDate.Remove('-');
	szMapStartTime.Remove(':');


	//Get Wafer End Date & Time
	//Get Date
	lWaferEndYear = (*m_psmfSRam)["WaferTable"]["Wafer End Year"];
	lWaferEndMonth = (*m_psmfSRam)["WaferTable"]["Wafer End Month"];

	szTempA.Format("%d", lWaferEndMonth);
	if (lWaferEndMonth < 10)
		szTempA = "0" + szTempA;

	lWaferEndDay = (*m_psmfSRam)["WaferTable"]["Wafer End Day"];
	szTempB.Format("%d", lWaferEndDay);
	if (lWaferEndDay < 10)
		szTempB = "0" + szTempB;

	szWaferEndDate.Format("%d", lWaferEndYear);
	szWaferEndDate = szWaferEndDate + szTempA + szTempB;

	//Get Time
	lWaferEndHour = (*m_psmfSRam)["WaferTable"]["Wafer End Hour"];
	szWaferEndHour.Format("%d", lWaferEndHour);
	if (lWaferEndHour < 10)
		szWaferEndHour = "0" + szWaferEndHour;

	lWaferEndMinute = (*m_psmfSRam)["WaferTable"]["Wafer End Minute"];
	szWaferEndMinute.Format("%d", lWaferEndMinute);
	if (lWaferEndMinute < 10)
		szWaferEndMinute = "0" + szWaferEndMinute;

	lWaferEndSecond = (*m_psmfSRam)["WaferTable"]["Wafer End Second"];
	szWaferEndSecond.Format("%d", lWaferEndSecond);
	if (lWaferEndSecond < 10)
		szWaferEndSecond = "0" + szWaferEndSecond;

	szWaferEndTime = szWaferEndHour + szWaferEndMinute + szWaferEndSecond;


	//Get Wafer total time , down time & run time
	lMapTime	= (*m_psmfSRam)["WaferTable"]["Map Run Time"];
	StartTime	= CTime::CTime(lWaferStartYear, lWaferStartMonth, lWaferStartDay, lWaferStartHour, lWaferStartMinute, lWaferStartSecond, 0);
	EndTime		= CTime::CTime(lWaferEndYear, lWaferEndMonth, lWaferEndDay, lWaferEndHour, lWaferEndMinute, lWaferEndSecond, 0);
	TimeDiff	= EndTime - StartTime;

	szMapRunTime.Format("%d", lMapTime);
	szMapTotalTime.Format("%d", TimeDiff.GetTotalSeconds());
	szMapDownTime.Format("%d", TimeDiff.GetTotalSeconds() - (LONGLONG)lMapTime);


	//Get Die counts from each Bin
	CUIntArray unaAvaGradeList;
	CUIntArray aulSelectedGradeList;
	int i;
	ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;
	ULONG ulMaxCount, ulMapTotal;
	CString szSelectedUnpick, szSelectedPicked;
	CString szMapUnSelected;
	CString szMainRankID;
	UCHAR ucSelectedGrade;

	szTempA = "";
	szTempB = "";
	ulMaxCount = 0;
	ulMapTotal = 0;


	//Get Selected grade list
	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);

	for (i = 1; i <= MS_MAX_BIN; i++)
	{
		m_WaferMapWrapper.GetStatistics((UCHAR)i + m_WaferMapWrapper.GetGradeOffset(), ulLeft, ulPicked, ulTotal);
		ulMapTotal += ulTotal;

		//Get Main RankID Grade
		if (ulLeft > ulMaxCount)
		{
			szMainRankID = m_oBinBlkMain.GrabRankID((UCHAR)i);
			if ( szMainRankID.IsEmpty() == TRUE )
			{
				szMainRankID = "0";
			}
			ulMaxCount = ulLeft;
		}

		if ( i <= 32 )
		{
			szText.Format("%d", ulPicked);
			szTempA = szTempA + ":" + szText;
			szTempB = szTempB + ":" + m_oBinBlkMain.GrabRankID((UCHAR)i);
		}
	}

	//OK, NG & Chip Count 
	szSelectedUnpick.Format("%d", m_WaferMapWrapper.GetSelectedTotalDice() - m_WaferMapWrapper.GetSelectedPickedDiceCount());
	szSelectedPicked.Format("%d", m_WaferMapWrapper.GetSelectedPickedDiceCount());
	szMapUnSelected.Format("%d", ulMapTotal - m_WaferMapWrapper.GetSelectedTotalDice());

	//if MainRankID = "0" then Chip Count = "0"
	if ( szMainRankID == "0" )
	{
		szMapUnSelected = "0";
	}

	//if Only grade 33 is selected, set OK & NG = 0
	if ( aulSelectedGradeList.GetSize() == 1 )
	{
		ucSelectedGrade = (UCHAR)(aulSelectedGradeList.GetAt(0) - m_WaferMapWrapper.GetGradeOffset());
		if ( ucSelectedGrade == 33 )
		{
			szSelectedPicked = "0";
			szSelectedUnpick = "0";
		}
	}


	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	if ( pAppMod != NULL )
	{
		//Send Message to TCP/IP Host comm
		szText =  szWaferEndDate + szWaferEndTime + ":"		//Finish Time
				+ szMapTotalTime + ":"						//Total Time
				+ szMapStartDate + szMapStartTime + ":"		//Start Time
				+ szMapRunTime + ":"						//Run Time
				+ szMapDownTime + ":"						//Down Time
				+ szMapUnSelected + ":"						//Chip Count (Map Total - Map Selected Total)
				+ szMainRankID + ":"						//Main RankID (Grade RankID of max remain die count's grade from all grade include grade 33)
				+ szSelectedPicked + ":"					//OK Count (Selected Bonded Die)
				+ szSelectedUnpick							//NG Count (Selected Un-Pick Die)
				+ szTempA + szTempB;						//Bin Chip Count & Rank ID
		
		return (pAppMod->SendHostWaferEnd(szText));
	}

	return TRUE;
}//end SendWaferEndInfoToHost


BOOL CBinTable::SendClearBinInfoToHost(ULONG ulPhyBlkId)
{
	if (m_bUseTcpIp == FALSE)
	{
		return TRUE;
	}

	UCHAR ucGrade;
	CString szGradeNo, szDieCount, szRankID, szMaxCount;
	CString szText;

	//Get Grade No
	ucGrade = m_oBinBlkMain.GrabGrade(ulPhyBlkId);
	szGradeNo.Format("%d", ucGrade);
	if (ucGrade < 10)
	{
		szGradeNo = "0" + szGradeNo;
	}

	//Get RankID
	szRankID = m_oBinBlkMain.GrabRankID(ucGrade);


	//Get Bonded die count & Max count
	szDieCount.Format("%d", m_oBinBlkMain.GrabNVNoOfBondedDie(ulPhyBlkId));
	szMaxCount.Format("%d", m_oBinBlkMain.GrabNoOfDiePerBlk(ulPhyBlkId));

	CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	if ( pAppMod != NULL )
	{
		szText = szGradeNo + ":" + szRankID + ":" + szDieCount + ":" + szMaxCount;
		return (pAppMod->SendHostClearBin(szText));
	}

	return TRUE;
}//end SendClearBinInfoToHost

BOOL CBinTable::LoadWaferDieCount()
{
	// Load Orginal Map Count From LastState msd
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;

	if (pUtl->LoadLastState("BT: LoadWaferDieCount") == TRUE)
	{
		psmf = pUtl->GetLastStateFile("BT: LoadWaferDieCount");
		if (psmf != NULL)	//v4.53A9
		{
			CPreBondEvent::m_nMapOrginalCount	= (*psmf)[WAFER_DATA][DIE_COUNT_BF_SCN];
			CPreBondEvent::m_ulMapOrgMinTotal	= (*psmf)[WAFER_DATA][DIE_MIN_COUNT_BF_SCN];
			CPreBondEvent::m_ulMapOrgUnselTotal	= (*psmf)[WAFER_DATA][DIE_UNSELECTED_COUNT_BF_SCN];
		}

		pUtl->CloseLastState("BT: LoadWaferDieCount");
		return TRUE;
	}

	return FALSE;
}


BOOL CBinTable::BT_LoadLastState()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	CString szTemp;

	if (pUtl->LoadLastState("BT: LoadLastState") == FALSE)
		return FALSE;

    psmf = pUtl->GetLastStateFile("BT: Load LastState");
	if (psmf != NULL)
	{
		m_szRankIDFilename			= (*psmf)[BT_TABLE_OPTION][BT_RANK_ID_FILENAME];
		m_lGenSummaryPeriodNum		= (LONG)(*psmf)[BT_TABLE_OPTION][BT_BIN_SUMMARY_FILE][BT_GEN_SUMMARY_PERIOD_NUM];
		m_szLastGenSummaryTime		= (*psmf)[BT_TABLE_OPTION][BT_BIN_SUMMARY_FILE][BT_LAST_GEN_SUMMARY_TIME];
		m_bNotGenerateWaferEndFile	= (BOOL)(LONG)(*psmf)[BT_TABLE_OPTION][BT_NOT_YET_GENERATE_WAFER_END_FILE];

		for (INT i=0; i<BT_SUMMARY_TIME_NO; i++)
		{
			szTemp.Format("Gen Time%d", i+1);
			m_szBinSummaryGenTime[i] = (*psmf)[BT_TABLE_OPTION][BT_BIN_SUMMARY_FILE][szTemp];	
		}
	}

	pUtl->CloseLastState("BT: Load LastState");
	return TRUE;
}

BOOL CBinTable::SaveLastState()
{
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();
    CStringMapFile  *psmf;
	CString szTemp;

	if (pUtl->LoadLastState("BT: SaveLastState") == FALSE)
		return FALSE;

    psmf = pUtl->GetLastStateFile("BT: SaveLastState");

	if (psmf != NULL)
	{
		(*psmf)[BT_TABLE_OPTION][BT_RANK_ID_FILENAME] = m_szRankIDFilename;
		(*psmf)[BT_TABLE_OPTION][BT_BIN_SUMMARY_FILE][BT_GEN_SUMMARY_PERIOD_NUM] = m_lGenSummaryPeriodNum;
		(*psmf)[BT_TABLE_OPTION][BT_BIN_SUMMARY_FILE][BT_LAST_GEN_SUMMARY_TIME] = m_szLastGenSummaryTime;
		(*psmf)[BT_TABLE_OPTION][BT_NOT_YET_GENERATE_WAFER_END_FILE] = m_bNotGenerateWaferEndFile;

		for (INT i=0; i<BT_SUMMARY_TIME_NO; i++)
		{
			szTemp.Format("Gen Time%d", i+1);
			(*psmf)[BT_TABLE_OPTION][BT_BIN_SUMMARY_FILE][szTemp] = m_szBinSummaryGenTime[i];	
		}
	}
	
	pUtl->UpdateLastState("BT: SaveLastState");
	pUtl->CloseLastState("BT: SaveLastState");
	return TRUE;
}

CString CBinTable::CalculateWaferTotalTime()
{
	CString szText;
	LONG lWaferStartYear, lWaferStartMonth, lWaferStartDay;
	LONG lWaferStartHour, lWaferStartMinute, lWaferStartSecond;

	LONG lWaferEndYear, lWaferEndMonth, lWaferEndDay;
	LONG lWaferEndHour, lWaferEndMinute, lWaferEndSecond;
	int nCol;
	CString szMapStartDate, szMapStartTime, szMapTotalTime;
	CTime StartTime, EndTime;
	CTimeSpan TimeDiff;

	//Get Map start time & date
	szMapStartDate = (*m_psmfSRam)["WaferTable"]["Map Start Date"];

	nCol = szMapStartDate.Find('-');
	if( nCol==-1 )
	{
		return "00";
	}
	lWaferStartYear = atoi((LPCTSTR)szMapStartDate.Left(nCol));	//Get year

	szText = szMapStartDate.Mid(nCol+1);
	nCol = szText.Find('-');
	if( nCol==-1 )
	{
		return "00";
	}
	lWaferStartMonth = atoi((LPCTSTR)szText.Left(nCol));		//Get month

	lWaferStartDay = atoi((LPCTSTR)szText.Mid(nCol+1));			//Get day

	szMapStartTime = (*m_psmfSRam)["WaferTable"]["Map Start Time"];

	nCol = szMapStartTime.Find(':');
	if( nCol==-1 )
	{
		return "00";
	}
	lWaferStartHour = atoi((LPCTSTR)szMapStartTime.Left(nCol));	//Get hour

	szText = szMapStartTime.Mid(nCol+1);
	nCol = szText.Find(':');
	if( nCol==-1 )
	{
		return "00";
	}
	lWaferStartMinute = atoi((LPCTSTR)szText.Left(nCol));		//Get min	

	lWaferStartSecond = atoi((LPCTSTR)szText.Mid(nCol+1));		//Get sec

	//Get Wafer End Date & Time
	//Get Date
	lWaferEndYear = (*m_psmfSRam)["WaferTable"]["Wafer End Year"];
	lWaferEndMonth = (*m_psmfSRam)["WaferTable"]["Wafer End Month"];
	lWaferEndDay = (*m_psmfSRam)["WaferTable"]["Wafer End Day"];
	//Get Time
	lWaferEndHour = (*m_psmfSRam)["WaferTable"]["Wafer End Hour"];
	lWaferEndMinute = (*m_psmfSRam)["WaferTable"]["Wafer End Minute"];
	lWaferEndSecond = (*m_psmfSRam)["WaferTable"]["Wafer End Second"];

	//Get Wafer total time , down time & run time
	StartTime	= CTime::CTime(lWaferStartYear, lWaferStartMonth, lWaferStartDay, lWaferStartHour, lWaferStartMinute, lWaferStartSecond, 0);
	EndTime		= CTime::CTime(lWaferEndYear, lWaferEndMonth, lWaferEndDay, lWaferEndHour, lWaferEndMinute, lWaferEndSecond, 0);
	TimeDiff	= EndTime - StartTime;

	szMapTotalTime.Format("%02ld:%02ld:%02ld", TimeDiff.GetTotalHours(), TimeDiff.GetMinutes(), TimeDiff.GetSeconds());

	return szMapTotalTime;
}	// to get wafer total handling time


INT CBinTable::OpWaferEndBackupMap()
{
    CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();

	if( m_bDisableBT )
		return gnOK;

	if( IsBurnIn() )
		return gnOK;

	if( IsPrescanEnable()==FALSE )
		return gnOK;

	if( IsPrescanning() )
		return gnOK;

	if( pUtl->GetPrescanDummyMap() || IsEnablePNP() )
		return gnOK;

	CString szEndBpMapPath		= (*m_psmfSRam)["MS896A"]["WaferEndBackupMapFilePath"];
	CString szBpMapExtension	= (*m_psmfSRam)["MS896A"]["BackupMapFileExt"];
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	CString szSpcSubPath = "";
	if( pApp->GetCustomerName()=="FATC" )
	{
		CTime theTime = CTime::GetCurrentTime();
		szSpcSubPath = theTime.Format("RPT_%y%m%d%H%M%S");
	}
	szEndBpMapPath = szEndBpMapPath + _T("\\") + szSpcSubPath;

	//Immediately quit fcn if backup map path not set up properly
	if (szEndBpMapPath.IsEmpty() || (szEndBpMapPath == ""))
	{
		SetErrorMessage("BT: Wafer End Backup Map fails with NULL backup path");				//v3.89
		return gnNOTOK;
	}

	//2. Check whether the path is existing 
	if ( CreateDirectory(szEndBpMapPath, NULL) == 0 )
	{
		if ( GetLastError() != ERROR_ALREADY_EXISTS )
		{
			SetErrorMessage("BT: Wafer End Backup Map fails with invalid backup path: " + szEndBpMapPath);	//v3.89
			SetAlert_Msg(IDS_BT_INVALID_DIRECTORY, szEndBpMapPath);
			return gnNOTOK;
		}
	}

	CMSLogFileUtility::Instance()->BT_BackUpMapLog("Start wafer end Backup");

	//1. Get Map file name details
	CString szFile = GetMapFileName();	// Get the original map filename and save a backup
	if ( szFile.IsEmpty() == TRUE )
	{
		SetErrorMessage("BT: wafer end Backup Map fails with NULL map file name");	//v3.89
		return gnNOTOK;
	}

	INT nResult = gnOK;
	INT nCol;
	CString szTemp, szMsg, szRptFile;
	CString szSourcePath, szSecFile, szSrcFile;

	nCol = szFile.ReverseFind(';');
	if ( nCol != -1 )
	{
		//Get 2nd file name & extension
		szSecFile = szFile.Mid(nCol+1);
		szFile = szFile.Left(nCol);
	}
	szSrcFile = szFile;

	nCol = szFile.ReverseFind('.');
	if ( nCol == -1 )
		szTemp = szFile;			// No file extension
	else
		szTemp = szFile.Left(nCol);
	nCol = szTemp.ReverseFind('\\');			// Remove the path
	if ( nCol != -1 )
	{
		//Get this map file original path
		szSourcePath = szTemp.Left(nCol);
		szTemp = szTemp.Mid(nCol+1);
	}

	CMSLogFileUtility::Instance()->BT_BackUpMapLog("wafer end Map FileName " + szFile);
	CMSLogFileUtility::Instance()->BT_BackUpMapLog("wafer ned Write File");

	//4. Copy local backup map into CTM map location
	TRY 
	{
		UpdateWaferMapHeader();
		CTime theTime = CTime::GetCurrentTime();

		if( m_bES100DisableSCN==FALSE )
		{
			szRptFile = szEndBpMapPath + _T("\\") + GetMapNameOnly() + _T(".SCN");
		}

		if( GetPsmEnable() )
		{
			szRptFile = szEndBpMapPath + _T("\\") + GetMapNameOnly() + _T(".PSM");
			szRptFile = szEndBpMapPath + _T("\\") + GetMapNameOnly() + _T(".") + szBpMapExtension;
			CopyFileWithQueue(szSrcFile, szRptFile, FALSE);
		}
		else if( m_WaferMapWrapper.IsMapValid() )
		{
			CString szASMBackupPath	= gszUSER_DIRECTORY + "\\MapFile\\BackupMap.txt";
			if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()==_T(""))
			{
				CString szLotNo = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];	
				szASMBackupPath	= gszUSER_DIRECTORY + _T("\\MapFile\\") + szLotNo + _T(".") + szBpMapExtension;
			}
			
			//CMSLogFileUtility::Instance()->BT_BackUpMapLog("Copy File to " + szRptFile);
			szRptFile = szEndBpMapPath + _T("\\") + GetMapNameOnly() + _T(".") + szBpMapExtension;

			//v4.40T6
			if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()==_T(""))
			{
				CString szLotNo = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];	
				szRptFile = szEndBpMapPath + _T("\\") + szLotNo + _T(".") + szBpMapExtension;
				CMSLogFileUtility::Instance()->BT_BackUpMapLog("Copy File (Nichia) to " + szRptFile);
			}

			if ( m_WaferMapWrapper.WriteMap(szASMBackupPath) == FALSE )
			{
				SetErrorMessage("BT: Backup Map fails with Write Map to: " + szASMBackupPath);	//v3.89
				szMsg.Format("1.%s", szASMBackupPath); 
				SetAlert_Msg(IDS_BT_CREATE_MAP_FAILED, szMsg);
				return gnNOTOK;
			}
			else
			{
				//v4.45T1
				CMSLogFileUtility::Instance()->BT_BackUpMapLog("Created ASM Backup RPT File at: " + szASMBackupPath);
			}

			BOOL bDeleteMap = FALSE;
			if (pApp->GetCustomerName() == CTM_NICHIA && pApp->GetProductLine()==_T(""))
			{
				bDeleteMap = TRUE;
			}
			if ( CopyFileWithQueue(szASMBackupPath, szRptFile, bDeleteMap) == FALSE )
			{
				szMsg.Format("RPT file copied FAIL to: %s\n", szRptFile); 
				CMSLogFileUtility::Instance()->BT_BackUpMapLog(szMsg);		//v4.45T1

				SetErrorMessage("BT: Backup Map fails with COPY to: " + szRptFile);		//v3.89
				szMsg.Format("2.%s", szRptFile); 
				SetAlert_Msg(IDS_BT_CREATE_MAP_FAILED, szMsg);
				return gnNOTOK;
			}
			else
			{
				szMsg.Format("RPT file copied OK to: %s", szRptFile); 
				CMSLogFileUtility::Instance()->BT_BackUpMapLog(szMsg);		//v4.45T1
			}

			szMsg.Format("WaferLabel-- WftEnd - Open another Bk map path:%d", pApp->m_bWaferLabelFile);
			CMSLogFileUtility::Instance()->WL_LogStatus(szMsg);
		}
		if (szSecFile.IsEmpty() != TRUE)
		{
			szSourcePath = szSourcePath + _T("\\") + szSecFile;
			szTemp = szEndBpMapPath + _T("\\") + szSecFile;
			CopyFileWithQueue(szSourcePath, szTemp, FALSE);
		}
	}
	CATCH (CFileException, e)
	{
		SetErrorMessage("BT: Backup Map fails with COPY Exception");				//v3.89
		szMsg.Format("2.%s", szRptFile); 
		SetAlert_Msg(IDS_BT_CREATE_MAP_FAILED, szMsg);
		return gnNOTOK;
	}
	END_CATCH


	CMSLogFileUtility::Instance()->BT_BackUpMapLog("Map is saved");
	return nResult;
}


VOID CBinTable::AOILoadMapGradesLegend(BOOL bCreate)
{
	int nFontSize = 17;

	if( bCreate )
	{
		m_BinTotalWrapper.Initialize("WaferStat");
		m_BinTotalWrapper.SetScrollBarSize(25);
		m_BinTotalWrapper.RemoveAllData();

		//Statistics Header
		CStringArray szaHeader;
		szaHeader.Add("Description");
		szaHeader.Add("Count");

		m_BinTotalWrapper.SetHeader(8404992, 10987412, "Grade", nFontSize, szaHeader);
	}

	if( m_WaferMapWrapper.IsMapValid()==FALSE )
	{
		return ;
	}
	CUIntArray aulAllGradesList;
	m_WaferMapWrapper.GetAvailableGradeList(aulAllGradesList);

	//Statistics Data
	ULONG ulGradesTotal = 0;
	ULONG ulLeft = 0, ulPicked = 0, ulTotal = 0;
	
	CString szData;
	CStringArray szaData;
	UCHAR ucGrade = 0;
	UCHAR ucOffset = m_WaferMapWrapper.GetGradeOffset();

	CStdioFile fBinFile;
	CString szFilename = "c:\\WinEagle\\system\\Algpar\\ledi\\AOIBinDesc.csv";
	CStringArray szaBinDesc;
	CUIntArray	 unaBinList;
	szaBinDesc.RemoveAll();
	unaBinList.RemoveAll();
	if( fBinFile.Open(szFilename, CFile::modeRead|CFile::shareDenyNone) )
	{
		CString szReading;
		CStringArray szaDataList;
		fBinFile.SeekToBegin();
		// get the fail counter limit
		//	BinId, Desc
		//	2, P Pad Defect
		if( fBinFile.ReadString(szReading) )
		{
			while( fBinFile.ReadString(szReading) )
			{
				szaDataList.RemoveAll();
				CUtility::Instance()->ParseRawData(szReading, szaDataList);
				if( szaDataList.GetSize()>=2 )
				{
					unaBinList.Add(atoi(szaDataList.GetAt(0)));
					szaBinDesc.Add(szaDataList.GetAt(1));
				}
			}
		}
		fBinFile.Close();
	}

	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	BOOL bZhongKe = FALSE;
	if( pApp->GetCustomerName()=="ZhongKe" && m_bPrescanSkipNgGrade )
	{
		bZhongKe = TRUE;
	}
	if( bZhongKe )
	{
		for(int j=m_ucPrescanDefectGrade; j<200; j++)
		{
			if( IsPrescanEnded()==FALSE )
			{
				break;
			}
			BOOL bFindGrade = FALSE;
			for (int k = 0; k < aulAllGradesList.GetSize(); k++)
			{
				ucGrade = aulAllGradesList.GetAt(k);
				if( ucGrade==(j+ucOffset) )
				{
					bFindGrade = TRUE;
					break;
				}
			}

			if( bFindGrade==FALSE )
			{
				continue;
			}

			szaData.RemoveAll();
			m_WaferMapWrapper.GetStatistics(ucGrade, ulLeft, ulPicked, ulTotal);

			if( ulTotal==0 )
				continue;

			ucGrade = ucGrade - ucOffset;

		//	description
			if( ucGrade>m_ucPrescanDefectGrade )
			{
				szData.Format(" Defect %d", ucGrade);
				for(int i=0; i<unaBinList.GetSize(); i++)
				{
					if( ucGrade==(m_ucPrescanDefectGrade+unaBinList.GetAt(i)) )
					{
						szData = szaBinDesc.GetAt(i);
						break;
					}
				}
			}
			else if( ucGrade==m_ucPrescanDefectGrade )
			{
				szData.Format(" Empty %d", ucGrade);
			}
			else
			{
				continue;
			}

			szaData.Add(szData);

			//	grade die count
			szData.Format("%d", ulTotal);
			szaData.Add(szData);
			ulGradesTotal += ulTotal;

			if( bCreate )
			{
				m_BinTotalWrapper.AddData(8404992, 10987412, ucGrade, nFontSize, szaData);
			}
			else
			{
				m_BinTotalWrapper.ModifyData(1, 8404992, 10987412, ucGrade, nFontSize, szaData);
			}

			szaData.RemoveAll();
		}
	}
	else	// Zhong Ke AOI
	{
		for (int i = 0; i < aulAllGradesList.GetSize(); i++)
		{
			szaData.RemoveAll();
			ucGrade = aulAllGradesList.GetAt(i);
			m_WaferMapWrapper.GetStatistics(ucGrade, ulLeft, ulPicked, ulTotal);

			ucGrade = ucGrade - ucOffset;

			//	description
			if( m_bPrescanSkipNgGrade && IsScanMapNgGrade(ucGrade) )
			{
				szData.Format(" Elec NG %d", ucGrade);
			}
			else if( ucGrade>=GetScanDefectGrade() )
			{
				szData.Format(" Defect %d", ucGrade);
				for(int i=0; i<unaBinList.GetSize(); i++)
				{
					if( ucGrade==(GetScanDefectGrade()+unaBinList.GetAt(i)) )
					{
						szData = szaBinDesc.GetAt(i);
						break;
					}
				}
			}
			else if( ucGrade>=GetScanBadcutGrade() )
			{
				szData.Format(" Badcut %d", ucGrade);
			}
			else
			{
				szData.Format(" Normal %d", ucGrade);
				continue;
			}
			szaData.Add(szData);

			//	grade die count
			szData.Format("%d", ulTotal);
			szaData.Add(szData);
			ulGradesTotal += ulTotal;

			if( bCreate )
			{
				m_BinTotalWrapper.AddData(8404992, 10987412, ucGrade, nFontSize, szaData);
			}
			else
			{
				m_BinTotalWrapper.ModifyData(1, 8404992, 10987412, ucGrade, nFontSize, szaData);
			}

			szaData.RemoveAll();
		}
	}

	CStringArray szaDataSum;
	szaDataSum.Add(" ");
	szData.Format("%d", ulGradesTotal);
	szaDataSum.Add(szData);

	m_BinTotalWrapper.SetSum(8404992, 10987412, "Total", nFontSize, szaDataSum);

	m_BinTotalWrapper.UpdateTotal();
} //end AOILoadMapGradesLegend

//OsramTrip 8/22
VOID CBinTable::LogCpkValue(ULONG ulBlkId, CString szFilePath, CString szBarcode)
{
	if (!IsSecsGemInit())
	{
		return;
	}

	double dSpcCpkX = (*m_psmfSRam)["MS896A"]["Spc CPK X"];
	double dSpcCpkY = (*m_psmfSRam)["MS896A"]["Spc CPK Y"];
	double dSpcCpkT = (*m_psmfSRam)["MS896A"]["Spc CPK T"];

	CString szMachineNo			= GetMachineNo();
	//CString szLotNumber		= (*m_psmfSRam)["MS896A"]["LotNumber"];
	CString szLotNumber			= (*m_psmfSRam)["MS896A"]["WaferLot Filename"];
	CString szFrameID			= szBarcode;

	CString szCpkLogPath		= szFilePath + "\\" + szMachineNo + "_cpk.csv";
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("LogCpkValue Path," + szCpkLogPath);
	CStdioFile Log;

	CString szTime;
	CTime theTime = CTime::GetCurrentTime();
	//szTime = theTime.Format("%Y%m%d%H%M%S");
	szTime = theTime.Format("%m/%d/%Y %I:%M %p");

	CString szContent;
	szContent.Format("%s, %s, %s, %.3f, %.3f, %.3f, %s",
		szLotNumber, szMachineNo, szFrameID, dSpcCpkX, dSpcCpkY, dSpcCpkT, szTime);
	CMSLogFileUtility::Instance()->BT_ClearBinCounterLog("LogCpkValue Content," + szContent);
	
	try
	{
		//OsramTrip 8/22
		SetGemValue(MS_SECS_SV_BT_BIN_NUMBER,	ulBlkId);
		SetGemValue(MS_SECS_SV_BPR_PB_CPK_INFO,	szContent);
		if (IsMSAutoLineMode())
			SendEvent(SG_CEID_PostBondCpkRpt_AUTOLINE);		//#8023
		else	
			SendEvent(SG_CEID_PostBondCpkRpt);				//#8021
	}
	catch(...)
	{
		DisplaySequence("LogCpkValue failure");
	}

	/*
	if (Log.Open(szCpkLogPath,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite) != FALSE)
	{
		Log.SeekToEnd();
		CString szLog;
		Log.WriteString(szContent);
		Log.WriteString("\n");
		Log.Close();
	}*/
}

BOOL CBinTable::GetFrameSlotID(ULONG ulBlkNo, LONG &lMagNo, LONG &lSlotNo)
{
	CStringMapFile *pSmfBLBC;
	BOOL bReturn = FALSE;
	CMSFileUtility  *pUtl = CMSFileUtility::Instance();

	if (pUtl->LoadConfigBLBarcode())
	{
		pSmfBLBC = pUtl->GetConfigFileBLBarcode();
		if (pSmfBLBC != NULL)
		{
			lMagNo = (*pSmfBLBC)[ulBlkNo]["Magazine"];
			lSlotNo = (*pSmfBLBC)[ulBlkNo]["SlotNo"];

			pUtl->CloseConfigBLBarcode();
			bReturn = TRUE;
		}
	}

	return bReturn;
}


BOOL CBinTable::WaferEndFileGenerating_Finisar()
{
	INT i=0, j=0;
	LONG lWaferEndYear, lWaferEndMonth, lWaferEndDay;
	LONG lWaferEndHour, lWaferEndMinute, lWaferEndSecond;
	LONG lMapTime;
	ULONG ulPrescanGoodNum = 0, ulPrescanDefectNum = 0, ulPrescanBadCutNum = 0, ulPrescanExtraNum = 0, ulPrescanEmptyNum = 0;
	CString szTotalNoOfBinBlks, szMapDate, szMapTime, szTempFilePath;
	CString szWaferEndDate, szWaferEndHour, szWaferEndMinute, szWaferEndSecond, szWaferEndTime;
	CString szEndMonth, szEndDay;
	CString szLotNo, szBinLotDir;
	CString szTitle, szContent, szWaferDownTime;
	CString szMachineNo, szOperatorID;
	CString szMapFileDate;
	CString szMapFilePath;
	CString szPKGFilename = "";
	ULONG ulDieState;
	BOOL bIfGenWaferEndFileOk = FALSE;
	CString szMacUpTime = "", szMacIdleTime = "", szMacSetupTime = "";
	CString szMacRunTime = "", szMacAlarmTime = "", szMacAssitTime = "", szMacDownTime = "";
	CString szPSMachineNo = "";
	CString szMinGradeCountGrade = "";
	CString szMinGradeCountDieNo = "";
	LONG ulMinGradeCount;
	CString szMinGradeCount;
	CString szPrescanGoodNum, szPrescanDefectNum, szPrescanBadCutNum, szPrescanExtraNum, szPrescanEmptyNum;
	CString szBIBatchId;
	CString szDiePitchX, szDiePitchY, szBinBlockDiePerRow, szBinBlockDiePerCol, szBinBlockFullCount;
	CString szMapEPID;
	CString szWaferMapDevice;
	CString szMapReSortingBin;
	CString szWaferMapDeviceNumber;
	CString szWaferMapAt;
	CString szWaferMapLotNo;
	CString szIsCentralized;
	CString szGradeOneInputCount;
	
	CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
	
	if (pApp == NULL)
	{
		return FALSE;
	}

	BOOL bIsNGGrade = FALSE;

	CString szUnSelectedGrade, szUnSelectedGradeUnPick;
	CString szUnSelectedTotal, szMinGradeDieTotal;
	ULONG	ulUnSelectedTotal = 0, ulMinGradeTotal = 0;
	CWaferEndFileInterface *pWaferEndFileInterface;

	szTitle.LoadString(HMB_BT_GEN_WAFEREND_FILE);

	if (m_WaferMapWrapper.IsMapValid() == FALSE)
	{
		SetNotGenerateWaferEndFile(FALSE);
		return TRUE;
	}

	if( pApp->GetCustomerName()=="FATC" )
	{
		OpWaferEndBackupMap();
	}

	szTempFilePath = m_szWaferEndPath;
	if (!IsPathExist(m_szWaferEndPath))
	{
		szContent.LoadString(HMB_BT_OUTPUT_INTO_BACKUP_FD);

		if (HmiMessage(szContent, szTitle, 103, 3) != glHMI_YES)
		{
			//HmiMessage("WaferEnd File is not generated!");
			SetAlert(IDS_BT_FAIL_TO_GEN_WEF);
			return FALSE;
		}

		szTempFilePath = gszUSER_DIRECTORY + "\\OutputFile\\WaferEnd";
	}

	szDiePitchX.Format("%d", _round(m_oBinBlkMain.GrabDDiePitchX(1)));
	szDiePitchY.Format("%d", _round(m_oBinBlkMain.GrabDDiePitchY(1)));
	szBinBlockDiePerRow.Format("%d",  m_oBinBlkMain.GrabNoOfDiePerRow(1));
	szBinBlockDiePerCol.Format("%d",  m_oBinBlkMain.GrabNoOfDiePerCol(1));

	if (m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1)) < m_oBinBlkMain.GrabNoOfDiePerBlk(1))
	{
		szBinBlockFullCount.Format("%d", m_oBinBlkMain.GrabInputCount(m_oBinBlkMain.GrabGrade(1)));
	}
	else
	{
		szBinBlockFullCount.Format("%d", m_oBinBlkMain.GrabNoOfDiePerBlk(1));
	}

	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Pitch X",		szDiePitchX);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Pitch Y",		szDiePitchY);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Per Row",		szBinBlockDiePerRow);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Die Per Col",		szBinBlockDiePerCol);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Block Full Bin Count",		szBinBlockFullCount);

	pApp->GetMachineTimes(szMacUpTime, szMacIdleTime, szMacSetupTime, szMacRunTime, 
		szMacAlarmTime, szMacAssitTime, szMacDownTime);

	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Up Time",		szMacUpTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Idle Time",	szMacIdleTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Setup Time",	szMacSetupTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Run Time",		szMacRunTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Alarm Time",	szMacAlarmTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Assit Time",	szMacAssitTime);
	m_GenerateWaferDatabase.SetGeneralInfoItems("MachineTime Down Time",	szMacDownTime);

	m_oBinBlkMain.GenBinBlkFile_Finisar();

	//Evervision : the point check list
	CString szCoorChain = (*m_psmfSRam)["MS896A"]["PointCheckPosition"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("Point Check Position", szCoorChain);

	//Map Base Num
	CString szMapBaseNum;
	szMapBaseNum.Format("%d",m_ulMapBaseNum);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Map Base Num", szMapBaseNum);  //4.51D9 CL_YZ

	//Evervision : Grade 1 count
	szGradeOneInputCount.Format("%d", m_oBinBlkMain.GrabInputCount(1));
	m_GenerateWaferDatabase.SetGeneralInfoItems("Grade One Input Count", szGradeOneInputCount);

	//BinBlock Info
	szTotalNoOfBinBlks.Format("%d", m_oBinBlkMain.GetNoOfBlk());
	szTotalNoOfBinBlks.Format("%d", 150);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Total No of Bin Blks", szTotalNoOfBinBlks);

	//Wafer end path
	//szTempFilePath.Replace("\\", "\\\\");
	m_GenerateWaferDatabase.SetGeneralInfoItems("Wafer End File Path", szTempFilePath);
	szMapFilePath = (*m_psmfSRam)["MS896A"]["MapFilePath"];

	m_GenerateWaferDatabase.SetGeneralInfoItems("Wafer Map Path", szMapFilePath);

	//Lot No & Lot directory
	szLotNo		= (*m_psmfSRam)["MS896A"]["LotNumber"];
	szBinLotDir = (*m_psmfSRam)["MS896A"]["Bin Lot Directory"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Lot No", szLotNo);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Bin Lot Directory", szBinLotDir);

	//Load Map time & date
	szMapDate = (*m_psmfSRam)["WaferTable"]["Load Map Date"];
	szMapTime = (*m_psmfSRam)["WaferTable"]["Load Map Time"];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Load Map Date", szMapDate);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Load Map Time", szMapTime);

	//Map start time & date
	szMapDate = (*m_psmfSRam)["WaferTable"]["Map Start Date"];
	szMapTime = (*m_psmfSRam)["WaferTable"]["Map Start Time"];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Start Date", szMapDate);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Start Time", szMapTime);

	//Map run time
	lMapTime = (*m_psmfSRam)["WaferTable"]["Map Run Time"];
	szMapTime.Format("%d", lMapTime);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Run Time", szMapTime);

	//Wafer End Date
	lWaferEndYear = (*m_psmfSRam)["WaferTable"]["Wafer End Year"];
	lWaferEndMonth = (*m_psmfSRam)["WaferTable"]["Wafer End Month"];

	szEndMonth.Format("%d", lWaferEndMonth);
	if (lWaferEndMonth < 10)
		szEndMonth = "0" + szEndMonth;

	lWaferEndDay = (*m_psmfSRam)["WaferTable"]["Wafer End Day"];
	szEndDay.Format("%d", lWaferEndDay);
	if (lWaferEndDay < 10)
		szEndDay = "0" + szEndDay;

	szWaferEndDate.Format("%d", lWaferEndYear);
	szWaferEndDate = szWaferEndDate + "-" + szEndMonth + "-" + szEndDay;

	//Wafer End Time
	lWaferEndHour = (*m_psmfSRam)["WaferTable"]["Wafer End Hour"];
	szWaferEndHour.Format("%d", lWaferEndHour);
	if (lWaferEndHour < 10)
		szWaferEndHour = "0" + szWaferEndHour;

	lWaferEndMinute = (*m_psmfSRam)["WaferTable"]["Wafer End Minute"];
	szWaferEndMinute.Format("%d", lWaferEndMinute);
	if (lWaferEndMinute < 10)
		szWaferEndMinute = "0" + szWaferEndMinute;

	lWaferEndSecond = (*m_psmfSRam)["WaferTable"]["Wafer End Second"];
	szWaferEndSecond.Format("%d", lWaferEndSecond);
	if (lWaferEndSecond < 10)
		szWaferEndSecond = "0" + szWaferEndSecond;

	szWaferEndTime = szWaferEndHour + ":" + szWaferEndMinute + ":" + szWaferEndSecond;

	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer End Date", szWaferEndDate);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer End Time", szWaferEndTime);

	for (i=0; i<BT_EXTRA_BIN_INFO_ITEM_NO; i++)
	{
		m_GenerateWaferDatabase.SetWaferSummaryItems(m_szaExtraBinInfoField[i], m_szaExtraBinInfo[i]);
	}

	ulMinGradeCount =	(LONG)(*m_psmfSRam)["Wafer Table Options"]["Min Grade Count"];
	szMinGradeCount.Format("%d", ulMinGradeCount);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Grade Count", szMinGradeCount);

	// write map header items
	szMapEPID = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_EPIID];
	szMapReSortingBin = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_RESORTINGBIN];

	CString szMapDevice = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE];
	szMapDevice.Remove('"');
	szMapDevice.Remove(',');
	//AfxMessageBox(szMapDevice , MB_SYSTEMMODAL);

	//For LatticePower
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Device", szMapDevice);

	m_GenerateWaferDatabase.SetWaferSummaryItems("WaferMap EPID", szMapEPID);
	m_GenerateWaferDatabase.SetWaferSummaryItems("WaferMap ResortingBin", szMapReSortingBin);

	szWaferMapLotNo = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];
	m_GenerateWaferDatabase.SetWaferSummaryItems("WaferMap LotNo", szWaferMapLotNo);
	
	//Die counts
	CUIntArray unaAvaGradeList,aulSelectedGradeList, aulNGGradeList;
	BOOL bInSelectedList = FALSE;
	ULONG ulLeft, ulPicked, ulTotal;
	ULONG ulMapGoodLeft, ulMapBondedTotal, ulMapScnTotal = 0, ulMapOrgTotal = 0, ulMapFailGradesTotal = 0;
	CString szMapGoodLeft, szMapBondedTotal, szMapScanTotal, szMapOrigTotal, szMapFailGradesTotal = "";
	CString szAvailableGradeList = "", szAvailableGradeTotal = "", szAvailableGradePick = "", szAvailableGradeLeft = "";
	CString szGrade, szTotal, szPick, szLeft;
	CString szRankIdName, szRankID, szRankBlkId;
	CString szBlkNo;

	ulMapGoodLeft = 0;
	ulMapBondedTotal = 0;

	m_WaferMapWrapper.GetSelectedGradeList(aulSelectedGradeList);
	m_WaferMapWrapper.GetAvailableGradeList(unaAvaGradeList);

	ULONG ulMaxRow, ulMaxCol, ulRow, ulCol;
	ulMaxRow = ulMaxCol = 0;
	if (!WM_CWaferMap::Instance()->GetWaferMapDimension(ulMaxRow, ulMaxCol))
	{
		return FALSE;
	}

	for (i = 0; i < unaAvaGradeList.GetSize(); i++)
	{
		bInSelectedList = FALSE;
		m_WaferMapWrapper.GetStatistics(unaAvaGradeList.GetAt(i), ulLeft, ulPicked, ulTotal);

		for (j=0; j< aulSelectedGradeList.GetSize(); j++)
		{
			if (unaAvaGradeList.GetAt(i) == aulSelectedGradeList.GetAt(j))
			{
				bInSelectedList = TRUE;
				break;
			}
		}

		for (ULONG ulRow = 0; ulRow < ulMaxRow; ulRow++)
		{
			for (ULONG ulCol = 0; ulCol < ulMaxCol; ulCol++)
			{
				UCHAR ucOrigGrade = CMS896AStn::m_WaferMapWrapper.GetOriginalDieInformation(ulRow, ulCol);
				if( ucOrigGrade==unaAvaGradeList.GetAt(i) )
				{
					ulMapOrgTotal++;
				}
				ulDieState = GetMapDieState(ulRow, ulCol);
				if (ulDieState < WT_MAP_DIESTATE_MISSING)
				{
					continue;
				}
				if( unaAvaGradeList.GetAt(i)==ucOrigGrade )
				{
					ulPicked--;
				}
				if( ulDieState<WT_MAP_DIESTATE_DEFECT )
				{
					CString szOriginalGrade = m_WaferMapWrapper.GetOriginalGradeString(unaAvaGradeList.GetAt(i));
					if( szOriginalGrade.Find("Fail")==-1 )
					{
						ulMapGoodLeft++;
					}
				}
			}
		}
		ulLeft = ulTotal - ulPicked;

		szGrade.Format("%d",unaAvaGradeList.GetAt(i)- m_WaferMapWrapper.GetGradeOffset());
		szPick.Format("%d", ulPicked);
		szLeft.Format("%d", ulLeft);
		szTotal.Format("%d",ulTotal);

		szAvailableGradeList = szAvailableGradeList + szGrade + ",";
		szAvailableGradeLeft = szAvailableGradeLeft + szLeft + ",";
		szAvailableGradePick = szAvailableGradePick + szPick + ",";
		szAvailableGradeTotal= szAvailableGradeTotal + szTotal + ",";

		if (bInSelectedList == FALSE)
		{
			// Previous Version use left, V3.26 updated to use Total no
			CString szDieTotal,szUnSelected;
			szUnSelected.Format("%d",unaAvaGradeList.GetAt(i)- m_WaferMapWrapper.GetGradeOffset());
			szUnSelectedGrade = szUnSelectedGrade + szUnSelected+ ",";
			szDieTotal.Format("%d",ulTotal);
			szUnSelectedGradeUnPick = szUnSelectedGradeUnPick + szDieTotal + ",";
			aulNGGradeList.Add(unaAvaGradeList.GetAt(i));
			ulUnSelectedTotal += ulTotal;
		}
		if ( (((ULONG) atoi(szMinGradeCount)) >= ulTotal) )
		{
			CString szMinTotal,szMinGrade;
			szMinGrade.Format("%d",unaAvaGradeList.GetAt(i)- m_WaferMapWrapper.GetGradeOffset());
			szMinGradeCountGrade = szMinGradeCountGrade + szMinGrade + ",";
			szMinTotal.Format("%d",ulTotal);
			szMinGradeCountDieNo = szMinGradeCountDieNo + szMinTotal + ",";
			ulMinGradeTotal += ulTotal;
		}

	//	ulMapGoodLeft += ulLeft;
		ulMapBondedTotal += ulPicked;
		ulMapScnTotal += ulTotal;
		CString szOriginalGrade = m_WaferMapWrapper.GetOriginalGradeString(unaAvaGradeList.GetAt(i));
		if( szOriginalGrade.Find("Fail")!=-1 )
		{
			ulMapFailGradesTotal += ulTotal;
		}
	}
	szUnSelectedTotal.Format("%d", ulUnSelectedTotal);
	szMinGradeDieTotal.Format("%d", ulMinGradeTotal);

	//Remove comma at the back
	m_GenerateWaferDatabase.SetWaferSummaryItems("UnSelected Grade",			szUnSelectedGrade);
	m_GenerateWaferDatabase.SetWaferSummaryItems("UnSelected Grade Die Left",	szUnSelectedGradeUnPick);
	m_GenerateWaferDatabase.SetWaferSummaryItems("UnSelected Grade Die Total",	szUnSelectedTotal);

	// available grade list information
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade",				szAvailableGradeList);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade Die Left",	szAvailableGradeLeft);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade Die Pick",	szAvailableGradePick);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Available Grade Die Total",	szAvailableGradeTotal);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Count Grade",		szMinGradeCountGrade);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Count Die Left",	szMinGradeCountDieNo);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Min Count Die Total",	szMinGradeDieTotal);

	szMapGoodLeft.Format("%d",			ulMapGoodLeft);
	szMapBondedTotal.Format("%d",		ulMapBondedTotal);
	szMapScanTotal.Format("%d",			ulMapScnTotal);
	szMapOrigTotal.Format("%d",			ulMapOrgTotal);
	szMapFailGradesTotal.Format("%d",	ulMapFailGradesTotal);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Die",				szMapScanTotal);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Sorted Die",		szMapBondedTotal);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Unsorted Die",	szMapGoodLeft);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total Fail device",			szMapFailGradesTotal);

	CString szSubBlk = "0";
	if( CMS896AApp::m_bEnableSubBin )
	{
		szSubBlk.Format("%d", m_nNoOfSubBlk);
	}
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Edge Die", szSubBlk);

	for (INT i = 1; i <= (LONG)m_oBinBlkMain.GetNoOfBlk(); i++)
	{
		szRankID = m_oBinBlkMain.GrabRankID(m_oBinBlkMain.GrabGrade(i));
		szBlkNo.Format("%d", i);

		if (i==1)
		{
			szRankBlkId = szBlkNo;
			szRankIdName = szRankID;
		}
		else
		{
			szRankBlkId = szRankBlkId + "," + szBlkNo;
			szRankIdName = szRankIdName + "," + szRankID;
		}
	}

	m_GenerateWaferDatabase.SetWaferSummaryItems("RankID BlkId",	szRankBlkId);
	m_GenerateWaferDatabase.SetWaferSummaryItems("RankID Name",		szRankIdName);
	
	//Map Filename
	m_GenerateWaferDatabase.SetWaferSummaryItems("Original Map Filename", GetMapFileName());

	m_GenerateWaferDatabase.SetWaferSummaryItems("Map Filename", GetMapNameOnly());
	
	szMapFileDate = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_DATE_IN_MAP_FILE];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Date In Map File", szMapFileDate);

	//Output Filename
	CString szClrBinOutPath = m_szOutputFilePath;
	if( m_bAutoUploadOutputFile )
		szClrBinOutPath = gszOUTPUT_FILE_TEMP_PATH;
	m_GenerateWaferDatabase.SetWaferSummaryItems("Output Filename", szClrBinOutPath);

	//Is centralized bond area ?
	//szIsCentralized = m_bCentralizedBondArea?"YES":"NO";
	szIsCentralized = m_oBinBlkMain.GrabIsCentralizedBondArea(1)? "YES":"NO";
	//HmiMessage("program: " + szIsCentralized);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Is Centralized Bond Area", szIsCentralized);

	//Wafer Id
	CString szWaferId;

	CMS896AStn::m_WaferMapWrapper.GetWaferID(szWaferId);
	szWaferId.Remove('\t');
	szWaferId.Remove(',');
	szWaferId.Remove('\"');
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Id", szWaferId);

	CString szWaferName;

	szWaferName = GetMapFileName();
	m_GenerateWaferDatabase.SetWaferSummaryItems("MapFileName", szWaferName);

	//Wafer map lot ID
	CString szMapLotID, szLotNumber;
	szMapLotID = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTID];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map LotID", szMapLotID);

	szLotNumber = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_LOTNUMBER];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map LotNo", szLotNumber);

	szWaferMapDeviceNumber = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_DEVICE];
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map DeviceNumber", szWaferMapDeviceNumber);

	szWaferMapAt = (*m_psmfSRam)["MS896A"][WT_MAP_HEADER][WT_MAP_HEADER_AT];
	szWaferMapAt.Replace('\n','*');
	m_GenerateWaferDatabase.SetWaferSummaryItems("Map At", szWaferMapAt);

	//Machine No	//v4.40T7
	szMachineNo = GetMachineNo();
	m_GenerateWaferDatabase.SetGeneralInfoItems("Machine No", szMachineNo);

	// Operator ID
	szOperatorID = (*m_psmfSRam)["MS896A"]["Operator Id"];

	CString szWaferLotFilePath = (*m_psmfSRam)["WaferTable"]["WaferLotInfoFilePath"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("WaferLotInfoFilePath", szWaferLotFilePath);

	m_GenerateWaferDatabase.SetGeneralInfoItems("Wafer End Summary Path", m_szWaferEndSummaryPath);
	m_GenerateWaferDatabase.SetGeneralInfoItems("Operator ID", szOperatorID);

	szPKGFilename = (*m_psmfSRam)["MS896A"]["PKG Filename"];
	m_GenerateWaferDatabase.SetGeneralInfoItems("PKG Filename", szPKGFilename);

	CMSPrescanUtility  *pUtl = CMSPrescanUtility::Instance();
	if (pUtl != NULL)
	{
		szPSMachineNo = pUtl->GetPsmMapMachineNo();
		m_GenerateWaferDatabase.SetGeneralInfoItems("PS MachineNo", szPSMachineNo);
	}

	
	//Chip, bad-cut, defect, ink, empty, rotate die counts
	//ULONG ulMissingDie;
	ULONG ulTotalNoOfChipDie, ulTotalNoOfBadCutDie, ulTotalNoOfDefectDie;
	ULONG ulTotalNoOfInkDie, ulTotalNoOfEmptyDie, ulTotalNoOfRotateDie;
	ULONG ulTotalNoOfMissingDie;
	ULONG ulTotalNoOfUpLookFailDie = 0;
	ULONG ulPSTotalNoOfEmptyDie, ulPSTotalNoOfDefectDie, ulPSTotalNoOfBadCutDie;
	ULONG ulPSNGTotalNoOfEmptyDie, ulPSNGTotalNoOfDefectDie, ulPSNGTotalNoOfBadCutDie;
	ULONG ulScnLowScoreTotal = 0;

	ulTotalNoOfInkDie = 0;
	ulTotalNoOfEmptyDie = 0;
	ulTotalNoOfRotateDie = 0;
	ulTotalNoOfChipDie = 0;
	ulTotalNoOfBadCutDie = 0;
	ulTotalNoOfDefectDie = 0;
	ulTotalNoOfMissingDie = 0;

	ulPSTotalNoOfEmptyDie = 0;
	ulPSTotalNoOfDefectDie = 0;
	ulPSTotalNoOfBadCutDie = 0;

	ulPSNGTotalNoOfEmptyDie = 0;
	ulPSNGTotalNoOfDefectDie = 0;
	ulPSNGTotalNoOfBadCutDie = 0;


	for (ulRow = 0; ulRow < ulMaxRow; ulRow++)
	{
		for (ulCol = 0; ulCol < ulMaxCol; ulCol++)
		{
			ulDieState = GetMapDieState(ulRow, ulCol);
			
			LONG encX = 0, encY = 0;
			double dDieAngle = 0;
			CString szDieScore = "0";
			USHORT usDieState = 0;
			if( GetScanInfo(ulRow, ulCol, encX, encY, dDieAngle, szDieScore, usDieState) )
			{
			}
			// Missing Die Check Since it has no grade so check first
			if( ulDieState>WT_MAP_DS_BH_UPLOOK_FAIL )
			{
				ulTotalNoOfUpLookFailDie++;
				continue;
			}

			if (ulDieState > WT_MAP_DIESTATE_MISSING)
			{
				ulTotalNoOfMissingDie++;
				continue;
			}

			if (ulDieState == WT_MAP_DIESTATE_DEFECT)
				ulTotalNoOfDefectDie++;
			else if (ulDieState == WT_MAP_DIESTATE_INK)
				ulTotalNoOfInkDie++;
			else if (ulDieState == WT_MAP_DIESTATE_CHIP)
				ulTotalNoOfChipDie++;
			else if (ulDieState == WT_MAP_DIESTATE_BADCUT)
				ulTotalNoOfBadCutDie++;
			else if (ulDieState == WT_MAP_DIESTATE_EMPTY)
				ulTotalNoOfEmptyDie++;
			else if (ulDieState == WT_MAP_DIESTATE_ROTATE)
				ulTotalNoOfRotateDie++;
			else if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_EMPTY || szDieScore=="-1" )
				ulPSTotalNoOfEmptyDie++;
			else if (ulDieState == WT_MAP_DS_UNPICK_REGRAB_EMPTY)
				ulScnLowScoreTotal++;
			else if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_DEFECT)
				ulPSTotalNoOfDefectDie++;
			else if (ulDieState == WT_MAP_DIESTATE_UNPICK_SCAN_BADCUT)
				ulPSTotalNoOfBadCutDie++;
		}
	}

	CString szTotalNoOfChipDie, szTotalNoOfBadCutDie, szTotalNoOfDefectDie, szTotalNoOfMissingDie;
	CString szTotalNoOfUpLookFail = "0";
	//CString	szMissingDie;
	CString szTotalNoOfInkDie, szTotalNoOfEmptyDie, szTotalNoOfRotateDie;
	CString szPSTotalNoOfEmptyDie, szScnLowScoreTotal, szPSTotalNoOfDefectDie, szPSTotalNoOfBadCutDie;
	CString szPSNGTotalNoOfEmptyDie, szPSNGTotalNoOfDefectDie, szPSNGTotalNoOfBadCutDie;
	CString szMDCycleCountBHZ1, szMDCycleCountBHZ2, szColletJamBHZ1, szColletJamBHZ2;
	ULONG ulMDCycleCountBHZ1, ulMDCycleCountBHZ2, ulColletJamBHZ1, ulColletJamBHZ2;
	//4.54T15 Wafer End Gen MD and CJ of BHZ1 and BHZ2
	ulMDCycleCountBHZ1 = (*m_psmfSRam)["BondHead"]["MissingDieBHZ1"]; 
	ulMDCycleCountBHZ2 = (*m_psmfSRam)["BondHead"]["MissingDieBHZ2"]; 
	ulColletJamBHZ1	   = (*m_psmfSRam)["BondHead"]["ColletJamBHZ1"];
	ulColletJamBHZ2	   = (*m_psmfSRam)["BondHead"]["ColletJamBHZ2"];
	
	szTotalNoOfChipDie.Format("%d",		ulTotalNoOfChipDie);
	szTotalNoOfBadCutDie.Format("%d",	ulTotalNoOfBadCutDie);
	szTotalNoOfDefectDie.Format("%d",	ulTotalNoOfDefectDie);
	szTotalNoOfInkDie.Format("%d",		ulTotalNoOfInkDie);
	szTotalNoOfEmptyDie.Format("%d",	ulTotalNoOfEmptyDie);
	szTotalNoOfRotateDie.Format("%d",	ulTotalNoOfRotateDie);
	szTotalNoOfMissingDie.Format("%d",	ulTotalNoOfMissingDie);
	szTotalNoOfUpLookFail.Format("%d",  ulTotalNoOfUpLookFailDie);
	szMDCycleCountBHZ1.Format("%d",		ulMDCycleCountBHZ1);
	szMDCycleCountBHZ2.Format("%d",		ulMDCycleCountBHZ2);
	szColletJamBHZ1.Format("%d",		ulColletJamBHZ1);
	szColletJamBHZ2.Format("%d",		ulColletJamBHZ2);
	
	szPSTotalNoOfEmptyDie.Format("%d", ulPSTotalNoOfEmptyDie);
	szScnLowScoreTotal.Format("%d", ulScnLowScoreTotal);
	szPSTotalNoOfDefectDie.Format("%d", ulPSTotalNoOfDefectDie);
	szPSTotalNoOfBadCutDie.Format("%d", ulPSTotalNoOfBadCutDie);

	szPSNGTotalNoOfEmptyDie.Format("%d", ulPSNGTotalNoOfEmptyDie);
	szPSNGTotalNoOfDefectDie.Format("%d", ulPSNGTotalNoOfDefectDie);
	szPSNGTotalNoOfBadCutDie.Format("%d", ulPSNGTotalNoOfBadCutDie);

	// Using the wafer map missing die count instead of
	//ulMissingDie = (ULONG)(*m_psmfSRam)["BondHead"]["CurrentWafer"]["MissingDie"];
	//szMissingDie.Format("%d", ulMissingDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Chip Die",	szTotalNoOfChipDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Bad-cut Die", szTotalNoOfBadCutDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Defect Die",	szTotalNoOfDefectDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Missing Die", szTotalNoOfMissingDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Up Look Fail", szTotalNoOfUpLookFail);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Ink Die",		szTotalNoOfInkDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Empty Die",	szTotalNoOfEmptyDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Rotate Die",	szTotalNoOfRotateDie);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Missing Die Cycle Count of BHZ1", szMDCycleCountBHZ1); //4.54T15
	m_GenerateWaferDatabase.SetWaferSummaryItems("Missing Die Cycle Count of BHZ2", szMDCycleCountBHZ2);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Collet Jam Cycle Count of BHZ1", szColletJamBHZ1);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Collet Jam Cycle Count of BHZ2", szColletJamBHZ2);


	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Total No of Empty Die", szPSTotalNoOfEmptyDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Low score device", szScnLowScoreTotal);
	
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Total No of Defect Die", szPSTotalNoOfDefectDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Total No of Bad-cut Die",	 szPSTotalNoOfBadCutDie);

	m_GenerateWaferDatabase.SetWaferSummaryItems("PS NG Total No of Empty Die", szPSNGTotalNoOfEmptyDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS NG Total No of Defect Die", szPSNGTotalNoOfDefectDie);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS NG Total No of Bad-cut Die", szPSNGTotalNoOfBadCutDie);

	ulPrescanGoodNum   = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_GOOD];
    ulPrescanDefectNum = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_DEFECT];
	ulPrescanBadCutNum = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_BADCUT];
	ulPrescanExtraNum  = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EXTRA];
    ulPrescanEmptyNum  = (ULONG)(LONG)(*m_psmfSRam)["WaferTable"][PRESCAN_RESULT_NUM_EMPTY];

	szPrescanGoodNum.Format("%d",	ulPrescanGoodNum);
	szPrescanDefectNum.Format("%d", ulPrescanDefectNum);
	szPrescanBadCutNum.Format("%d", ulPrescanBadCutNum);
	szPrescanExtraNum.Format("%d",	ulPrescanExtraNum);
	szPrescanEmptyNum.Format("%d",	ulPrescanEmptyNum);



	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Good Num",		szPrescanGoodNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Defect Num",	szPrescanDefectNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS BadCut Num",	szPrescanBadCutNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Extra Num",	szPrescanExtraNum);
	m_GenerateWaferDatabase.SetWaferSummaryItems("PS Empty Num",	szPrescanEmptyNum);
	
	szWaferDownTime = CalculateWaferEndDownTime();
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Down Time", szWaferDownTime);

	CString szWaferTotalTime;
	szWaferTotalTime = CalculateWaferTotalTime();
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Total Time", szWaferTotalTime);

	//Write the no of die in the wafer before wafer scn
	CString szOrginalCount;
	szOrginalCount.Format("%d",CPreBondEvent::m_nMapOrginalCount);
	m_GenerateWaferDatabase.SetWaferSummaryItems("Total No of Die before SCN",szOrginalCount);

	ULONG ulSelectedTotal;
	CString szSelectedTotal;

	ulSelectedTotal = m_WaferMapWrapper.GetSelectedTotalDice();
	szSelectedTotal.Format("%d", ulSelectedTotal);

	m_GenerateWaferDatabase.SetWaferSummaryItems("Selected Total", szSelectedTotal);

	// Prescan End
	CString szPrescanEndMonth;
	CString szPrescanEndDay;
	CString szPrescanEndHour;
	CString szPrescanEndMinute;
	CString szPrescanEndSecond;
	szPrescanEndMonth.Format("%d", m_stScanEndCTime.GetMonth());
	szPrescanEndDay.Format("%d", m_stScanEndCTime.GetDay());
	szPrescanEndHour.Format("%d", m_stScanEndCTime.GetHour());
	szPrescanEndMinute.Format("%d", m_stScanEndCTime.GetMinute());
	szPrescanEndSecond.Format("%d", m_stScanEndCTime.GetSecond());

	// Prescan Start
	CString szPrescanStartMonth;
	CString szPrescanStartDay;
	CString szPrescanStartHour;
	CString szPrescanStartMinute;
	CString szPrescanStartSecond;
	szPrescanStartMonth.Format("%d", m_stScanStartCTime.GetMonth());
	szPrescanStartDay.Format("%d", m_stScanStartCTime.GetDay());
	szPrescanStartHour.Format("%d", m_stScanStartCTime.GetHour());
	szPrescanStartMinute.Format("%d", m_stScanStartCTime.GetMinute());
	szPrescanStartSecond.Format("%d", m_stScanStartCTime.GetSecond());

	// Scan Time
	CString szWaferScanTime;
	CTimeSpan stScanTimeDiff = m_stScanStartCTime - m_stScanEndCTime;
	szWaferScanTime.Format("%d:%d:%d",stScanTimeDiff.GetHours(),stScanTimeDiff.GetMinutes(),stScanTimeDiff.GetSeconds());
	m_GenerateWaferDatabase.SetWaferSummaryItems("Wafer Scan Time", szWaferDownTime);

	CMS896AStn::m_GenerateWaferDatabase.GenEntireDatabase();

	CMS896AStn::m_QueryWaferDatabase.LoadDatabase();

	CString szWaferEndFileFormat = (*m_psmfSRam)["MS896A"]["WaferEndFileFormat"];
	pWaferEndFileInterface = CWaferEndFactoryMap::GetInstance()->CreateInstance(szWaferEndFileFormat);

	if (pWaferEndFileInterface == NULL)
	{
		//HmiMessage("Error: Wafer End File Object can't be created");
		SetAlert(IDS_BT_FAIL_TO_CREATE_OBJ);
	}
	else
	{
		bIfGenWaferEndFileOk = pWaferEndFileInterface->GenWaferEndFile(&CMS896AStn::m_QueryWaferDatabase);

		pWaferEndFileInterface->CleanUp();
		delete pWaferEndFileInterface;
	}

	CMS896AStn::m_QueryWaferDatabase.CleanUp();
	CMS896AStn::m_GenerateWaferDatabase.CleanUp();

	if (bIfGenWaferEndFileOk == FALSE)
	{
		SetAlert(IDS_BT_GEN_WAFERENDFILE_FAILED);
		return FALSE;
	}


	SetNotGenerateWaferEndFile(FALSE);

	SetAlarmLog("begin to show summary message");
	CString szTemp;

	CStdioFile cfTempEndFile;
	if (cfTempEndFile.Open(_T("c:\\mapsorter\\exe\\WaferEndTempLogFile.txt"), 
		CFile::modeRead|CFile::shareExclusive|CFile::typeText) )
	{
		CString szLineData;
		while( cfTempEndFile.ReadString(szLineData)!=NULL )
		{
			SetAlarmLog(szLineData);
			szContent += (szLineData + "\n");
		}
		cfTempEndFile.Close();
	}
	SetAlarmLog("ato show summary message");
	HmiMessage_Red_Back(szContent, "Summary", glHMI_MBX_CLOSE, glHMI_ALIGN_LEFT, 3600000, glHMI_MSG_MODAL, 0, 650, 880);
	SetAlarmLog("Finisar wafer end summary\n");
	SetAlarmLog(szContent + "\n");
	BOOL bIsEnableWaferLoader = (BOOL)(LONG)(*m_psmfSRam)["WaferLoaderStn"]["Enabled"];

	if (bIsEnableWaferLoader == FALSE)
		return TRUE;

	// will cause dead lock
	LONG lSlotNo;
	lSlotNo = (*m_psmfSRam)["WaferTable"]["WT1LoadSlot"];
	
	SetGemValue("WaferId", szWaferId);								//3003
	SetGemValue("SlotNo", lSlotNo);									//3004
	SetGemValue("CassettePos", "WFR");								//3005
	SetGemValue("TotalBinableDieOnWafer", ulSelectedTotal);			//3040
	SetGemValue("TotalDieBinedFromWafer", ulMapBondedTotal);		//3041
	SetGemValue("MapLoadTime",				szMapTime);				//3007		//v4.36T1	//3E EquipMgr
    SendEvent(SG_CEID_WaferCompleted, TRUE);						//8017

	return TRUE;
} //end WaferEnd FileGenerating


//ChangeLight request 1: Record current time into file
BOOL CBinTable::RecordNewBinTimeToFile(CString szFileName)
{
	CStdioFile cfOutputFile;
	CFileException e;

	TRY
	{
		if (cfOutputFile.Open(szFileName, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite, &e) == FALSE)
		{
			return FALSE;
		}
		else
		{
			CTime HeaderTime = CTime::GetCurrentTime();
			CString	szDate = HeaderTime.Format("%Y-%m-%d");
			CString szTime = HeaderTime.Format("%H:%M:%S");

			cfOutputFile.SeekToBegin();
			cfOutputFile.WriteString(szDate + "," + szTime);
			cfOutputFile.WriteString("\n");
			cfOutputFile.Close();

			return TRUE;
		}
	}
	CATCH (CFileException, e)
	{
	} END_CATCH

	return FALSE;
}


BOOL CBinTable::CheckIfNeedToRestoreNVRAMData()
{
	//v4.65A3
	LONG lNVBlock=0, lNVCount=0;
	if (m_oBinBlkMain.IsNVTempDataNeedToRestoreAtWarmStart(lNVBlock, lNVCount))
	{
		CString szError;
		szError.Format("BT ERROR: detected NV RAM has Bin #%ld data to be recovered (DieCount = %ld)",
							lNVBlock, lNVCount);
		CMSLogFileUtility::Instance()->MS_LogOperation(szError);
		HmiMessage_Red_Yellow(szError, "BT WARM-START");

		//v4.67
		CMS896AApp* pApp = (CMS896AApp*) AfxGetApp();
		CTime ctDateTime = CTime::GetCurrentTime();
		CString szDate = ctDateTime.Format("_%Y%m%d_%H%M%S.bin");
		BOOL bStatus = pApp->ExportNVRAMDataToTxtFile("c:\\MapSorter\\Exe\\NVRAM" + szDate);
		if (!bStatus)
		{
			SetErrorMessage("BT: CheckIfNeedToRestoreNVRAMData - FAIL to export NVRAM to file - c:\\MapSorter\\Exe\\NVRAM" + szDate);
		}

		if (!m_WaferMapWrapper.IsMapValid())
		{
			szError = _T("BT WarmStart: MAP Data is NOT valid to recover NVRAM Data!");
			CMSLogFileUtility::Instance()->MS_LogOperation(szError);
			HmiMessage_Red_Yellow(szError, "BT WARM-START");
		}
		else
		{
			if (!m_oBinBlkMain.RestoreNVTempFileDataAtWarmStart())
			{
				szError = _T("BT WarmStart: Fail to restore NVRam backup data!");
				CMSLogFileUtility::Instance()->MS_LogOperation(szError);
				HmiMessage_Red_Yellow(szError, "BT WARM-START");
			}
			else
			{
				szError = _T("NV RAM data restored and TEMP file is updated; NVRAM will be reset.");
				CMSLogFileUtility::Instance()->MS_LogOperation(szError);
				HmiMessage(szError, "BT WARM-START");
				m_oBinBlkMain.ResetNVTempFileData();
			}
		}
	}

	return TRUE;
}