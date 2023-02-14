#if !defined(AFX_SPECTRAGRIDDLG_H__999CE1C0_C53C_11D5_A8B9_0002B3090409__INCLUDED_)
#define AFX_SPECTRAGRIDDLG_H__999CE1C0_C53C_11D5_A8B9_0002B3090409__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpectraGridDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg dialog

class CSpectraGridDlg : public CDialog
{
// Construction
public:
	CSpectraGridDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpectraGridDlg)
	enum { IDD = IDD_SPECGRID_DLG };
	CString	m_csFitNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectraGridDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpectraGridDlg)
	afx_msg void OnSelchangeFitMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRAGRIDDLG_H__999CE1C0_C53C_11D5_A8B9_0002B3090409__INCLUDED_)
