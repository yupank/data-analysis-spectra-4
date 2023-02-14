#include "Stdafx.h"
#include "Spectra4.h"
#include "Spectra4Dlg.h"
#include "Spectra4View.h"
#include "GridCtrl.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSpectraGridDlg* pSpectraGridDlg;
extern CSpectra4View* ActiveView;
extern int	EDRChanReadFlag[8];
extern char szNum[64];
char ParamName[10][50];
CModeName cModeName[40];
int NMode=1;

/*****************************************/
void UpdateNameStr(int Mode)
{
	ParamName[MAX][0]='\0';	ParamName[WID][0]='\0';	ParamName[SPLIT][0]='\0';
	ParamName[MIX][0]='\0';	ParamName[MULT][0]='\0';ParamName[TIME][0];
	sprintf(ParamName[MULT],"%s", "N");
	switch (Mode)
   	{
		case GAUSLOR: case GAUSDIR: case LORDIR: case ABSORB: case SKEW_UN:
			sprintf(ParamName[MAX],"%s", "maximum");
			sprintf(ParamName[WID],"%s", "width");
			sprintf(ParamName[SPLIT],"%s", "splitting");
			sprintf(ParamName[MIX],"%s", "gauss %");
			break;
		case BINOM: case BINOM_QU: case BINOM_EF: case BINOM_SUB: case COMPOUND: case BINOM_NZ:
			sprintf(ParamName[MAX],"%s", "p");
			sprintf(ParamName[WID],"%s", "dQn");
			sprintf(ParamName[TIME],"%s", "dQs");
			sprintf(ParamName[SPLIT],"%s", "Q");
			//sprintf(MultNameStr,"%s", "N site");
			if (Mode==BINOM_EF)
        		sprintf(ParamName[MIX],"%s", "extra failures");
			if (Mode==BINOM_SUB)
         		sprintf(ParamName[MIX],"%s", "EC50");
			if (Mode==COMPOUND)
				sprintf(ParamName[MIX],"%s", "p variation");
			break;
		case RIDEC_I: case RIDEC_O: case STEPRESP:
      		sprintf(ParamName[MAX],"%s", "begining");
			sprintf(ParamName[WID],"%s", "rise time");
			sprintf(ParamName[SPLIT],"%s", "decay time");
			if (Mode==STEPRESP)
			{
         		sprintf(ParamName[MIX],"%s", "fraction");
				sprintf(ParamName[TIME],"%s", "time step");
				sprintf(ParamName[MULT],"%s", "decay2,%");
			}
			break;
		case UNIMODAL:
        	sprintf(ParamName[MAX],"%s", "mean");
			sprintf(ParamName[WID],"%s", "StDev");
			break;
		case BIMODAL:
			sprintf(ParamName[MAX],"%s", "maximum");
			sprintf(ParamName[WID],"%s", "width");
			sprintf(ParamName[SPLIT],"%s", "splitting");
			sprintf(ParamName[MIX],"%s", "fraction");
			sprintf(ParamName[TIME],"%s", "width2,rel.u");
			break;
		case SINUS:
        	sprintf(ParamName[MAX],"%s", "phase");
			sprintf(ParamName[WID],"%s", "period");
			sprintf(ParamName[SPLIT],"%s", "gate time");
			sprintf(ParamName[MIX],"%s", "Amplitude");
			sprintf(ParamName[TIME],"%s", "Duration");
			break;
		case CLAMP_ST: case CLAMP_FST: case CLAMP_FST2:
      		sprintf(ParamName[MAX],"%s", "Rs/Ra");
			sprintf(ParamName[WID],"%s", "cable Length");
			sprintf(ParamName[SPLIT],"%s", "Rs/Rshunt");
			sprintf(ParamName[MIX],"%s", "Rs/Rcable");
			sprintf(ParamName[TIME],"%s", "tau membrane");
			break;
		case DIFFUS_1: 
      		sprintf(ParamName[MAX],"%s", "R");
			sprintf(ParamName[WID],"%s", "D");
			sprintf(ParamName[SPLIT],"%s", "Uptake");
			sprintf(ParamName[MIX],"%s", "Z");
			break;
		case DIFFUS_2:
      		sprintf(ParamName[MAX],"%s", "R");
			sprintf(ParamName[WID],"%s", "D");
			sprintf(ParamName[SPLIT],"%s", "Uptake");
			sprintf(ParamName[MIX],"%s", "Z");
			break;
		case SPILL_1N: case SPILL_1A:
      		/*
			sprintf(ParamName[MAX],"%s", "OutR_r");
			sprintf(ParamName[WID],"%s", "D");
			sprintf(ParamName[SPLIT],"%s", "Uptake");
			sprintf(ParamName[MIX],"%s", "int. vol");
			sprintf(ParamName[TIME],"%s", "OutUptake");
			*/
			sprintf(ParamName[MAX],"%s", "Rmax");
			sprintf(ParamName[WID],"%s", "D");
			sprintf(ParamName[SPLIT],"%s", "Uptake");
			sprintf(ParamName[MIX],"%s", "Zmax");
			sprintf(ParamName[TIME],"%s", "density");
			break;
		case SPILL_2N:	case SPILL_2A:
      		sprintf(ParamName[MAX],"%s", "Rmax");
			sprintf(ParamName[WID],"%s", "D");
			sprintf(ParamName[SPLIT],"%s", "Uptake");
			sprintf(ParamName[MIX],"%s", "Zmax");
			sprintf(ParamName[TIME],"%s", "density");
			break;
	}
}


