//**************************************************************************
//*	Name:			alpacadriver.cpp
//*
//*	Author:			Mark Sproul (C) 2019, 2020
//*					msproul@skychariot.com
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
//*	References:
//*		https://ascom-standards.org/api/#/Dome%20Specific%20Methods/get_dome__device_number__athome
//*		https://github.com/OpenPHDGuiding/phd2/tree/master/cameras/zwolibs
//*
//*		https://ascom-standards.org/Help/Developer/html/P_ASCOM_DriverAccess_Camera_ReadoutModes.htm
//*
//*		https://www.astro.louisville.edu/software/xmccd/archive/xmccd-4.1/xmccd-4.1e/docs/cfitsio/cfitsio.pdf
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul msproul@skychariot.com
//*****************************************************************************
//*	Apr  5,	2019	<MLS> Attended lecture by Bob Denny introducing Alpaca protocol
//*	Apr  9,	2019	<MLS> Created alpaca_driver.c
//*	Apr 12,	2019	<MLS> Parsing of the get/put command working
//*	Apr 12,	2019	<MLS> Added _ENABLE_CAMERA_
//*	Apr 12,	2019	<MLS> Added _ENABLE_DOME_
//*	Apr 12,	2019	<MLS> Added _ENABLE_FILTERWHEEL_
//*	Apr 12,	2019	<MLS> Added _ENABLE_FOCUSER_
//*	Apr 12,	2019	<MLS> Added _ENABLE_ROTATOR_
//*	Apr 12,	2019	<MLS> Added _ENABLE_TELESCOPE_
//*	Apr 12,	2019	<MLS> Started on Dome control
//*	Apr 12,	2019	<MLS> Added TYPE_CmdEntry structure
//*	Apr 14,	2019	<MLS> Added TYPE_DeviceTable structure
//*	Apr 14,	2019	<MLS> Added Common commands table
//*	Apr 14,	2019	<MLS> JSON response working
//*	Apr 15,	2019	<MLS> Added SocketWriteData()
//*	Apr 15,	2019	<MLS> Added SendSupportedActions()
//*	Apr 16,	2019	<MLS> Breaking up AlpacaCallback() to reduce complexity
//*	Apr 16,	2019	<MLS> Added ProcessGetPutRequest()
//*	Apr 18,	2019	<MLS> Downloaded ZWO Filter Wheel library
//*	May  1,	2019	<MLS> Switched to buffered Json output
//*	May  6,	2019	<MLS> Added threads
//*	May  6,	2019	<MLS> Added _ENABLE_OBSERVINGCONDITIONS_
//*	May  6,	2019	<MLS> Added _ENABLE_SAFETYMONITOR_
//*	May  7,	2019	<MLS> Added observingconditions parsing
//*	May 10,	2019	<MLS> Added GetTimeString()
//*	May 11,	2019	<MLS> Added _ENABLE_SWITCH_
//*	May 14,	2019	<MLS> Added GenerateHTMLcmdLinkTable()
//*	May 21,	2019	<MLS> Added logging
//*	May 22,	2019	<MLS> Added logging HTML output
//*	Aug 30,	2019	<MLS> Started on alpaca driver base class
//*	Oct  4,	2019	<MLS> cDeviceNum is now set correctly on class creation
//*	Oct  9,	2019	<MLS> Added ProcessCmdLineArgs()
//*	Oct 20,	2019	<MLS> Added processing for "setup"
//*	Oct 20,	2019	<MLS> Added processing for "management"
//*	Oct 20,	2019	<MLS> Added discovery protocol via another thread
//*	Nov  8,	2019	<MLS> Added GetKeyWordArgument()
//*	Nov  8,	2019	<MLS> Added millis()
//*	Nov 20,	2019	<MLS> Added CountDevicesByType()
//*	Nov 25,	2019	<MLS> Updated web output to support .png files
//*	Nov 27,	2019	<MLS> Added ProcessManagementRequest()
//*	Nov 27,	2019	<MLS> Added ToLowerStr() & StripTrailingSpaces()
//*	Dec  1,	2019	<MLS> Management commands are no longer logged
//*	Dec  6,	2019	<MLS> Spent the day at Naval Academy working with Jeff Larsen
//*	Dec 20,	2019	<MLS> Added version info to web output
//*	Dec 29,	2019	<MLS> Added -t cmd line option to specify the default telescope config
//*	Jan  1,	2020	<MLS> Added ability to set title for web page,
//*	Jan  1,	2020	<MLS> Moved OutputHTMLrowData() to base class
//*	Jan  3,	2020	<MLS> Added InitObsConditionGloblas()
//*	Jan 14,	2020	<MLS> Added Check_udev_rulesFile()
//*	Jan 18,	2020	<MLS> Added Read_OSreleaseVersion()
//*	Jan 19,	2020	<MLS> Added Read_CpuInfo()
//*	Jan 26,	2020	<MLS> Added _ENABLE_DISCOVERY_QUERRY_
//*	Feb  3,	2020	<MLS> Added gPlatformString
//*	Feb 10,	2020	<MLS> Added ExtractArgValue()
//*	Feb 18,	2020	<MLS> Added -q (quiet) -v (verbose default) options to command line
//*	Feb 22,	2020	<MLS> Changed command logging to only when there are errors
//*	Mar 27,	2020	<MLS> Added _ENABLE_QHY_
//*	Mar 28,	2020	<MLS> Removed device number from constructor, it is determined internally
//*	Mar 29,	2020	<MLS> Changed my alpaca port to 6800
//*	Mar 30,	2020	<MLS> Started working on passing Alpaca/ASCOM Conform testing
//*	Mar 31,	2020	<MLS> Added _DEBUG_CONFORM_
//*	Apr  2,	2020	<MLS> Rewrote ParseHTMLdataIntoReqStruct()
//*	Apr  3,	2020	<MLS> Added Command statistics RecordCmdStats()
//*	Apr  3,	2020	<MLS> Added OutputHTML_CmdStats()
//*	Apr  3,	2020	<MLS> Added GetCmdNameFromMyCmdTable() to all drivers
//*	Apr 20,	2020	<MLS> Added Get_Readall_Common()
//*	Apr 27,	2020	<MLS> Added Get_Readall_CPUstats()
//*	Apr 27,	2020	<MLS> Added CPU temp to readall output
//*	May  2,	2020	<MLS> Added Slit Tracker device
//*	May 19,	2020	<MLS> Added bogomips to cpustats
//*	Jun 15,	2020	<MLS> Added DumpRequestStructure()  (it got lost somewhere along the way)
//*	Jul  7,	2020	<MLS> Added compile flag _INCLUDE_WIRINGPI_
//*	Jul 16,	2020	<MLS> Compiling and running on 64bit Raspberry Pi OS
//*	Jul 20,	2020	<MLS> Fixed bug in GetKeyWordArgument(), returned garbage if no argument
//*	Jul 20,	2020	<MLS> Added Put_Connected(), AlpacaConnect() and AlpacaDisConnect()
//*	Jul 20,	2020	<MLS> Added "exit" command for clean shutdown of sony camera
//*	Sep  1,	2020	<MLS> Re-organized the case statements in all of the ProcessCommand() functions
//*	Nov 30,	2020	<MLS> All (TYPE_ASCOM_STATUS) type casts have been removed
//*	Dec  3,	2020	<MLS> First release of source code to outside
//*	Dec 12,	2020	<MLS> Started github repository https://github.com/msproul/AlpacaPi
//*	Dec 28,	2020	<MLS> Finished making all Alpaca error messages uniform
//*	Jan 10,	2020	<MLS> Changed SendSupportedActions() to Get_SupportedActions()
//*	Jan 10,	2020	<MLS> Pushed build 74 up to github
//*****************************************************************************

#include	<stdio.h>
#include	<stdlib.h>
#include	<ctype.h>
#include	<string.h>
#include	<sys/time.h>
#include	<sys/resource.h>
#include	<gnu/libc-version.h>
#include	<signal.h>


#ifdef _USE_OPENCV_
	#include "opencv/highgui.h"
	#include "opencv2/highgui/highgui_c.h"
#endif

#ifdef _ENABLE_FITS_
	#ifndef _FITSIO_H
		#include <fitsio.h>
	#endif // _FITSIO_H
#endif // _ENABLE_FITS_
#ifdef _ENABLE_JPEGLIB_
	#include	<jpeglib.h>
#endif


#include <pthread.h>

#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"

#include	"JsonResponse.h"
#include	"alpacadriver.h"
#include	"alpacadriver_helper.h"
#include	"eventlogging.h"
#include	"socket_listen.h"
#include	"discoverythread.h"
#include	"html_common.h"
#include	"observatory_settings.h"
#include	"obsconditions_globals.h"
#include	"cpu_stats.h"

//#define _DEBUG_CONFORM_


#define		kAlpacaListenPort	6800

uint32_t	gClientID				=	0;
uint32_t	gClientTransactionID	=	0;
uint32_t	gServerTransactionID	=	0;		//*	we are the server, we will increment this each time a transaction occurs
bool		gErrorLogging			=	false;	//*	write errors to log file if true
bool		gConformLogging			=	false;	//*	log all commands to log file to match up with Conform

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_ASI_)
	#include	"cameradriver_ASI.h"
#endif
#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_ATIK_)
	#include	"cameradriver_ATIK.h"
#endif
#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_TOUP_)
	#include	"cameradriver_TOUP.h"
#endif
#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_QHY_)
	#include	"cameradriver_QHY.h"
#endif

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_FLIR_)
	#include	"cameradriver_FLIR.h"
#endif
#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_SONY_)
	#include	"cameradriver_SONY.h"
#endif


#ifdef	_ENABLE_DOME_
//	#include	"domedriver.h"
	#include	"domedriver_rpi.h"
#endif
#ifdef	_ENABLE_ROR_
	#include	"domedriver_ror_rpi.h"
#endif


#ifdef _ENABLE_FILTERWHEEL_
	#include	"filterwheeldriver.h"
	#include	"filterwheeldriver_ZWO.h"
#endif

#ifdef _ENABLE_FOCUSER_
	#include	"focuserdriver.h"
	#include	"focuserdriver_nc.h"
#endif
#ifdef _ENABLE_MULTICAM_
	#include	"multicam.h"
#endif
#ifdef _ENABLE_SWITCH_
	#include	"switchdriver.h"
	#include	"switchdriver_rpi.h"
#endif
#ifdef _ENABLE_OBSERVINGCONDITIONS_
	#include	"obsconditionsdriver.h"
#endif
#if defined(_ENABLE_OBSERVINGCONDITIONS_) && defined(__arm__)
	#include	"obsconditionsdriver_rpi.h"
#endif

#ifdef _ENABLE_CALIBRATION_
	#include	"calibrationdriver.h"
	#include	"calibrationdriver_rpi.h"
#endif // _ENABLE_CALIBRATION_

#ifdef _ENABLE_TELESCOPE_
	#include	"telescopedriver.h"
	#include	"telescopedriver_lx200.h"
#endif // _ENABLE_TELESCOPE_


#ifdef _ENABLE_SHUTTER_
	#include	"shutterdriver_arduino.h"
#endif

#ifdef _ENABLE_SLIT_TRACKER_
	#include	"slittracker.h"
#endif // _ENABLE_SLIT_TRACKER_

#include	"managementdriver.h"

#if defined(__arm__) && defined(_INCLUDE_WIRINGPI_)
	#include <wiringPi.h>
#endif

AlpacaDriver		*gAlpacaDeviceList[kMaxDevices];
bool				gKeepRunning				=	true;
int					gDeviceCnt					=	0;
bool				gLiveView					=	false;
bool				gAutoExposure				=	false;
bool				gDisplayImage				=	false;
bool				gVerbose					=	true;
const char			gValueString[]				=	"Value";
char				gDefaultTelescopeRefID[kDefaultRefIdMaxLen]	=	"";
char				gWebTitle[80]				=	"AlpacaPi";
char				gOsReleaseString[64]		=	"";
char				gCpuInfoString[64]			=	"";
char				gPlatformString[64]			=	"";
double				gBogoMipsValue				=	0.0;
char				gFullVersionString[128];

//*****************************************************************************
static void	InitDeviceList(void)
{
int		ii;

	for (ii=0; ii<kMaxDevices; ii++)
	{
		gAlpacaDeviceList[ii]	=	NULL;
	}
	gDeviceCnt	=	0;
}


//*****************************************************************************
const TYPE_CmdEntry	gCommonCmdTable[]	=
{

	{	"action",				kCmd_Common_action,				kCmdType_PUT	},
	{	"commandblind",			kCmd_Common_commandblind,		kCmdType_PUT	},
	{	"commandbool",			kCmd_Common_commandbool,		kCmdType_PUT	},
	{	"commandstring",		kCmd_Common_commandstring,		kCmdType_PUT	},
	{	"connected",			kCmd_Common_connected,			kCmdType_BOTH	},
	{	"description",			kCmd_Common_description,		kCmdType_GET	},
	{	"driverinfo",			kCmd_Common_driverinfo,			kCmdType_GET	},
	{	"driverversion",		kCmd_Common_driverversion,		kCmdType_GET	},
	{	"interfaceversion",		kCmd_Common_interfaceversion,	kCmdType_GET	},
	{	"name",					kCmd_Common_name,				kCmdType_GET	},
	{	"supportedactions",		kCmd_Common_supportedactions,	kCmdType_GET	},

#ifdef _INCLUDE_EXIT_COMMAND_
	//*	the exit command was implemented for a special case application, it is not intended
	//*	to be used in the normal astronomy community
	{	"exit",					kCmd_Common_exit,				kCmdType_GET	},
#endif // _INCLUDE_EXIT_COMMAND_

	{	"",						-1,	0x00	}
};



