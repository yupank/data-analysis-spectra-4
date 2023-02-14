// Pulsed.h    PULSE 8.5 Data sructure
typedef unsigned char BOOLEAN;
typedef unsigned char Char;
typedef double LONGREAL;
typedef void *ADDRESS;
typedef signed short INT16;
typedef unsigned short SET16;
typedef unsigned char ENUM_8;
typedef struct String2Type_Struct {Char a [2];} String2Type;
typedef struct String4Type_Struct {Char a [4];} String4Type;
typedef struct String10Type_Struct {Char a [10];} String10Type;
typedef struct String14Type_Struct {Char a [14];} String14Type;
typedef struct CommentType_Struct {Char a [80];} CommentType;
typedef struct RootTextType_Struct {Char a [400];} RootTextType;
typedef ENUM_8 DataAbscissaType;
typedef ENUM_8 DataFormatType;
typedef struct
	{
	LONGREAL Time;
	INT32 StimCount;
	INT32 SweepCount;
	INT32 AverageCount;
	BOOLEAN Leak;
	BOOLEAN SecondTrace;
	String14Type Label;
	INT32 DataPoints;
	INT32 Data;
	ADDRESS DataPointer;
	LONGREAL DataFactor1;
	LONGREAL DataFactor2;
	LONGREAL CSlow;
	LONGREAL GSeries;
	LONGREAL RsValue;
	LONGREAL MConductance;
	LONGREAL ZeroCurrent;
	LONGREAL OnlineYResult;
	LONGREAL OnlineXResult;
	INT32 TotalPoints;
	INT32 Offset;
	SET16 SweepKind;
	INT32 FuraPoints;
	INT32 FuraData;
	ADDRESS FuraPointer;
	LONGREAL OnlineYResult2;
	LONGREAL OnlineXResult2;
	LONGREAL DispFactor1;
	LONGREAL DispFactor2;
	DataFormatType DataFormat;
	DataAbscissaType DataAbscissa;
	LONGREAL Timer;
	INT32 FuraStart;
	LONGREAL VideoTime;
	String4Type Spares;
	}TSweep;
typedef ENUM_8 RecordingModeType;
typedef struct EPC9Type_Struct {unsigned char a [104];} EPC9Type;
typedef struct
	{
	LONGREAL Time;
	LONGREAL Bandwidth;
	LONGREAL PipettePotential;
	LONGREAL CellPotential;
	LONGREAL PipetteResistance;
	LONGREAL SealResistance;
	LONGREAL BackgroundNoise;
	LONGREAL Temperature;
	LONGREAL PipettePressure;
	LONGREAL UserParam1Value;
	String14Type UserParam1Name;
	String2Type UserParam1Unit;
	LONGREAL UserParam2Value;
	String14Type UserParam2Name;
	String2Type UserParam2Unit;
	RecordingModeType RecordingMode;
	BYTE SeVideoMode;
	CommentType Comment;
	EPC9Type EPC9State;
	INT32 InternalSolution;
	INT32 ExternalSolution;
	String2Type ExtraYUnit1;
	String2Type ExtraYUnit2;
	String4Type DispYUnit1;
	String4Type DispYUnit2;
	LONGREAL FuraK;
	LONGREAL FuraMin;
	LONGREAL FuraMax;
	LONGREAL LockInExtPhase;
	LONGREAL Timer;
	LONGREAL ExtraLongReal1;
	LONGREAL ExtraLongReal2;
	LONGREAL ExtraLongReal3;
	LONGREAL ExtraLongReal4;
	LONGREAL ExtCalPhase;
	LONGREAL ExtCalAttenuation;
	LONGREAL PLPhase;
	LONGREAL PLPhaseY1;
	LONGREAL PLPhaseY2;
	BOOLEAN ExtCalValid;
	BOOLEAN PLPhaseValid;
	}TSeries;
/****************************/
typedef struct
	{
   public:
	String14Type Label;
	CommentType Text;
	INT32 ExperimentNumber;
	LONGREAL ExtraLongReal;
	}TGroup;
/****************************/
typedef struct
	{
   public:
	INT16 Version;
	String14Type VersionName;
	String14Type FileName;
	RootTextType Comments;
	LONGREAL StartTime;
	}TRoot;

