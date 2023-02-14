#include "stdafx.h"
#include "math.h"
#define ID_NOISE_CALIB	405
#define PTComponent TComponent*
#define PTParam     TParam*
#define PTMark      TMark*
#define NONE        2000
#define READY		NONE+1
#define ON			NONE+2
#define GAUSDIR		NONE+10
#define LORDIR		NONE+11
#define GAUSLOR		NONE+12
#define ABSORB		NONE+13
#define FILE		NONE+14
#define FILE_DIR	NONE+15
#define FILE_NORM	NONE+16
#define SUMM		NONE+14
#define OLD			NONE+18
#define NUMBER		NONE+19
#define NAME		NONE+20
#define AUTO		NONE+21
#define ARR_UP		NONE+22
#define ALL			NONE+23
#define MAIN		NONE+24
#define UPPER		NONE+25
#define LOWER		NONE+26
#define RIDEC_I		NONE+27
#define RIDEC_O		NONE+28
#define KIN_IN		NONE+29
#define KIN_EX		NONE+30
#define BAR_O		NONE+31
#define BAR_F		NONE+32
#define BAR_H		NONE+33
#define TRI_O		NONE+40
#define TRI_F		NONE+41
#define CROSS_R		NONE+42
#define CROSS_D		NONE+43
#define CROSS_T		NONE+44
#define CIRCLE_O	NONE+45
#define CIRCLE_F	NONE+46
#define HIST_F		NONE+47
#define HIST_H		NONE+48
#define HIST_O		NONE+49
#define DST_BAR		NONE+50

#define ARR_DN		NONE+51
#define ARR_RT		NONE+52
#define ARR_LT		NONE+53
#define ARR_UR		NONE+54
#define ARR_UL		NONE+55
#define ARR_DL		NONE+56
#define ARR_DR		NONE+57
#define TRACE_RES  	NONE+58
#define UNIMODAL	NONE+17
#define LIN_CORR	NONE+59
#define LIN_REGR	NONE+60

#define BINOM		NONE+61
#define COMPOUND	NONE+62
#define BINOM_SUB	NONE+63
#define BINOM_EF	NONE+64
#define BINOM_QU	NONE+65

#define CLAMP_ST		NONE+66
#define CLAMP_FST		NONE+67
#define CLAMP_FST2		NONE+68

#define MEM_		NONE+70
#define MEM_NONE	MEM_+1
#define MEM_DATA	MEM_+2
#define MEM_BASE	MEM_+3
#define MEM_SPLINE	MEM_+4

#define TEST_		NONE+80
#define TEST_MSR	TEST_+1
#define TEST_T		TEST_+2
#define TEST_KS		TEST_+3
#define TEST_LLH	TEST_+4
#define TEST_CHI	TEST_+5

#define	FILE_			NONE+90
#define	FILE_DATA		FILE_+1
#define	FILE_ORG		FILE_+2

#define	DIFFUS_1		NONE+100
#define	DIFFUS_2		NONE+101
#define	STEPRESP		NONE+102
#define	SPILL_1N		NONE+103
#define	SPILL_2N		NONE+104
#define	SPILL_1A		NONE+105
#define	SPILL_2A		NONE+106
#define	SKEW_UN			NONE+107
#define	SINUS			NONE+108
#define BINOM_NZ		NONE+109
#define BIMODAL			NONE+110
#define LORDIR_ASM		NONE+111
#define AUDIO_RESP		NONE+14


#define N_TITLE			32
#define N_LINE		  	16
#define N_Spline 	  	300
#define TextLen     	256
#define NClickMax   	10
#define NDATA_MAX 		60000
#define N_ReagentMax	200
#define N_StageMax		100
#define MinFloat		1.0e-33
#define DiffSize		1500
#define NMediat			1177

#define OFFSET_MAX 	0
#define OFFSET_WID 	1
#define OFFSET_SPL 	2
#define OFFSET_MIX 	3
#define OFFSET_TIM	4
#define OFFSET_NUM 	5

#define in 0x01
#define out 0x02
#define noreplace 0x40
float const PI=3.141593;
struct TParam
{
	float Kf;
	float Zero;
	float Amp;
	float Wid;
	float WidH;
	float WidL;
	float MaxH;
	float MaxL;
	float SplitH;
	float SplitL;
	float Max;
	float Mix;
	float MixL;
	float MixH;
	float Split;
	float ExtMin;
	float ExtMax;
	int   Mode;
	int	Mult;
	int	MultL;
	int	MultH;
	int   FixWid;
	int   FixMax;
	int   FixSplit;
	int	Select;
	float Time;
	float TimeL;
	float TimeH;
	float	NormCf;
	char  Name[20];
	void Copy (TParam*);
	void Paste (TParam*,int ScaleCopy=1);
};