//*****************************************************************************
bool	GetCmdNameFromTable(const int cmdNumber, char *comandName, const TYPE_CmdEntry *cmdTable, char *getPut)
{
int		iii;
bool	foundIt;

	strcpy(comandName, "????");
	foundIt	=	false;
	iii		=	0;
	while ((foundIt == false) && (cmdTable[iii].commandName[0] > 0x20))
	{
		if (cmdNumber == cmdTable[iii].enumValue)
		{
			strcpy(comandName, cmdTable[iii].commandName);
			*getPut	=	cmdTable[iii].get_put;
			foundIt	=	true;
		}
		iii++;
	}
	return(foundIt);
}


//*****************************************************************************
AlpacaDriver::AlpacaDriver(TYPE_DEVICETYPE argDeviceType)
{
int		alpacaDeviceNum;
int		ii;

//	CONSOLE_DEBUG("---------------------------------------");
//	CONSOLE_DEBUG_W_NUM(__FUNCTION__, argDeviceType);

	cMagicCookie				=	kMagicCookieValue;
	cDeviceConnected			=	true;
	cDriverVersion				=	1;
	cDeviceName[0]				=	0;
	cDeviceModel[0]				=	0;
	cDeviceManufacturer[0]		=	0;
	cDeviceManufAbrev[0]		=	0;
	cDeviceDescription[0]		=	0;
	cDeviceSerialNum[0]			=	0;
	cDeviceVersion[0]			=	0;
	cDriverversionStr[0]		=	0;
	cDeviceFirmwareVersStr[0]	=	0;

	cUniqueID.part1				=	'ALPA';				//*	4 byte manufacturer code
	cUniqueID.part2				=	kBuildNumber;		//*	software version number
	cUniqueID.part3				=	1;					//*	model number
	cUniqueID.part4				=	0;
	cUniqueID.part5				=	0;					//*	serial number

	//========================================
	//*	discovery stuff
	cDiscoveryThreadActive		=	false;
	cDiscoveryThreadID			=	0;
	cBroadcastSocket			=	-1;
	cDiscoveryCount				=	0;

	//*	add the device to the list
	cDeviceType	=	argDeviceType;
	//*	we have to figure out which index this devices is for this device type
	alpacaDeviceNum	=	0;
	for (ii=0; ii<gDeviceCnt; ii++)
	{
		//*	check to see if there are any other devices of this type
		if (argDeviceType == gAlpacaDeviceList[ii]->cDeviceType)
		{
			alpacaDeviceNum++;
		}
	}
	cDeviceNum		=	alpacaDeviceNum;
//	CONSOLE_DEBUG_W_NUM("cDeviceNum\t=", cDeviceNum);

	strcpy(cDeviceName, "unknown");

	if (gDeviceCnt < kMaxDevices)
	{
		gAlpacaDeviceList[gDeviceCnt]	=	this;
		gDeviceCnt++;
	}

	//*	command statistics
	for (ii=0; ii<kCommonCmdCnt; ii++)
	{
		memset(&cCommonCMdStats[ii], 0, sizeof(TYPE_CMD_STATS));
	}
	for (ii=0; ii<kDeviceCmdCnt; ii++)
	{
		memset(&cDeviceCMdStats[ii], 0, sizeof(TYPE_CMD_STATS));
	}
	GetAlpacaName(argDeviceType, cAlpacaName);
	LogEvent(	cAlpacaName,
				"Created",
				NULL,
				kASCOM_Err_Success,
				"");
}

//**************************************************************************************
// Destructor
//**************************************************************************************
AlpacaDriver::~AlpacaDriver( void )
{
int	ii;

	cMagicCookie	=	0;

	CONSOLE_DEBUG(__FUNCTION__);
	//*	remove this device from the list
	for (ii=0; ii<kMaxDevices; ii++)
	{
		if (gAlpacaDeviceList[ii] == this)
		{
			gAlpacaDeviceList[ii]	=	NULL;
		}
	}

}

//**************************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::ProcessCommand(TYPE_GetPutRequestData *reqData)
{
TYPE_ASCOM_STATUS		alpacaErrCode;

	CONSOLE_DEBUG("We should not be here, this routine needs to be over-ridden");
	//*	do nothing, this routine should be overridden
	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	return(alpacaErrCode);

}

