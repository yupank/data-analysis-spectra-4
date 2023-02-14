#if !defined(AFX_SPECTRAGRIDDLG_H__999CE1C0_C53C_11D5_A8B9_0002B3090409__INCLUDED_)
#define AFX_SPECTRAGRIDDLG_H__999CE1C0_C53C_11D5_A8B9_0002B3090409__INCLUDED_


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SpectraGridDlg.h : header file
//
#include "GridCtrl.h"
class TSpectrum;
class CGridCtrl;
class CSpectra4View;


/// //////////////////////////////////////////
// CSpectraDlgBar

class CSpectraDlgBar : public CDialogBar
{
public:
	float yBeg,yEnd, xBeg, xEnd;
	float m_Beg;
	float m_End;
	int m_xSwitch;
	int m_ySwitch;

	CSpectraDlgBar();   // standard constructor
	BOOL Create(CWnd* pParentWnd);
	void Apply();
	void ReScale(int);
	void GetAxis();
	
// Dialog Data
	//{{AFX_DATA(CSpectraDlgBar)
	enum { IDD = IDD_DIALOGBAR, ID = ID_COMP_DIALOGBAR };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	//CGridCtrl m_GridCtrl;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectraDlgBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	//}}AFX_VIRTUAL


// Implementation
protected:
	

	// Generated message map functions
	//{{AFX_MSG(CSpectraDlgBar)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnAxisSwitch();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////
// CSpectra4Dlg
class CSpectra4Dlg:public CDialog
{
public:
		enum { IDD = IDD_DLG_AXES};
		TSpectrum* pSpectrum;
		CView* m_pView;
		CSpectra4Dlg(CWnd* pParent);   // standard constructor
		CSpectra4Dlg(CView* pView);
				
		BOOL Create();
};
/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg dialog

class CSpectraGridDlg : public CDialog
{
// Construction
public:
	
	CSpectraGridDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL Create();
// Dialog Data
	//{{AFX_DATA(CSpectraGridDlg)
	enum { IDD = IDD_SPECGRID_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	
	CGridCtrl* GetGrid(int FitGrid=0);
	void UpdateFitNum(int Num);
	void	UpdateMode(int Mode);
	int		GetMode();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpectraGridDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL
// Implementation
protected:
	char		m_szFitNum[5];
	CListBox	cModeList;
	CGridCtrl	m_GridCtrl;
	CGridCtrl	m_FitGridCtrl;
	// Generated message map functions
	//{{AFX_MSG(CSpectraGridDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnSelchangeFitMode();
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CSpecScanDlg dialog

class CSpecScanDlg : public CDialog
{
// Construction
public:
	CSpecScanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpecScanDlg)
	enum { IDD = IDD_DLG_SCAN };
	int		m_PointNum;
	//}}AFX_DATA
	CSpectra4View* pView;
	CButton m_MinSw; CButton m_MaxSw; CButton m_AveSw; CButton m_AreSw; CButton m_SlpSw;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecScanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecScanDlg)
		// NOTE: the ClassWizard will add member functions here
	afx_msg void OnSelchangeFitMode();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CFitSetDlg dialog

class CFitSetDlg : public CDialog
{
// Construction
public:
	CFitSetDlg(CWnd* pParent = NULL);   // standard constructor
	CSpectra4View* pView;
	CButton m_MdInward; CButton m_MdOutward; CButton m_MdGauss;
// Dialog Data
	//{{AFX_DATA(CFitSetDlg)
	enum { IDD = IDD_FITSET };
	int		m_CicleMax;
	int		m_StepMax;
	float	m_AccurH;
	float	m_AccurL;
	float	m_TresX;
	float	m_HighX;
	float	m_LowX;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFitSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFitSetDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CRandDlg dialog

class CRandDlg : public CDialog
{
// Construction
public:
	CRandDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRandDlg)
	enum { IDD = IDD_RAND_DLG };
	float	m_AmpHigh;
	float	m_AmpLow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRandDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRandDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSpecOptionsDlg dialog

class CSpecOptionsDlg : public CDialog
{
// Construction
public:
	CSpecOptionsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSpecOptionsDlg)
	CListBox	cChanList;
	enum { IDD = IDD_TOOLS_OPTS };
	int		m_nBefore;
	int		m_nCreate;
	int		m_nStim;
	int		m_nAfter;
	int		m_SwapSer;
	int		m_MaxData;
	float	m_Depth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecOptionsDlg)
	protected:
	//virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecOptionsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//afx_msg void OnOptSwapSer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CHistDlg dialog

class CHistDlg : public CDialog
{
// Construction
public:
	CHistDlg(CWnd* pParent = NULL);   // standard constructor
	
// Dialog Data
	//{{AFX_DATA(CHistDlg)
	enum { IDD = IDD_TOOLS_HIST };
	int		m_BegPoint;
	int		m_EndPoint;
	float	m_BinSize;
	int		m_NBin;
	int		m_CumHst;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHistDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CFitResultDlg dialog

class CFitResultDlg : public CDialog
{
// Construction
public:
	CFitResultDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFitResultDlg)
	enum { IDD = IDD_FITRES };
	char	m_SwNumStr[64];
	char	m_AccurStr[64];
	char	m_PeakNumStr[64];
	char	m_SgnStr[64];
	float	m_Max;
	float	m_Trise;
	float	m_Tdec;
	float	m_Amp;
	float	m_Max2;
	float	m_Trise2;
	float	m_Tdec2;
	float	m_Amp2;
	int		m_CommandFlag; // -1 stop!, 0 - try again, 1 - accept, 2 -reject
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFitResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFitResultDlg)
	afx_msg void OnTryAgain();
	afx_msg void OnStop();
	afx_msg void OnZero();
	afx_msg void OnZero2();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSweepNumDlg dialog

class CSweepNumDlg : public CDialog
{
// Construction
public:
	CSweepNumDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSweepNumDlg)
	enum { IDD = IDD_TRACE_NUM };
	int		m_BegNum;
	int		m_EndNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSweepNumDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSweepNumDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CTimeCourseDlg dialog

class CTimeCourseDlg : public CDialog
{
// Construction
public:
	CTimeCourseDlg(CWnd* pParent = NULL);   // standard constructor
	virtual void OnOK();
// Dialog Data
	//{{AFX_DATA(CTimeCourseDlg)
	enum { IDD = IDD_TMCOURSE };
	float	m_begX;
	float	m_endX;
	float	m_stepX;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeCourseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTimeCourseDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CSpecScaleDlg dialog

class CSpecScaleDlg : public CDialog
{
// Construction
public:
	CSpecScaleDlg(CWnd* pParent = NULL);   // standard constructor
	CSpectra4View* pView;
	int	Norm;
	float	Factor;
// Dialog Data
	//{{AFX_DATA(CSpecScaleDlg)
	enum { IDD = IDD_SCALE_DLG };
	CComboBox	m_FactorBox;
	int		m_BegComp;
	int		m_EndComp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSpecScaleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSpecScaleDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CFitSeekDlg dialog

class CFitSeekDlg : public CDialog
{
// Construction
public:
	CFitSeekDlg(CWnd* pParent = NULL);   // standard constructor
	virtual void OnOK();
// Dialog Data
	//{{AFX_DATA(CFitSeekDlg)
	enum { IDD = IDD_SEEK_FIT };
	float	m_Amp1;
	float	m_tRise1;
	float	m_tDec1;
	float	m_Amp2;
	float	m_tRise2;
	float	m_tDec2;
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFitSeekDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFitSeekDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CVarAnSetDlg dialog

class CVarAnSetDlg : public CDialog
{
// Construction
public:
	CVarAnSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVarAnSetDlg)
	enum { IDD = IDD_VARAN_SET };
	float	m_AnBeg;
	float	m_AnEnd;
	float	m_NsBeg;
	float	m_NsEnd;
	float	m_tDec;
	float	m_FitMix;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVarAnSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVarAnSetDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPECTRAGRIDDLG_H__999CE1C0_C53C_11D5_A8B9_0002B3090409__INCLUDED_)