int		EquSet(float far*, float far*, float far*, int);
int		EquSet2(double far*, double far*, double far*, int);
long int Factorial(int);
int		RandomBinom(int N, float P);
float	RandomNorm(float Mean, float SD);
float	Rand(void);
void	SprintFloat(float Val, char* szNum,int ExtraDig=0);
float	ValidFloat(float Val, float Min=-1);
void	DiffSourceCalc(float R, float D, float Uptake, int NVes, float* Base, float* SourceData, int NPoint);
void	MediatCalc(float R, float D, float Z, float Btot, int NVes, float* Base, float* MediatData, int NPoint);
float Upt(float t, float g);
float Upt(float t);
float	NMDArec(float t);
float ProbFunc(TParam* NewPar, int Num, int jD, int Mode=0);



void	MediatCalc(TParam* MediatPar);
/******************************/
struct TOldParam
{
	float Kf;
	float Zero;
	float Amp;
	float Wid;
	float WidH;
	float WidL;
	float MaxH;
	float MaxL;
	float SplitH;
	float SplitL;
	float Max;
	float Mix;
	float Split;
	float ExtMin;
	float ExtMax;
	int   Mode;
	int	Mult;
	int   FixWid;
	int   FixMax;
	int   FixSplit;
	int	Select;
	float Time;
	float	NormCf;
	char  Name[20];
	void CopyOld2New(TParam*);
	void CopyNew2Old(TParam*);
};



//typedef
/****************************/
typedef struct TPhourier
{
	float FreqL;  // begining of frequency range (rel.un.)
	float FreqH;  // end      of frequency range (rel.un.)
	float CutFreq;//discrimination frequency
	float CutAmp; //discrimination amplitude
	float Factor;
	int   NNodes;
	int   On;
};
/****************************/
typedef struct TMark
{
	public:
	float 	X;
	float 	Y;
	float 	SizeX;
	float	SizeY;
	float	TextX;
	float	TextY;
	int   	Style;
	int		Visible;
	char  	Text[TextLen];
	//void 	SetStr(char*);
	//int 	GetStr(char*);
};
/******************************/
typedef struct TFitResult
{
public:
   float	tRise;
   float	tDec;
   float	tBeg;
   float	Amp;
   float	TotAmp;
   int		AcceptFlag;
};
/*************************************************************************\
*          TComponent - BASE     class                                    *
*                          Warning!                                       *
* Memory allocation may be done both before and after creation            *
\*************************************************************************/
class TComponent
{
protected:
	int 		CreateMode;
	int        	NPoint;        // number of points
	float far* 	Data;	 	     // pointer to the array of component data(only Y-values)
	float far* 	Base;          // X-values
	float far* 	Splines;	     //coefficients of slpline-functions
	int        	IsSplined;
	int			StimPoint;
	int			FilterOn;
	TParam*		FilterPar;
	
public:
	float EgV[201];
	float EgF[201];
	int	NA;
	TComponent (float* PtrData, float*  PtrBase, int DestN); //constructors
	TComponent (float* PtrBase, int DestN);
	TComponent (int DestN);
	TComponent	(void){NPoint=0;};
	~TComponent();
	void    Set(float far*, float far*,int);
	void	Paste(TComponent* tagComp, int CopyDataRef=0);
	float* 	GetData(void);
	void	SetData(float far*);
	void	InitData(float);
	void    Save(const char* FileName, float Dx,float Dy, int Format=FILE_ORG);
	int     Read(const char* FileName, int Format=FILE_DATA);
	void    Reset(TParam* NewPar);
	void	Randomize(TParam*);
	void    Randomize(float,float);
	float	LikeHoodTest(TParam*);
	void	SetFilter(TParam*);

