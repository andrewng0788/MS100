/////////////////////////////////////////////////////////////////
// NichiaMgntSys.h : Header file for Nichia Management Sub-system Class
/////////////////////////////////////////////////////////////////
#pragma once

#include "StringMapFile.h"


//=================================
// DEFINE macros (General)
//=================================



//////////////////////////////////////////////////////////////
// CLASS Definitions
//////////////////////////////////////////////////////////////

class CMSSemitekMgntSubSystem: public CObject
{
public:
	
	//======================================================================
	// Class Constructor/Destructor Fcns
	//======================================================================
	CMSSemitekMgntSubSystem();
	~CMSSemitekMgntSubSystem();

	VOID CleanUp();
	BOOL LoadData(BOOL bLoadPRMFile=FALSE);
	BOOL SaveData(VOID);

	//======================================================================
	// Public GET/SET Fcns
	//======================================================================
	VOID SetEnabled(CONST BOOL bEnable);
	BOOL IsEnabled();
	CString GetLastError();

	//======================================================================
	// Public Supporting Fcns
	BOOL GenerateEmptyFrameFile(ULONG ulBlk,ULONG ulBondedDie,int nSerial, CString szBarcode);


protected:

	//======================================================================
	// Other Utility functions
	//======================================================================



private:

	//======================================================================
	// Critical sections
	//======================================================================
	CStringMapFile		*m_psmfSRam;

	BOOL		m_bIsEnabled;
	CString		m_szErrMsg;

public:

	//======================================================================
	// Class Attributes
	//======================================================================


};	