CSpectra4Dlg::CSpectra4Dlg(CWnd* pParent)
	: CDialog(CSpectra4Dlg::IDD, pParent)
{
	m_pView = NULL;
	pSpectrum = NULL;
}
BOOL CSpectra4Dlg::Create()
{
	return CDialog::Create(CSpectra4Dlg::IDD);
}

CSpectra4Dlg::CSpectra4Dlg(CView* pView)
{
	m_pView = pView;
	
}
// SpectraGridDlg.cpp : implementation file
//


/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg dialog


CSpectraGridDlg::CSpectraGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpectraGridDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpectraGridDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSpectraGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectraGridDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_FIT_MODE, cModeList);
	DDX_GridControl(pDX, IDC_GRIDCTRL, m_GridCtrl);
	DDX_GridControl(pDX, IDC_FIT_PARAM, m_FitGridCtrl);
	DDX_Text(pDX, IDC_FIT_NUM, m_szFitNum, 5);
}


BEGIN_MESSAGE_MAP(CSpectraGridDlg, CDialog)
	//{{AFX_MSG_MAP(CSpectraGridDlg)
		// NOTE: the ClassWizard will add message map macros here
	ON_LBN_SELCHANGE(IDC_FIT_MODE, OnSelchangeFitMode)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg message handlers