	float   SMod(void);          // summ of absolute values
	float   SSqu(void);          // summ of squares
	float   Area(int Beg=0, int End=0);          // area under the curve
	float   Area(float xBeg, float xEnd);
	float   MinVal(int StartPoint=0, int EndPoint =0, int Smooth = 0);        // minimum of Y-values
	float   MaxVal(int StartPoint=0, int EndPoint =0, int Smooth = 0);        // maximum of Y_values
	float	NoiseVal(int StartPoint=20);
	int     ReSpline(void);      // computing the new values of spline coefficients
	float   OutSpline(float X, int FastFlag=0);
	float   ApproxZeroLeft(void);
	float   ApproxZeroRight(void);
	float	GetBaseLevel(int Beg,int End=15);
	int     Smooth(int,int);
	//int     Exclude(int);
	void    SmoothCut(float Begin, float End, float Fraction=0.5);
	void    SmoothCut(int, int, float);
	void    Clip(int Beg=0, int End=-1);
	void    Derive(void);
	int     FindPoint(float);       // returns the number of nearest X values
	int 	ExtremSearch(TPhourier* Filter, float far* LockX, int NMax, int SearchMode=0, float *PNoise=NULL,int StartPoint=-1,int EndPoint=-1);
	int 	ExtremSearch(float dW, float CutAmp, float far* LockX, float far* LockY, int NMax);
	float	Slope (float BegX, float EndX);
	float	Slope (int Beg=0, int End=0);
	void    PhrFilterHigh(TPhourier*);
	//void    Dot(RECT*,TDC*,PTMark);
	void    Dot(RECT* OutRect,HDC SpDC, PTMark ViewLabel, float* DataY=NULL);
	//void    ShowLabel(RECT*,TDC*,PTMark);
	void    ShowLabel(RECT*,HDC,PTMark);
	void    Draw(RECT*,HDC);
 	void    SetStimPoint(int);
	void	StatRep(float*, float*, float, float);
	void	StatRep(float*, float*,int,int);
	
	// Warning! - all binary operations require equal dimentions of both Components.
	//  Otherwise, first Component won't change
	void operator +=(TComponent*);
	void operator -=(TComponent*);
	void operator *=(float);
	void operator /=(float);
	void operator -=(float);
	void operator +=(float);
	void operator =(TComponent*);
	friend float Mult(TComponent*, TComponent*);// member-by-member multiplication
	friend float Norm(TComponent* Comp_A, TComponent* Comp_B,int Mode=TEST_MSR);// member-by-member root-mean square error
};

/**********************************************************************\
**********                     Kinetic simulation             **********
\**********************************************************************/
typedef struct TRConst
{
	char	Name[16];
   float	Min;
   float	Max;
   float Val;
   short int 	Fixed;
};
/***************************/
typedef struct TReagent
{
	char 			Name[16];
   short int 	cfTerm[N_StageMax]; // coefficients for terms in the equation for this variable
   short int	Fixed;
   float	Init;
};
/***************************/
typedef	struct TStageStr
{
	char	nmR1[16];	// 1st reagent name   //
   char	nmR2[16];	// 2nd reagent name   //
   char	nmP1[16];	// 1st product name   //
   char	nmP2[16];	//	2nd product name   //
   char	nmRC[16];	// rate constant name //
   char	cfRC[16];   // r.c. coefficient   //
};
/***************************/
typedef	struct TStageIndex
{
	short int	iR1;	// 1st reagent name         //
   short int	iR2;	// 2nd reagent name         //
   short int	iP1;	// 1st product name         //
   short int	iP2;	//	2nd product name         //
   short int 	iRC;	// rate constant name       //
   short int	cfRC; // rate constant coefficien //
};

/***************************/
class	TKinetics
{
protected:
	HGLOBAL	HMem[N_ReagentMax];
   int		NMem;
   int		NVarMax;
   float*	DataBegin;
   int		BytesRem;
   int 		NPoint;
   float		tmStep;
   float* 	tmBase;
   float* 	VarY[N_ReagentMax];  //point to reagent data
   int		NTerm;
   TStageIndex	Stage[N_StageMax];
public:
	int	NVar;
   int	NStage;
   int	NConst;
   int	TagNum;
	TRConst	RateConst[N_StageMax];
   float		Term[N_StageMax];
   TReagent	Reagent[N_ReagentMax];
   //Methods
   TKinetics(float*	Base,	int nPoint);
   ~TKinetics();

   int		SetScheme(TStageStr*, int);  //returns zero if there are undefined reagents of rate constants
   int		GetScheme(TStageStr*);  // returns number of stages
   int		CheckRConst(void); 	// checks the rate constant values, 1 - "O'K", 0 - error
   int		CheckRConstLimits(void); // checks the rate constant limits, 1 - "O'K", 0 - error
   int		CheckInitVal(void);   	// checks the initial values , 1 - "O'K", 0 - error
   float*   GetData(int nVar=-1);
   int		Simulate(void);		// return 1 if process is covnerging,       //
   										// 0 - if step was automatically decreased  //
                                 // -1 - in case of diverging process        //

 };
