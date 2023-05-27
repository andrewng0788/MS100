#pragma once
#include "prheader.h"

// prescan relative code
/***************************************************************
* Inputs:
*       WPARAM: point to pr search command struct (PR_SRCH_DIE_CMD* or PR_INSP_CMD*)
*       LPARAM: ignore (0)
* Effect:
*	Reset all data
***************************************************************/
#define UWM_SETUP_SEARCH_COMMAND_MSG _T("UWM_SETUP_SEARCH_COMMAND-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")
#define UWM_SETUP_SEARCH_COMMAND2_MSG _T("UWM_SETUP_SEARCH_COMMAND2-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")

/***************************************************************
* Inputs:
*       WPARAM: pointer to SearchAreaInfo struct
*       LPARAM: ignored (0)
* Effect:
*	PR controller signal SearchThread to start working
***************************************************************/
#define UWM_GALLERY_START_SEARCH_MSG _T("UWM_GALLERY_START_SEARCH-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")

/***************************************************************
* Inputs:
*       WPARAM: pointer to SearchAreaInfo struct
*       LPARAM: pointer to PRGallerySrchReplies struct
* Effect:
*	SearchThread signal pr controller of search done
***************************************************************/
#define UWM_GALLERY_SEARCH_DONE_MSG _T("UWM_GALLERY_SEARCH_DONE-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")

/***************************************************************
* Inputs:
*       WPARAM: ignore (0)
*       LPARAM: ignore (0)
* Effect:
*	signal program end
***************************************************************/
#define UWM_SHUTDOWN_MSG _T("UWM_SHUTDOWN-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")

/***************************************************************
* Inputs:
*       WPARAM: ignore (0)
*       LPARAM: ignore (0)
* Effect:
*	Paurse PR search thread
***************************************************************/
#define UWM_PAUSE_PR_MSG _T("UWM_PAUSE_PR-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")

/***************************************************************
* Inputs:
*       WPARAM: ignore (0)
*       LPARAM: ignore (0)
* Effect:
*	Resume PR search thread
***************************************************************/
#define UWM_RESUME_PR_MSG _T("UWM_RESUME_PR-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")

#define UWM_LOAD_CONFIG_MSG			_T("UWM_LOAD_CONFIG-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")
#define UWM_DISPLAY_PR_WINDOWS_MSG	_T("UWM_DISPLAY_PR_WINDOWS-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")
#define UWM_ERASE_LOG_MSG			_T("UWM_ERASE_LOG-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")
#define UWM_SAVE_LOG_MSG			_T("UWM_SAVE_LOG-{C068B4B5-DEA5-490f-B563-0B1243D9CA44}")


#define DECLARE_USER_MESSAGE(name) \
	static const UINT name = ::RegisterWindowMessage(name##_MSG);



typedef struct SearchAreaInfo
{
	int				m_threadID;
	int				m_stationID;
	ULONG			m_imageID;
	ULONG			m_PrGrabID;
	// sub image search to handle die over 100
	bool			m_bSubImgEnable;
	LONG			m_lSubImgTgtIdx;
	PR_WORD			m_wSubImgULX;
	PR_WORD			m_wSubImgULY;
	PR_WORD			m_wSubImgLRX;
	PR_WORD			m_wSubImgLRY;
// AOI PR TEST
	double			m_dInspPrX;
	double			m_dInspPrY;
// AOI PR TEST
} SearchAreaInfo;

