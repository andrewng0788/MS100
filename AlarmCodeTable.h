//================================================================
// #include and #define
//================================================================
#ifndef __ALARM_CODE_TABLE_H
#define __ALARM_CODE_TABLE_H
#pragma once

class CAlarmCode : public CObject
{
public:
	CAlarmCode();
	VOID SetAlarmCode(const CString szAlarmCode, const CString szAlarmDescription);
	BOOL IsSame(const CString szAlarmCode);

private:
	CString m_szAlarmCode;
	CString m_szAlarmDescription;
	
};

//================================================================
// Class definition
//================================================================
class CIgnoreAlarmCodeTable : public CObject
{
	#define IGNORE_ALARM_CODE_DATABASE "C:\\MapSorter\\sys\\IgnoreAlarmCodeTable.csv"
public:
	CIgnoreAlarmCodeTable();
	~CIgnoreAlarmCodeTable();
	static CIgnoreAlarmCodeTable *Instance();

public:
	VOID CreateIgnoreAlarmCodeTable();
	BOOL IsInIgnoreAlarmCodeTable(CString szAlarmCode);
	VOID ClearIgnoreAlarmCodeTable();

private:
	CObArray m_IgnoreAlarmCodeTables;
public:
	static CIgnoreAlarmCodeTable *m_pInstance;
};
#endif