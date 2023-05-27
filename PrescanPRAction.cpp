#include "stdafx.h"

#include "PrescanPRAction.h"
#include "MS896A_Constant.h"
#include "FileUtil.h"

CDefectScanPRAction::CDefectScanPRAction()
: m_goodChangeGrade(false),m_defectiveChangeGrade(false),m_emptyChangeGrade(false), m_badCutChangeGrade(false),
m_goodGrade(0), m_defectiveGrade(0), m_emptyGrade(0), m_badCutGrade(0)
{

}

CDefectScanPRAction::~CDefectScanPRAction()
{
}


int
CDefectScanPRAction::GoodDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col)
{
	return 1;
}

int
CDefectScanPRAction::DefectiveDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col)
{
	return 1;
}

int
CDefectScanPRAction::EmptyDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col)
{
	return 1;
}

int
CDefectScanPRAction::BadCutDieAction(CWaferMapWrapper& wafermapWrapper, int row, int col)
{
	return 1;
}

bool CDefectScanPRAction::IsGoodAction()
{
	return m_goodChangeGrade;
}

void
CDefectScanPRAction::SetGoodGrade(bool state, unsigned char grade)
{
	m_goodChangeGrade = state;
	m_goodGrade = grade;
}

void
CDefectScanPRAction::SetDefectiveGrade(bool state, unsigned char grade)
{
	m_defectiveChangeGrade = state;
	m_defectiveGrade = grade;
}

void
CDefectScanPRAction::SetEmptyGrade(bool state, unsigned char grade)
{
	m_emptyChangeGrade = state;
	m_emptyGrade = grade;
}

void
CDefectScanPRAction::SetBadCutGrade(bool state, unsigned char grade)
{
	m_badCutChangeGrade = state;
	m_badCutGrade = grade;
}
