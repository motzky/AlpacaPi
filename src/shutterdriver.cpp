//**************************************************************************
//*	Name:			shutterdriver.cpp
//*
//*	Author:			Mark Sproul (C) 2020
//*
//*	Description:	C++ Driver for Alpaca protocol
//*
//*****************************************************************************
//*	AlpacaPi is an open source project written in C/C++
//*
//*	Use of this source code for private or individual use is granted
//*	Use of this source code, in whole or in part for commercial purpose requires
//*	written agreement in advance.
//*
//*	You may use or modify this source code in any way you find useful, provided
//*	that you agree that the author(s) have no warranty, obligations or liability.  You
//*	must determine the suitability of this source code for your use.
//*
//*	Redistributions of this source code must retain this copyright notice.
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul
//*****************************************************************************
//*	Feb 10,	2020	<MLS> Created shutterdriver.cpp
//*	Feb 10,	2020	<MLS> Started on Shutter control
//*	Apr 22,	2020	<MLS> Added readall to shutter driver
//*****************************************************************************

#ifdef _ENABLE_SHUTTER_

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdbool.h>
#include	<ctype.h>
#include	<stdint.h>

#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"



#include	"RequestData.h"
#include	"JsonResponse.h"
#include	"eventlogging.h"

#include	"alpacadriver.h"
#include	"alpacadriver_helper.h"
#include	"shutterdriver.h"


#if defined(__arm__) && !defined(_ENABLE_PI_HAT_SESNSOR_BOARD_)
	#include <wiringPi.h>
#else
	#define	LOW		0
	#define	HIGH	1
#endif




//*****************************************************************************
void	CreateShutterObjects(void)
{

	new ShutterDriver(0);
}


//*****************************************************************************
//*	shutter commands
enum
{
	kCmd_Shutter_altitude	=	0,	//*	The dome altitude
	kCmd_Shutter_cansetaltitude,	//*	Indicates whether the dome altitude can be set
	kCmd_Shutter_cansetshutter,		//*	Indicates whether the dome shutter can be opened
	kCmd_Shutter_shutterstatus,		//*	Status of the dome shutter or roll-off roof
	kCmd_Shutter_slewing,			//*	Indicates whether the any part of the dome is moving
	kCmd_Shutter_abortslew,			//*	Immediately cancel current dome operation.
	kCmd_Shutter_closeshutter,		//*	Close the shutter or otherwise shield telescope from the sky.
	kCmd_Shutter_openshutter,		//*	Open shutter or otherwise expose telescope to the sky.
	kCmd_Shutter_slewtoaltitude,	//*	Slew the dome to the given altitude position.

	//*	commands added by MLS
	kCmd_Shutter_Extras,
	kCmd_Shutter_stopshutter,
	kCmd_Shutter_readall,

	kCmd_Shutter_last
};



//*****************************************************************************
static TYPE_CmdEntry	gShutterCmdTable[]	=
{

	{	"altitude",			kCmd_Shutter_altitude,			kCmdType_GET	},
	{	"cansetaltitude",	kCmd_Shutter_cansetaltitude,	kCmdType_GET	},
	{	"cansetshutter",	kCmd_Shutter_cansetshutter,		kCmdType_GET	},
	{	"shutterstatus",	kCmd_Shutter_shutterstatus,		kCmdType_GET	},
	{	"abortslew",		kCmd_Shutter_abortslew,			kCmdType_PUT	},
	{	"closeshutter",		kCmd_Shutter_closeshutter,		kCmdType_PUT	},
	{	"openshutter",		kCmd_Shutter_openshutter,		kCmdType_PUT	},
	{	"slewtoaltitude",	kCmd_Shutter_slewtoaltitude,	kCmdType_PUT	},

	{	"--extras",			kCmd_Shutter_Extras,			kCmdType_GET	},
	{	"stop",				kCmd_Shutter_stopshutter,		kCmdType_PUT	},
	{	"readall",			kCmd_Shutter_readall,			kCmdType_GET	},



	{	"",						-1,	0x00	}
};

