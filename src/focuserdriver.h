//**************************************************************************
//*	Name:			focuserdriver.h
//*
//*	Author:			Mark Sproul
//*
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul
//*****************************************************************************
//*	Nov 28,	2020	<MLS> Updated return values to TYPE_ASCOM_STATUS
//*****************************************************************************
//#include	"focuserdriver.h"


#ifndef	_FOCUSER_DRIVER_H_
#define	_FOCUSER_DRIVER_H_


#ifndef _ALPACA_DRIVER_H_
	#include	"alpacadriver.h"
#endif

//*****************************************************************************
//focuser
//*****************************************************************************
enum
{
	kCmd_Focuser_absolute	=	0,		//*	Indicates whether the focuser is capable of absolute position.
	kCmd_Focuser_ismoving,				//*	Indicates whether the focuser is currently moving.
	kCmd_Focuser_maxincrement,			//*	Returns the focuser's maximum increment size.
	kCmd_Focuser_maxstep,				//*	Returns the focuser's maximum step size.
	kCmd_Focuser_position,				//*	Returns the focuser's current position.
	kCmd_Focuser_stepsize,				//*	Returns the focuser's step size.
	kCmd_Focuser_tempcomp,				//*	Retrieves the state of temperature compensation mode
										//*	Sets the device's temperature compensation mode.
	kCmd_Focuser_tempcompavailable,		//*	Indicates whether the focuser has temperature compensation.
	kCmd_Focuser_temperature,			//*	Returns the focuser's current temperature.
	kCmd_Focuser_halt,					//*	Immediately stops focuser motion.
	kCmd_Focuser_move,					//*	Moves the focuser to a new position.

	//*	added by MLS
	kCmd_Focuser_Extras,
	kCmd_Focuser_moverelative,			//*	Moves a specific amount
	kCmd_Focuser_readall,				//*	Reads all of the values


	kCmd_Focuser_last
};

//**************************************************************************************
class FocuserDriver: public AlpacaDriver
{
	public:

		//
		// Construction
		//
						FocuserDriver(const int argDevNum);
		virtual			~FocuserDriver(void);

		virtual	TYPE_ASCOM_STATUS	ProcessCommand(TYPE_GetPutRequestData *reqData);
		virtual	void				OutputHTML(TYPE_GetPutRequestData *reqData);
		virtual	void				OutputHTML_Part2(TYPE_GetPutRequestData *reqData);
		virtual	int32_t				RunStateMachine(void);
		virtual bool				GetCmdNameFromMyCmdTable(const int cmdNumber, char *comandName, char *getPut);

		TYPE_ASCOM_STATUS	Get_Absolute(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Ismoving(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Maxincrement(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Maxstep(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Position(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Stepsize(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Tempcomp(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);

		TYPE_ASCOM_STATUS	Put_Tempcomp(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg);
		TYPE_ASCOM_STATUS	Get_Tempcompavailable(	TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Get_Temperature(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString);
		TYPE_ASCOM_STATUS	Put_Halt(				TYPE_GetPutRequestData *reqData, char *alpacaErrMsg);
		TYPE_ASCOM_STATUS	Put_Move(				TYPE_GetPutRequestData *reqData, char *alpacaErrMsg);

		//*	not part of the Alpaca standard
		TYPE_ASCOM_STATUS	Put_MoveRelative(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg);
		TYPE_ASCOM_STATUS	Get_Readall(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg);

		bool	RotationSupported(void);


		//*	these are access functions for FITS output
		int32_t	GetFocuserPostion(void);
		void	GetFocuserManufacturer(char *manufactString);
		void	GetFocuserModel(char *modelName);
		void	GetFocuserVersion(char *versionString);
		void	GetFocuserSerialNumber(char *serialNumString);
		double	GetFocuserTemperature(void);
		double	GetFocuserVoltage(void);


		//*	focuser specific commands
		virtual	TYPE_ASCOM_STATUS	SetFocuserPostion(const int32_t newPosition, char *alpacaErrMsg);
		virtual	TYPE_ASCOM_STATUS	HaltFocuser(char *alpacaErrMsg);


		//*	these functions are for rotator access
		int32_t			GetRotatorPosition(void);
		int32_t			GetRotatorStepsPerRev(void);
		bool			GetRotatorIsMoving(void);


	protected:

		int32_t			cFocuserPGet_Readallostion;
		int32_t			cFocuserPostion;
		int32_t			cPrevFocuserPostion;
		int32_t			cNewFocuserPostion;
		int32_t			cMaxStep;				//*	the maximum position the focuser can go to
		int32_t			cMaxIncrement;			//*	Maximum increment size allowed by the focuser;
												//*	i.e. the maximum number of steps allowed in one move operation
		bool			cFocusIsMoving;
		double			cFocuserStepSize;

		//*	this is for support of Moonlite NiteCrawler
		bool			cIsNiteCrawler;
		bool			cFocuserSupportsRotation;
		int32_t			cRotatorPostion;
		int32_t			cRotatorStepsPerRev;
		int32_t			cPrevRotatorPostion;
		int32_t			cNewRotatorPostion;
		bool			cRotatorIsMoving;

		bool			cFocuserSupportsAux;
		int32_t			cAuxPosition;
		int32_t			cPrevAuxPostion;
		int32_t			cNewAuxPostion;
		bool			cAuxIsMoving;


		bool			cFocuserHasTemperature;
		double			cFocuserTemp;			//*	degrees C
		bool			cHasTempComp;
		bool			cTempCompEnabled;

		bool			cFocuserHasVoltage;
		double			cFocuserVoltage;
		bool			cSwitchIN;
		bool			cSwitchOUT;
		bool			cSwitchROT;
		bool			cSwitchAUX1;
		bool			cSwitchAUX2;

		int				cInvalidStringErrCnt;

};

void	CreateFocuserObjects(void);


#endif	//	_FOCUSER_DRIVER_H_