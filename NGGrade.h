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

#ifndef _NG_GRADE_H_
#define _NG_GRADE_H_

//For NG Grade
#define		BPR_PB_HAVE_NG_GRADE		"Have NG Grade"
#define		BPR_PB_NG_GRADE_148_SET		"NG Grade 148 Set"
#define		BPR_PB_NG_GRADE_149_SET		"NG Grade 149 Set"
#define		BPR_PB_NG_GRADE_150_SET		"NG Grade 150 Set"
#define		BPR_PB_NG_GRADE_Other_SET	"NG Grade Other Set"
#define		BPR_PB_NG_GRADE_Other_LIST	"NG Grade Other List"
#define		BPR_SRH_NG_NDIE_PASSSCORE	"NG Grade Pass score"

#include "StringMapFile.h"


class CNGGrade : public CObject
{
public:
	CNGGrade(); //Constructor
	~CNGGrade(); //Deconstructor
	static CNGGrade *Instance();

	VOID WriteData(CMapElement *pElement);
	VOID ReadData(CMapElement *pElement);

	BOOL IsHaveNGGrade();
	BOOL IsNGBlock(const ULONG ulBlkInUse);
	VOID GetNGGradeBlockList(CUIntArray &aulNGGradeBlockList);
	LONG GetCurrentNGGradeBlock();
	BOOL IsInNGGradeBlockList(const LONG lBlockID, CUIntArray &aulNGGradeBlockList);
public:
	BOOL	m_bNGGrade148;
	BOOL	m_bNGGrade149;
	BOOL	m_bNGGrade150;
	BOOL	m_bNGGradeOther;
	BOOL	m_bHaveNGGrade;
	CString	m_szNGGradeOtherList;
	LONG	m_lSrchNGDieScore;

	CUIntArray m_aulNGGradeBlockList;
private:
	static CNGGrade *m_pInstance;
};
#endif