BOOL CSpectraGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	HICON newIcon= AfxGetApp()->LoadIcon(IDR_SPECTRTYPE);
	SetIcon(newIcon,TRUE);	
	m_GridCtrl.SetRowCount(20);
    m_GridCtrl.SetFixedRowCount(1);
    m_GridCtrl.SetColumnCount(8);
    m_GridCtrl.SetFixedColumnCount(1);
	m_GridCtrl.m_pView = NULL;
	m_FitGridCtrl.m_pView = NULL;
	m_FitGridCtrl.m_HScrollFlag =0;
	// fill rows/cols with text
	for (int row = 0; row < m_GridCtrl.GetRowCount(); row++)
		for (int col = 0; col < m_GridCtrl.GetColumnCount(); col++)
		{ 
			m_GridCtrl.SetColumnWidth(col,50);
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < m_GridCtrl.GetFixedRowCount()) {
				Item.nFormat = DT_LEFT|DT_WORDBREAK;
				Item.szText.Format(_T("Column %d"),col);
			} else if (col < m_GridCtrl.GetFixedColumnCount()) {
				Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.szText.Format(_T("Row %d"),row);
			} else {
				Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				//Item.szText.Format(_T("%d"),row*col);
			}
			m_GridCtrl.SetItem(&Item);
		}

	//m_GridCtrl.AutoSize();
	m_GridCtrl.SetColumnWidth(0,45);
	for (int col = 1; col < m_GridCtrl.GetColumnCount(); col++)
		m_GridCtrl.SetColumnWidth(col,48);
	
	m_FitGridCtrl.SetRowCount(7);
    m_FitGridCtrl.SetFixedRowCount(1);
    m_FitGridCtrl.SetColumnCount(5);
    m_FitGridCtrl.SetFixedColumnCount(1);
	
	// fill rows/cols with text
	for (row = 0; row < m_FitGridCtrl.GetRowCount(); row++)
		for ( col = 0; col < m_FitGridCtrl.GetColumnCount(); col++)
		{ 
			//m_FitGridCtrl.SetColumnWidth(col,45);
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < m_FitGridCtrl.GetFixedRowCount()) {
				Item.nFormat = DT_LEFT|DT_WORDBREAK;
				//Item.nFormat = DT_LEFT;
				Item.szText.Format(_T("Column %d"),col);
			} else if (col < m_FitGridCtrl.GetFixedColumnCount()) {
				//Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
				Item.szText.Format(_T("Row %d"),row);
			} else {
				//Item.nFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
				Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE;
				//Item.szText.Format(_T("%d"),row*col);
			}
			m_FitGridCtrl.SetItem(&Item);
		}
	m_FitGridCtrl.SetItemText(0,0,"Parameter");
	m_FitGridCtrl.SetItemText(0,1,"Value");
	m_FitGridCtrl.SetItemText(0,2,"Step");
	m_FitGridCtrl.SetItemText(0,3,"From");
	m_FitGridCtrl.SetItemText(0,4,"Till");
	//m_FitGridCtrl.AutoSize();
	m_FitGridCtrl.SetColumnWidth(0,66);
	for (col = 1; col < m_FitGridCtrl.GetColumnCount(); col++)
		m_FitGridCtrl.SetColumnWidth(col,48);
	
	// Initializing Mode ListBox
	
	
	//strcpy((cModeName)->Name,"data file");
	//(cModeName)->ID=FILE;
	strcpy((cModeName)->Name,"Inward current (monoexponential)");
	(cModeName)->ID=RIDEC_I;
	strcpy((cModeName+1)->Name,"Outward current (monoexponential)");
	(cModeName+1)->ID=RIDEC_O;
	strcpy((cModeName+2)->Name,"Concentration clamp");
	(cModeName+2)->ID=STEPRESP;
	strcpy((cModeName+3)->Name,"Normal(Gauss) distribution");
	(cModeName+3)->ID=UNIMODAL;
	strcpy((cModeName+4)->Name,"Binomial distribution");
	(cModeName+4)->ID=BINOM;
	strcpy((cModeName+5)->Name,"BiModal(Gauss) distribution");
	(cModeName+5)->ID=BIMODAL;
	strcpy((cModeName+6)->Name,"Binomal non-Zero(spontaneous)");
	(cModeName+6)->ID=BINOM_NZ;
	strcpy((cModeName+7)->Name,"QuasiBinomial distribution");
	(cModeName+7)->ID=BINOM_QU;
	strcpy((cModeName+8)->Name,"Binomial with extra failures");
	(cModeName+8)->ID=BINOM_EF;
	strcpy((cModeName+9)->Name,"Binomial sublinear distribution");
	(cModeName+9)->ID=BINOM_SUB;
	strcpy((cModeName+10)->Name,"Compound distribution");
	(cModeName+10)->ID=COMPOUND;
   	strcpy((cModeName+11)->Name,"Skewed unimodal distribution");
	(cModeName+11)->ID=SKEW_UN;
	strcpy((cModeName+12)->Name,"Sinusoid");
	(cModeName+12)->ID=SINUS;
	strcpy((cModeName+13)->Name,"V-clamp full");
	(cModeName+13)->ID=CLAMP_ST;
	strcpy((cModeName+14)->Name,"V-clamp fast1");
	(cModeName+14)->ID=CLAMP_FST;
	strcpy((cModeName+15)->Name,"V-clamp fast2");
	(cModeName+15)->ID=CLAMP_FST2;
	strcpy((cModeName+16)->Name,"Diffusion 1");
	(cModeName+16)->ID=DIFFUS_1;
	strcpy((cModeName+17)->Name,"Diffusion 2");
	(cModeName+17)->ID=DIFFUS_2;
	strcpy((cModeName+18)->Name,"Spillover:NMDAR");
	(cModeName+18)->ID=SPILL_1N;
	strcpy((cModeName+19)->Name,"Spillover:AMPAR");
	(cModeName+19)->ID=SPILL_1A;
	strcpy((cModeName+20)->Name,"Gauss/Lorentz");
	(cModeName+20)->ID=GAUSLOR;
	strcpy((cModeName+21)->Name,"Lorentz derivative");
	(cModeName+21)->ID=LORDIR;
	strcpy((cModeName+22)->Name,"Gauss derivative");
	(cModeName+22)->ID=GAUSDIR;
	//strcpy((cModeName+22)->Name,"Lorentz asymmetrical");
	//(cModeName+22)->ID=LORDIR_ASM;	
	strcpy((cModeName+23)->Name,"Absorbance doublet");
	(cModeName+23)->ID=ABSORB;
	strcpy((cModeName+24)->Name,"from file");
	(cModeName+24)->ID=FILE;
	NMode=25;
	for (int j=0; j<NMode; j++)
		cModeList.AddString((cModeName+j)->Name);
	//cModeList.SetCurSel(0);
	UpdateMode(RIDEC_I);
	UpdateData(FALSE);
	/*
    
   
   
   
	*/	
	return FALSE;  // return TRUE  unless you set the focus to a control
}

