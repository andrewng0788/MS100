#ifndef PREBONDEVENT_H
#define PREBONDEVENT_H

#pragma once
//================================================================
// #include and #define
//================================================================
#include "WaferMapWrapper.h"

#ifndef MS50_64BIT
	#include "WSDecDll.h"
#endif

#include "FileUtil.h"

// For Die Count before Wafer Scan update
#define WAFER_DATA "WaferData"

//================================================================
// struct, enum definition
//================================================================
typedef struct
{
	long x;
	long y;
	long t;
} PhyPos;

//================================================================
// Class definition
//================================================================
class CMS896AApp;
class CBinTable;
class CWaferTable;
class CBondHead;

class CPreBondEvent : public CWaferMapEvent
{
public:
	// ===========================================================
	// Constructor and destructor
	CPreBondEvent(void);
	virtual ~CPreBondEvent(void);

	// ===========================================================
	// Get Functions
	virtual BOOL Prepare(const unsigned long ulEventID);
	virtual BOOL Execute(const unsigned long ulEventID, const BOOL bResult);
	void SetApp(CMS896AApp* pApp);
	void SetBondHead(CBondHead* pBondHead);
	void SetBinTable(CBinTable* pBinTable);
	void SetWaferTable(CWaferTable* pWaferTable);
	void SetWaferMapWrapper(CWaferMapWrapper* pWaferMapWrapper);
	BOOL SelectGradeToPick();
	BOOL SetIgnoreGrade();			//v3.00T1
	void CopyMapFile(BOOL bScnLoaded);

	static ULONG	m_nMapOrginalCount;
	static ULONG	m_ulMapOrgUnselTotal;
	static ULONG	m_ulMapOrgMinTotal;

protected:
	CMS896AApp*			m_pApp;	
	CBondHead*			m_pBondHead;	
	CBinTable*			m_pBinTable;	
	CWaferTable*		m_pWaferTable;	
	CWaferMapWrapper*	m_pWaferMapWrapper;

private:
	CString		strTemp;
	BOOL		m_bDebug;

#ifndef MS50_64BIT
	WSDecDll	m_cWSDecode;
#endif

	BOOL CalculateMapOriginalCount();
	BOOL GetWS896Info(CString strFileName);
	BOOL GetNoRptPsmInfo(CString strFileName);

};

//================================================================
// Class definition
//================================================================

class CPreBondEventBin : public CWaferMapEvent
{
public:
	// ===========================================================
	// Constructor and destructor
	CPreBondEventBin(void);
	virtual ~CPreBondEventBin(void);

	// ===========================================================
	// Get Functions
	virtual BOOL Prepare(const unsigned long ulEventID);
	virtual BOOL Execute(const unsigned long ulEventID, const BOOL bResult);

	// ===========================================================
	// Set Functions
	void SetMapWrapperBin(CWaferMapWrapper* pMapWrapper);
	void SetWaferTableBin(CWaferTable* pWaferTable);

	static BOOL	m_bMapLoaded;

protected:

	CWaferMapWrapper*	m_pMapWrapperBin;
	CWaferTable*		m_pWaferTableBin;	

private:

};

#endif  // PREBONDEVENT_H

//================================================================
// End of file PreBondEvent.h
//================================================================