//*****************************************************************************
ShutterDriver::ShutterDriver(const int argDevNum)
	:AlpacaDriver(kDeviceType_Shutter)
{
	CONSOLE_DEBUG(__FUNCTION__);

	cAltitude_Dbl			=	0.0;
	cShutterStatus			=	kShutterStatus_Unknown;
	cPreviousShutterStatus	=	kShutterStatus_Unknown;

	strcpy(cDeviceName, "shutter");
	strcpy(gWebTitle, "shutter");

	Init_Hardware();

	strcpy(cDeviceDescription, "Shutter");

}

//**************************************************************************************
// Destructor
//**************************************************************************************
ShutterDriver::~ShutterDriver( void )
{

}

//*****************************************************************************
void	ShutterDriver::Init_Hardware(void)
{

	CONSOLE_DEBUG(__FUNCTION__);
}



//*****************************************************************************
void	ShutterDriver::CheckSensors(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
}


//*****************************************************************************
//*	return number of microseconds allowed for delay
//*****************************************************************************
int32_t		ShutterDriver::RunStateMachine(void)
{
int32_t		minDealy_microSecs;
char		stateString[48];

	CONSOLE_DEBUG("SHOULD NOT BE HERE");

//*	this function should be overloaded

	minDealy_microSecs			=	1000000;		//*	default to 1 second

	if (cShutterStatus != cPreviousShutterStatus)
	{
		Shutter_GetStatusString(cShutterStatus, stateString);
		LogEvent(	"shutter",
					"StateChanged",
					NULL,
					kASCOM_Err_Success,
					stateString);
		cPreviousShutterStatus	=	cShutterStatus;
	}
	return(minDealy_microSecs);
}


#pragma mark -