BOOL CSpectraGridDlg::Create()
{
	return CDialog::Create(CSpectraGridDlg::IDD);
}
CGridCtrl* CSpectraGridDlg::GetGrid(int FitGrid)
{ 
	if (FitGrid) 
		return &m_FitGridCtrl;
	else	
		return &m_GridCtrl;
}

void CSpectraGridDlg::OnCancel()
{
	pSpectraGridDlg = NULL;
	CDialog::OnCancel();
}
void CSpectraGridDlg::OnOK()
{
	
}
//
int CSpectraGridDlg::GetMode()
{
	int j=cModeList.GetCurSel();
	if ((j>=0)&&(j<NMode))
		return (cModeName+j)->ID;
	else return FILE;
}
//
void CSpectraGridDlg::UpdateFitNum(int Num)
{
	itoa(Num,m_szFitNum,4);
	UpdateData(FALSE);

}
//
void CSpectraGridDlg::UpdateMode(int Mode)
{
	int j;
	UpdateNameStr(Mode);
	
	for (int row=1;row<7;row++)
	{
		m_FitGridCtrl.SetItemText(row,0,ParamName[row-1]);
		m_FitGridCtrl.RedrawRow(row);
	}
	j=0;
	while ((cModeName+j)->ID!=Mode) j++;
	cModeList.SetCurSel(j);
}
//
void CSpectraGridDlg::OnSelchangeFitMode()
{
	UpdateData(TRUE);
	int j = cModeList.GetCurSel();
	UpdateMode((cModeName+j)->ID);
}
//////////////////////////////////////////////////////////////////
// CSpectraDgBar implementation
////////////////////////////////////////////////////////////////
CSpectraDlgBar::CSpectraDlgBar()
	: CDialogBar()
{
	//{{AFX_DATA_INIT(CSpectraDlgBar)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	xBeg = 0;	xEnd = 0;	m_xSwitch = 1;
	yBeg = 0;	yEnd = 0;	m_ySwitch = 0;
	m_Beg = yBeg;	m_End = yEnd;
	//UpdateData(FALSE);
}


void CSpectraDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpectraDlgBar)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Radio(pDX, IDC_AX_X, m_xSwitch);
	DDX_Radio(pDX, IDC_AX_Y, m_ySwitch);
	DDX_Text(pDX, IDC_AX_BEG, m_Beg);
	//DDV_MinMaxFloat(pDX, m_Beg, -10000, 10000);
	DDX_Text(pDX, IDC_AX_END, m_End);
	//DDV_MinMaxFloat(pDX, m_End, -10000, 10000);
	//}}AFX_DATA_MAP
	//DDX_GridControl(pDX, IDC_GRIDCTRL1, m_GridCtrl);
	
}
void CSpectraDlgBar::Apply()
{
	UpdateData(TRUE);
	if (!m_xSwitch)	{ xBeg = m_Beg;	xEnd = m_End;}
	if (!m_ySwitch)	{ yBeg = m_Beg;	yEnd = m_End;}
	ActiveView->UpdateScale(0);
	UpdateData(FALSE);
}
void CSpectraDlgBar::ReScale(int Mode)
{
	float Scale = m_End-m_Beg;
	switch (Mode)
	{
		case 0: m_Beg-=0.2*Scale; m_End-=0.2*Scale; break;
		case 1: m_Beg*=2.0; m_End*=2.0; break;
		case 2: m_Beg/=2.0; m_End/=2.0; break;
		case 3: m_Beg+=0.2*Scale; m_End+=0.2*Scale; break;
	}
	if (!m_xSwitch)	{ xBeg = m_Beg;	xEnd = m_End;}
	if (!m_ySwitch)	{ yBeg = m_Beg;	yEnd = m_End;}
	ActiveView->UpdateScale(0);
	UpdateData(FALSE);
}
BEGIN_MESSAGE_MAP(CSpectraDlgBar, CDialogBar)
	//{{AFX_MSG_MAP(CSpectraGridDlg)
		// NOTE: the ClassWizard will add message map macros here
	ON_BN_CLICKED(IDC_AX_X, OnAxisSwitch)
	ON_BN_CLICKED(IDC_AX_Y, OnAxisSwitch)
		//}}AFX_MSG_MAP
	//(WM_INITDIALOG, HandleInitDialog)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSpectraDlgBar message handlers
