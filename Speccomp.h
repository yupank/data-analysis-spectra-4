#include "component.h"


// Scientific programm for spectra proceeding - version 2.0 ( August, 1993 )
#define DLG_HIST	1
#define MAX 	0
#define WID 	1
#define SPLIT 	2
#define MIX 	3
#define TIME 	4
#define MULT	5

#define GALOR	0
#define RIDEC	1


#define NCompMax 5000
#define MaxSaveColumnNum 100

#define MarginL 0.12
#define MarginR 0.05
#define MarginT 0.05
#define MarginB 0.1

//char*	ConfStr="Spectra config";
int ReadData(const char*, float far*, float far*, int);
/****************************/
typedef struct
{
	float	StepMax;
	float	StepWid;
	float	StepSplit;
	float	StepMix;
	float	NewAccur;
	float	OldAccur;
	float	NewCicleAccur;
	float	OldCicleAccur;
	int		StepRel;
	int		NStepMax;
	int		NCicleMax;
	int		StepPerCicle;
	int		crStep;
	int		crCicle;
	int		totStep;
	int		BegComp;
	float	AccurL;
	float	AccurH;
	float	AccurHrej;
	int		StepProgress;
}TFitParam;
/*******************************/
typedef struct
{
int	ID;
int	Num;
float	Value;
char  Name[64];
}TDlgParam;

/****************************/
class TFrame
{
	public:
	float Beg;
	float End;
	float Step;
	float BegMark;
	float Lock;
	float Factor;
	float NMark;
	int   Style;
	int 	ReScaleFlag;
	int	LogScale;
	TFrame(void);
	//void 	SetStr(char*);
	//int 	GetStr(char*);
	void	SetRoundScale(void);
	void	SetStep(void);
	void  Import(TFrame*);

};

/************************************************************************\
*                              Class TSpectrum                           *
\************************************************************************/
class TSpectrum
{
	public:
	int 		NSComp;
	HGLOBAL 	HSMem[1000];
	HGLOBAL 	HCompMem;
	HGLOBAL 	HParamMem;
	HGLOBAL 	HFitMem;
	float far*	FitDomLow;
	float far*	FitDomHigh;
	float far*	FitDomCenter;
	float far*	FitDomAccur;
	int			FitDomSize;
	float far* 	FitComp;
	float far* 	DataBegin;           // pointer to the begining of the free memory for new data
	int     	NSMem;                  // full number of memory blocks in use
	DWORD  	BytesRem;       			// number of free bytes in current memory block
	int     	NSPoint;                // maximum number of points in the spectrum component
	TComponent*	SpecComp;     		// pointers to the	components adresses, 0-Summ,1-Main
	TComponent* BcgrComp;
	float far*  SpecBase;          // pointer to the spectrum X-data
	TFitResult*	CompFitResult;
	int			NFitResult;        // number of components been fitted, defauult = 0 
	float   	DeltaX;
	float   	DeltaY;
	float		Accurasy;
	TParam*   SpecParam;    // parameters of components
	TParam*   BcgrParam; 
	LOGBRUSH  SpecBrush;
	LOGPEN    SpecPen[16];  // 0-frame,1-summ, 2-15 - components
	LOGFONT	 SpecFont[N_TITLE+1];// N_TITLE - Font for frame
	TFrame*   X_Axis;
	TFrame*   Y_Axis;
	//PTMark    Marker;
	PTMark	Label;     // labels for the curves
	PTMark	Title;     // Titles & Markers
	int		DotStyle;
	float	EndSelect;
	float 	BegSelect;
	int		BegSelPoint;
	int		EndSelPoint;
	float	StDev;
	float	MeanSqrErr;
	float	NoiseSqr;
	float	CreateScaleX;
	int     CreateMode;
	int		CreateNum;
	int		ShowSumm;
	int		IsSelected;
	int 	UniScaled;
	int		Normalized;
	int		SelPageSize;
	int		SelNum;
	int		TagComp; //target component for fitting, default = 1
  int		BegFitComp; // number of first model component to find
	//Screen block parameters
	long	int ADCTime;
	short	int Param[18];
	char    TimeStr[9];
	char	CommentStr[21];