//**************************************************************************************
TYPE_ASCOM_STATUS		AlpacaDriver::ProcessCommand_Common(	TYPE_GetPutRequestData	*reqData,
																const int				cmdEnum,
																char					*alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode;
int					mySocket;

//	CONSOLE_DEBUG(__FUNCTION__);

	mySocket		=	reqData->socket;
	alpacaErrCode	=	kASCOM_Err_Success;
	switch(cmdEnum)
	{
		//========================================================================================
		//*	Common commands
		//========================================================================================
		case kCmd_Common_action:			//*	Invokes the specified device-specific action.
		case kCmd_Common_commandblind:		//*	Transmits an arbitrary string to the device
		case kCmd_Common_commandbool:		//*	Transmits an arbitrary string to the device and returns a boolean value from the device.
		case kCmd_Common_commandstring:		//*	Transmits an arbitrary string to the device and returns a string value from the device.
			JsonResponse_Add_String(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									"",
									INCLUDE_COMMA);
			break;

		case kCmd_Common_connected:			//*	GET--Retrieves the connected state of the device
											//*	PUT--Sets the connected state of the device
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_Connected(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Connected(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Common_description:		//*	Device description
			alpacaErrCode	=	Get_Description(reqData, alpacaErrMsg, gValueString);
			break;


		case kCmd_Common_driverinfo:		//*	Device driver description
			alpacaErrCode	=	Get_Driverinfo(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Common_driverversion:		//*	Driver Version
			alpacaErrCode	=	Get_Driverversion(reqData, alpacaErrMsg, gValueString);
			break;

		//*	The ASCOM Device interface version number that this device supports.
		case kCmd_Common_interfaceversion:
			alpacaErrCode	=	Get_Interfaceversion(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Common_name:				//*	Device name
			alpacaErrCode	=	Get_Name(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Common_supportedactions:	//*	Returns the list of action names supported by this driver.
			alpacaErrCode	=	Get_SupportedActions(reqData, NULL);
			break;

#ifdef _INCLUDE_EXIT_COMMAND_
		//*	the exit command was implemented for a special case application, it is not intended
		//*	to be used in the normal astronomy community
		case kCmd_Common_exit:
			gKeepRunning	=	false;
			break;
#endif // _INCLUDE_EXIT_COMMAND_

		default:
			alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Unrecognized command");
			CONSOLE_DEBUG(alpacaErrMsg);
			strcpy(reqData->alpacaErrMsg, alpacaErrMsg);
			break;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Connected(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

	JsonResponse_Add_Bool(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							cDeviceConnected,
							INCLUDE_COMMA);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Put_Connected(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;
bool	foundKeyWord;
bool	connectFlag;
char	argumentString[32];

	foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
											"Connected",
											argumentString,
											(sizeof(argumentString) -1));
	if (foundKeyWord)
	{
		connectFlag	=	IsTrueFalse(argumentString);
		if (connectFlag)
		{
			AlpacaConnect();
		}
		else
		{
			AlpacaDisConnect();
		}
	}

	JsonResponse_Add_Bool(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							gValueString,
							true,
							INCLUDE_COMMA);

	return(alpacaErrCode);
}

//*****************************************************************************
void	AlpacaDriver::AlpacaConnect(void)
{
//	CONSOLE_DEBUG(__FUNCTION__);
}

//*****************************************************************************
void	AlpacaDriver::AlpacaDisConnect(void)
{
//	CONSOLE_DEBUG(__FUNCTION__);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Description(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

#ifdef _DEBUG_CONFORM_
//	CONSOLE_DEBUG_W_LONG("description length\t=", strlen(cDeviceDescription))
//	CONSOLE_DEBUG_W_STR("cDeviceDescription\t=", cDeviceDescription)
#endif // _DEBUG_CONFORM_

	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							cDeviceDescription,
							INCLUDE_COMMA);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Driverinfo(			TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							"AlpacaPi C++ Open source by Mark Sproul, msproul@skychariot.com",
							INCLUDE_COMMA);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Driverversion(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							kVersionString,
							INCLUDE_COMMA);

	if (strlen(cDriverversionStr) > 0)
	{
		JsonResponse_Add_String(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"libraryVersion",
								cDriverversionStr,
								INCLUDE_COMMA);
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Interfaceversion(	TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
	JsonResponse_Add_Int32(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							cDriverVersion,
							INCLUDE_COMMA);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Name(				TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
//	CONSOLE_DEBUG_W_STR("cDeviceName\t=", cDeviceName)
	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							cDeviceName,
							INCLUDE_COMMA);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Readall_Common(				TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
//	CONSOLE_DEBUG(__FUNCTION__);
	Get_Connected(			reqData, alpacaErrMsg, "connected");
	Get_Description(		reqData, alpacaErrMsg, "description");
	Get_Driverinfo(			reqData, alpacaErrMsg, "driverinfo");
	Get_Driverversion(		reqData, alpacaErrMsg, "driverversion");
	Get_Interfaceversion(	reqData, alpacaErrMsg, "interfaceversion");
	Get_Name(				reqData, alpacaErrMsg, "name");

//	CONSOLE_DEBUG("exit");
	return(kASCOM_Err_Success);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_Readall_CPUstats(				TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
double		cpuTemp_DegC;
double		cpuTemp_DegF;
uint32_t	upTime;
char		textBuff[128];
int			upTime_Seconds;
int			upTime_Minutes;
int			upTime_Hours;
int			upTime_Days;
int			ram_Megabytes;
double		freeDiskSpace_Gigs;

	CONSOLE_DEBUG(__FUNCTION__);
	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"platform",
							gPlatformString,
							INCLUDE_COMMA);

	if (gBogoMipsValue > 1.0)
	{
		JsonResponse_Add_Double(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"bogomips",
								gBogoMipsValue,
								INCLUDE_COMMA);
	}
	//====================================================
	cpuTemp_DegC	=	CPUstats_GetTemperature(NULL);
	cpuTemp_DegF	=	((cpuTemp_DegC * (9.0/5.0)) + 32);
	JsonResponse_Add_Double(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"cpuTemp_DegC",
							cpuTemp_DegC,
							INCLUDE_COMMA);

	JsonResponse_Add_Double(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"cpuTemp_DegF",
							cpuTemp_DegF,
							INCLUDE_COMMA);

	//====================================================
	upTime	=	CPUstats_GetUptime();

	JsonResponse_Add_Int32(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"uptime_secs",
							upTime,
							INCLUDE_COMMA);

	upTime_Days		=	upTime / (24 * 60 * 60);
	upTime_Hours	=	(upTime / (60 * 60)) - (upTime_Days * 24);
	upTime_Minutes	=	(upTime / 60) - (upTime_Days * 24 * 60)- (upTime_Hours * 60);
	upTime_Seconds	=	upTime % 60;
	sprintf(textBuff, "%d days %02d:%02d:%02d",	upTime_Days,
												upTime_Hours,
												upTime_Minutes,
												upTime_Seconds);
	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"uptime_days",
							textBuff,
							INCLUDE_COMMA);

	//====================================================
	ram_Megabytes	=	CPUstats_GetTotalRam();
	JsonResponse_Add_Int32(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"totalRam_Megabytes",
							ram_Megabytes,
							INCLUDE_COMMA);
	//====================================================
	ram_Megabytes	=	CPUstats_GetFreeRam();
	JsonResponse_Add_Int32(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"freeRam_Megabytes",
							ram_Megabytes,
							INCLUDE_COMMA);

	//====================================================
	freeDiskSpace_Gigs	=	CPUstats_GetFreeDiskSpace("/") / 1024.0;
//	CONSOLE_DEBUG_W_DBL("freeDiskSpace_Gigs\t=", freeDiskSpace_Gigs);

	JsonResponse_Add_Double(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"freeDisk_Gigabytes",
							freeDiskSpace_Gigs,
							INCLUDE_COMMA);

	return(kASCOM_Err_Success);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	AlpacaDriver::Get_SupportedActions(TYPE_GetPutRequestData *reqData, const TYPE_CmdEntry *theCmdTable)
{
int		iii;
char	lineBuffer[256];
int		mySocketFD;

	mySocketFD	=	reqData->socket;

	JsonResponse_Add_RawText(mySocketFD,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"\t\"Value\": \r\n\t[\r\n");

	//*	fist list the common commands
	iii		=	0;
	while (gCommonCmdTable[iii].commandName[0] != 0)
	{
		strcpy(lineBuffer, "\t\t\"");
		strcat(lineBuffer, gCommonCmdTable[iii].commandName);
		strcat(lineBuffer, "\"");
		strcat(lineBuffer, ",");
		strcat(lineBuffer, "\r\n");

		JsonResponse_Add_RawText(mySocketFD,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								lineBuffer);
		iii++;
	}

	//*	output the commands for the specific device
	if (theCmdTable != NULL)
	{
		iii		=	0;
		while (theCmdTable[iii].commandName[0] != 0)
		{
			//*	dont print out the entry if it begins with "-"
			if (theCmdTable[iii].commandName[0] != '-')
			{
				strcpy(lineBuffer, "\t\t\"");
				strcat(lineBuffer, theCmdTable[iii].commandName);
				strcat(lineBuffer, "\"");
				//*	do we need a comma?  The last one does not get a comma
				if (theCmdTable[iii+1].commandName[0] != 0)
				{
					strcat(lineBuffer, ",");
				}
				strcat(lineBuffer, "\r\n");

				JsonResponse_Add_RawText(mySocketFD,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										lineBuffer);
			}
			iii++;
		}
	}
	JsonResponse_Add_RawText(mySocketFD,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"\t],\r\n");
	return(kASCOM_Err_Success);
}


//*****************************************************************************
void	AlpacaDriver::OutputHTML(TYPE_GetPutRequestData *reqData)
{
//	CONSOLE_DEBUG(__FUNCTION__);
}

//*****************************************************************************
void	AlpacaDriver::OutputHTML_Part2(TYPE_GetPutRequestData *reqData)
{
//	CONSOLE_DEBUG(__FUNCTION__);
}

//*****************************************************************************
bool	AlpacaDriver::GetCmdNameFromMyCmdTable(const int cmdNumber, char *comandName, char *getPut)
{
	CONSOLE_DEBUG("This function should be over-ridden");
//	GetCmdNameFromMyCmdTable(cmdNumber, cmdName, gCommonCmdTable, getPut);
	strcpy(comandName, "----");
	exit(0);
	return(false);

}

//*****************************************************************************
static void	GenerateCmdStatsEntry(	const char	*cmdName,
									const char	getPutIndicator,
									const int	connCnt,
									const int	getCnt,
									const int	putCnt,
									const int	errorCnt,
									char		*lineBuffer)
{
char	getBuff[64];
char	putBuff[64];
bool	getPutErr;

	if (strncmp(cmdName, "--extra", 7) == 0)
	{
		strcpy(lineBuffer, "<TR><TD COLSPAN=5><CENTER>--Extra non-Alpaca--</TD></TR>");
	}
	else
	{
		getPutErr	=	false;
		//*	Get count
		if ((getPutIndicator == 'G') || (getPutIndicator == 'B'))
		{
			sprintf(getBuff, "<TD><CENTER>%d</TD>", getCnt);
		}
		else if (getCnt > 0)
		{
			//*	this is an error,
			sprintf(getBuff, "<TD><CENTER><FONT COLOR=RED>%d</TD>", getCnt);
			getPutErr	=	true;
		}
		else
		{
			strcpy(getBuff, "<TD><CENTER>--</TD>");
		}

		//*	Put count
		if ((getPutIndicator == 'P') || (getPutIndicator == 'B'))
		{
			sprintf(putBuff, "<TD><CENTER>%d</TD>", putCnt);
		}
		else if (putCnt > 0)
		{
			//*	this is an error,
			sprintf(getBuff, "<TD><CENTER><FONT COLOR=RED>%d</TD>", putCnt);
			getPutErr	=	true;
		}
		else
		{
			strcpy(putBuff, "<TD><CENTER>--</TD>");
		}
		sprintf(lineBuffer,	"<TR><TD>%s</TD><TD><CENTER>%d</TD>%s%s<TD><CENTER>%d</TD>",
								cmdName,
								connCnt,
								getBuff,
								putBuff,
								errorCnt);
	#define	_SHOW_GETPUT_ERRORS
	#ifdef _SHOW_GETPUT_ERRORS
		if (getPutErr)
		{
			strcat(lineBuffer, "<TD><FONT COLOR=RED><<-Error</TD>\r\n");
		}
		strcat(lineBuffer, "</TR>\r\n");
	#endif // _SHOW_GETPUT_ERRORS
	}
}

//*****************************************************************************
void	AlpacaDriver::OutputHTML_CmdStats(TYPE_GetPutRequestData *reqData)
{
char	lineBuffer[512];
//char	getBuff[64];
//char	putBuff[64];
int		mySocketFD;
int		iii;
char	cmdName[32];
bool	foundIt;
int		total_Conn;
int		total_Get;
int		total_Put;
int		total_Errors;
char	getPutIndicator;

//	CONSOLE_DEBUG_W_STR(__FUNCTION__, cDeviceName);

	mySocketFD	=	reqData->socket;

	SocketWriteData(mySocketFD,	"<CENTER>\r\n");

	sprintf(lineBuffer, "%s<BR>\r\n", cDeviceName);
	SocketWriteData(mySocketFD,	lineBuffer);
	SocketWriteData(mySocketFD,	"<TABLE BORDER=1>\r\n");

	sprintf(lineBuffer,	"<TR><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\r\n",
							"Command",
							"Total #",
							"Get #",
							"Put #",
							"Errors");

	SocketWriteData(mySocketFD,	lineBuffer);
	total_Conn		=	0;
	total_Get		=	0;
	total_Put		=	0;
	total_Errors	=	0;

	//===============================================================
	//*	first do the common commands
	strcpy(cmdName, "???");
	for (iii=0; iii<kCommonCmdCnt; iii++)
	{
		//*	get the info about this command
		foundIt	=	GetCmdNameFromTable((kCmd_Common_action + iii),
										cmdName,
										gCommonCmdTable,
										&getPutIndicator);
		if (foundIt)
		{
			GenerateCmdStatsEntry(	cmdName,
									getPutIndicator,
									cCommonCMdStats[iii].connCnt,
									cCommonCMdStats[iii].getCnt,
									cCommonCMdStats[iii].putCnt,
									cCommonCMdStats[iii].errorCnt,
									lineBuffer);

			SocketWriteData(mySocketFD,	lineBuffer);

			total_Conn		+=	cCommonCMdStats[iii].connCnt;
			total_Get		+=	cCommonCMdStats[iii].getCnt;
			total_Put		+=	cCommonCMdStats[iii].putCnt;
			total_Errors	+=	cCommonCMdStats[iii].errorCnt;
		}
	}

	SocketWriteData(mySocketFD,	"<TR><TD COLSPAN=5><HR></TD></TR>\r\n");

	//===============================================================
	//*	now do the commands for this device
	for (iii=0; iii<kDeviceCmdCnt; iii++)
	{
		//*	get the info about this command
		foundIt	=	GetCmdNameFromMyCmdTable(iii, cmdName, &getPutIndicator);
		if (foundIt)
		{
			GenerateCmdStatsEntry(	cmdName,
									getPutIndicator,
									cDeviceCMdStats[iii].connCnt,
									cDeviceCMdStats[iii].getCnt,
									cDeviceCMdStats[iii].putCnt,
									cDeviceCMdStats[iii].errorCnt,
									lineBuffer);

			SocketWriteData(mySocketFD,	lineBuffer);

			total_Conn		+=	cDeviceCMdStats[iii].connCnt;
			total_Get		+=	cDeviceCMdStats[iii].getCnt;
			total_Put		+=	cDeviceCMdStats[iii].putCnt;
			total_Errors	+=	cDeviceCMdStats[iii].errorCnt;
		}
	}
	SocketWriteData(mySocketFD,	"<TR><TD COLSPAN=5><HR></TD></TR>\r\n");
	sprintf(lineBuffer,	"<TR><TH>%s</TH><TH>%d</TH><TH>%d</TH><TH>%d</TH><TH>%d</TH></TR>\r\n",
							"Totals",
							total_Conn,
							total_Get,
							total_Put,
							total_Errors);
	SocketWriteData(mySocketFD,	lineBuffer);


	SocketWriteData(mySocketFD,	"</TABLE>\r\n");
	SocketWriteData(mySocketFD,	"</CENTER>\r\n");
	SocketWriteData(mySocketFD,	"<P>\r\n");

}


//*****************************************************************************
void	AlpacaDriver::OutputHTMLrowData(int socketFD, const char *string1, const char *string2)
{
	SocketWriteData(socketFD,	"<TR>\r\n\t<TD>");
	SocketWriteData(socketFD,	string1);
	SocketWriteData(socketFD,	"</TD>\r\n\t<TD>");
	SocketWriteData(socketFD,	string2);
	SocketWriteData(socketFD,	"</TD>\r\n</TR>\r\n");

}


//*****************************************************************************
int32_t	AlpacaDriver::RunStateMachine(void)
{
	return(5 * 1000 * 1000);
}


//*****************************************************************************
void	AlpacaDriver::RecordCmdStats(int cmdNum, char getput, TYPE_ASCOM_STATUS alpacaErrCode)
{
int		tblIdx;

	//*	check for common command index ( > 1000)
	if (cmdNum >= kCmd_Common_action)
	{
		tblIdx	=	cmdNum - kCmd_Common_action;
		if ((tblIdx >= 0) && (tblIdx < kCommonCmdCnt))
		{
			cCommonCMdStats[tblIdx].connCnt++;
			if (getput == 'G')
			{
				cCommonCMdStats[tblIdx].getCnt++;
			}
			else if (getput == 'P')
			{
				cCommonCMdStats[tblIdx].putCnt++;
			}

			if (alpacaErrCode != 0)
			{
				cCommonCMdStats[tblIdx].errorCnt++;
			}
		}
	}
	else if ((cmdNum >= 0) && (cmdNum < kDeviceCmdCnt))
	{
		cDeviceCMdStats[cmdNum].connCnt++;
		if (getput == 'G')
		{
			cDeviceCMdStats[cmdNum].getCnt++;
		}
		else if (getput == 'P')
		{
			cDeviceCMdStats[cmdNum].putCnt++;
		}

		if (alpacaErrCode != 0)
		{
			cDeviceCMdStats[cmdNum].errorCnt++;
		}
	}
	else
	{
		CONSOLE_DEBUG("Command index out of bounds");
	}
}


#pragma mark -

//*****************************************************************************
int	SocketWriteData(const int socket, const char *dataBuffer)
{
int		bufferLen;
int		bytesWritten;

#ifdef _DEBUG_CONFORM_
//	CONSOLE_DEBUG_W_STR("socket>\t", dataBuffer);
#endif // _DEBUG_CONFORM_

	bufferLen		=	strlen(dataBuffer);
	bytesWritten	=	write(socket, dataBuffer, bufferLen);
	if (bytesWritten < 0)
	{
	//	fprintf(stderr, "ERROR writing to socket");
		CONSOLE_ERROR("ERROR writing to socket");
		CONSOLE_ERROR_W_NUM("bytesWritten\t=", bytesWritten);
	}
	return(bytesWritten);
}



//*******************************************************
TYPE_DeviceTable	gDeviceTable[]	=
{
	{	"Camera",				kDeviceType_Camera				},
	{	"Dome",					kDeviceType_Dome				},
	{	"Filterwheel",			kDeviceType_Filterwheel			},
	{	"Focuser",				kDeviceType_Focuser				},
	{	"Management",			kDeviceType_Management			},
	{	"Observingconditions",	kDeviceType_Observingconditions	},
	{	"Rotator",				kDeviceType_Rotator				},
	{	"SafetyMonitor",		kDeviceType_SafetyMonitor		},
	{	"Shutter",				kDeviceType_Shutter				},
	{	"Switch",				kDeviceType_Switch				},
	{	"Telescope",			kDeviceType_Telescope			},
	{	"CoverCalibrator",		kDeviceType_CoverCalibrator		},

	//*	extras defined by MLS
	{	"Multicam",				kDeviceType_Multicam			},
	{	"SlitTracker",			kDeviceType_SlitTracker			},

	{	"",				-1						}
};



//*****************************************************************************
void	GetDeviceTypeFromEnum(const int deviceEnum, char *deviceTypeString)
{
int		ii;
	ii	=	0;
	while ((gDeviceTable[ii].deviceType[0] > 0x20 ) && (gDeviceTable[ii].enumValue >= 0))
	{
		if (deviceEnum == gDeviceTable[ii].enumValue)
		{
			strcpy(deviceTypeString, gDeviceTable[ii].deviceType);
		}
		ii++;
	}
}

//*****************************************************************************
static int	FindDeviceTypeByString(const char *deviceTypeStr)
{
int		ii;
int		enumValue;

	enumValue	=	-1;
	ii				=	0;
	while ((gDeviceTable[ii].deviceType[0] != 0) && (enumValue < 0))
	{
		if (strcasecmp(deviceTypeStr, gDeviceTable[ii].deviceType) == 0)
		{
			enumValue	=	gDeviceTable[ii].enumValue;
		}
		ii++;
	}
	return(enumValue);
}

//*****************************************************************************
static void	StrcpyToEOL(char *newString, const char *oldString, const int maxLen)
{
int		ii;

	ii	=	0;
	while ((oldString[ii] >= 0x20) && (ii < maxLen))
	{
		newString[ii]	=	oldString[ii];
		ii++;
	}
	if (ii < maxLen)
	{
		newString[ii]	=	0;
	}
	else
	{
		newString[maxLen-1]	=	0;
	}

}

//*****************************************************************************
void	GetTimeString(char *timeString)
{
time_t		currentTime;
struct tm	*linuxTime;

	if (timeString != NULL)
	{
		timeString[0]	=	0;
		currentTime		=	time(NULL);
		if (currentTime != -1)
		{
			linuxTime		=	localtime(&currentTime);
			sprintf(timeString, "%d/%d/%d %02d:%02d:%02d",
									(1 + linuxTime->tm_mon),
									linuxTime->tm_mday,
									(1900 + linuxTime->tm_year),
									linuxTime->tm_hour,
									linuxTime->tm_min,
									linuxTime->tm_sec);
		}
	}
}

//*****************************************************************************
const char	gBadResponse400[]	=
{
	"HTTP/1.0 400 Bad Request\r\n"
	"Server: AkamaiGHost\r\n"
	"Mime-Version: 1.0\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 207\r\n"
	"Connection: close\r\n"
	"\r\n"
	"<!DOCTYPE html>\r\n"
	"<HTML><HEAD>\r\n"
	"<TITLE>Invalid URL</TITLE>\r\n"
	"</HEAD><BODY>\r\n"
	"<H1>Invalid URL</H1>\r\n"
	"The requested URL , is invalid.<p>\r\n"
//	"Reference&#32;&#35;9&#46;d0fb4317&#46;1555413904&#46;e81982\r\n"
	"</BODY></HTML>\r\n"
};

//*****************************************************************************
const char	gHtmlHeader[]	=
{
	"HTTP/1.0 200 \r\n"
//	"Server: alpaca\r\n"
//	"Mime-Version: 1.0\r\n"
	"Content-Type: text/html\r\n"
	"Connection: close\r\n"
	"\r\n"
	"<!DOCTYPE html>\r\n"
	"<HTML><HEAD>\r\n"



};

#define _USE_BLACK_HTML_
//*****************************************************************************
const char	gHtmlNightMode[]	=
{
#ifdef _USE_BLACK_HTML_
//	"<BODY bgcolor=#111111>\r\n"
	"<BODY bgcolor=#000000>\r\n"
	"<FONT COLOR=white>\r\n"
	"<BODY link=red>\r\n"
	"<BODY vlink=pink>\r\n"
#else
//	"<BODY bgcolor=#111111>\r\n"
	"<BODY bgcolor=#40098E>\r\n"
//	"<BODY bgcolor=#222222>\r\n"
	"<FONT COLOR=white>\r\n"
	"<BODY link=red>\r\n"
//	"<BODY vlink=pink>\r\n"
	"<BODY vlink=green>\r\n"
#endif

};



#pragma mark -
//*****************************************************************************
static void	SendHtmlResponse(TYPE_GetPutRequestData *reqData)
{
char	lineBuffer[256];
char	separaterLine[]	=	"<HR SIZE=4 COLOR=RED>\r\n";
//char	separaterLine[]	=	"<HR SIZE=4 COLOR=BLUE>\r\n";
int		mySocketFD;
int		ii;


	if (reqData != NULL)
	{
		mySocketFD	=	reqData->socket;

		SocketWriteData(mySocketFD,	gHtmlHeader);

		sprintf(lineBuffer, "<TITLE>%s</TITLE>\r\n", gWebTitle);
		SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	gHtmlNightMode);
		SocketWriteData(mySocketFD,	"</HEAD><BODY>\r\n<CENTER>\r\n");
		SocketWriteData(mySocketFD,	"<H1>Alpaca device driver Web server</H1>\r\n");
		sprintf(lineBuffer, "<H3>%s</H3>\r\n", gWebTitle);
		SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</CENTER>\r\n");

		OutPutObservatoryInfoHTML(mySocketFD);

		SocketWriteData(mySocketFD,	"<A HREF=../log target=log>Click here for log</A><BR>\r\n");
		SocketWriteData(mySocketFD,	"<A HREF=../stats target=log>Click here for stats</A><BR>\r\n");

	#ifdef	_ENABLE_CAMERA_
		SocketWriteData(mySocketFD,	"Camera support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_DOME_
		SocketWriteData(mySocketFD,	"Dome support is enabled<BR>\r\n");
	#endif
	#ifdef	_ENABLE_ROR_
		SocketWriteData(mySocketFD,	"ROR support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_FILTERWHEEL_
		SocketWriteData(mySocketFD,	"Filterwheel support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_FOCUSER_
		SocketWriteData(mySocketFD,	"Focuser support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_ROTATOR_
		SocketWriteData(mySocketFD,	"Rotator support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_TELESCOPE_
		SocketWriteData(mySocketFD,	"Telescope support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_OBSERVINGCONDITIONS_
		SocketWriteData(mySocketFD,	"Observingconditions support is enabled<BR>\r\n");
	#endif
	#ifdef _ENABLE_CALIBRATION_
		SocketWriteData(mySocketFD,	"CoverCalibration support is enabled<BR>\r\n");
	#endif // _ENABLE_CALIBRATION_

	#ifdef	_ENABLE_SAFETYMONITOR_
		SocketWriteData(mySocketFD,	"SafetyMonitor support is enabled<BR>\r\n");
	#endif

	#ifdef	_ENABLE_SWITCH_
		SocketWriteData(mySocketFD,	"Switch support is enabled<BR>\r\n");
	#endif
	#ifdef _ENABLE_MULTICAM_
		SocketWriteData(mySocketFD,	"Multicam is enabled<BR>\r\n");
	#endif
	#ifdef _ENABLE_SHUTTER_
		SocketWriteData(mySocketFD,	"Shutter is enabled<BR>\r\n");
	#endif
	#ifdef _ENABLE_SLIT_TRACKER_
		SocketWriteData(mySocketFD,	"Slit Tracker is enabled<BR>\r\n");
	#endif // _ENABLE_SLIT_TRACKER_

		SocketWriteData(mySocketFD,	separaterLine);
		//=============================================================================
		//*	print out a table of active devices
		SocketWriteData(mySocketFD,	"<CENTER>\r\n");


		SocketWriteData(mySocketFD,	"The following devices are configured on this Remote Server:<P>\r\n");
		SocketWriteData(mySocketFD,	"<TABLE BORDER=2>\r\n");
		//*	do the header row
		SocketWriteData(mySocketFD,	"\t<TR>\r\n");
		SocketWriteData(mySocketFD,	"\t\t<TH><FONT COLOR=yellow>Device Type</TH>\r\n");
		SocketWriteData(mySocketFD,	"\t\t<TH><FONT COLOR=yellow>Device Number</TH>\r\n");
		SocketWriteData(mySocketFD,	"\t\t<TH><FONT COLOR=yellow>Device Name</TH>\r\n");
		SocketWriteData(mySocketFD,	"\t\t<TH><FONT COLOR=yellow>Description</TH>\r\n");
		SocketWriteData(mySocketFD,	"\t</TR>\r\n");

		for (ii=0; ii<gDeviceCnt; ii++)
		{
			if (gAlpacaDeviceList[ii] != NULL)
			{
				SocketWriteData(mySocketFD,	"\t<TR>\r\n");

				SocketWriteData(mySocketFD,	"\t\t<TD>\r\n");
					SocketWriteData(mySocketFD,	gAlpacaDeviceList[ii]->cAlpacaName);
				SocketWriteData(mySocketFD,	"\t\t</TD>\r\n");

				sprintf(lineBuffer, "<TD><CENTER>%d</TD>\r\n", gAlpacaDeviceList[ii]->cDeviceNum);
				SocketWriteData(mySocketFD,	lineBuffer);

				SocketWriteData(mySocketFD,	"\t\t<TD>\r\n");
					SocketWriteData(mySocketFD,	gAlpacaDeviceList[ii]->cDeviceName);
				SocketWriteData(mySocketFD,	"\t\t</TD>\r\n");

				SocketWriteData(mySocketFD,	"\t\t<TD>\r\n");
					SocketWriteData(mySocketFD,	gAlpacaDeviceList[ii]->cDeviceDescription);
				SocketWriteData(mySocketFD,	"\t\t</TD>\r\n");

				SocketWriteData(mySocketFD,	"\t</TR>\r\n");

			}
		}

		SocketWriteData(mySocketFD,	"</TABLE>\r\n");
		SocketWriteData(mySocketFD,	"</CENTER>\r\n");

		//**********************************************************
		//*	Output the html for each device
		for (ii=0; ii<gDeviceCnt; ii++)
		{
			if (gAlpacaDeviceList[ii] != NULL)
			{
				SocketWriteData(mySocketFD,	separaterLine);
				gAlpacaDeviceList[ii]->OutputHTML(reqData);
				gAlpacaDeviceList[ii]->OutputHTML_Part2(reqData);
			}
		}

		//**********************************************************
		SocketWriteData(mySocketFD,	separaterLine);

		SocketWriteData(mySocketFD,	"<CENTER>\r\n");
		SocketWriteData(mySocketFD,	"<H3>Versions</H3>\r\n");
		SocketWriteData(mySocketFD,	"<TABLE BORDER=1>\r\n");

		//*	this software
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>AlpacaDriver</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%s</TD>\r\n", kVersionString);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");

		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>Build #</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%d</TD>\r\n", kBuildNumber);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");


		//*	OS version
		if (strlen(gOsReleaseString) > 0)
		{
			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>OS Version</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%s</TD>\r\n", gOsReleaseString);
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");
		}

		//*	cpu we are running on
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>cpu</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%s</TD>\r\n", gCpuInfoString);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");

		//*	gcc version
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>gcc</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%s</TD>\r\n", __VERSION__);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");

		//*	glib version
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>libc version</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%s</TD>\r\n", gnu_get_libc_version());
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");

#if defined(_INCLUDE_WIRINGPI_) && defined(__arm__) && defined(__WIRING_PI_H__)
			//*	wiringPi version
		int		wiringPi_verMajor;
		int		wiringPi_verMinor;
		char	wiringPi_VerString[32];


		wiringPiVersion(&wiringPi_verMajor, &wiringPi_verMinor);
		sprintf(wiringPi_VerString, "%d.%d", wiringPi_verMajor, wiringPi_verMinor);
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>wiringPi version</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%s</TD>\r\n", wiringPi_VerString);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");
#endif


#ifdef _ENABLE_FITS_
		//*	cfitsio version
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>FITS (cfitsio)</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%d.%d</TD>\r\n", CFITSIO_MAJOR, CFITSIO_MINOR);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");
#endif // _ENABLE_FITS_

#ifdef _USE_OPENCV_
		//*	openCV version
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>OpenCV</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%s</TD>\r\n", CV_VERSION);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");
#endif

#ifdef _ENABLE_JPEGLIB_
		//*	jpeg lib version
		SocketWriteData(mySocketFD,	"<TR>\r\n");
			SocketWriteData(mySocketFD,	"<TD>libjpeg</TD>\r\n");
			sprintf(lineBuffer,	"<TD>%d</TD>\r\n", JPEG_LIB_VERSION);
			SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</TR>\r\n");
#endif



		if ((sizeof(long) != 4) || (sizeof(int*) != 4))
		{
			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(char)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(char));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");

			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(short)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(short));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");

			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(int)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(int));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");

			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(float)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(float));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");

			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(long)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(long));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");

			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(ptr)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(int*));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");

			SocketWriteData(mySocketFD,	"<TR>\r\n");
				SocketWriteData(mySocketFD,	"<TD>sizeof(double)</TD>\r\n");
				sprintf(lineBuffer,	"<TD>%ld</TD>\r\n", (long)sizeof(double));
				SocketWriteData(mySocketFD,	lineBuffer);
			SocketWriteData(mySocketFD,	"</TR>\r\n");
		}

		SocketWriteData(mySocketFD,	"</TABLE>\r\n");
		SocketWriteData(mySocketFD,	"</CENTER>\r\n");

		//**********************************************************
		SocketWriteData(mySocketFD,	separaterLine);
		SocketWriteData(mySocketFD,	"<H3>Links</H3>\r\n");
		SocketWriteData(mySocketFD,	"<UL>\r\n");

		SocketWriteData(mySocketFD,	"\t<LI><A HREF=https://ascom-standards.org/Developer/Alpaca.htm target=link>https://ascom-standards.org/Developer/Alpaca.htm</A>\r\n");
		SocketWriteData(mySocketFD,	"\t<LI><A HREF=https://ascom-standards.org/api/ target=link>https://ascom-standards.org/api/</A>\r\n");
//		SocketWriteData(mySocketFD,	"\t<LI><A HREF=https://agenaastro.com/zwo-astronomy-cameras-buyers-guide.html target=link>https://agenaastro.com/zwo-astronomy-cameras-buyers-guide.html</A>\r\n");
		SocketWriteData(mySocketFD,	"\t<LI><A HREF=https://agenaastro.com/articles/zwo-astronomy-cameras-buyers-guide.html target=link>https://agenaastro.com/articles/zwo-astronomy-cameras-buyers-guide.html</A>\r\n");
//		SocketWriteData(mySocketFD,	"\t<LI><A HREF=https://agenaastro.com/articles/guides/zwo-buyers-guide.html target=link>https://agenaastro.com/articles/guides/zwo-buyers-guide.html</A>\r\n");

		SocketWriteData(mySocketFD,	"\t<LI><A HREF=https://astronomy-imaging-camera.com/software-drivers target=link>https://astronomy-imaging-camera.com/software-drivers</A>\r\n");

		SocketWriteData(mySocketFD,	"</UL>\r\n");


		//**********************************************************
		SocketWriteData(mySocketFD,	separaterLine);
		SocketWriteData(mySocketFD,	"Compiled on ");
		SocketWriteData(mySocketFD,	__DATE__);
		SocketWriteData(mySocketFD,	"\r\n<BR>");
		SocketWriteData(mySocketFD,	"C++ version\r\n<BR>");
		SocketWriteData(mySocketFD,	"(C) 2020-21 by Mark Sproul msproul@skychariot.com\r\n<BR>");

		SocketWriteData(mySocketFD,	"</BODY></HTML>\r\n");
	}
	else
	{
	//	CONSOLE_DEBUG("reqData is NULL");
	}
}


//*****************************************************************************
static void	SendHtmlStats(TYPE_GetPutRequestData *reqData)
{
char	lineBuffer[256];
char	separaterLine[]	=	"<HR SIZE=4 COLOR=RED>\r\n";
//char	separaterLine[]	=	"<HR SIZE=4 COLOR=BLUE>\r\n";
int		mySocketFD;
int		ii;

	if (reqData != NULL)
	{
		mySocketFD	=	reqData->socket;
		SocketWriteData(mySocketFD,	gHtmlHeader);
//		SocketWriteData(mySocketFD,	gHtmlNightMode);
		sprintf(lineBuffer, "<TITLE>%s</TITLE>\r\n", gWebTitle);
		SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</HEAD><BODY>\r\n<CENTER>\r\n");
		SocketWriteData(mySocketFD,	"<H1>Alpaca device driver Web server</H1>\r\n");
		sprintf(lineBuffer, "<H3>%s</H3>\r\n", gWebTitle);
		SocketWriteData(mySocketFD,	lineBuffer);
		SocketWriteData(mySocketFD,	"</CENTER>\r\n");

		OutPutObservatoryInfoHTML(mySocketFD);

		for (ii=0; ii<gDeviceCnt; ii++)
		{
			if (gAlpacaDeviceList[ii] != NULL)
			{
				SocketWriteData(mySocketFD,	separaterLine);
				gAlpacaDeviceList[ii]->OutputHTML_CmdStats(reqData);
			}
		}


		SocketWriteData(mySocketFD,	separaterLine);
		SocketWriteData(mySocketFD,	"Compiled on ");
		SocketWriteData(mySocketFD,	__DATE__);
		SocketWriteData(mySocketFD,	"\r\n<BR>");
		SocketWriteData(mySocketFD,	"C++ version\r\n<BR>");
		SocketWriteData(mySocketFD,	"(C) 2020-21 by Mark Sproul msproul@skychariot.com\r\n<BR>");

		SocketWriteData(mySocketFD,	"</BODY></HTML>\r\n");
	}
	else
	{
	//	CONSOLE_DEBUG("reqData is NULL");
	}
}


//*****************************************************************************
void	GenerateHTMLcmdLinkTable(	int			socketFD,
									const char	*deviceName,
									const int	deviceNum,
									const TYPE_CmdEntry *cmdTable)
{
int		ii;
char	lineBuffer[256];

//	CONSOLE_DEBUG_W_STR("deviceName\t=", deviceName);
//	CONSOLE_DEBUG_W_NUM("deviceNum\t=", deviceNum);
	//*	now generate links to all of the commands
	SocketWriteData(socketFD,	"\r\n<UL>\r\n");
	ii	=	0;
	while (gCommonCmdTable[ii].commandName[0] != 0)
	{
		sprintf(lineBuffer,	"\t<LI><A HREF=../api/v1/%s/%d/%s target=cmd>%s</A>\r\n",
									deviceName,
									deviceNum,
									gCommonCmdTable[ii].commandName,
									gCommonCmdTable[ii].commandName);
		SocketWriteData(socketFD,	lineBuffer);
		ii++;
	}

	SocketWriteData(socketFD,	"<P>\r\n");

	ii	=	0;
	while (cmdTable[ii].commandName[0] != 0)
	{
		sprintf(lineBuffer,	"\t<LI><A HREF=../api/v1/%s/%d/%s target=cmd>%s</A>\r\n",
									deviceName,
									deviceNum,
									cmdTable[ii].commandName,
									cmdTable[ii].commandName);
		SocketWriteData(socketFD,	lineBuffer);
//		CONSOLE_DEBUG(lineBuffer);
		ii++;
	}
	SocketWriteData(socketFD,	"</UL>\r\n");
}

//*****************************************************************************
const char	gJpegHeader[]	=
{
	"HTTP/1.0 200 ok\r\n"
//	"Server: alpaca\r\n"
	"Mime-Version: 1.0\r\n"
	"Content-Type: image/jpeg\r\n"
	"Connection: close\r\n"
	"\r\n"

};

//*****************************************************************************
static void	SendJpegResponse(int socket, char *jpegFileName)
{
FILE			*filePointer;
int				numRead;
int				bytesWritten;
int				totalBytesWritten;
bool			keepGoing;
char			dataBuffer[1024];
char			myJpegFileName[128];
char			*myFilenamePtr;

	CONSOLE_DEBUG(__FUNCTION__);

	SocketWriteData(socket,	gJpegHeader);

	if (jpegFileName != NULL)
	{
		CONSOLE_DEBUG_W_STR("jpegFileName=", jpegFileName);

		myFilenamePtr	=	jpegFileName;
		if (*myFilenamePtr == '/')
		{
			myFilenamePtr++;
		}
		strncpy(myJpegFileName, myFilenamePtr, 100);

		myFilenamePtr	=	strstr(myJpegFileName, ".jpg");
		if (myFilenamePtr != NULL)
		{
			myFilenamePtr[4]	=	0;
		}
		myFilenamePtr	=	strstr(myJpegFileName, ".png");
		if (myFilenamePtr != NULL)
		{
			myFilenamePtr[4]	=	0;
		}
	}
	else
	{
		strcpy(myJpegFileName, "image.jpg");
	}
	CONSOLE_DEBUG_W_STR("myJpegFileName=", myJpegFileName);

	filePointer	=	fopen(myJpegFileName, "r");
	if (filePointer != NULL)
	{
		CONSOLE_DEBUG("File is open");
		totalBytesWritten	=	0;
		keepGoing	=	true;
		while (keepGoing)
		{
			numRead	=	fread(dataBuffer, 1, 1000, filePointer);
			if ((numRead > 0) || (feof(filePointer)))
			{
				bytesWritten	=	write(socket, dataBuffer, numRead);
				totalBytesWritten	+=	bytesWritten;
			}
			else
			{
				keepGoing	=	false;
			}

			if (feof(filePointer))
			{
				keepGoing	=	false;
			}
		}
		fclose(filePointer);
		CONSOLE_DEBUG_W_NUM("totalBytesWritten\t=",	totalBytesWritten);
	}
	else
	{
		CONSOLE_DEBUG("Failed to open file");
	}
	CONSOLE_DEBUG(__FUNCTION__);
}


//*****************************************************************************
static int	ProcessAlpacaAPIrequest(TYPE_GetPutRequestData	*reqData,
									char					*parseChrPtr)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
//int				alpacaVerNum;
int					deviceTypeEnum;
int					cc;
int					ii;
int					slen;
int					cmdBuffLen;
char				deviceStringBuffer[kDevStrLen];
char				*slashPtr;
char				*httpPtr;
bool				deviceFound;
char				argumentString[64];
bool				foundKeyWord;

#ifdef _DEBUG_CONFORM_
//	CONSOLE_DEBUG("/api/ found");
//	CONSOLE_DEBUG_W_STR("parseChrPtr\t=", parseChrPtr);
#endif // _DEBUG_CONFORM_

	deviceTypeEnum	=	-1;
	//*	so far so good, now get the version
	parseChrPtr	+=	5;
	if (*parseChrPtr == 'v')
	{
		parseChrPtr++;
	}
	if (isdigit(*parseChrPtr))
	{
//		alpacaVerNum	=	atoi(parseChrPtr);
//		CONSOLE_DEBUG_W_NUM("Alpaca version\t=", alpacaVerNum);

		//*	now skip over the version number
		while ((*parseChrPtr != '/') && (*parseChrPtr > 0))
		{
			parseChrPtr++;
		}
		if (*parseChrPtr == '/')
		{
			parseChrPtr++;
		}

		//*	we should now be to what really matters
#ifdef _DEBUG_CONFORM_
		CONSOLE_DEBUG_W_STR("Device data=", parseChrPtr);
#endif // _DEBUG_CONFORM_
		cc	=	0;
		while ((*parseChrPtr >= 0x20) && (cc < kDevStrLen))
		{
			deviceStringBuffer[cc++]	=	*parseChrPtr;
			deviceStringBuffer[cc]		=	0;

			parseChrPtr++;
		}

		strncpy(reqData->deviceType, deviceStringBuffer, 60);
		reqData->deviceType[59]	=	0;
		slashPtr	=	strchr(reqData->deviceType, '/');
		if (slashPtr != NULL)
		{
			*slashPtr	=	0;
		}


		//*	now get the device number
		slashPtr	=	strchr(deviceStringBuffer, '/');
		if (slashPtr != NULL)
		{
			slashPtr++;
			reqData->deviceNumber	=	atoi(slashPtr);

			//*	now skip the device number
			while (isdigit(*slashPtr))
			{
				slashPtr++;
			}
			if (*slashPtr == '/')
			{
				slashPtr++;
			}
			StrcpyToEOL(reqData->cmdBuffer, slashPtr, kDevStrLen);
			httpPtr	=	strstr(reqData->cmdBuffer, "HTTP");
			if (httpPtr != NULL)
			{
				*httpPtr	=	0;
				httpPtr--;
				while (*httpPtr <= 0x20)
				{
					*httpPtr	=	0;
					httpPtr--;
				}
			}

			//*	check the command string make sure its terminated properly
			slen	=	strlen(reqData->cmdBuffer);
			for (ii=0; ii<slen; ii++)
			{
				if ((reqData->cmdBuffer[ii] == '%') || (reqData->cmdBuffer[ii] == '"'))
				{
					reqData->cmdBuffer[ii]	=	0;
					break;
				}
			}
		}

#ifdef _DEBUG_CONFORM_
		CONSOLE_DEBUG_W_STR("Device type\t=",	reqData->deviceType);
//		DumpRequestStructure(__FUNCTION__, reqData);
#endif // _DEBUG_CONFORM_

		//	curl -X PUT "https://virtserver.swaggerhub.com/ASCOMInitiative/api/v1/camera/0/cooleron"
		//			-H  "accept: application/json"
		//			-H  "Content-Type: application/x-www-form-urlencoded"
		//			-d "CoolerOn=true&ClientID=223&ClientTransactionID=45"
		//	filterwheel/0/interfaceversion?ClientTransactionID=2&ClientID=12498 HTTP/1.1

		//*	Check for client ID
		foundKeyWord	=	GetKeyWordArgument(reqData->contentData, "ClientID", argumentString, 31);
		if (foundKeyWord)
		{
			gClientID	=	atoi(argumentString);
		}
#ifdef _DEBUG_CONFORM_
		else
		{
//			CONSOLE_DEBUG("gClientID NOT FOUND");
		}
#endif // _DEBUG_CONFORM_

		//*	Check for ClientTransactionID
		foundKeyWord	=	GetKeyWordArgument(reqData->contentData, "ClientTransactionID", argumentString, 31);
		if (foundKeyWord)
		{
			gClientTransactionID	=	atoi(argumentString);
		}
#ifdef _DEBUG_CONFORM_
		else
		{
//			CONSOLE_DEBUG("gClientTransactionID NOT FOUND");
		}
		CONSOLE_DEBUG_W_NUM("gClientID\t=", gClientID);
		CONSOLE_DEBUG_W_NUM("gClientTransactionID\t=", gClientTransactionID);

#endif // _DEBUG_CONFORM_


		//*	extract out the command itself for easier processing by the handlers
		cmdBuffLen		=	strlen(reqData->cmdBuffer);
		cc				=	0;
		while (	(reqData->cmdBuffer[cc] > 0x20) &&
				(reqData->cmdBuffer[cc] != '&') &&
				(reqData->cmdBuffer[cc] != '?') &&
				(cc < cmdBuffLen ))
		{
			reqData->deviceCommand[cc]	=	reqData->cmdBuffer[cc];
			cc++;
			reqData->deviceCommand[cc]	=	0;
		}

		//*******************************************
		//*	now do something with the data
		deviceTypeEnum	=	FindDeviceTypeByString(reqData->deviceType);
		deviceFound	=	false;
		for (ii=0; ii<gDeviceCnt; ii++)
		{
			if (gAlpacaDeviceList[ii] != NULL)
			{
			#ifdef _DEBUG_CONFORM_
			//	CONSOLE_DEBUG_W_NUM("gAlpacaDeviceList[ii]->cDeviceType\t=", gAlpacaDeviceList[ii]->cDeviceType);
			//	CONSOLE_DEBUG_W_NUM("gAlpacaDeviceList[ii]->cDeviceNum\t=", gAlpacaDeviceList[ii]->cDeviceNum);
			#endif // _DEBUG_CONFORM_

				if ((gAlpacaDeviceList[ii]->cDeviceType == deviceTypeEnum) &&
					(gAlpacaDeviceList[ii]->cDeviceNum == reqData->deviceNumber))
				{
					deviceFound		=	true;
					alpacaErrCode	=	gAlpacaDeviceList[ii]->ProcessCommand(reqData);


					reqData->alpacaErrCode	=	alpacaErrCode;
					if (gConformLogging)
					{
						LogToDisk(kLog_Conform, reqData);
					}
					if ((alpacaErrCode != 0) && gErrorLogging)
					{
						LogToDisk(kLog_Error, reqData);
					}

					break;
				}
			}
		}

		if (deviceFound == false)
		{
			CONSOLE_DEBUG("Device not found");
			CONSOLE_DEBUG_W_STR("Looking for device type\t=", reqData->deviceType);
			SocketWriteData(reqData->socket,	gBadResponse400);
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
		}
	}
	else
	{
#ifdef _DEBUG_CONFORM_
		CONSOLE_DEBUG_W_STR("kASCOM_Err_InvalidValue", parseChrPtr);
#endif // _DEBUG_CONFORM_
		alpacaErrCode	=	kASCOM_Err_InvalidValue;
	}


#ifdef _DEBUG_CONFORM_
	//*	log everything
#else
	if ((alpacaErrCode != 0) && (alpacaErrCode != kASCOM_Err_NotSupported))
#endif // _DEBUG_CONFORM_
	{
	char	myCommandString[256];
	char	*quesMkPtr;

		strcpy(myCommandString, reqData->cmdBuffer);
		quesMkPtr	=	strchr(myCommandString, '?');
		if (quesMkPtr != NULL)
		{
			*quesMkPtr	=	0;
		}

		//*	log the event
		LogEvent(	reqData->deviceType,
					myCommandString,
					NULL,
					alpacaErrCode,
					reqData->alpacaErrMsg);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
static TYPE_ASCOM_STATUS	ProcessManagementRequest(TYPE_GetPutRequestData	*reqData,
													char					*parseChrPtr)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
int					ii;
int					parseDataLen;
int					cmdStrLen;

	if (parseChrPtr != NULL)
	{
		if (strncmp(parseChrPtr, "/management/", 12) == 0)
		{
			parseChrPtr	+=	12;
		}
		if (*parseChrPtr == 'v')
		{
			parseChrPtr++;
			while (*parseChrPtr != '/')
			{
				parseChrPtr++;
			}
		}
		if (*parseChrPtr == '/')
		{
			parseChrPtr++;
		}

		parseDataLen	=	strlen(parseChrPtr);
		cmdStrLen		=	0;
		for (ii=0; ii<parseDataLen; ii++)
		{
			cmdStrLen++;
			if (parseChrPtr[ii] <= 0x20)
			{
				break;
			}
		}
		if (cmdStrLen < kMaxCommandLen)
		{
			strncpy(reqData->deviceCommand, parseChrPtr, cmdStrLen);
			reqData->deviceCommand[cmdStrLen]	=	0;
			StripTrailingSpaces(reqData->deviceCommand);
		}
//		CONSOLE_DEBUG_W_STR("reqData->deviceCommand\t=", reqData->deviceCommand);
//		CONSOLE_DEBUG_W_NUM("cmdStrLent=", cmdStrLen);
	}

	for (ii=0; ii<gDeviceCnt; ii++)
	{
		if (gAlpacaDeviceList[ii] != NULL)
		{
			if (gAlpacaDeviceList[ii]->cDeviceType == kDeviceType_Management)
			{
				alpacaErrCode	=	gAlpacaDeviceList[ii]->ProcessCommand(reqData);
				break;
			}
		}
	}
	return(alpacaErrCode);
}

//*****************************************************************************
//*
//*	PUT /api/v1/filterwheel/0/connected HTTP/1.1
//*	BinarySerialisation: true
//*	Authorization: Basic Og==
//*	Accept: application/json, text/json, text/x-json, text/javascript, application/xml, text/xml
//*	User-Agent: RestSharp/106.6.9.0
//*	Content-Type: application/x-www-form-urlencoded
//*	Host: 192.168.1.164:6800
//*	Content-Length: 53
//*	Accept-Encoding: gzip, deflate
//*	Connection: Keep-Alive
//*
//*	ClientTransactionID=31&ClientID=18194&Connected=False
//*****************************************************************************
static void	ParseHTMLdataIntoReqStruct(const char *htmlData, TYPE_GetPutRequestData	*reqData)
{
char			*contentLenPtr;
char			lineBuff[1024];
char			firstLine[1024];
unsigned int	iii;
unsigned int	ccc;
unsigned int	sLen;
int				lineCnt;
char			theChar;
bool			isContent;
bool			isFirstLine;
char			*queMrkPtr;

//	CONSOLE_DEBUG(__FUNCTION__);
	if ((htmlData != NULL) && (reqData != NULL))
	{
		if (strncasecmp(htmlData, "GET", 3) == 0)
		{
			reqData->get_putIndicator	=	'G';
		}
		else 	if (strncasecmp(htmlData, "PUT", 3) == 0)
		{
			reqData->get_putIndicator	=	'P';
		}
		else
		{
			CONSOLE_DEBUG("GET/PUT NOT SPECIFIED!!!!!!!");
			reqData->get_putIndicator	=	'?';
		}
		sLen		=	strlen(htmlData);
//		CONSOLE_DEBUG_W_NUM("htmlData length\t=", sLen);
//		CONSOLE_DEBUG_W_NUM("sizeof(lineBuff)\t=", sizeof(lineBuff));


		//*	keep a copy of the entire thing
		strcpy(reqData->htmlData, htmlData);


		//*	go through the entire data and treat them as separate lines of text
		lineCnt		=	0;
		isContent	=	false;
		ccc			=	0;
		theChar		=	0;
		iii			=	0;
		isFirstLine	=	true;
		while (iii <= sLen)
		{
			theChar		=	htmlData[iii];
			if ((theChar >= 0x20) || (theChar == 0x09))
			{
				if (ccc < sizeof(lineBuff))
				{
					lineBuff[ccc]	=	theChar;
					ccc++;
				}
			}
			else
			{
				lineCnt++;
				lineBuff[ccc]	=	0;
				ccc				=	0;
//				CONSOLE_DEBUG_W_STR("lineBuff\t=", lineBuff);
				if (isFirstLine)
				{
					strcpy(firstLine, lineBuff);
					isFirstLine	=	false;
				}
				//*	now lets see if this is anything we care about
				if (strlen(lineBuff) > 0)
				{
					if (isContent)
					{
						strcat(reqData->contentData, lineBuff);
						strcat(reqData->contentData, "\r\n");
					}
					else
					{
						if (strncasecmp(lineBuff, "Content-Length", 14) == 0)
						{
							contentLenPtr	=	lineBuff;
							contentLenPtr	+=	15;
							while (*contentLenPtr == 0x20)
							{
								contentLenPtr++;
							}
							reqData->contentLength	=	atoi(contentLenPtr);
					#ifdef _DEBUG_CONFORM_
							CONSOLE_DEBUG("Content-Length: was found");
					#endif // _DEBUG_CONFORM_

						}
					}
				}
				else
				{
					isContent	=	true;
				}

				//*	check the next char, make sure we dont process CR and LF
				if ((theChar == 0x0d) && (htmlData[iii+1] == 0x0a))
				{
					//*	skip the line feed
					iii++;
				}
				else if ((theChar == 0x0a) && (htmlData[iii+1] == 0x0d))
				{
					//*	skip the cr
					iii++;
				}
			}

			iii++;

		}
		if (reqData->get_putIndicator == 'G')
		{
			//*	the get data is in a different location
			queMrkPtr	=	strchr(firstLine, '?');
			if (queMrkPtr != NULL)
			{
				queMrkPtr++;
				strcpy(reqData->contentData, queMrkPtr);
			}
		}
	#ifdef _DEBUG_CONFORM_
		CONSOLE_DEBUG_W_STR("contentData\t=", reqData->contentData);
	#endif // _DEBUG_CONFORM_
	}
	else
	{
		CONSOLE_DEBUG("args are NULL");
	}
}

//*****************************************************************************
static int	ProcessGetPutRequest(const int socket, char *htmlData)
{
int						returnCode	=	-1;
char					*parseChrPtr;
TYPE_GetPutRequestData	reqData;

#ifdef _DEBUG_CONFORM_
	CONSOLE_DEBUG("=========================================================================================");
	CONSOLE_DEBUG("GET/PUT");
	CONSOLE_DEBUG_W_STR("htmlData\t=", htmlData);
#endif // _DEBUG_CONFORM_

	memset(&reqData, 0, sizeof(TYPE_GetPutRequestData));
	//*	the TYPE_GetPutRequestData simplifies parsing and passing of the
	//*	parsed data to subroutines
	reqData.socket				=	socket;
	reqData.get_putIndicator	=	htmlData[0];
//	DumpRequestStructure(__FUNCTION__, &reqData);

	ParseHTMLdataIntoReqStruct(htmlData, &reqData);

	parseChrPtr			=	(char *)htmlData;
	parseChrPtr			+=	3;
	while (*parseChrPtr == 0x20)
	{
		parseChrPtr++;
	}

#ifdef _DEBUG_CONFORM_
//	CONSOLE_DEBUG_W_STR("Alpaca data=", parseChrPtr);
#endif // _DEBUG_CONFORM_

	//*	standard ALPACA api call
	if (strncmp(parseChrPtr, "/api/", 5) == 0)
	{
//		CONSOLE_DEBUG(__FUNCTION__);
		returnCode	=	ProcessAlpacaAPIrequest(&reqData, parseChrPtr);
	}
	//*	standard ALPACA setup
	else if (strncmp(parseChrPtr, "/setup", 6) == 0)
	{
		SendHtmlResponse(&reqData);
	}
	//*	standard ALPACA management
	else if (strncmp(parseChrPtr, "/management", 11) == 0)
	{
		ProcessManagementRequest(&reqData, parseChrPtr);
	}
	//*	extra web interface
	else if (strncmp(parseChrPtr, "/web", 4) == 0)
	{
		SendHtmlResponse(&reqData);
	}
	//*	extra log interface
	else if (strncmp(parseChrPtr, "/log", 4) == 0)
	{
		SendHtmlLog(socket);
	}
	//*	Stats interface
	else if (strncmp(parseChrPtr, "/stats", 6) == 0)
	{
		SendHtmlStats(&reqData);
	}
	else if (strncmp(parseChrPtr, "/favicon.ico", 12) == 0)
	{
		//*	do nothing, this is my web browser sends this
//		CONSOLE_DEBUG("Ignored");
	}
	else if (strncmp(parseChrPtr, "/image.jpg", 10) == 0)
	{
//		CONSOLE_DEBUG("image.jpg");
		SendJpegResponse(socket, NULL);
	}
	else if (strstr(parseChrPtr, ".jpg") != NULL)
	{
//		CONSOLE_DEBUG(".....jpg");
		SendJpegResponse(socket, parseChrPtr);
	}
	else if (strstr(parseChrPtr, ".png") != NULL)
	{
		SendJpegResponse(socket, parseChrPtr);
	}
	else
	{
		CONSOLE_DEBUG_W_STR("Incomplete alpaca command\t=",	htmlData);
		SocketWriteData(socket,	gBadResponse400);
	}
	return(returnCode);
}

//*****************************************************************************
//*	this function is called from the socket handler with the received data
//*	It will parse through the data checking all of the normal alpaca requirements
//*	and then call the appropriate function based on the device type
//*****************************************************************************
static int AlpacaCallback(const int socket, char *htmlData, long byteCount)
{
int		returnCode	=	-1;

	//*	we are looking for GET or PUT
	if (strncmp(htmlData, "GET /favicon.ico", 16) == 0)
	{
		//*	do nothing
	}
	else if ((strncmp(htmlData, "GET", 3) == 0) || (strncmp(htmlData, "PUT", 3) == 0))
	{
//		CONSOLE_DEBUG("ProcessGetPutRequest");
		returnCode	=	ProcessGetPutRequest(socket, htmlData);
	}
	else
	{
		CONSOLE_DEBUG_W_STR("Invalid HTML get/put command\t=",	htmlData);
	}

	gServerTransactionID++;	//*	we are the "server"

	return(returnCode);
}


//*****************************************************************************
static void	*ListenThread(void *arg)
{
//	CONSOLE_DEBUG(__FUNCTION__);

	SocketListen_SetCallback(&AlpacaCallback);

	SocketListen_Init(kAlpacaListenPort);

	while (gKeepRunning)
	{
		SocketListen_Poll();
	}

	SocketListen_Close();

	return(NULL);
}


//**************************************************************************
//*	examples
//*		PRETTY_NAME="Mendel GNU/Linux 4 (Day)"
//*		PRETTY_NAME="Ubuntu 16.04 LTS"
//*		Model		: Raspberry Pi 3 Model B Plus Rev 1.3
//*
//*	returns true on successful extraction
//**************************************************************************
static bool	ExtractArgValue(char *string, char delimChar, char *valueString)
{
int		slen;
char	*delimPtr;
bool	successFlag;

	successFlag	=	false;
	delimPtr	=	strchr(string, delimChar);
	if (delimPtr != NULL)
	{
		//*	skip the delim char
		delimPtr++;
		while ((*delimPtr == ' ') || (*delimPtr == '"'))
		{
			delimPtr++;
		}
		strcpy(valueString, delimPtr);
		slen	=	strlen(valueString);
		while (((valueString[slen] < 0x20) || (valueString[slen] == '"')) && (slen > 0))
		{
			valueString[slen]	=	0;
			slen--;
		}
		successFlag	=	true;
	}
	return(successFlag);
}


//**************************************************************************
static void	Read_OSreleaseVersion(void)
{
FILE	*filePointer;
char	lineBuff[256];
int		slen;
char	codeName[64];
bool	codeNameFound;

	filePointer	=	fopen("/etc/os-release", "r");
	if (filePointer != NULL)
	{
		codeName[0]		=	0;
		codeNameFound	=	false;
		while (fgets(lineBuff, 200, filePointer))
		{
			slen	=	strlen(lineBuff);
			if (slen > 1)
			{
				if (strncmp(lineBuff, "PRETTY_NAME", 11) == 0)
				{
					ExtractArgValue(lineBuff, '=', gOsReleaseString);
				}
				else if (strncmp(lineBuff, "UBUNTU_CODENAME", 15) == 0)
				{
					//*	this was added for the case of the nvidia jetson board.
					codeNameFound	=	ExtractArgValue(lineBuff, '=', codeName);
				}
			}
		}
		fclose(filePointer);

		if (codeNameFound)
		{
			//*	check to see if the code name is in the release string
			if (strstr(gOsReleaseString, codeName) == NULL)
			{
				//*	it is NOT there, put it in
				strcat(gOsReleaseString, " (");
				strcat(gOsReleaseString, codeName);
				strcat(gOsReleaseString, ")");
			}
		}
	}
	else
	{
		CONSOLE_DEBUG("/etc/os-release not found");
	}
}

//**************************************************************************
static void	Read_CpuInfo(void)
{
FILE	*filePointer;
char	lineBuff[256];
int		slen;
bool	stillNeedModel;
char	argValueString[64];
char	*stringPtr;

	//*	set the default value in case we fail to read /proc/cpuinfo
	strcpy(gPlatformString,	"");
	strcpy(gCpuInfoString,	"");
	stillNeedModel	=	true;

#if defined(__arm__) && !defined(_PLATFORM_STRING_)
	strcpy(gPlatformString,	"Raspberry Pi - ");
#endif



#if defined(__arm__)
	strcpy(gCpuInfoString,	"Arm");
#elif defined( __ARM_ARCH)
	strcpy(gCpuInfoString,	"Arm");
#elif defined( __x86_64)
	strcpy(gCpuInfoString,	"64 bit x86");
#elif defined( __i386__)
	strcpy(gCpuInfoString,	"32 bit x86");
#endif


	filePointer	=	fopen("/proc/cpuinfo", "r");
	if (filePointer != NULL)
	{
		while (fgets(lineBuff, 200, filePointer))
		{
			slen	=	strlen(lineBuff);
			if (slen > 1)
			{
				if (strncmp(lineBuff, "model name", 10) == 0)
				{
					ExtractArgValue(lineBuff, ':', gCpuInfoString);
				}
				else if (strncmp(lineBuff, "Model", 5) == 0)
				{
					//*	so far I have only found this is only on Raspberry Pi
					ExtractArgValue(lineBuff, ':', gPlatformString);
					stillNeedModel	=	false;
				}
				else if (strncmp(lineBuff, "Revision", 8) == 0)
				{
					if (stillNeedModel)
					{
						ExtractArgValue(lineBuff, ':', argValueString);
						if (strcmp(argValueString, "a020d3") == 0)
						{
							strcpy(gPlatformString,	"Raspberry Pi 3");
						}
						else
						{
							strcat(gPlatformString,	argValueString);
						}
					}
				}
				else if (strncasecmp(lineBuff, "bogomips", 8) == 0)
				{
					ExtractArgValue(lineBuff, ':', argValueString);
					gBogoMipsValue	=	atof(argValueString);
				}
			}
		}
		fclose(filePointer);
	}
	else
	{
		CONSOLE_DEBUG("failed to open /proc/cpuinfo");
	}

	//=======================================================================
	filePointer	=	fopen("/sys/firmware/devicetree/base/model", "r");
	if (filePointer != NULL)
	{
		lineBuff[0]	=	0;
		stringPtr	=	fgets(lineBuff, 200, filePointer);
		if (stringPtr != NULL)
		{
			if (strlen(lineBuff) > 5)
			{
				strcpy(gPlatformString, lineBuff);
			}
		}
		else
		{
			CONSOLE_DEBUG("Error reading /sys/firmware/devicetree/base/model");
		}
		fclose(filePointer);
	}

	//*	check to see if we have a valid platform string
	if (strlen(gPlatformString) == 0)
	{
	#ifdef _PLATFORM_STRING_
		//*	_PLATFORM_STRING_ can be defined in the make file
		//	exmple, note the back slashes
		//	jetson		:	DEFINEFLAGS		+=	-D_PLATFORM_STRING_=\"Nvidia-jetson\"
		strcpy(gPlatformString, _PLATFORM_STRING_);
	#endif

	}
#if (__SIZEOF_POINTER__ == 8)
	strcat(gPlatformString, " (64 bit)");
#elif (__SIZEOF_POINTER__ == 4)
//	strcat(gPlatformString, " (32 bit)");
#endif
}

//*****************************************************************************
static void	PrintHelp(const char *appName)
{
	printf("usage: %s [-acdehlqvt]\r\n", appName);
	printf("\ta\tAuto exposure\r\n");
	printf("\tc\tConform logging, log ALL commands to disk\r\n");
	printf("\td\tDisplay images as they are taken\r\n");
	printf("\te\tError logging, log errors commands to disk\r\n");
	printf("\th\tThis help message\r\n");
	printf("\tl\tLive mode\r\n");
	printf("\tq\tquiet (less console messages)\r\n");
	printf("\tv\tverbose (more console messages default)\r\n");
	printf("\tt<profile>\tWhich telescope profile to use\r\n");
}

//*****************************************************************************
static void	ProcessCmdLineArgs(int argc, char **argv)
{
int		ii;
char	theChar;

	for (ii=1; ii<argc; ii++)
	{
		if (argv[ii][0] == '-')
		{
			theChar	=	argv[ii][1];
			switch(theChar)
			{
				//	"-a" means auto adjust exposure
				case 'a':
					gAutoExposure	=	true;
					break;

				//	-c means Conform logging
				case 'c':
					gErrorLogging	=	true;	//*	write errors to log file if true
					break;

				//	-e means Error logging
				case 'e':
					gConformLogging	=	true;	//*	log all commands to log file to match up with Conform
					break;


				//	"-d" means display image after download from camera
				case 'd':
					gDisplayImage	=	true;
					break;

				//	"-h" means print help
				case 'h':
					PrintHelp(argv[0]);
					exit(0);
					break;

				//	"-l" means live view
				case 'l':
				#ifdef _USE_OPENCV_
					gLiveView	=	true;
				#else
					CONSOLE_DEBUG("Cannot do live mode without opencv");
				#endif
					break;

				//	"-q" means quiet
				case 'q':
					gVerbose	=	false;
					break;

				//	"-v" means verbose
				case 'v':
					gVerbose	=	true;
					break;

				//*	"-t" means which telescope profile to use
				//*	either -tNEWT16 or -t Newt16
				case 't':
//					CONSOLE_DEBUG_W_STR("argv[ii]\t=", argv[ii]);
					if (strlen(argv[ii]) > 2)
					{
						strncpy(gDefaultTelescopeRefID, &argv[ii][2], (kDefaultRefIdMaxLen - 2));
						gDefaultTelescopeRefID[kDefaultRefIdMaxLen - 1]	=	0;
					}
					else if (argc > (ii+1))
					{
						ii++;
						strncpy(gDefaultTelescopeRefID, argv[ii], (kDefaultRefIdMaxLen - 2));
						gDefaultTelescopeRefID[kDefaultRefIdMaxLen - 1]	=	0;
					}
//					CONSOLE_DEBUG_W_STR("gDefaultTelescopeRefID\t=", gDefaultTelescopeRefID);
					break;

			}
		}
	}
}

void sigterm_handler(int s)
{
    gKeepRunning = false;
}

static	int32_t	gMainLoopCntr	=	0;
//*****************************************************************************
int	main(int argc, char **argv)
{
pthread_t		threadID;
int				threadErr;
uint32_t		delayTime_microSecs;
uint32_t		delayTimeForThisTask;
int				ii;
int				cameraCnt;

struct sigaction sigIntHandler;
sigIntHandler.sa_handler = sigterm_handler;
sigemptyset(&sigIntHandler.sa_mask);
sigIntHandler.sa_flags = 0;

sigaction(SIGINT, &sigIntHandler, nullptr);


#if defined(_ENABLE_FITS_) || defined(_ENABLE_JPEGLIB_)
	char			lineBuffer[64];
#endif
//struct rlimit	myRlimit;
//int				errorCode;

	printf("Alpaca driver\r\n");
	sprintf(gFullVersionString, "%s - %s build #%d", kApplicationName, kVersionString, kBuildNumber);


	AddLibraryVersion("software", "gcc", __VERSION__);
	AddLibraryVersion("software", "libc", gnu_get_libc_version());

#ifdef _ENABLE_FITS_
	//*	cfitsio version
	sprintf(lineBuffer,	"%d.%d", CFITSIO_MAJOR, CFITSIO_MINOR);
	AddLibraryVersion("software", "cfitsio", lineBuffer);
#endif // _ENABLE_FITS_

#ifdef _USE_OPENCV_
	//*	openCV version
	AddLibraryVersion("software", "opencv", CV_VERSION);
#endif

#ifdef _ENABLE_JPEGLIB_
	//*	jpeg lib version
	sprintf(lineBuffer,	"%d", JPEG_LIB_VERSION);
	AddLibraryVersion("software", "libjpeg", lineBuffer);
#endif

	//*	check resoure limits
//	errorCode	=	getrlimit(RLIMIT_NOFILE, &myRlimit);
//	CONSOLE_DEBUG_W_NUM("getrlimit returned errorCode\t=",	errorCode);
//	CONSOLE_DEBUG_W_NUM("myRlimit.rlim_cur\t=",	myRlimit.rlim_cur);
//	CONSOLE_DEBUG_W_NUM("myRlimit.rlim_max\t=",	myRlimit.rlim_max);

	strcpy(gDefaultTelescopeRefID,	"");
	strcpy(gPlatformString,			"");

	strcpy(gWebTitle, "Alpaca Driver");

	Read_OSreleaseVersion();
	Read_CpuInfo();

	InitObsConditionGloblas();
	ProcessCmdLineArgs(argc, argv);

//	CONSOLE_DEBUG_W_INT32("sizeof(int)\t=",		(long)sizeof(int));
//	CONSOLE_DEBUG_W_INT32("sizeof(long)\t=",	(long)sizeof(long));

	InitDeviceList();

	LogEvent(	"AlpacaPi",
				NULL,
				NULL,
				kASCOM_Err_Success,
				"System Started");

	LogEvent(	"AlpacaPi",
				NULL,
				NULL,
				kASCOM_Err_Success,
				gFullVersionString);

	ObservatorySettings_Init();
	ObservatorySettings_ReadFile();




//*********************************************************
//*	Cameras
#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_ATIK_)
	CreateATIK_CameraObjects();
#endif

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_ASI_)
	CreateASI_CameraObjects();
#endif

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_TOUP_)
	CreateTOUP_CameraObjects();
#endif

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_QHY_)
	CreateQHY_CameraObjects();