//*****************************************************************************
//*	valid commands
//*		http://127.0.0.1:6800/api/v1/shutter/altitude
//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::ProcessCommand(TYPE_GetPutRequestData *reqData)
{
int					cmdEnumValue;
int					cmdType;
TYPE_ASCOM_STATUS	alpacaErrCode;
char				alpacaErrMsg[256];
//int				myDeviceNum;
int				mySocket;

	CONSOLE_DEBUG_W_STR(__FUNCTION__, reqData->deviceCommand);


	//*	make local copies of the data structure to make the code easier to read
	mySocket	=	reqData->socket;
//	myDeviceNum	=	reqData->deviceNumber;

	strcpy(alpacaErrMsg, "");
	alpacaErrCode	=	kASCOM_Err_Success;

	//*	set up the json response
	JsonResponse_CreateHeader(reqData->jsonTextBuffer, kMaxJsonBuffLen);

	//*	this is not part of the protocol, I am using it for testing
	JsonResponse_Add_String(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"Device",
								cDeviceName,
								INCLUDE_COMMA);

	JsonResponse_Add_String(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"Command",
								reqData->deviceCommand,
								INCLUDE_COMMA);

//	JsonResponse_Add_Int32(		mySocket,
//								reqData->jsonTextBuffer,
//								kMaxJsonBuffLen,
//								"PreviousState",
//								domeState,
//								INCLUDE_COMMA);

	//*	look up the command
	cmdEnumValue	=	FindCmdFromTable(reqData->deviceCommand, gShutterCmdTable, &cmdType);
	switch(cmdEnumValue)
	{
		//----------------------------------------------------------------------------------------
		//*	Common commands that we want to over ride
		//----------------------------------------------------------------------------------------
		case kCmd_Common_supportedactions:	//*	Returns the list of action names supported by this driver.
			SendSupportedActions(reqData, gShutterCmdTable);
			break;

		//========================================================================================
		//*	shutter specific commands
		//========================================================================================
		case kCmd_Shutter_altitude:			//*	The dome altitude
			alpacaErrCode	=	Get_Altitude(reqData, alpacaErrMsg, gValueString);
			break;


		case kCmd_Shutter_cansetaltitude:		//*	Indicates whether the dome altitude can be set
			alpacaErrCode	=	Get_Cansetaltitude(reqData, alpacaErrMsg, gValueString);
			break;


		case kCmd_Shutter_cansetshutter:		//*	Indicates whether the dome shutter can be opened
			alpacaErrCode	=	Get_Cansetshutter(reqData, alpacaErrMsg, gValueString);
			break;


		//*	Returns the status of the dome shutter or roll-off roof. 0 = Open, 1 = Closed, 2 = Opening, 3 = Closing, 4 = Shutter status error
		case kCmd_Shutter_shutterstatus:		//*	Status of the dome shutter or roll-off roof
			alpacaErrCode	=	Get_Shutterstatus(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Shutter_abortslew:
		//	if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_StopShutter(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Shutter_closeshutter:			//*	Close the shutter or otherwise shield telescope from the sky.
		//	if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_CloseShutter(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Shutter_openshutter:			//*	Open shutter or otherwise expose telescope to the sky.
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_OpenShutter(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("Wrong format message, its should be PUT")
			}
			break;

		case kCmd_Shutter_slewtoaltitude:		//*	Slew the dome to the given altitude position.
			strcpy(alpacaErrMsg, "Command not implemented");
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			break;

		case kCmd_Shutter_stopshutter:			//*	Stop shutter movement
			alpacaErrCode	=	Put_StopShutter(reqData, alpacaErrMsg);
			break;

		case kCmd_Shutter_readall:
			alpacaErrCode	=	Get_Readall(reqData, alpacaErrMsg);
			break;

		//----------------------------------------------------------------------------------------
		//*	let anything undefined go to the common command processor
		//----------------------------------------------------------------------------------------
		default:
			alpacaErrCode	=	ProcessCommand_Common(reqData, cmdEnumValue);
			if (alpacaErrCode != 0)
			{
				strcpy(alpacaErrMsg, reqData->alpacaErrMsg);
			}
			break;
	}
	RecordCmdStats(cmdEnumValue, reqData->get_putIndicator, alpacaErrCode);

	//*	send the response information
	JsonResponse_Add_Int32(		mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"ClientTransactionID",
								gClientTransactionID,
								INCLUDE_COMMA);

	JsonResponse_Add_Int32(		mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"ServerTransactionID",
								gServerTransactionID,
								INCLUDE_COMMA);

	JsonResponse_Add_Int32(		mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"ErrorNumber",
								alpacaErrCode,
								INCLUDE_COMMA);


	JsonResponse_Add_String(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"ErrorMessage",
								alpacaErrMsg,
								NO_COMMA);

	JsonResponse_Add_Finish(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								kInclude_HTTP_Header);

	//*	this is for the logging function
	strcpy(reqData->alpacaErrMsg, alpacaErrMsg);
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Get_Altitude(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
	if (reqData != NULL)
	{
		JsonResponse_Add_Double(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									responseString,
									cAltitude_Dbl,
									INCLUDE_COMMA);

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Get_Cansetaltitude(	TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								false,
								INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Get_Cansetshutter(	TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								true,
								INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Get_Shutterstatus(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
char				statusString[32];
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								cShutterStatus,
								INCLUDE_COMMA);

//		CONSOLE_DEBUG_W_NUM("cShutterStatus=", cShutterStatus);
		Shutter_GetStatusString(cShutterStatus, statusString);
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"shutterstatus-string",
									statusString,
									INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Get_Slewing(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Put_AbortSlew(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}



//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Put_OpenShutter(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
bool				shutterOpenOK;
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	shutterOpenOK	=	OpenShutter();
	if (shutterOpenOK)
	{
		cShutterStatus	=	kShutterStatus_Opening;
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"OpenShutter",
									"OK",
									INCLUDE_COMMA);
	}
	else
	{
		cShutterStatus	=	kShutterStatus_Error;
		alpacaErrCode	=	kASCOM_Err_NotConnected;
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"OpenShutter",
									"failed",
									INCLUDE_COMMA);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Put_CloseShutter(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
bool				shutterCloseOK;
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	shutterCloseOK	=	CloseShutter();
	if (shutterCloseOK)
	{
		cShutterStatus	=	kShutterStatus_Closing;
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"CloseShutter",
									"OK",
									INCLUDE_COMMA);
	}
	else
	{
		cShutterStatus	=	kShutterStatus_Error;
		alpacaErrCode	=	kASCOM_Err_NotConnected;
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"CloseShutter",
									"failed",
									INCLUDE_COMMA);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Put_StopShutter(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
bool				shutterStopOK;
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	shutterStopOK	=	StopShutter();
	if (shutterStopOK)
	{
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"StopShutter",
									"OK",
									INCLUDE_COMMA);
	}
	else
	{
		cShutterStatus	=	kShutterStatus_Error;
		alpacaErrCode	=	kASCOM_Err_NotConnected;
		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"StopShutter",
									"failed",
									INCLUDE_COMMA);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	ShutterDriver::Get_Readall(	TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		//*	do the common ones first
		Get_Readall_Common(	reqData,	alpacaErrMsg);

		Get_Altitude(		reqData,	alpacaErrMsg,	"altitude");
		Get_Cansetaltitude(	reqData,	alpacaErrMsg,	"cansetaltitude");
		Get_Cansetshutter(	reqData,	alpacaErrMsg,	"cansetshutter");
		Get_Shutterstatus(	reqData,	alpacaErrMsg,	"shutterstatus");
		Get_Slewing(		reqData,	alpacaErrMsg,	"slewing");


		//===============================================================
		JsonResponse_Add_String(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"version",
								gFullVersionString,
								INCLUDE_COMMA);



		alpacaErrCode	=	kASCOM_Err_Success;
		strcpy(alpacaErrMsg, "");
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}





//*****************************************************************************
void	ShutterDriver::OutputHTML(TYPE_GetPutRequestData *reqData)
{
int			mySocketFD;
char		lineBuffer[128];

	CONSOLE_DEBUG(__FUNCTION__);


	mySocketFD	=	reqData->socket;

	SocketWriteData(mySocketFD,	"<CENTER>\r\n");

	SocketWriteData(mySocketFD,	"<TABLE BORDER=1>\r\n");

	//*-----------------------------------------------------------
	SocketWriteData(mySocketFD,	"<TR>\r\n");
	SocketWriteData(mySocketFD,	"\t<TD>Shutter status:</TD><TD>");
	Shutter_GetStatusString(cShutterStatus, lineBuffer);
	SocketWriteData(mySocketFD,	lineBuffer);
	SocketWriteData(mySocketFD,	"</TD></TR>\r\n");


	//*-----------------------------------------------------------
	sprintf(lineBuffer,	"\t<TR><TD>Altitude</TD><TD>%1.2f</TD></TR>\r\n",	cAltitude_Dbl);
	SocketWriteData(mySocketFD,	lineBuffer);


	SocketWriteData(mySocketFD,	"</TABLE>\r\n");
	SocketWriteData(mySocketFD,	"<P>\r\n");


	SocketWriteData(mySocketFD,	"</CENTER>\r\n");

	//*	now generate links to all of the commands
	GenerateHTMLcmdLinkTable(mySocketFD, "shutter", 0, gShutterCmdTable);

}


//*****************************************************************************
bool	ShutterDriver::GetCmdNameFromMyCmdTable(const int cmdNumber, char *comandName, char *getPut)
{
bool	foundIt;

	foundIt	=	GetCmdNameFromTable(cmdNumber, comandName, gShutterCmdTable, getPut);
	return(foundIt);
}

#pragma mark -

//*****************************************************************************
bool	ShutterDriver::OpenShutter(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
	return(false);
}


//*****************************************************************************
bool	ShutterDriver::CloseShutter(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
	return(false);
}


//*****************************************************************************
bool	ShutterDriver::StopShutter(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
	return(false);
}


//*****************************************************************************
void	Shutter_GetStatusString(const int status, char *statusString)
{
	switch(status)
	{
		case kShutterStatus_Open:		strcpy(statusString,	"Open");	break;
		case kShutterStatus_Closed:		strcpy(statusString,	"Closed");	break;
		case kShutterStatus_Opening:	strcpy(statusString,	"Opening");	break;
		case kShutterStatus_Closing:	strcpy(statusString,	"Closing");	break;
		case kShutterStatus_Error:		strcpy(statusString,	"Error");	break;

		case kShutterStatus_Unknown:
		default:						strcpy(statusString,	"unknown");	break;
	}
}


#endif	//	_ENABLE_DOME_