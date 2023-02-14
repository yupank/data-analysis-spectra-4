// Spectra4.h : main header file for the SPECTRA4 application
//

#if !defined(AFX_SPECTRA4_H__4CDFB005_BFCE_11D5_A8B3_0002B3090409__INCLUDED_)
#define AFX_SPECTRA4_H__4CDFB005_BFCE_11D5_A8B3_0002B3090409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif


#include "resource.h"       // main symbols


/////////////////////////////////////////////////////////////////////////////
// CSpectra4App:
// See Spectra4.cpp for the implementation of this class
//

class CSpectra4App : public CWinApp
{
public:
	CSpectra4App();
	CMultiDocTemplate* pDocTemplate;
	void ReadPULSE();
	void ReadPULSEMac();
	void ReadSCRData();
	void ReadASCII();
	void ReadConfig();
	void OnFitTrace();
	void OnFitRand();
	void OnSpecRand();
	void OnToolsOptions();
	void OnToolsHist();
	void OnToolsTimeCourse();
	void OnToolsPowerSpec();
	void OnToolsAutoCorr();
	void OnToolsMinal();
	void OnToolsExtrapolate();
	void OnLotLoad();
	void OnLotAnalyze();
	void OnLotPredict();
	void OnLotRateHst();
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectra4App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSpectra4App)
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg	void OnCloseAll();
	afx_msg	void OnStartGrid();
	afx_msg	void OnAxisApply();
	
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRA4_H__4CDFB005_BFCE_11D5_A8B3_0002B3090409__INCLUDED_)