#endif

//#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_FLIR_) && (__GNUC__ > 5)
#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_FLIR_)
	CreateFLIR_CameraObjects();
#endif


#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_SONY_)
	CreateSONY_CameraObjects();
#endif


//*********************************************************
//*	Multicam
#ifdef _ENABLE_MULTICAM_
	cameraCnt	=	CountDevicesByType(kDeviceType_Camera);
	if (cameraCnt > 1)
	{
		CreateMultiCamObject();
	}
#endif

//*********************************************************
//*	Focuser
#ifdef _ENABLE_FOCUSER_
	CreateFocuserNiteCrawlerObjects();
#endif

//*********************************************************
//*	Filter wheel
#ifdef _ENABLE_FILTERWHEEL_
	CreateZWOFilterWheelObjects();
#endif




//*********************************************************
//*	Dome
#ifdef _ENABLE_DOME_
	CreateDomeObjectsRPi();
#endif
//*********************************************************
//*	Shutter
#ifdef _ENABLE_SHUTTER_
//	CreateShutterObjects();
	CreateShuterArduinoObjects();
#endif
#ifdef	_ENABLE_ROR_
	CreateDomeObjectsROR();
#endif // _ENABLE_ROR_
//*********************************************************
//*	Switch
#if defined(_ENABLE_SWITCH_) && defined(__arm__)
	CreateSwitchObjectsRPi();
