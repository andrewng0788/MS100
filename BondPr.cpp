/////////////////////////////////////////////////////////////////
// BondPr.cpp : interface of the CBondPr class
//
//	Description:
//		MS896A Mapping Die Sorter
//
//	Date:		Friday, June 04, 2004
//	Revision:	1.00
//
//	By:			Kelvin Chak
//				AAA Software Group
//
//	Copyright @ ASM Technology Singapore Pte. Ltd., 2004.
//	ALL rights reserved.
//
/////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MarkConstant.h"
#include "MS896A.h"
#include "MS896A_Constant.h"
#include "BondPr.h"
#include "WaferPr.h"
#include "prGeneral.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CBondPr, CMS896AStn)

CBondPr::CBondPr(): m_evBPRLatched(FALSE, TRUE, "BprLatchedEvt"),
					m_evBhTReady(FALSE, TRUE, "BhTReadyEvt"),
					m_evBhTReadyForBPR(FALSE, TRUE, "BhTReadyForBPREvt"),
					m_evBTStable(FALSE, TRUE, "BtStableEvt"),
					m_evBTReSrchDie(FALSE, TRUE, "BTReSchDie"),				//v4.xx
					m_evBPRPostBondDone(FALSE, TRUE, "BPRPostBondDone"),		//v4.40T6
					m_evWPRGrabImageReady(FALSE, TRUE, "WPRGrabImageReadyEvt"),
					m_evBhReadyForBPRHwTrigger(FALSE, TRUE, "BhReadyForBPRHwTrigger")
{
	m_szEventPrefix			= "BPR";

	InitVariable();
}

CBondPr::~CBondPr()
{
}

BOOL CBondPr::InitInstance()
{
	CMS896AStn::InitInstance();
	(*m_psmfSRam)["BondPrStn"]["Enabled"] = TRUE;
	if( m_bDisableBT )
		(*m_psmfSRam)["BondPrStn"]["Enabled"] = FALSE;


	//v4.58A3
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if (pApp->GetFeatureStatus(MS896A_FUNC_VISION_BH_UPLOOK_PR))
	{
		m_bDisplayUnitInUm		= TRUE;			//v4.58A2
	}
	else if (IsMS90())
	{
		m_bDisplayUnitInUm		= TRUE;			//v4.59A16	//Renesas MS90
	}

	InitSPCControl();
#ifdef NU_MOTION
	InitSPCControl_Arm1();
	InitSPCControl_Arm2();
#endif

	GetAxisInformation();				//v4.37T11
	
	m_dBTXRes = GetChannelResolution(MS896A_CFG_CH_BINTABLE_X);		//v4.59A30
	m_dBTYRes = GetChannelResolution(MS896A_CFG_CH_BINTABLE_Y);		//v4.59A30

	m_pPrZoomSensorMode = new CPrZoomSensorMode(MS899_BOND_CAM_ID, MS899_BOND_CAM_SEND_ID, MS899_BOND_CAM_RECV_ID, m_pPrGeneral);
	m_bHardwareReady = TRUE;
	return TRUE;
}

INT CBondPr::ExitInstance()
{
	// Temporary comment: Save data will make search die fail
	// SavePrData();		// Save data to file before exit
	return CMS896AStn::ExitInstance();
}



PR_UBYTE CBondPr::GetPostBondPRSenderID()
{
	return (PR_UBYTE)MS899_BOND_CAM_SEND_ID;
}

PR_UBYTE CBondPr::GetPostBondPRReceiverID()
{
	return (PR_UBYTE)MS899_BOND_CAM_RECV_ID;
}


BOOL CBondPr::InitData()
{
	
	// since all len zoom and foucs are different in different machine, so clear the pr record
	// HUGA request to just load the image but not the record

	if (IsLoadingPortablePKGFile() == TRUE && IsLoadingPKGFile() == TRUE)
	{
		LoadBondPrOption();
		SavePrData(FALSE);

		if (CMS896AApp::m_bErasePortablePackageFileImage == TRUE)
		{
			FreeAllPrRecords();
			// save data again to update display
			SavePrData(FALSE);
		}
	}
	else if (CMS896AStn::m_bNoPRRecordForPKGFile == TRUE && IsLoadingPKGFile() == TRUE)
	{
		FreeAllPrRecords();
		// save data again to update display
		SavePrData(FALSE);
	}
	else
	{
		CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
		BOOL bIsManualLoadPkgFile = pApp->IsManualLoadPkgFile();

		LoadBondPrOption();
		LoadPrData();

		if (bIsManualLoadPkgFile)	//v4.53A22
		{
			//Save new PR record IDs (by auto-assign method) from PR_DownloadRecordProcessCmd 
			//	into MSD file after Manual-Load PKG;
			//SavePrData(FALSE);
			SaveRecordID();
		}
	}

	//aNichia001
	CMS896AApp *pApp = (CMS896AApp*) AfxGetApp();
	if ((pApp->GetCustomerName() == "Nichia") || (pApp->GetCustomerName() == "Citizen"))
	{
		m_bDisplayUnitInUm	= TRUE;
	}

	BPR_LiveViewZoom(BPR_GetNmlZoom());
	return TRUE;
}

VOID CBondPr::ClearData()
{
	LoadPrData();
	FreeAllPrRecords();			// Free all PR records
}


/////////////////////////////////////////////////////////////////
//Update Functions
/////////////////////////////////////////////////////////////////
VOID CBondPr::UpdateOutput()
{
	if (!m_fHardware)
		return;

	if ((m_qState == UN_INITIALIZE_Q) || (m_qState == DE_INITIAL_Q))
		return;
}

VOID CBondPr::UpdateProfile()
{
	//Add Your Code Here
	//CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	//CSingleLock slLock(&pAppMod->m_csMachine);

	m_bUpdateProfile = FALSE;

	try
	{
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	//slLock.Unlock();
}

VOID CBondPr::UpdatePosition()
{
	//Add Your Code Here
	//CMS896AApp* pAppMod = dynamic_cast<CMS896AApp*> (m_pModule);
	//CSingleLock slLock(&pAppMod->m_csDevice);
	
	m_bUpdatePosition = FALSE;

	try
	{
	}
	catch(CAsmException e)
	{
		DisplayException(e);
	}

	//slLock.Unlock();
}


