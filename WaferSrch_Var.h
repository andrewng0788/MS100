
#ifndef WAFER_SRCH_VAR_H
#define WAFER_SRCH_VAR_H

typedef struct 
{
	BOOL		bShowPRStatus;
	BOOL		bNormalDie;
	LONG		lRefDieNo;
	BOOL		bDisableBackupAlign;
} SRCH_TYPE;

typedef struct {
	BOOL bStatus;
	BOOL bGoodDie;
	BOOL bFullDie;
	LONG lRefDieNo;
	LONG lX;
	LONG lY;
} REF_TYPE;

#endif