#endif	//	_ENABLE_SWITCH_

//*********************************************************
//*	Observing conditions
#if defined(_ENABLE_OBSERVINGCONDITIONS_) && defined(__arm__)
	CreateObsConditionRpiObjects();
#elif defined(_ENABLE_OBSERVINGCONDITIONS_)
//	CreateObsConditionObjects();
#endif


#ifdef _ENABLE_CALIBRATION_
//	CreateCalibrationObjects();
	CreateCalibrationObjectsRPi();
#endif // _ENABLE_CALIBRATION_

//*********************************************************
//*	Telescope
#ifdef _ENABLE_TELESCOPE_
	CreateTelescopeObjects();
#endif // _ENABLE_TELESCOPE_


//*********************************************************
//*	Slit tacker
#ifdef _ENABLE_SLIT_TRACKER_
	CreateSlitTrackerObjects();
#endif // _ENABLE_SLIT_TRACKER_




	//*********************************************************
	//*	Management
	CreateManagementObject();

	//*********************************************************
	StartDiscoveryListenThread(kAlpacaListenPort);

	cameraCnt	=	CountDevicesByType(kDeviceType_Camera);
	CONSOLE_DEBUG_W_NUM("cameraCnt=", cameraCnt);

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_FITS_) && defined(_ENABLE_DISCOVERY_QUERRY_)
	//*	for now, we dont need this on all devices
	if (cameraCnt > 0)
	{
		StartDiscoveryQuerryThread();
	}
