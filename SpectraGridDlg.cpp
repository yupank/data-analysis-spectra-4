// SpectraGridDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Spectra4.h"
#include "Spectra4Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg dialog


CSpectraGridDlg::CSpectraGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpectraGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpectraGridDlg)
	m_csFitNum = _T("");
	//}}AFX_DATA_INIT
}


void CSpectraGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectraGridDlg)
	DDX_Text(pDX, IDC_FIT_NUM, m_csFitNum);
	DDV_MaxChars(pDX, m_csFitNum, 5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSpectraGridDlg, CDialog)
	//{{AFX_MSG_MAP(CSpectraGridDlg)
	ON_LBN_SELCHANGE(IDC_FIT_MODE, OnSelchangeFitMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg message handlers

void CSpectraGridDlg::OnSelchangeFitMode() 
{
	// TODO: Add your control notification handler code here
	
}
