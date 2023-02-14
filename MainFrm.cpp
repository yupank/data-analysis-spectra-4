// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Spectra4.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern CStatusBar* pStatusBar;
extern CSpectraDlgBar* pDialogBar;
extern CToolBar CompToolBar;
extern int* Lotable;
extern int LotSize;
extern float* LotPr;
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	IDS_INDICAT1,
	IDS_INDICAT2,
	IDS_INDICAT3,
	//ID_SEPARATOR,           // status line indicator
	//ID_INDICATOR_CAPS,
	//ID_INDICATOR_NUM,
	//ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
	if(LotSize)
	{
		delete[] Lotable; delete[] LotPr;
		LotSize=0;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	CompToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |CBRS_TOP|
		CBRS_GRIPPER| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC); 
	CompToolBar.LoadToolBar(IDR_COMP_TOOLBAR);
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	else	pStatusBar = &m_wndStatusBar; //easy access to status bar
	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	CompToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&CompToolBar);
	//m_wndDialogBar.Create(this,IDD_DIALOGBAR,CBRS_TOP |CBRS_SIZE_FIXED,ID_COMP_DIALOGBAR); 
	m_wndDialogBar.Create(this);
	pDialogBar = &m_wndDialogBar;
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Size the main window to 3/4 screen size 
	cs.cy = ::GetSystemMetrics(SM_CYSCREEN);
	cs.cx = ::GetSystemMetrics(SM_CXSCREEN);
	cs.cx*=0.6;
	cs.cy*=0.9;
	//cs.y = ((cs.cy * 2) - cs.cy) / 2;
	//cs.x = ((cs.cx * 2) - cs.cx) / 2;
	//cs.y = cs.cy / 2;
	//cs.x = cs.cx / 2;
	cs.x=cs.cx*0.02;
	cs.y=cs.cy*0.02;
	return CMDIFrameWnd::PreCreateWindow(cs);
	
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