#endif


	threadErr	=	pthread_create(&threadID, NULL, &ListenThread, NULL);
	if (threadErr != 0)
	{
		CONSOLE_DEBUG_W_NUM("threadErr=", threadErr);
	}


	//========================================================================================
	gKeepRunning	=	true;
	while (gKeepRunning)
	{
		gMainLoopCntr++;

		delayTime_microSecs	=	(1000000 / 2);		//*	default to 1/2 second
		for (ii=0; ii<gDeviceCnt; ii++)
		{
			//==================================================================================
			//*	Run state machines for enabled device.
			//*	Not all devices have state machines to run
			if (gAlpacaDeviceList[ii] != NULL)
			{
				delayTimeForThisTask	=	gAlpacaDeviceList[ii]->RunStateMachine();
				//*	figure out what is the minimum delay time we have
				if (delayTimeForThisTask < delayTime_microSecs)
				{
					delayTime_microSecs	=	delayTimeForThisTask;
				}

			}
		}
		if (delayTime_microSecs < 10)
		{
			delayTime_microSecs	=	10;
		}
//		CONSOLE_DEBUG_W_INT32("delayTime_microSecs\t=", delayTime_microSecs);
		usleep(delayTime_microSecs);

	}

	threadErr = pthread_join(threadID, nullptr);
	if(threadErr != 0)
	{
		CONSOLE_DEBUG_W_INT32("Error joining listen thread\t:", threadErr);
	}

	//*	the program has been told to quit, go through and delete the objects
	for (ii=0; ii<kMaxDevices; ii++)
	{
		if (gAlpacaDeviceList[ii] != NULL)
		{
			CONSOLE_DEBUG_W_STR("Deleting ", gAlpacaDeviceList[ii]->cDeviceName);
			delete gAlpacaDeviceList[ii];
		}
	}

	return(0);
}