void CSpectraDlgBar::OnAxisSwitch()
{
	UpdateData(TRUE);
	GetAxis();
	
}
//
void CSpectraDlgBar::GetAxis()
{
	if (!m_xSwitch)	{ m_Beg = xBeg;	m_End = xEnd;}
	if (!m_ySwitch)	{ m_Beg = yBeg;	m_End = yEnd;}
	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CSpectraGridDlg message handlers

BOOL CSpectraDlgBar::Create(CWnd* pParentWnd)
{
	
	BOOL Result = CDialogBar::Create(pParentWnd, CSpectraDlgBar::IDD,CBRS_BOTTOM | CBRS_FLYBY | CBRS_SIZE_DYNAMIC ,CSpectraDlgBar::ID);
	SetDlgItemText(IDC_AX_BEG, "0");
	SetDlgItemText(IDC_AX_END, "0");
	CheckRadioButton(IDC_AX_X, IDC_AX_Y, IDC_AX_Y);
	return Result;
}

/////////////////////////////////////////////////////////////////////////////
// CSpecScanDlg dialog


CSpecScanDlg::CSpecScanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecScanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecScanDlg)
	m_PointNum = 5;
	//}}AFX_DATA_INIT
}

BOOL CSpecScanDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_PointNum = pView->ExtremSmoothPoint;
	m_MinSw.SetCheck(0);	m_MaxSw.SetCheck(0);	m_AveSw.SetCheck(0); m_AreSw.SetCheck(0); m_SlpSw.SetCheck(0);
	switch(pView->ScanMode)
	{
		case SCAN_MIN: m_MinSw.SetCheck(1); break;
		case SCAN_MAX: m_MaxSw.SetCheck(1); break;
		case SCAN_AVE: m_AveSw.SetCheck(1); break;
		case SCAN_SLP: m_SlpSw.SetCheck(1); break;
		case SCAN_ARE: m_AreSw.SetCheck(1); break;
	}
	UpdateData(FALSE);
	return TRUE;
}
void CSpecScanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecScanDlg)
	DDX_Control(pDX, IDC_SCAN_MIN, m_MinSw);
	DDX_Control(pDX, IDC_SCAN_MAX, m_MaxSw);
	DDX_Control(pDX, IDC_SCAN_AVE, m_AveSw);
	DDX_Control(pDX, IDC_SCAN_ARE, m_AreSw);
	DDX_Control(pDX, IDC_SCAN_SLP, m_SlpSw);
	DDX_Text(pDX, IDC_SCAN_NUM, m_PointNum);
	DDV_MinMaxInt(pDX, m_PointNum, 1, 1000);
	//}}AFX_DATA_MAP
}

void CSpecScanDlg::OnOK()
{
	UpdateData(TRUE);
	pView->ExtremSmoothPoint = m_PointNum;
	if(m_MaxSw.GetCheck()) pView->ScanMode = SCAN_MAX;
	if(m_MinSw.GetCheck()) pView->ScanMode = SCAN_MIN;
	if(m_AveSw.GetCheck()) pView->ScanMode = SCAN_AVE;
	if(m_AreSw.GetCheck()) pView->ScanMode = SCAN_ARE;
	if(m_SlpSw.GetCheck()) pView->ScanMode = SCAN_SLP;
	CDialog::OnOK();
}
BEGIN_MESSAGE_MAP(CSpecScanDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecScanDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecScanDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CFitSetDlg dialog


CFitSetDlg::CFitSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFitSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFitSetDlg)
	m_CicleMax = 0;
	m_StepMax = 0;
	m_AccurH = 0.0f;
	m_AccurL = 0.0f;
	//}}AFX_DATA_INIT
}
BOOL CFitSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_MdInward.SetCheck(0);	m_MdOutward.SetCheck(0);	m_MdGauss.SetCheck(0); 
	switch(pView->Spectrum->CreateMode)
	{
		case RIDEC_I: m_MdInward.SetCheck(1); break;
		case RIDEC_O: m_MdOutward.SetCheck(1); break;
		case GAUSLOR:m_MdGauss.SetCheck(1); break;
		default: m_MdGauss.SetCheck(1); break;
		
	}
	UpdateData(FALSE);
	return TRUE;
}

void CFitSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFitSetDlg)
	DDX_Text(pDX, IDC_FITSET_CICLE, m_CicleMax);
	DDV_MinMaxInt(pDX, m_CicleMax, 0, 100);
	DDX_Text(pDX, IDC_FITSET_STEP, m_StepMax);
	DDV_MinMaxInt(pDX, m_StepMax, 0, 10000);
	DDX_Text(pDX, IDC_FITSET_AC_HIGH, m_AccurH);
	DDV_MinMaxFloat(pDX, m_AccurH, 1.e-004f, 100.f);
	DDX_Text(pDX, IDC_FITSET_AC_LOW, m_AccurL);
	DDV_MinMaxFloat(pDX, m_AccurL, 1.e-009f, 100.f);
	DDX_Text(pDX, IDC_FITSET_TY, m_TresX);
	DDX_Text(pDX, IDC_FITSET_LOWX, m_LowX);
	DDX_Text(pDX, IDC_FITSET_HIGHX, m_HighX);
	DDX_Control(pDX, IDC_FITSET_MDIN, m_MdInward);
	DDX_Control(pDX, IDC_FITSET_MDOUT, m_MdOutward);
	DDX_Control(pDX, IDC_FITSET_MDGS, m_MdGauss);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFitSetDlg, CDialog)
	//{{AFX_MSG_MAP(CFitSetDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFitSetDlg message handlers

void CFitSetDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);	
	if(m_MdInward.GetCheck()) pView->Spectrum->CreateMode = RIDEC_I;
	if(m_MdOutward.GetCheck()) pView->Spectrum->CreateMode = RIDEC_O;
	if(m_MdGauss.GetCheck()) pView->Spectrum->CreateMode = GAUSLOR;
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CRandDlg dialog


CRandDlg::CRandDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRandDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRandDlg)
	m_AmpHigh = 0.0f;
	m_AmpLow = 0.0f;
	//}}AFX_DATA_INIT
}


void CRandDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRandDlg)
	DDX_Text(pDX, IDC_RAND_HIGH, m_AmpHigh);
	DDX_Text(pDX, IDC_RAND_LOW, m_AmpLow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRandDlg, CDialog)
	//{{AFX_MSG_MAP(CRandDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRandDlg message handlers

void CRandDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);	
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CSpecOptionsDlg dialog


CSpecOptionsDlg::CSpecOptionsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecOptionsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecOptionsDlg)
	m_nBefore = 0;
	m_nCreate = 0;
	m_nStim = 0;
	m_nAfter = 0;
	m_SwapSer = 0;
	m_Depth = 0.0f;
	//}}AFX_DATA_INIT
}


void CSpecOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecOptionsDlg)
	DDX_Text(pDX, IDC_OPTS_BEF, m_nBefore);
	DDV_MinMaxInt(pDX, m_nBefore, 1, 100);
	DDX_Text(pDX, IDC_OPTS_CREATE, m_nCreate);
	DDX_Text(pDX, IDC_OPTS_MAXPOINT, m_MaxData);
	DDV_MinMaxInt(pDX, m_nCreate, 3, 16000);
	DDX_Text(pDX, IDC_OPTS_STIM, m_nStim);
	DDV_MinMaxInt(pDX, m_nStim, 1, 1000);
	DDX_Text(pDX, IDC_POTS_AFT, m_nAfter);
	DDV_MinMaxInt(pDX, m_nAfter, 0, 100);
	DDX_Check(pDX, IDC_OPT_SWPSER, m_SwapSer);
	DDX_Text(pDX, IDC_OPT_DEPTH, m_Depth);
	DDV_MinMaxFloat(pDX, m_Depth, 0.f, 1.f);
	DDX_Control(pDX, IDC_CHAN_LIST, cChanList);
	//}}AFX_DATA_MAP
}
BOOL CSpecOptionsDlg::OnInitDialog()
{
	int j;
	CDialog::OnInitDialog();
	for (j=0;j<8;j++)
	{
		sprintf(szNum,"Ch. %i",j);
		cChanList.AddString(szNum);
		cChanList.SetSel(j,EDRChanReadFlag[j]);
		//if (EDRChanReadFlag[j])
			//cChanList.SetCurSel(j);
	}
	//cChanList.SetSel( GetSelItems(8,EDRChanReadFlag);
	UpdateData(FALSE);
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSpecOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecOptionsDlg)
	//ON_BN_CLICKED(IDC_OPT_SWPSER, OnOptSwapSer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecOptionsDlg message handlers

void CSpecOptionsDlg::OnOK() 
{
	// TODO: Add extra validation here
	int j;
	UpdateData(TRUE);
	for (j=0;j<8;j++)
		EDRChanReadFlag[j]=cChanList.GetSel(j);
	CDialog::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CHistDlg dialog


CHistDlg::CHistDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHistDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHistDlg)
	m_BegPoint = 0;
	m_EndPoint = 0;
	m_BinSize = 0.0f;
	m_NBin = 0;
	m_CumHst=0;
	//}}AFX_DATA_INIT
}

BOOL CHistDlg::OnInitDialog()
{
	UpdateData(FALSE);
	return TRUE;
}
void CHistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHistDlg)
	DDX_Text(pDX, IDC_HIST_BEGP, m_BegPoint);
	DDV_MinMaxInt(pDX, m_BegPoint, 0, 1000000);
	DDX_Text(pDX, IDC_HIST_ENDP, m_EndPoint);
	DDV_MinMaxInt(pDX, m_EndPoint, 1, 1000000);
	DDX_Text(pDX, IDC_HIST_BIN, m_BinSize);
	DDV_MinMaxFloat(pDX, m_BinSize, 0.f, 1.e+008f);
	DDX_Text(pDX, IDC_HIST_NBIN, m_NBin);
	DDV_MinMaxInt(pDX, m_NBin, 10, 10000);
	DDX_Check(pDX, IDC_CUMHST, m_CumHst);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHistDlg, CDialog)
	//{{AFX_MSG_MAP(CHistDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistDlg message handlers

void CHistDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CFitResultDlg dialog


CFitResultDlg::CFitResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFitResultDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFitResultDlg)
	//m_SwNumStr = "";
	//m_AccurStr = "";
	//m_PeakNumStr = "";
	//m_SgnStr = "";
	m_Max = 0.0f;
	m_Trise = 0.0f;
	m_Tdec = 0.0f;
	m_Amp = 0.0f;
	m_Max2 = 0.0f;
	m_Trise2 = 0.0f;
	m_Tdec2 = 0.0f;
	m_Amp2 = 0.0f;

	//}}AFX_DATA_INIT
}


void CFitResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFitResultDlg)
	DDX_Text(pDX, IDC_FITRES_SWN, m_SwNumStr,strlen(m_SwNumStr));
	DDX_Text(pDX, IDC_FITRES_ACCUR, m_AccurStr,strlen(m_AccurStr));
	DDX_Text(pDX, IDC_FITRES_PKN, m_PeakNumStr,strlen(m_PeakNumStr));
	//DDX_Text(pDX, IDC_FITRES_SGN, m_SgnStr,strlen(m_SgnStr));
	DDX_Text(pDX, IDC_FITRES_MAX, m_Max);
	DDX_Text(pDX, IDC_FITRES_TRISE, m_Trise);
	DDX_Text(pDX, IDC_FITRES_TDEC, m_Tdec);
	DDX_Text(pDX, IDC_FITRES_AMP, m_Amp);
	DDX_Text(pDX, IDC_FITRES_MAX2, m_Max2);
	DDX_Text(pDX, IDC_FITRES_TRISE2, m_Trise2);
	DDX_Text(pDX, IDC_FITRES_TDEC2, m_Tdec2);
	DDX_Text(pDX, IDC_FITRES_AMP2, m_Amp2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFitResultDlg, CDialog)
	//{{AFX_MSG_MAP(CFitResultDlg)
	ON_BN_CLICKED(IDC_FITRES_TRY, OnTryAgain)
	ON_BN_CLICKED(IDC_FITRES_ZERO, OnZero)
	ON_BN_CLICKED(IDC_FITRES_ZERO2, OnZero2)
	ON_BN_CLICKED(IDC_FITRES_STOP, OnStop)
	ON_BN_DOUBLECLICKED(IDC_FITRES_TRY, OnTryAgain)
	//ON_BN_DOUBLECLICKED(IDCANCEL, OnCancel)
	//ON_BN_DOUBLECLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFitResultDlg message handlers

void CFitResultDlg::OnTryAgain() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_CommandFlag=0;
	CDialog::OnOK();
}
void CFitResultDlg::OnZero() 
{
	// TODO: Add your control notification handler code here
	m_Amp=0;
	UpdateData(FALSE);
	
}
void CFitResultDlg::OnZero2() 
{
	// TODO: Add your control notification handler code here
	m_Amp2=0;
	UpdateData(FALSE);
	
}
void CFitResultDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_CommandFlag=2;
	CDialog::OnCancel();
}

void CFitResultDlg::OnOK() 
{
	// TODO: Add extra validation here
	m_CommandFlag=1;
	CDialog::OnOK();
}
void CFitResultDlg::OnStop() 
{
	// TODO: Add extra validation here
	m_CommandFlag=-1;
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CSweepNumDlg dialog


CSweepNumDlg::CSweepNumDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSweepNumDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSweepNumDlg)
	//m_BegNum = 0;
	//m_EndNum = 0;
	//}}AFX_DATA_INIT
}


void CSweepNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSweepNumDlg)
	DDX_Text(pDX, IDC_TRACENUM_FROM, m_BegNum);
	DDV_MinMaxInt(pDX, m_BegNum, 1, 20000);
	DDX_Text(pDX, IDC_TRACENUM_TO, m_EndNum);
	DDV_MinMaxInt(pDX, m_EndNum, 1, 20000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSweepNumDlg, CDialog)
	//{{AFX_MSG_MAP(CSweepNumDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSweepNumDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CTimeCourseDlg dialog


CTimeCourseDlg::CTimeCourseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTimeCourseDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTimeCourseDlg)
	m_begX = 0.0f;
	m_endX = 0.0f;
	m_stepX = 0.0f;
	//}}AFX_DATA_INIT
}


void CTimeCourseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTimeCourseDlg)
	DDX_Text(pDX, IDC_EDIT1, m_begX);
	DDX_Text(pDX, IDC_EDIT2, m_endX);
	DDX_Text(pDX, IDC_EDIT4, m_stepX);
	//}}AFX_DATA_MAP
}

void CTimeCourseDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}
BEGIN_MESSAGE_MAP(CTimeCourseDlg, CDialog)
	//{{AFX_MSG_MAP(CTimeCourseDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTimeCourseDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CSpecScaleDlg dialog


CSpecScaleDlg::CSpecScaleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSpecScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSpecScaleDlg)
	m_BegComp = 0;
	m_EndComp = 0;
	//}}AFX_DATA_INIT
}


void CSpecScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSpecScaleDlg)
	DDX_Control(pDX, IDC_FACTOR_COMBO, m_FactorBox);
	DDX_Text(pDX, IDC_BEG, m_BegComp);
	DDV_MinMaxInt(pDX, m_BegComp, 0, 1000000);
	DDX_Text(pDX, IDC_END, m_EndComp);
	DDV_MinMaxInt(pDX, m_EndComp, 0, 1000000);
	//}}AFX_DATA_MAP
}
BOOL CSpecScaleDlg::OnInitDialog()
{
	float scale[3];
	int i,j;
	scale[0]=0.01;
	scale[1]=0.02;
	scale[2]=0.05;
	CDialog::OnInitDialog();
	m_FactorBox.AddString("Normalize");
	for (j=0;j<3;j++)
		for(i=0;i<3;i++)
		{
			sprintf(szNum,"%5.3f",scale[i]);
			m_FactorBox.AddString(szNum);
			scale[i]*=10.0;
		}	
		Norm=-1;
	m_BegComp=1;
	m_EndComp = pView->Spectrum->NSComp-1;
	Factor=0.0;
	UpdateData(FALSE);
	return TRUE;
}
void CSpecScaleDlg::OnOK()
{
	UpdateData(TRUE);
	m_FactorBox.GetWindowText(szNum,strlen(szNum));
	if(szNum[0]=='N' || szNum[0]=='n')
		Norm=1;
	else
		{
			Factor=atof(szNum);
			Norm=0;
		}
	CDialog::OnOK();
}
BEGIN_MESSAGE_MAP(CSpecScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CSpecScaleDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSpecScaleDlg message handlers
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CFitSeekDlg dialog


CFitSeekDlg::CFitSeekDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFitSeekDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFitSeekDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CFitSeekDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_FITSEEK_AMP1, m_Amp1);
	DDX_Text(pDX, IDC_FITSEEK_AMP2, m_Amp2);
	DDX_Text(pDX, IDC_FITSEEK_TR1, m_tRise1);
	DDX_Text(pDX, IDC_FITSEEK_TR2, m_tRise2);
	DDX_Text(pDX, IDC_FITSEEK_TD1, m_tDec1);
	DDX_Text(pDX, IDC_FITSEEK_TD2, m_tDec2);
	//{{AFX_DATA_MAP(CFitSeekDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
void CFitSeekDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(TRUE);
	CDialog::OnOK();
}

BEGIN_MESSAGE_MAP(CFitSeekDlg, CDialog)
	//{{AFX_MSG_MAP(CFitSeekDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFitSeekDlg message handlers
/////////////////////////////////////////////////////////////////////////////
// CVarAnSetDlg dialog


CVarAnSetDlg::CVarAnSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVarAnSetDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVarAnSetDlg)
	m_AnBeg = 0.0f;
	m_AnEnd = 0.0f;
	m_NsBeg = 0.0f;
	m_NsEnd = 0.0f;
	m_tDec = 0.0f;
	m_FitMix = 0.0f;
	//}}AFX_DATA_INIT
}


void CVarAnSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVarAnSetDlg)
	DDX_Text(pDX, IDC_AN_BEG, m_AnBeg);
	DDX_Text(pDX, IDC_AN_END, m_AnEnd);
	DDX_Text(pDX, IDC_NS_BEG, m_NsBeg);
	DDX_Text(pDX, IDC_NS_END, m_NsEnd);
	DDX_Text(pDX, IDC_TDEC, m_tDec);
	DDV_MinMaxFloat(pDX, m_tDec, 0.0, 100000.0);
	DDX_Text(pDX, IDC_FIT_MIX, m_FitMix);
	DDV_MinMaxFloat(pDX, m_FitMix, 0.0, 1.0);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVarAnSetDlg, CDialog)
	//{{AFX_MSG_MAP(CVarAnSetDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVarAnSetDlg message handlers