	TSpectrum(void);
	~TSpectrum(void);
	int   	CreateComp(void);   //allocates memory, creates objects "TComponent" and TParam, and induces NSComp
	void  	ResetAxis(int ResetX=1, int IgnoreSel=1);
	void	SetSelNum(int Dir, int PgSize=0, int InitNum = -1);
	int		SetSelStr(char* Str);
	void	GetSelStr(char* Str);
	void	SelCompIndex(int*);
	int 	SelComp(int DestNum, int DeSelectMode=1);
	int		FindSelComp(int LastNum=0);
	void	CopySelComp(TSpectrum* DestSpec, int SimpleCopy=0, int IgnoreSelect=0);
	int		PasteSelComp(TSpectrum* SourceSpec, int SimpleCopy=0, int IgnoreSelect=0);
	int		NewComp(float far* DataX,float far* DataY,int N_Data, int SimpleCopy=0);
	void	RemoveComp(int);
	void	ClearAllComp(int LastNum=2);
	int		Read(const char* FileName, int nBefore, int nAfter,int Format); // returns  zero if data format is incorrect
	void	SaveComp(const char* FileName);
	int		ReadComp(const char* FileName, int nBefore,int nAfter, int StimPoint = 100);
	void	SaveSCRData(const char* FileName);
	void	SaveResults(const char* FileName);
	void	ReadResults(const char* FileName);
	void	SaveConfig(const char* FileName);
	void	ReadConfig(const char* FileName,int SimpleCopy=1);
	void	ResetComp(int DestN);
	float	GetAccur(void);
	float	LeastSqr(int TestMode=TEST_MSR);
	void 	BackSubst(int);
	float	AllSumm(int SelectedMode=1);
	float	Average(int SelectedMode=1);
	void	Correct(int CurveN, int PointN);
	void	ReLine(float far* , float far*, int );
	void	ReLine2(float far* , float far*, int );
	void	ReScale(int Beg, int End, float Factor);
	//void  ReDraw(RECT* OutRect, TDC* SpDC);
	void	ReDraw(RECT* OutRect, HDC hDC);
   //void	ReDraw(RECT* OutRect, const char* FileName, HDC refDC=NULL);
	void	InvertColor(void);
	void	Monochrome(void);
	void	SelChange(float,float,int);
	void	Smooth(int nCurve, float Fraction=0.1);
	void	Phourier(TPhourier*);
	void	PhrFilter(TPhourier*);
	float	Intensity(void);
	void	Normalize(int StartPoint=0);
	void	DeNormalize();
	int		FitStep(TFitParam* FitState);
	int		FitFind(TFitParam* FitState);
	void	AutoFit(TFitParam* FitState);
	float	ScanReport(char* ScanStr, int StartPoint=0);
	float	StatReport(int StartPoint=0);
	float	Test(void);
	void	CreateHist(float* HistBase, float* HistData, int NBin, int PDFMode, int BegPoint, int EndPoint, int StimPoint, float* BinSize=NULL);
	int		CreateTimeCourse(float tmBeg, float tmEnd, float tmInt, float* tmBase, float* tmData,int tmDataSize);
	int		CreatePowerSpec(float* tmBase, float* tmData, int tmSize); 
	float	AutoCorr(float*  acBase, float * acData, float* Period=NULL, float* sdPer=NULL);
	int		SeekComp(float Amp1, float Amp2,int StimPoint=50);
	int		SeekFitResult(float Amp1, float Amp2, float tR1, float tR2, float tD1, float tD2, int IgnoreFlag=0);
	int		GetFitResultMargin(TFitResult* MinResult, TFitResult* MaxResult);
	int		CopySelPoint(float* BufX, float* BufY, int SelFlag = -1);
	int		Extrapolate(float* 	xBase, float* yData, TPhourier* Filter, TFitParam* FitState, float xSize=0, int CurveN=1);
	float	Area();
	float	Slope(int Beg=0, int End=0, int Comp=1);
	float	OutSpline(int jComp, float X, int FastFlag=0);


};