#pragma mark -
#pragma mark Helper functions

static uint32_t	gSystemStartSecs = 0;


//*****************************************************************************
uint32_t	Calc_millisFromTimeStruct(struct timeval *theTimeStruct)
{
uint32_t	elapsedSecs;
uint32_t	milliSecs;

	elapsedSecs	=	theTimeStruct->tv_sec - gSystemStartSecs;
	milliSecs	=	(elapsedSecs * 1000) + (theTimeStruct->tv_usec / 1000);
	return(milliSecs);
}

#if !defined(__arm__) || defined(_INCLUDE_MILLIS_)

//*****************************************************************************
uint32_t	millis(void)
{
uint32_t	elapsedSecs;
uint32_t	milliSecs;
struct timeval	currentTime;

	gettimeofday(&currentTime, NULL);

	if (gSystemStartSecs == 0)
	{
		gSystemStartSecs	=	currentTime.tv_sec;
	}
	elapsedSecs	=	currentTime.tv_sec - gSystemStartSecs;
	milliSecs	=	(elapsedSecs * 1000) + (currentTime.tv_usec / 1000);
	return(milliSecs);
}
#endif	//	!defined(__arm__) || defined(_INCLUDE_MILLIS_)


//*****************************************************************************
//*	returns -1 if not found
//*****************************************************************************
int	FindCmdFromTable(const char *theCmd, const TYPE_CmdEntry *theCmdTable, int *cmdType)
{
int		ii;
int		cmdEnumValue;

	cmdEnumValue	=	-1;
	ii				=	0;
	while ((theCmdTable[ii].commandName[0] != 0) && (cmdEnumValue < 0))
	{
		if (strcasecmp(theCmd, theCmdTable[ii].commandName) == 0)
		{
			cmdEnumValue	=	theCmdTable[ii].enumValue;
		}
		ii++;
	}
	//*	if we havent found the command, look it up in the common table
	if (cmdEnumValue < 0)
	{
		ii				=	0;
		while ((gCommonCmdTable[ii].commandName[0] != 0) && (cmdEnumValue < 0))
		{
			if (strcasecmp(theCmd, gCommonCmdTable[ii].commandName) == 0)
			{
				cmdEnumValue	=	gCommonCmdTable[ii].enumValue;
			}
			ii++;
		}
	}


	return(cmdEnumValue);
}

//*****************************************************************************
void	FormatTimeString(time_t *time, char *timeString)
{
struct tm	*linuxTime;

	if ((time != NULL) && (timeString != NULL))
	{
		linuxTime		=	gmtime(time);

		sprintf(timeString, "%d/%d/%d %02d:%02d:%02d",
								(1 + linuxTime->tm_mon),
								linuxTime->tm_mday,
								(1900 + linuxTime->tm_year),
								linuxTime->tm_hour,
								linuxTime->tm_min,
								linuxTime->tm_sec);

	}
}

//*****************************************************************************
//	DATE-OBS	String - The UTC date and time at the start of the exposure in
//	the ISO standard 8601 format: '2002-09-07T15:42:17.123' (CCYY-MM-
//	DDTHH:MM:SS.SSS).
//*****************************************************************************
void	FormatTimeStringISO8601(struct timeval *tv, char *timeString)
{
struct tm	*linuxTime;
long		milliSecs;

	if ((tv != NULL) && (timeString != NULL))
	{
		linuxTime		=	gmtime(&tv->tv_sec);
		milliSecs		=	tv->tv_usec / 1000;

		sprintf(timeString, "%d-%02d-%02dT%02d:%02d:%02d.%03ld",
								(1900 + linuxTime->tm_year),
								(1 + linuxTime->tm_mon),
								linuxTime->tm_mday,
								linuxTime->tm_hour,
								linuxTime->tm_min,
								linuxTime->tm_sec,
								milliSecs);

	}
}

//*****************************************************************************
bool	IsTrueFalse(const char *trueFalseString)
{
bool	trueFalseFlag;

	if (strcasecmp(trueFalseString, "true") == 0)
	{
		trueFalseFlag	=	true;
	}
	else
	{
		trueFalseFlag	=	false;
	}
	return(trueFalseFlag);
}


//*****************************************************************************
//*	This finds the unique keyword in the data string.
//*	the keyword must be terminated with a "=" in order to return
//*	a valid argument.
//*	The method is a little slow but it insures non-ambiguity.
//*	For example, "Duration=" and "Duration1="
//*****************************************************************************
bool	GetKeyWordArgument(const char *dataSource, const char *keyword, char *argument, int maxArgLen)
{
int		dataSrcLen;
int		iii;
int		jjj;
bool	foundKeyWord;
char	myKeyWord[256];
char	myArgString[256];
int		ccc;
char	theChar;

#ifdef _DEBUG_CONFORM_
	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG_W_STR("dataSource\t=", dataSource);
	CONSOLE_DEBUG_W_STR("keyword\t=", keyword);
#endif // _DEBUG_CONFORM_


	foundKeyWord	=	false;
	if ((dataSource != NULL) && (keyword != NULL) && (argument != NULL))
	{
		//*	this steps through the string looking for keywords
		//*	Once the keyword is found, it MUST be followed by an "="
		argument[0]	=	0;
		dataSrcLen	=	strlen(dataSource);
		iii			=	0;
		ccc			=	0;
		while ((foundKeyWord == false) && (iii <= dataSrcLen))
		{
			theChar	=	dataSource[iii];
//-			CONSOLE_DEBUG_W_HEX("theChar\t=", theChar);
			if ((theChar == '=') || (theChar == '&') || (theChar < 0x20))
			{
				//*	we have a keyword, lets see what it is
				myKeyWord[ccc]		=	0;

				//*	now extract the argument
				if (dataSource[iii] == '=')
				{
					iii			+=	1;			//*	skip the "="
				}
				jjj				=	0;
				myArgString[0]	=	0;
				while ((dataSource[iii] >= 0x20) && (dataSource[iii] != '&') && (ccc < maxArgLen))
				{
					myArgString[jjj]	=	dataSource[iii];
					myArgString[jjj+1]	=	0;
					iii++;
					jjj++;
				}
			#ifdef _DEBUG_CONFORM_
				CONSOLE_DEBUG_W_STR("myKeyWord\t\t=", myKeyWord);
				CONSOLE_DEBUG_W_STR("myArgString\t=", myArgString);
			#endif // _DEBUG_CONFORM_

				if (strcasecmp(myKeyWord, keyword) == 0)
				{
					foundKeyWord	=	true;
					strcpy(argument, myArgString);
				}
				ccc	=	0;
			}
			else
			{
				myKeyWord[ccc]		=	theChar;
				myKeyWord[ccc+1]	=	0;
				ccc++;
			}
			iii++;
		}
	}
	else
	{
		CONSOLE_DEBUG("1 of the 3 arguments is null");
		foundKeyWord	=	false;
	}
#ifdef _DEBUG_CONFORM_
//	if (foundKeyWord)
//	{
//		CONSOLE_DEBUG("We found what we are looking for");
//		CONSOLE_DEBUG_W_STR("myKeyWord\t\t=", myKeyWord);
//		CONSOLE_DEBUG_W_STR("argument\t\t=", argument);
//	}
#endif // _DEBUG_CONFORM_
	return(foundKeyWord);
}

//**************************************************************************************
//*	Count devices by type
//**************************************************************************************
int	CountDevicesByType(const int deviceType)
{
int		deviceCount;
int		ii;

	deviceCount	=	0;
	for (ii=0; ii<gDeviceCnt; ii++)
	{
		if (gAlpacaDeviceList[ii] != NULL)
		{
			if (gAlpacaDeviceList[ii]->cDeviceType == deviceType)
			{
				deviceCount++;
			}
		}
	}
	return(deviceCount);
}


//**************************************************************************************
//*	returns the number of active filter wheels.
int	GetFilterWheelCnt(void)
{
int		filterWhlCnt;

	filterWhlCnt	=	CountDevicesByType(kDeviceType_Filterwheel);
	return(filterWhlCnt);
}


//*****************************************************************************
AlpacaDriver	*FindDeviceByType(const int deviceType)
{
int				ii;
AlpacaDriver	*devicePtr;

	devicePtr	=	NULL;
	for (ii=0; ii<gDeviceCnt; ii++)
	{
		if (gAlpacaDeviceList[ii] != NULL)
		{
			//*	make sure the object is valid
			if (gAlpacaDeviceList[ii]->cMagicCookie == kMagicCookieValue)
			{
				if (gAlpacaDeviceList[ii]->cDeviceType == deviceType)
				{
					devicePtr	=	gAlpacaDeviceList[ii];
				}
			}
		}
	}
	return(devicePtr);

}

//*****************************************************************************
void	ToLowerStr(char *theString)
{
int		ii;

	ii	=	0;
	while (theString[ii] > 0)
	{
		theString[ii]	=	tolower(theString[ii]);
		ii++;
	}
}

//*****************************************************************************
void	StripTrailingSpaces(char *theString)
{
int		ii;
int		sLen;

	sLen	=	strlen(theString);
	ii		=	sLen - 1;
	while (ii > 0)
	{
		if (theString[ii] <= 0x20)
		{
			theString[ii]	=	0;
		}
		else
		{
			break;
		}
		ii--;
	}
}


//**************************************************************************************
bool	Check_udev_rulesFile(const char *rulesFileName)
{
char		rulesFilePath[256];
FILE		*filePointer;
bool		rulesFileOK;

//	/lib/udev/rules.d/99-atik.rules

	rulesFileOK	=	false;

	strcpy(rulesFilePath, "/lib/udev/rules.d/");
	strcat(rulesFilePath, rulesFileName);
	filePointer	=	fopen(rulesFilePath, "r");
	if (filePointer != NULL)
	{
		rulesFileOK	=	true;

		fclose(filePointer);
	}

	//*	if we didnt find it, look in /etc/udev/rules.d/
	if (rulesFileOK == false)
	{
		strcpy(rulesFilePath, "/etc/udev/rules.d/");
		strcat(rulesFilePath, rulesFileName);

		filePointer	=	fopen(rulesFilePath, "r");
		if (filePointer != NULL)
		{
			rulesFileOK	=	true;

			fclose(filePointer);
		}
	}

	//*	if we still didnt find it and the name does not start with "99", check that
	if (rulesFileOK == false)
	{
		if (rulesFileName[0] != '9')
		{
			strcpy(rulesFilePath, "/lib/udev/rules.d/");
			strcat(rulesFilePath, "99-");
			strcat(rulesFilePath, rulesFileName);
			filePointer	=	fopen(rulesFilePath, "r");
			if (filePointer != NULL)
			{
				rulesFileOK	=	true;

				fclose(filePointer);
			}
		}
	}
	return(rulesFileOK);
}

//**************************************************************************
void			GetAlpacaName(TYPE_DEVICETYPE deviceType, char *alpacaName)
{
	switch(deviceType)
	{

		case kDeviceType_Management:			strcpy(alpacaName, "Management");		break;
		case kDeviceType_Camera:				strcpy(alpacaName, "Camera");			break;
		case kDeviceType_Dome:					strcpy(alpacaName, "Dome");				break;
		case kDeviceType_Filterwheel:			strcpy(alpacaName, "Filterwheel");		break;
		case kDeviceType_Focuser:				strcpy(alpacaName, "Focuser");			break;
		case kDeviceType_Rotator:				strcpy(alpacaName, "Rotator");			break;
		case kDeviceType_Telescope:				strcpy(alpacaName, "Telescope");		break;
		case kDeviceType_Observingconditions:	strcpy(alpacaName, "Observingconditions");	break;
		case kDeviceType_SafetyMonitor:			strcpy(alpacaName, "SafetyMonitor");	break;
		case kDeviceType_Shutter:				strcpy(alpacaName, "Shutter");			break;
		case kDeviceType_Switch:				strcpy(alpacaName, "Switch");			break;
		case kDeviceType_Multicam:				strcpy(alpacaName, "Multicam");			break;
		case kDeviceType_SlitTracker:			strcpy(alpacaName, "SlitTracker");		break;
		case kDeviceType_CoverCalibrator:		strcpy(alpacaName, "CoverCalibrator");	break;
		default:								strcpy(alpacaName, "unknown");		break;
	}
}

#pragma mark -

TYPE_OBS_GLOBALS	gEnvData;

//**************************************************************************
void	InitObsConditionGloblas(void)
{
	memset(&gEnvData, 0, sizeof(TYPE_OBS_GLOBALS));
	gEnvData.siteDataValid	=	false;
	gEnvData.domeDataValid	=	false;

}

//**************************************************************************
void	DumpRequestStructure(const char *functionName, TYPE_GetPutRequestData	*reqData)
{
	printf("%s\r\n", functionName);
	printf("Dev#=%d\tG/P=%c\r\n",	reqData->deviceNumber, reqData->get_putIndicator);
	printf("HTML\t=%s\r\n",			reqData->htmlData);
	printf("cmdBuffer\t=%s\r\n",	reqData->cmdBuffer);
	printf("contentData\t=%s\r\n",	reqData->contentData);

	printf("\r\n");
}
