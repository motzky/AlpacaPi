//*****************************************************************************
//*	Name:			cameradriver.cpp
//*
//*	Author:			Mark Sproul (C) 2019, 2020
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
//*
//*	References:
//*		https://ascom-standards.org/api/#/Dome%20Specific%20Methods/get_dome__device_number__athome
//*		https://github.com/OpenPHDGuiding/phd2/tree/master/cameras/zwolibs
//*
//*	Fits Info
//*		http://tigra-astronomy.com/sbfitsext-guidelines-for-fits-keywords
//*		http://iraf.noao.edu/projects/ccdmosaic/imagedef/fitsdic.html
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul
//*****************************************************************************
//*	Apr 14,	2019	<MLS> Created cameradriver.c
//*	Apr 15,	2019	<MLS> Added command table for camera
//*	Apr 17,	2019	<MLS> Added Camera_OutputHTML()
//*	Aug 26,	2019	<MLS> Started on C++ version of alpaca camera driver
//*	Sep  3,	2019	<MLS> Added initialization to class constructor
//*	Sep 26,	2019	<MLS> Working on organizing camera C++ class
//*	Oct  2,	2019	<MLS> Added AllcateImageBuffer()
//*	Oct 26,	2019	<MLS> Added IsCameraIDvalid()
//*	Nov  2,	2019	<MLS> Downloaded and installed cfitsio-3.47 library
//*	Nov  3,	2019	<MLS> Added cameraSerialNum
//*	Nov  3,	2019	<MLS> Added Get_ImageReady()
//*	Nov  6,	2019	<MLS> Added GenerateFileNameRoot()
//*	Nov  7,	2019	<MLS> Added SetSerialNumInFileName()
//*	Nov  8,	2019	<MLS> Added sidereal time support
//*	Nov  9,	2019	<MLS> Added SetObjectName(), SetTelescopeName(), SetFileNamePrefix()
//*	Nov  9,	2019	<MLS> Added SetInstrumentName()
//*	Nov 10,	2019	<MLS> Added Put_TelescopeInfo()
//*	Nov 11,	2019	<MLS> Telescope info comments are described in observatory settings
//*	Nov 12,	2019	<MLS> Added setexposuretime, and setlivemode
//*	Nov 13,	2019	<MLS> Added SaveNextImage() & SetImageType()
//*	Nov 13,	2019	<MLS> Can now set image mode (type), RAW8, RAW16, RGB24, Y8
//*	Nov 17,	2019	<MLS> Added SetFileNameSuffix()
//*	Nov 19,	2019	<MLS> AVI video starting to work
//*	Nov 19,	2019	<MLS> Added Put_StartVideo() and Put_StopVideo()
//*	Nov 19,	2019	<MLS> Start_Video(), Stop_Video() and Take_Video() are camera specific
//*	Nov 24,	2019	<MLS> The ATIK-460ex has an odd number of pixels/row
//*	Nov 24,	2019	<MLS> Added mismatch image row sizes to handle odd number of pixels
//*	Nov 25,	2019	<MLS> Working on RGB,PNG color image output
//*	Nov 30,	2019	<MLS> Added histogram csv output
//*	Dec  1,	2019	<MLS> Image files are now saved in a sub directory
//*	Dec  4,	2019	<MLS> Added GetImageTypeString()
//*	Dec  7,	2019	<MLS> Finished #ifdefs so that openCV can be disabled at compile time
//*	Dec  8,	2019	<MLS> Added filelist command, returns current list of files in image data dir
//*	Dec  9,	2019	<MLS> Added file name sorting to filelist command
//*	Dec 17,	2019	<MLS> Added cExposureResolution to camera class
//*	Dec 17,	2019	<MLS> Changed setlivemode to livemode with Get and Put
//*	Dec 17,	2019	<MLS> Added cDisplayImage
//*	Dec 18,	2019	<MLS> Added cAutoAdjustExposure & cAutoAdjustStepSz_us
//*	Dec 19,	2019	<MLS> Added 'autoexposure' command
//*	Dec 19,	2019	<MLS> Added Get_AutoExposure() & Put_AutoExposure()
//*	Dec 19,	2019	<MLS> cDisplayImage is setable from the cmd line (-d)
//*	Dec 22,	2019	<MLS> Added RunStateMachine_TakingPicture() to clean up code
//*	Jan  6,	2020	<MLS> Added RunStateMachine_Idle() to clean up code
//*	Jan  6,	2020	<MLS> Added startsequence, Put_StartSequence()
//*	Jan  7,	2020	<MLS> Changed AVI file to use MP42 (MPEG-4) CODEC, now working
//*	Jan  8,	2020	<MLS> Added ProcessExposureOptions()
//*	Jan  9,	2020	<MLS> Added DeltaDuration option to sequence
//*	Jan 11,	2020	<MLS> openCV insists on displaying images full size.
//*	Jan 11,	2020	<MLS> Added scaling for live view
//*	Jan 11,	2020	<MLS> Added DisplayLiveImage()
//*	Jan 11,	2020	<MLS> More work on FireCapture output file
//*	Jan 30,	2020	<MLS> Moving image save code to separate file
//*	Feb  8,	2020	<MLS> Added kCmd_Camera_status to camera driver
//*	Feb 16,	2020	<MLS> Added support for gain; Get_Gain(), Put_Gain()
//*	Feb 18,	2020	<MLS> Fixed bug in live view when image format gets changed
//*	Mar  3,	2020	<MLS> Added SetImageTypeIndex()
//*	Mar  5,	2020	<MLS> Changed setexposuretime to exposuretime with PUT and GET
//*	Mar  6,	2020	<MLS> Finished Put_ExposureTime()
//*	Mar  9,	2020	<MLS> Changed kCmd_Camera_status to kCmd_Camera_readall all for consistancy
//*	Mar  9,	2020	<MLS> Changed Get_Status() to Get_Readall()
//*	Mar 17,	2020	<MLS> Fixed bug, imagedata directory was not being closed
//*	Mar 18,	2020	<MLS> Updated imagearray command to output current data
//*	Mar 20,	2020	<MLS> Added version string to readall
//*	Mar 21,	2020	<MLS> Added filename options to readall
//*	Mar 22,	2020	<MLS> Added filenameoptions command
//*	Apr  2,	2020	<MLS> Added Put_binX() & Put_binY()
//*	Apr  2,	2020	<MLS> Added Get_numX(), Get_numY(), Put_numX(), Put_numY()
//*	Apr  2,	2020	<MLS> Added Get_startX(), Get_startY(), Put_startX(), Put_startY()
//*	Apr  2,	2020	<MLS> Added Put_Pulseguide()
//*	Apr  4,	2020	<MLS> Added Get_Sensortype() & Get_Exposuremax()
//*	Apr  5,	2020	<MLS> Added Get_DisplayImage() & Put_DisplayImage()
//*	Apr 10,	2020	<MLS> Added sidebar to image display
//*	Apr 11,	2020	<MLS> Get_Sidebar Put_Sidebar() & Put_Sidebar()
//*	Apr 11,	2020	<MLS> Get_Exposuremin()
//*	Apr 27,	2020	<MLS> Added cpu stats readall output
//*	Apr 28,	2020	<MLS> Added sidebar background color to readall output
//*	Jun  8,	2020	<MLS> Fixed JSON quote error in Read_Readoutmodes()
//*	Jun  9,	2020	<MLS> Added auxiliary text tag
//*	Jun 10,	2020	<MLS> Added readoutmodes to ReadAll
//*	Jun 15,	2020	<MLS> Workingon Start/StopVideo()
//*	Jun 21,	2020	<MLS> Added rgbarray command to camera driver
//*	Jun 21,	2020	<MLS> Added _INCLUDE_ALPACA_EXTRAS_
//*	Jun 21,	2020	<MLS> Added Get_RGBarray()
//*	Jun 25,	2020	<MLS> Changed JSON xmit buffer limit to 1475, significant speed improvement
//*	Jun 29,	2020	<MLS> Added Read_AlapcaCameraState()
//*	Jun 20,	2020	<MLS> Added cFN_includeRefID to include the refID in the file name
//*	Jun 21,	2020	<MLS> Added RunStateMachine_Device() so sub class can have some time
//*	Dec 12,	2020	<MLS> Major reorganization of class variables to better match ASCOM docs
//*****************************************************************************
//*	Jan  1,	2119	<TODO> ----------------------------------------
//*	Jun 26,	2119	<TODO> Add support for sub frames
//*****************************************************************************
//*	 gcc -dM -E - < /dev/null
//*************************S****************************************************
//*	Length: unspecified [text/plain]
//*	Saving to: "imagearray.1"
//*
//*		[		<=>		   ] 163,891,406 1.43MB/s   in 1m 50s
//*
//*	2019-04-30 21:22:24 (1.42 MB/s) - "imagearray.1" saved [163891406]
//*****************************************************************************

#ifdef _ENABLE_CAMERA_

#include	<ctype.h>
#include	<dirent.h>
#include	<errno.h>
#include	<math.h>
#include	<stdbool.h>
#include	<stdint.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/time.h>
#include	<sys/stat.h>
#include	<sys/types.h>
#include	<time.h>
#include	<unistd.h>

#if defined(__arm__)
	#include <wiringPi.h>
#endif


#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"



#ifdef _USE_OPENCV_
	#include "opencv/highgui.h"
	#include "opencv2/highgui/highgui_c.h"
	#include "opencv2/imgproc/imgproc_c.h"
#endif

#ifdef _ENABLE_FITS_
	#ifndef _FITSIO_H
		#include <fitsio.h>
	#endif // _FITSIO_H
	#ifndef _FITSIO2_H
	//	#include <fitsio2.h>
	#endif // _FITSIO2_H
#endif // _ENABLE_FITS_


#include	"JsonResponse.h"
#include	"eventlogging.h"

#include	"alpaca_defs.h"
#include	"cpu_stats.h"

#include	"alpacadriver.h"
#include	"alpacadriver_helper.h"
#include	"cameradriver.h"
#include	"observatory_settings.h"


//*****************************************************************************
const char	*gCameraStateStrings[]	=
{
	"Idle",
	"TakingPicture",
	"StartVideo",
	"TakingVideo",
	"undefined"
};

#define	kMaxCameraCnt	5



//*****************************************************************************
//*	this table was obtained from
//*	https://agenaastro.com/zwo-astronomy-cameras-buyers-guide.html
const TYPE_CmdEntry	gCameraCmdTable[]	=
{

	{	"bayeroffsetx",				kCmd_Camera_bayeroffsetX,			kCmdType_GET	},
	{	"bayeroffsety",				kCmd_Camera_bayeroffsetY,			kCmdType_GET	},
	{	"binx",						kCmd_Camera_binX,					kCmdType_BOTH	},
	{	"biny",						kCmd_Camera_binY,					kCmdType_BOTH	},
	{	"camerastate",				kCmd_Camera_camerastate,			kCmdType_GET	},
	{	"cameraxsize",				kCmd_Camera_cameraxsize,			kCmdType_GET	},
	{	"cameraysize",				kCmd_Camera_cameraysize,			kCmdType_GET	},
	{	"canabortexposure",			kCmd_Camera_canabortexposure,		kCmdType_GET	},
	{	"canasymmetricbin",			kCmd_Camera_canasymmetricbin,		kCmdType_GET	},
	{	"canfastreadout",			kCmd_Camera_canfastreadout,			kCmdType_GET	},
	{	"cangetcoolerpower",		kCmd_Camera_cangetcoolerpower,		kCmdType_GET	},
	{	"canpulseguide",			kCmd_Camera_canpulseguide,			kCmdType_GET	},
	{	"cansetccdtemperature",		kCmd_Camera_cansetccdtemperature,	kCmdType_GET	},
	{	"canstopexposure",			kCmd_Camera_canstopexposure,		kCmdType_GET	},
	{	"ccdtemperature",			kCmd_Camera_ccdtemperature,			kCmdType_GET	},
	{	"cooleron",					kCmd_Camera_cooleron,				kCmdType_BOTH	},
	{	"coolerpower",				kCmd_Camera_coolerpower,			kCmdType_GET	},
	{	"electronsperadu",			kCmd_Camera_electronsperadu,		kCmdType_GET	},
	{	"exposuremax",				kCmd_Camera_exposuremax,			kCmdType_GET	},
	{	"exposuremin",				kCmd_Camera_exposuremin,			kCmdType_GET	},
	{	"exposureresolution",		kCmd_Camera_exposureresolution,		kCmdType_GET	},
	{	"fastreadout",				kCmd_Camera_fastreadout,			kCmdType_BOTH	},
	{	"fullwellcapacity",			kCmd_Camera_fullwellcapacity,		kCmdType_GET	},
	{	"gain",						kCmd_Camera_gain,					kCmdType_BOTH	},
	{	"gainmax",					kCmd_Camera_gainmax,				kCmdType_GET	},
	{	"gainmin",					kCmd_Camera_gainmin,				kCmdType_GET	},
	{	"gains",					kCmd_Camera_gains,					kCmdType_GET	},
	{	"hasshutter",				kCmd_Camera_hasshutter,				kCmdType_GET	},
	{	"heatsinktemperature",		kCmd_Camera_heatsinktemperature,	kCmdType_GET	},
	{	"imagearray",				kCmd_Camera_imagearray,				kCmdType_GET	},
	{	"imagearrayvariant",		kCmd_Camera_imagearrayvariant,		kCmdType_GET	},
	{	"imageready",				kCmd_Camera_imageready,				kCmdType_GET	},
	{	"ispulseguiding",			kCmd_Camera_ispulseguiding,			kCmdType_GET	},
	{	"lastexposureduration",		kCmd_Camera_lastexposureduration,	kCmdType_GET	},
	{	"lastexposurestarttime",	kCmd_Camera_lastexposurestarttime,	kCmdType_GET	},
	{	"maxadu",					kCmd_Camera_maxadu,					kCmdType_GET	},
	{	"maxbinx",					kCmd_Camera_maxbinX,				kCmdType_GET	},
	{	"maxbiny",					kCmd_Camera_maxbinY,				kCmdType_GET	},
	{	"numx",						kCmd_Camera_numX,					kCmdType_BOTH	},
	{	"numy",						kCmd_Camera_numY,					kCmdType_BOTH	},
	{	"percentcompleted",			kCmd_Camera_percentcompleted,		kCmdType_GET	},
	{	"pixelsizex",				kCmd_Camera_pixelsizeX,				kCmdType_GET	},
	{	"pixelsizey",				kCmd_Camera_pixelsizeY,				kCmdType_GET	},
	{	"readoutmode",				kCmd_Camera_readoutmode,			kCmdType_BOTH	},
	{	"readoutmodes",				kCmd_Camera_readoutmodes,			kCmdType_GET	},
	{	"sensorname",				kCmd_Camera_sensorname,				kCmdType_GET	},
	{	"sensortype",				kCmd_Camera_sensortype,				kCmdType_GET	},
	{	"setccdtemperature",		kCmd_Camera_setccdtemperature,		kCmdType_BOTH	},
	{	"startx",					kCmd_Camera_startX,					kCmdType_BOTH	},
	{	"starty",					kCmd_Camera_startY,					kCmdType_BOTH	},
	{	"abortexposure",			kCmd_Camera_abortexposure,			kCmdType_PUT	},
	{	"pulseguide",				kCmd_Camera_pulseguide,				kCmdType_PUT	},
	{	"startexposure",			kCmd_Camera_startexposure,			kCmdType_PUT	},
	{	"stopexposure",				kCmd_Camera_stopexposure,			kCmdType_PUT	},

#ifdef _INCLUDE_ALPACA_EXTRAS_
	//*	items added by MLS
	{	"--extras",					kCmd_Camera_Extras,					kCmdType_GET	},
	{	"settelescopeinfo",			kCmd_Camera_settelescopeinfo,		kCmdType_PUT	},
	{	"exposuretime",				kCmd_Camera_exposuretime,			kCmdType_BOTH	},
	{	"filelist",					kCmd_Camera_filelist,				kCmdType_GET	},
	{	"livemode",					kCmd_Camera_livemode,				kCmdType_BOTH	},
	{	"sidebar",					kCmd_Camera_sidebar,				kCmdType_BOTH	},
	{	"savenextimage",			kCmd_Camera_savenextimage,			kCmdType_PUT	},
	{	"startsequence",			kCmd_Camera_startsequence,			kCmdType_PUT	},

	{	"startvideo",				kCmd_Camera_startvideo,				kCmdType_PUT	},
	{	"stopvideo",				kCmd_Camera_stopvideo,				kCmdType_PUT	},
	{	"framerate",				kCmd_Camera_framerate,				kCmdType_GET	},
	{	"autoexposure",				kCmd_Camera_autoexposure,			kCmdType_BOTH	},
	{	"displayimage",				kCmd_Camera_displayimage,			kCmdType_BOTH	},

	{	"filenameoptions",			kCmd_Camera_filenameoptions,		kCmdType_PUT	},
	{	"rgbarray",					kCmd_Camera_rgbarray,				kCmdType_GET	},


	{	"readall",					kCmd_Camera_readall,				kCmdType_GET	},
#endif // _INCLUDE_ALPACA_EXTRAS_

	{	"",						-1,	0x00	}
};



//**************************************************************************************
CameraDriver::CameraDriver(void)
	:AlpacaDriver(kDeviceType_Camera)
{
int	ii;

	CONSOLE_DEBUG(__FUNCTION__);
	//*	set all of the class data to known states

	//======================================================
	//*	Start with the ASCOM properties
	//*	set everything to false first
	cCanAbortExposure				=	false;
	cCanAsymmetricBin				=	false;
	cCanStopExposure				=	true;



	//======================================================
	cUpdateOtherDevices				=	true;
	cCurrentBinX					=	1;
	cCurrentBinY					=	1;
	cTempReadSupported				=	false;
	cCameraTemp_Dbl					=	0.0;
	cCoolerPowerLevel				=	0;
	cLastCameraErrMsg[0]			=	0;
	cDeviceName[0]					=	0;
	cSensorName[0]					=	0;
	cLastJpegImageName[0]			=	0;
	cCameraID						=	-1;
	cCameraIsOpen					=	false;
	cCameraXsize					=	0;
	cCameraYsize					=	0;
	cBayerPattern					=	0;
	cPixelSizeX						=	0.0;
	cPixelSizeY						=	0.0;
	cIsColorCam						=	false;
	cHasShutter						=	false;
	cSt4Port						=	false;
	cIsPulseGuiding					=	false;
	cIsCoolerCam					=	false;
	cIsUSB3Host						=	false;
	cIsUSB3Camera					=	false;
	cElectronsPerADU				=	0.0;
	cIsTriggerCam					=	false;
	cExposureResolution				=	1.0;
	cBitDepth						=	0;
	cMaxbinX						=	0;
	cMaxbinY						=	0;
	for (ii=0; ii<kNumSupportedFormats; ii++)
	{
		cSupportedFormats[ii]		=	-1;
	}
	for (ii=0; ii<kMaxImageTypes; ii++)
	{
		memset(&cSupportedImageTypes[ii], 0, sizeof(TYPE_SUPPORTED_IMG_TYPE));
		cSupportedImageTypes[ii].internalImgageType	=	kImageType_Invalid;
	}

	cGainMin						=	0;
	cGainMax						=	0;
	cGain_default					=	0;
	cGain							=	0;
	cExposureMin_us					=	32;
	cExposureMax_us					=	2000 * 1000 *1000;
	cExposureMax_seconds			=	10000.0;
	cExposureDefault_us				=	0;
	cCanFastReadout					=	false;
	cCanGetCoolerPower				=	false;
	cCansetccdtemperature			=	false;
	cCanRead8Bit					=	true;
	cHighSpeedMode					=	0;
	cAlpacaCameraState				=	kALPACA_CameraState_Idle;

	cDesiredImageType				=	kImageType_RAW8;
	cCurrAlpacaImgTypeIdx			=	0;
	CONSOLE_DEBUG_W_NUM("cDesiredImageType\t=",		cDesiredImageType);

	cNumFramesRequested				=	200;		//*	the number of frames requested
	cNumFramesToSave				=	200;		//*	the number of frames left to go, 0 means none
	cNumFramesSaved					=	0;			//*	in constructor
	cNumVideoFramesSaved			=	0;
	cVideoDuration_secs				=	0;
	cTotalFramesSaved				=	0;
	cFramesRead						=	0;
	cFrameRate						=	0.0;
	//*	init the data buffers to nothing
	cInternalCameraState			=	kCameraState_Idle;
	cCameraDataBuffer				=	NULL;
	cCameraBGRbuffer				=	NULL;
	cLastExposureStartTime.tv_sec	=	0;
	cLastExposureEndTime.tv_sec		=	0;
	cLastexposureduration_us		=	0;

	//===================================
	//*	subframe information
	cStartX							=	0;
	cStartY							=	0;
	cNumX							=	0;
	cNumY							=	0;

	cCameraDataBuffLen				=	0;
	cAutoAdjustExposure				=	gAutoExposure;
	cAutoAdjustStepSz_us			=	5;
	cSequenceDelay_us				=	0;
	cSeqDeltaExposure_us			=	0;
	cCameraAutoExposure				=	false;
	cCurrentExposure_us				=	1000;
	cExposureFailureCnt				=	0;
	cDisplayImage					=	gDisplayImage;
	cSaveImages						=	false;
	cSaveNextImage					=	false;
	cNewImageReadyToDisplay			=	false;
	cNewImageReadyToDownload		=	false;
	cWorkingLoopCnt					=	0;
#ifdef _USE_OPENCV_
	cCreateOpenCVwindow				=	true;
	cOpenCV_Image					=	NULL;
	cOpenCV_LiveDisplay				=	NULL;
	cOpenCV_Histogram				=	NULL;
	cCreateHistogramWindow			=	true;
	cOpenCV_videoWriter				=	NULL;
	strcpy(cOpenCV_ImgWindowName, "opencv");	//*	this gets overwritten by the sub class
	cOpenCV_ImgWindowValid			=	false;
	cDisplayCrossHairs				=	false;
	cCrossHairX						=	0;
	cCrossHairY						=	0;
	cLeftButtonDown					=	false;
	cRightButtonDown				=	false;
	cDrawRectangle					=	false;
	cLastLClickX					=	0;
	cLastLClickY					=	0;
	cCurrentMouseX					=	0;
	cCurrentMouseY					=	0;
	cTextFont						=	cvFont(1.0, 1);
	cOverlayTextFont				=	cvFont(2.0, 1);
	cVideoOverlayColor				=	CV_RGB(255,	0,	0);
	cSideBarBlk						=	CV_RGB(0,	0,	0);

	cVideoCreateTimeStampFile		=	true;
	cVideoTimeStampFilePtr			=	NULL;
	cDisplaySideBar					=	kSideBar_Left;
	cSideBarBGcolor					=	cvScalarAll(0);

	LoadAlpacaImage();
#endif // _USE_OPENCV_
	cAVIfourcc						=	0;

	cImageSeqNumber					=	0;
	if (gLiveView)
	{
		cImageMode					=	kImageMode_Live;
	}
	else
	{
		cImageMode					=	kImageMode_Single;
	}


	strcpy(cDeviceName,			"CameraDriver");
	strcpy(cDeviceDescription,	"Camera");

	strcpy(cDeviceManufAbrev,	"uknwn");
	strcpy(cFileNamePrefix,		"TEST");
	strcpy(cFileNameSuffix,		"");
	strcpy(cFileNameRoot,		"");
	cFN_includeSerialNum		=	false;	//*	include serial number in FileName
	cFN_includeManuf			=	true;	//*	include Manufacturer in FileName
	cFN_includeFilter			=	true;
	cFN_includeRefID			=	true;

	strcpy(cTelescopeModel,		"");
	strcpy(cObjectName,			"unknown");
	strcpy(cAuxTextTag,			"");

	//*	if there was a telescope refID specified on the command line, set it
	if (strlen(gDefaultTelescopeRefID) > 0)
	{
		GetTelescopeSettingsByRefID(gDefaultTelescopeRefID, 0, &cTS_info);
	}
	else
	{
		GetTelescopeSettingsByRefID(NULL, 0, &cTS_info);
	}

	if ((strlen(cTS_info.telescp_manufacturer) > 0) || (strlen(cTS_info.telescp_model) > 0))
	{
		strcpy(cTelescopeModel, cTS_info.telescp_manufacturer);
		strcat(cTelescopeModel, "-");
		strcat(cTelescopeModel, cTS_info.telescp_model);
	}
#ifdef _USE_OPENCV_
	SetOpenCVcolors(NULL);
#endif // _USE_OPENCV_

#ifdef _ENABLE_FILTERWHEEL_
	cConnectedFilterWheel			=	NULL;
	cFilterWheelCurrPos				=	-1;
	cFilterWheelCurrName[0]			=	0;
#endif // _ENABLE_FILTERWHEEL_

#ifdef _ENABLE_FOCUSER_
	cConnectedFocuser		=	NULL;
#endif	//	_ENABLE_FOCUSER_


#ifdef	_ENABLE_ROTATOR_
	cConnectedRotator		=	NULL;
#endif // _ENABLE_ROTATOR_

	cFocuserInfoValid		=	false;
	cRotatorInfoValid		=	false;
	cFilterWheelInfoValid	=	false;

	mkdir(kImageDataDir, 0744);

	SendDiscoveryQuery();
}

//**************************************************************************************
// Destructor
//**************************************************************************************
CameraDriver::~CameraDriver(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
}


//**************************************************************************************
void	CameraDriver::SetSerialNumInFileName(bool enable)
{
	cFN_includeSerialNum	=	enable;
	cFN_includeManuf		=	enable;
}


//**************************************************************************************
void	CameraDriver::SetObjectName(const char *newObjectName)
{
	if (strlen(newObjectName) < kObjectNameMaxLen)
	{
		strcpy(cObjectName, newObjectName);
	}
	else
	{
		strncpy(cObjectName, newObjectName, (kObjectNameMaxLen-1));
		cObjectName[kObjectNameMaxLen-1]	=	0;
	}
}

//**************************************************************************************
void	CameraDriver::SetTelescopeName(const char *newTelescopeName)
{
	if (strlen(newTelescopeName) < kTelescopeNameMaxLen)
	{
		strcpy(cTelescopeModel, newTelescopeName);
	}
	else
	{
		strncpy(cTelescopeModel, newTelescopeName, (kTelescopeNameMaxLen-1));
		cTelescopeModel[kTelescopeNameMaxLen-1]	=	0;
	}
}

//**************************************************************************************
void	CameraDriver::SetInstrumentName(const char *newInstrumentName)
{
	if (strlen(newInstrumentName) < kInstrumentNameMaxLen)
	{
		strcpy(cTS_info.instrument, newInstrumentName);
	}
	else
	{
		strncpy(cTS_info.instrument, newInstrumentName, (kInstrumentNameMaxLen-1));
		cTS_info.instrument[kInstrumentNameMaxLen-1]	=	0;
	}
}


//**************************************************************************************
void	CameraDriver::SetFileNamePrefix(const char *newFNprefix)
{
	if (strlen(newFNprefix) < kFileNamePrefixMaxLen)
	{
		strcpy(cFileNamePrefix, newFNprefix);
	}
	else
	{
		strncpy(cFileNamePrefix, newFNprefix, (kFileNamePrefixMaxLen-1));
		cFileNamePrefix[kFileNamePrefixMaxLen-1]	=	0;
	}
}

//**************************************************************************************
void	CameraDriver::SetFileNameSuffix(const char *newFNsuffix)
{
	CONSOLE_DEBUG(__FUNCTION__);
	if (strlen(newFNsuffix) < kFileNamePrefixMaxLen)
	{
		strcpy(cFileNameSuffix, newFNsuffix);
	}
	else
	{
		strncpy(cFileNameSuffix, newFNsuffix, (kFileNamePrefixMaxLen-1));
		cFileNameSuffix[kFileNamePrefixMaxLen-1]	=	0;
	}
	CONSOLE_DEBUG_W_STR("cFileNameSuffix\t=", cFileNameSuffix);
}

#pragma mark -

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::ProcessCommand(TYPE_GetPutRequestData *reqData)
{
TYPE_ASCOM_STATUS	alpacaErrCode;
char				alpacaErrMsg[256];
int					cmdEnumValue;
int					cmdType;
int					myDeviceNum;
int					mySocket;
bool				httpHeaderSent;
char				httpHeader[500];

//	CONSOLE_DEBUG(__FUNCTION__);

//*	delete this when not testing
	cMaxbinX	=	1;
	cMaxbinY	=	1;

	alpacaErrCode	=	kASCOM_Err_PropertyNotImplemented;
	strcpy(alpacaErrMsg, "");
	strcpy(cLastCameraErrMsg, "");

	//*	make local copies of the data structure to make the code easier to read
	mySocket	=	reqData->socket;
	myDeviceNum	=	reqData->deviceNumber;
	if (myDeviceNum >= kMaxCameraCnt)
	{
		//*	protect the bounds so we dont have to later
		myDeviceNum	=	0;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Device number out of bounds, using device #0");
	}

	httpHeaderSent	=	false;

#ifndef _JETSON_
	CONSOLE_DEBUG_W_STR("reqData->deviceCommand\t=",	reqData->deviceCommand);
#endif // _JETSON_

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


	//*	look up the command
	cmdEnumValue	=	FindCmdFromTable(reqData->deviceCommand, gCameraCmdTable, &cmdType);
	if (cmdEnumValue < 0)
	{
		CONSOLE_DEBUG_W_STR("Command not found\t=",	reqData->deviceCommand);
	}
	switch(cmdEnumValue)
	{
		//----------------------------------------------------------------------------------------
		//*	Common commands that we want to over ride
		//----------------------------------------------------------------------------------------
		case kCmd_Common_supportedactions:	//*	Returns the list of action names supported by this driver.
			alpacaErrCode	=	Get_SupportedActions(reqData, gCameraCmdTable);
			break;

		//========================================================================================
		//*	Camera specific commands
		//========================================================================================
		case kCmd_Camera_bayeroffsetX:			//*	Returns the X offset of the Bayer matrix.
		case kCmd_Camera_bayeroffsetY:			//*	Returns the Y offset of the Bayer matrix.
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "AlpacaPi: Command not implemented");
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			break;

		//*	binx and biny are the same on ZWO cameras
		case kCmd_Camera_binX:					//*	Returns the binning factor for the X axis.
												//*	Sets the binning factor for the X axis.
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_binX(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_binX(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_binY:					//*	Returns the binning factor for the Y axis.
												//*	Sets the binning factor for the Y axis.
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_binY(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_binY(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_camerastate:			//*	Returns the camera operational state.
			alpacaErrCode	=	Get_Camerastate(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_cameraxsize:			//*	Returns the width of the CCD camera chip.
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCameraXsize,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_cameraysize:			//*	Returns the height of the CCD camera chip.
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCameraYsize,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_canabortexposure:		//*	Indicates whether the camera can abort exposures.
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCanAbortExposure,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_canasymmetricbin:		//*	Indicates whether the camera supports asymmetric binning
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCanAsymmetricBin,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_canfastreadout:		//*	Indicates whether the camera has a fast readout mode.
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCanFastReadout,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_cangetcoolerpower:		//*	Indicates whether the camera's cooler power setting can be read.
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCanGetCoolerPower,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_canpulseguide:			//*	Returns a flag indicating whether this camera supports pulse guiding
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCanPulseGuide,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_cansetccdtemperature:	//*	Returns a flag indicating whether this camera supports setting the CCD temperature
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCansetccdtemperature,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_canstopexposure:		//*	Returns a flag indicating whether this camera can stop an exposure that is in progress
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cCanStopExposure,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_ccdtemperature:		//*	Returns the current CCD temperature
			alpacaErrCode	=	Get_CCDtemperature(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_cooleron:				//*	GET- Returns the current cooler on/off state.
												//*	PUT- Turns the camera cooler on and off
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_Cooleron(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Cooleron(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_coolerpower:			//*	Returns the present cooler power level
			alpacaErrCode	=	Get_CoolerPowerLevel(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_electronsperadu:		//*	Returns the gain of the camera
			JsonResponse_Add_Double(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cElectronsPerADU,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_exposuremax:			//*	Returns the maximum exposure time supported by StartExposure.
			alpacaErrCode	=	Get_Exposuremax(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_exposuremin:			//*	Returns the Minimium exposure time
			alpacaErrCode	=	Get_Exposuremin(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_exposureresolution:	//*	Returns the smallest increment in exposure time supported by StartExposure.
			JsonResponse_Add_Double(	mySocket,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										gValueString,
										cExposureResolution,
										INCLUDE_COMMA);
			JsonResponse_Add_String(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"Units",
									"Seconds",
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_fastreadout:			//*	Returns whether Fast Readout Mode is enabled.
												//*	Sets whether Fast Readout Mode is enabled.
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_Fastreadout(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Fastreadout(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_fullwellcapacity:		//*	Reports the full well capacity of the camera
			alpacaErrCode	=	Get_Fullwellcapacity(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_gain:					//*	Returns an index into the Gains array
												//*	Sets an index into the Gains array.
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_Gain(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Gain(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
			}
			break;


		case kCmd_Camera_gainmax:				//*	Maximum value of Gain
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cGainMax,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_gainmin:				//*	Minimum value of Gain
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cGainMin,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_gains:					//*	Gains supported by the camera
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Command not implemented");
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			break;

		case kCmd_Camera_hasshutter:			//*	Indicates whether the camera has a mechanical shutter
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cHasShutter,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;


		case kCmd_Camera_heatsinktemperature:	//*	Returns the current heat sink temperature.
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Command not implemented");
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			break;

		case kCmd_Camera_imagearray:			//*	Returns an array of integers containing the exposure pixel values
		case kCmd_Camera_imagearrayvariant:		//*	Returns an array of int containing the exposure pixel values
			if (reqData->get_putIndicator == 'G')
			{
				JsonResponse_FinishHeader(httpHeader, "");
				JsonResponse_SendTextBuffer(mySocket, httpHeader);
				httpHeaderSent	=	true;
				alpacaErrCode	=	Get_Imagearray(reqData, alpacaErrMsg);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Put not allowed for imagearray");
			}
			break;


		case kCmd_Camera_imageready:			//*	Indicates that an image is ready to be downloaded
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_ImageReady(reqData, alpacaErrMsg);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_ispulseguiding:		//*	Indicates that the camera is pulse guideing.
			//*	needs to be updated when we implement pulse guiding
			JsonResponse_Add_Bool(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cIsPulseGuiding,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_lastexposureduration:	//*	Duration of the last exposure
			alpacaErrCode	=	Get_Lastexposureduration(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_lastexposurestarttime:	//*	Start time of the last exposure in FITS standard format.
			alpacaErrCode	=	Get_Lastexposurestarttime(reqData,  alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_maxadu:				//*	Camera's maximum ADU value
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Command not implemented");
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			break;

		case kCmd_Camera_maxbinX:				//*	Maximum binning for the camera X axis
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cMaxbinX,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_maxbinY:				//*	Maximum binning for the camera Y axis
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cMaxbinY,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;


		case kCmd_Camera_numX:					//*	Returns the current subframe width
												//*	Sets the current subframe width
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_numX(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_numX(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_numY:					//*	Returns the current subframe height
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_numY(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_numY(reqData, alpacaErrMsg);
			}
			break;

												//*	Sets the current subframe height
		case kCmd_Camera_percentcompleted:		//*	Indicates percentage completeness of the current operation
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Command not implemented");
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			break;

		case kCmd_Camera_pixelsizeX:			//*	Width of CCD chip pixels (microns)
			JsonResponse_Add_Double(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cPixelSizeX,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_pixelsizeY:			//*	Height of CCD chip pixels (microns)
			JsonResponse_Add_Double(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cPixelSizeY,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_readoutmode:			//*	Indicates the canera's readout mode as an index into the array ReadoutModes
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_Readoutmode(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Readoutmode(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;
												//*	Set the camera's readout mode
		case kCmd_Camera_readoutmodes:			//*	List of available readout modes
			alpacaErrCode	=	Get_Readoutmodes(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_sensorname:			//*	Sensor name
			JsonResponse_Add_String(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									cSensorName,
									INCLUDE_COMMA);
			alpacaErrCode	=	kASCOM_Err_Success;
			break;

		case kCmd_Camera_sensortype:			//*	Type of information returned by the the camera sensor (monochrome or colour)
			alpacaErrCode	=	Get_Sensortype(reqData, alpacaErrMsg, gValueString);
			break;

		case kCmd_Camera_setccdtemperature:		//*	Returns the current camera cooler setpoint in degrees Celsius.
												//*	Set the camera's cooler setpoint (degrees Celsius).
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_SetCCDtemperature(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_SetCCDtemperature(reqData, alpacaErrMsg);
				CONSOLE_DEBUG_W_HEX("alpacaErrCode\t=", alpacaErrCode);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
				CONSOLE_ABORT("We shouldnt be here!!!!!!!!!!!!!!!!!")
			}
			break;

		case kCmd_Camera_startX:				//*	Return the current subframe X axis start position
												//*	Sets the current subframe X axis start position
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_startX(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_startX(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_startY:				//*	Return the current subframe Y axis start position
												//*	Sets the current subframe Y axis start position
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_startY(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_startY(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_abortexposure:			//*	Aborts the current exposure
			alpacaErrCode	=	Put_AbortExposure(reqData, alpacaErrMsg);
			break;

		case kCmd_Camera_pulseguide:			//*	Pulse guide in the specified direction for the specified time.
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	kASCOM_Err_NotImplemented;
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Pulseguide(reqData, alpacaErrMsg);
			}
			break;

		case kCmd_Camera_startexposure:			//*	Starts an exposure
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_StartExposure(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request")
			}
			break;

		case kCmd_Camera_stopexposure:			//*	Stops the current exposure
			alpacaErrCode	=	Put_StopExposure(reqData, alpacaErrMsg);
			break;

		//***********************************************************************************
		//*	these are added and not part of the alpaca spec
		case kCmd_Camera_settelescopeinfo:
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_TelescopeInfo(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_exposuretime:
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_ExposureTime(reqData, alpacaErrMsg);
			}
			else
			{
				alpacaErrCode	=	Get_ExposureTime(reqData, alpacaErrMsg, gValueString);
			}
			break;

		//*	was setlivemode
		case kCmd_Camera_livemode:
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_LiveMode(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_LiveMode(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request")
			}
			break;

#ifdef _USE_OPENCV_
		case kCmd_Camera_sidebar:
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_Sidebar(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Sidebar(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request")
			}
			break;
#endif // _USE_OPENCV_

		case kCmd_Camera_savenextimage:
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_SaveNextImage(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_startsequence:
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_StartSequence(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_startvideo:
			alpacaErrCode	=	Put_StartVideo(reqData, alpacaErrMsg);
			break;

		case kCmd_Camera_stopvideo:
			if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_StopVideo(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_framerate:
			break;

		case kCmd_Camera_filelist:
			alpacaErrCode	=	Get_Filelist(reqData, alpacaErrMsg);
			break;

		case kCmd_Camera_autoexposure:
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_AutoExposure(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_AutoExposure(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request")
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_displayimage:
			if (reqData->get_putIndicator == 'G')
			{
				alpacaErrCode	=	Get_DisplayImage(reqData, alpacaErrMsg, gValueString);
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_DisplayImage(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request")
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_filenameoptions:
			if (reqData->get_putIndicator == 'G')
			{
				//*	nothing here
			}
			else if (reqData->get_putIndicator == 'P')
			{
				alpacaErrCode	=	Put_Filenameoptions(reqData, alpacaErrMsg);
			}
			else
			{
				CONSOLE_DEBUG("invalid request");
				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
			}
			break;

		case kCmd_Camera_rgbarray:
			if (reqData->get_putIndicator == 'G')
			{
				JsonResponse_FinishHeader(httpHeader, "");
				JsonResponse_SendTextBuffer(mySocket, httpHeader);
//				CONSOLE_DEBUG_W_STR("httpHeader\t=", httpHeader);
				httpHeaderSent	=	true;
				alpacaErrCode	=	Get_RGBarray(reqData, alpacaErrMsg);
			}
			else if (reqData->get_putIndicator == 'P')
			{

				alpacaErrCode	=	kASCOM_Err_InvalidOperation;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Invalid PUT");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
			break;

		case kCmd_Camera_readall:
			alpacaErrCode	=	Get_Readall(reqData, alpacaErrMsg);
			break;

		//----------------------------------------------------------------------------------------
		//*	let anything undefined go to the common command processor
		//----------------------------------------------------------------------------------------
		default:
			alpacaErrCode	=	ProcessCommand_Common(reqData, cmdEnumValue, alpacaErrMsg);
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
								(httpHeaderSent == false));

	if (cmdEnumValue != kCmd_Camera_imagearray)
	{
//		CONSOLE_DEBUG_W_STR("JSON=", reqData->jsonTextBuffer);
	}
	//*	this is for the logging function
	strcpy(reqData->alpacaErrMsg, alpacaErrMsg);
	return(alpacaErrCode);
}

//*****************************************************************************
static void	ProcessTelescopeKeyWord(	const char			*sourceData,
										const char			*keyword,
										char				*returnString,
										const unsigned int	maxLen)
{
bool	keywordFound;
char	myValueString[256];

//	CONSOLE_DEBUG(__FUNCTION__);
	memset(myValueString, 0, sizeof(myValueString));
	keywordFound		=	GetKeyWordArgument(	sourceData,
												keyword,
												myValueString,
												(sizeof(myValueString) -1));
	if (keywordFound)
	{
//		CONSOLE_DEBUG_W_STR("Keyword found:", keyword);
//		CONSOLE_DEBUG_W_STR("Value string:", myValueString);
		if (strlen(myValueString) < maxLen)
		{
			strcpy(returnString, myValueString);
		}
		else
		{
			strncpy(returnString, myValueString, (maxLen-1));
			returnString[maxLen-1]	=	0;
		}
	}
}

//*****************************************************************************
//*	Process exposure options
//*			Object
//*			Prefix
//*			Suffix
//*			Duration
//*
//*	Examples
//*		Duration=1000.0&Prefix=DSO&Suffix=GRN HTTP/1.1
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::ProcessExposureOptions(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
bool	durationFound;
char	duarationString[32];
double	myExposureDuration_secs;
double	myExposure_usecs;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		ProcessTelescopeKeyWord(reqData->contentData,	"Object",		cObjectName,			kObjectNameMaxLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Prefix",		cFileNamePrefix,		kFileNamePrefixMaxLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Suffix",		cFileNameSuffix,		kFileNamePrefixMaxLen);

//		CONSOLE_DEBUG_W_STR("Suffix", cFileNameSuffix);


		durationFound		=	GetKeyWordArgument(	reqData->contentData,
													"Duration",
													duarationString,
													(sizeof(duarationString) -1));

		//*	if duration is NOT found, leave it alone
		if (durationFound)
		{
			myExposureDuration_secs	=	atof(duarationString);
			// CONFORM expects an exception with text "negative duration"
			if(myExposureDuration_secs < 0.0)
			{
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "negative duration");
				return kASCOM_Err_InvalidValue;
			}
//			CONSOLE_DEBUG_W_DBL("myExposureDuration_secs\t=", myExposureDuration_secs);
			myExposure_usecs		=	myExposureDuration_secs * 1000 * 1000;
			if (myExposure_usecs < cExposureMin_us)
			{
				myExposure_usecs	=	cExposureMin_us;
				CONSOLE_DEBUG_W_DBL("Setting to default minimum exposure time of", myExposure_usecs);
			}
			else if (myExposure_usecs > cExposureMax_us)
			{
				myExposure_usecs	=	cExposureMax_us;
				CONSOLE_DEBUG_W_DBL("Setting to default maximum exposure time of", myExposure_usecs);
			}
			cCurrentExposure_us		=	myExposure_usecs;
		}
		else
		{
//			CONSOLE_DEBUG("'Duration' not specified");
		}

	}
	else
	{
		CONSOLE_DEBUG("Internal Error");
	//	alpacaErrCode	=	kASCOM_Err_InternalError;
		exit(0);
	}

	return kASCOM_Err_Success;
}


#pragma mark -

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Fullwellcapacity(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
double				fullWellCapacity;
int					maxPixelValue;

	if (reqData != NULL)
	{
		maxPixelValue		=	pow(2, cBitDepth);
		fullWellCapacity	=	maxPixelValue * cElectronsPerADU;
		JsonResponse_Add_Double(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								fullWellCapacity,
								INCLUDE_COMMA);

		JsonResponse_Add_String(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"comment-fullwell",
								"Callulated value = (2^bitdepth) * cElectronsPerADU",
								INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Gain(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					cameraGainValue;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		cameraGainValue	=	0;
		alpacaErrCode	=	Read_Gain(&cameraGainValue);
		if (alpacaErrCode == kASCOM_Err_Success)
		{
			cGain	=	cameraGainValue;
		}
		else
		{
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to read gain");
			alpacaErrCode	=	kASCOM_Err_InternalError;
//			CONSOLE_DEBUG_W_STR("alpacaErrMsg\t=",		alpacaErrMsg);
//			CONSOLE_DEBUG_W_LONG("cGainMin\t=",			cGainMin);
//			CONSOLE_DEBUG_W_LONG("cGainMax\t=",			cGainMax);
//			CONSOLE_DEBUG_W_NUM("cameraGainValue\t=",	cameraGainValue);
//			CONSOLE_DEBUG_W_NUM("cGain\t=",				cGain);
		}
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,			//	gValueString,
								cGain,
								INCLUDE_COMMA);

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
//	curl -X PUT "https://virtserver.swaggerhub.com/ASCOMInitiative/api/v1/camera/0/gain"
//	-H  "accept: application/json"
//	-H  "Content-Type: application/x-www-form-urlencoded"
//	-d "Gain=2&ClientID=67&ClientTransactionID=8888"
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Gain(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					newGainValue;
bool				gainFound;
char				gainString[32];

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		gainFound		=	GetKeyWordArgument(	reqData->contentData,
												"Gain",
												gainString,
												(sizeof(gainString) -1));
		if (gainFound)
		{
			newGainValue	=	atoi(gainString);
			if ((newGainValue >= cGainMin) && (newGainValue <= cGainMax))
			{
				alpacaErrCode	=	Write_Gain(newGainValue);
				if (alpacaErrCode == kASCOM_Err_Success)
				{
					cGain	=	newGainValue;
				}
			}
			else
			{
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Gain value outside of min/max");
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Gain not specified");
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			CONSOLE_DEBUG(alpacaErrMsg);
		}
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								gValueString,
								cGain,
								INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_binX(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,		//gValueString,
								cCurrentBinX,
								INCLUDE_COMMA);

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_binY(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;


	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,		//gValueString,
								cCurrentBinY,
								INCLUDE_COMMA);

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_binX(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
char				argumentString[32];
bool				foundKeyWord;
int					newBinValue;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"BinX",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			CONSOLE_DEBUG_W_STR("argumentString\t=", argumentString);
			newBinValue	=	atoi(argumentString);
			if ((newBinValue >= 1) && (newBinValue <= cMaxbinX))
			{
				cCurrentBinX	=	newBinValue;
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Value out of range");
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Keyword 'BinX' not found");
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			CONSOLE_DEBUG(alpacaErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_binY(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
char				argumentString[32];
bool				foundKeyWord;
int					newBinValue;

	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"BinY",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			newBinValue	=	atoi(argumentString);
                        if ((newBinValue >= 1) && (newBinValue <= cMaxbinY))
			{
				cCurrentBinY	=	newBinValue;
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Value out of range");
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
			}
		}
		else
		{
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Keyword 'BinY' not found");
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_numX(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,		//gValueString,
								cNumX,
								INCLUDE_COMMA);

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_numY(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,		//gValueString,
								cNumY,
								INCLUDE_COMMA);

	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_numX(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
char				argumentString[32];
bool				foundKeyWord;
int					newValue;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"NumX",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			newValue	=	atoi(argumentString);
			// ?????
			// CONFORM expects no erron, even if PUT /numx/ in case newValue > cCameraXsize / cCurrentBinX
			// instead, error should be reported on PUT /startexposure/
			if ((newValue >= 1))// && (newValue <= cCameraXsize))
			{
				cNumX			=	newValue;
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Value out of range");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_numY(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
char				argumentString[32];
bool				foundKeyWord;
int					newValue;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"NumY",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			newValue	=	atoi(argumentString);
                        // ?????
                        // CONFORM expects no erron, even if PUT /numy/ in case newValue > cCameraXsize / cCurrentBinX
                        // instead, error should be reported on PUT /startexposure/
                        if ((newValue >= 1)) // && (newValue <= cCameraXsize))
			{
				cNumY			=	newValue;
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
                                GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Value out of range");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_startX(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,		//gValueString,
								cStartX,
								INCLUDE_COMMA);
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_startY(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,		//gValueString,
								cStartY,
								INCLUDE_COMMA);

		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_startX(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
char				argumentString[32];
bool				foundKeyWord;
int					newValue;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"StartX",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			newValue	=	atoi(argumentString);
			if (newValue >= 0)
			{
				cStartX			=	newValue;
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
                                GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Value out of range");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Keyword 'StartX' not found");
			CONSOLE_DEBUG(alpacaErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_startY(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
char				argumentString[32];
bool				foundKeyWord;
int					newValue;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"StartY",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			newValue	=	atoi(argumentString);
			if (newValue >= 0)
			{
				cStartY			=	newValue;
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
                                GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Value out of range");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Keyword 'StartY' not found");
			CONSOLE_DEBUG(alpacaErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_CCDtemperature(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS		alpacaErrCode;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		if (cTempReadSupported)
		{
			alpacaErrCode	=	Read_SensorTemp();
			if (alpacaErrCode == 0)
			{
				JsonResponse_Add_Double(reqData->socket,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										responseString,
										cCameraTemp_Dbl,
										INCLUDE_COMMA);

//				JsonResponse_Add_String(reqData->socket,
//										reqData->jsonTextBuffer,
//										kMaxJsonBuffLen,
//										"Comment",
//										"Deg C",
//										INCLUDE_COMMA);

			}
			else
			{
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to read temperature:");
				strcat(alpacaErrMsg, cLastCameraErrMsg);
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
//*	GET- Returns the current cooler on/off state.
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Cooleron(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
bool				coolerState;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		if (cIsCoolerCam)
		{
			alpacaErrCode	=	Read_CoolerState(&coolerState);
			if (alpacaErrCode == 0)
			{
		//		CONSOLE_DEBUG(__FUNCTION__);
			}
			else if (alpacaErrCode == kASCOM_Err_NotImplemented)
			{
				coolerState	=	false;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Not implemented:");
				strcat(alpacaErrMsg, cLastCameraErrMsg);
		//		CONSOLE_DEBUG(alpacaErrMsg);
			}
			else
			{
				coolerState	=	false;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to read cooler state:");
				strcat(alpacaErrMsg, cLastCameraErrMsg);
		//		CONSOLE_DEBUG(alpacaErrMsg);
			}
			JsonResponse_Add_Bool(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									responseString,			//	"Value",
									coolerState,
									INCLUDE_COMMA);
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}



//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Cooleron(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
char				argumentString[32];
bool				foundKeyWord;

	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		if (cIsCoolerCam)
		{
			foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
													"CoolerOn",
													argumentString,
													(sizeof(argumentString) -1));
			if (foundKeyWord)
			{
				CONSOLE_DEBUG_W_STR("argumentString\t=", argumentString);

				if (strcmp(argumentString, "true") == 0)
				{
					alpacaErrCode	=	Cooler_TurnOn();
				}
				else
				{
					alpacaErrCode	=	Cooler_TurnOff();
				}

				if (alpacaErrCode != kASCOM_Err_Success)
				{
					GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, cLastCameraErrMsg);
				}
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_InvalidValue;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "'CoolerOn' is missing");
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "This Camera does not support cooling");
			CONSOLE_DEBUG(alpacaErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_CoolerPowerLevel(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;

	cLastCameraErrMsg[0]	=	0;
	if (reqData != NULL)
	{
		if (cIsCoolerCam)
		{
			alpacaErrCode		=	Read_CoolerPowerLevel();
			if (alpacaErrCode == 0)
			{
				JsonResponse_Add_Int32(	reqData->socket,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										responseString,
										cCoolerPowerLevel,
										INCLUDE_COMMA);
			}
			else
			{
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to read cooler level:, Camera Err=");
				strcat(alpacaErrMsg, cLastCameraErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_NotImplemented;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "This Camera does not support cooling");
//			CONSOLE_DEBUG(alpacaErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
//*	reports the maximum exposure time supported by StartExposure.
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Exposuremax(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	if (reqData != NULL)
	{
		cExposureMax_seconds	=	(1.0 * cExposureMax_us) / 1000000.0;
		JsonResponse_Add_Double(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									responseString,
									cExposureMax_seconds,
									INCLUDE_COMMA);

		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Exposuremin(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode;

	if (reqData != NULL)
	{
		cExposureMin_seconds	=	(1.0 * cExposureMin_us) / 1000000.0;
		JsonResponse_Add_Double(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									responseString,
									cExposureMin_seconds,
									INCLUDE_COMMA);

		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Pulseguide(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
bool				directionFound;
bool				durationFound;
char				directionString[32];
char				durationString[32];

	CONSOLE_DEBUG(__FUNCTION__);
	if (cCanPulseGuide)
	{
		if (reqData != NULL)
		{
			if (cSt4Port)
			{
				//*	"Direction=1&Duration=2&ClientID=34&ClientTransactionID=56"
				directionFound	=	GetKeyWordArgument(	reqData->contentData,
														"Direction",
														directionString,
														(sizeof(directionString) -1));

				durationFound	=	GetKeyWordArgument(	reqData->contentData,
														"Duration",
														directionString,
														(sizeof(durationString) -1));
				if (directionFound && durationFound)
				{
					alpacaErrCode	=	kASCOM_Err_Success;
					cIsPulseGuiding	=	true;
					gettimeofday(&cPulseGuideStartTime, NULL);
				}
				else
				{
					alpacaErrCode	=	kASCOM_Err_InvalidValue;
					GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Keywords missing");
					CONSOLE_DEBUG(alpacaErrMsg);
				}
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_NotImplemented;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Pulse guiding not supported");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InternalError;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_NotImplemented;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Pulse Guide not finished")
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Readoutmode(	TYPE_GetPutRequestData	*reqData,
									char					*alpacaErrMsg,
									const char				*responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					iii;
char				currentImgTypeString[kImgTypeStrMaxLen];

	//*	this is a little convoluted.
	//*	the index that has to reported is based on the list of image types

	currentImgTypeString[0]	=	0;

	//*	update the current alpaca read out mode
	for (iii=0; iii<kMaxImageTypes; iii++)
	{
		if (cDesiredImageType == cSupportedImageTypes[iii].internalImgageType)
		{
			cCurrAlpacaImgTypeIdx	=	iii;
			strcpy(currentImgTypeString, cSupportedImageTypes[iii].imageTypeString);
			break;
		}
	}

	JsonResponse_Add_Int32(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,				//	"Value",
							cCurrAlpacaImgTypeIdx,
							INCLUDE_COMMA);


	JsonResponse_Add_String(reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"readoutmode-str",			//	"Value",
							currentImgTypeString,
							INCLUDE_COMMA);

	return(alpacaErrCode);
}

//*****************************************************************************
//*	Returns a value indicating whether the sensor is monochrome, or what Bayer matrix it encodes. Where:
//*
//*		0 = Monochrome,
//*		1 = Colour not requiring Bayer decoding
//*		2 = RGGB Bayer encoding
//*		3 = CMYG Bayer encoding
//*		4 = CMYG2 Bayer encoding
//*		5 = LRGB TRUESENSE Bayer encoding.
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Sensortype(	TYPE_GetPutRequestData	*reqData,
									char					*alpacaErrMsg,
									const char				*responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					sensortType;

	if (cIsColorCam)
	{
		sensortType	=	1;
	}
	else
	{
		sensortType	=	0;
	}


	JsonResponse_Add_Int32(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,				//	"Value",
							sensortType,
							INCLUDE_COMMA);
	return(alpacaErrCode);
}


//*****************************************************************************
//	curl -X PUT "https://virtserver.swaggerhub.com/ASCOMInitiative/api/v1/camera/0/readoutmode"
//			-H  "accept: application/json"
//			-H  "Content-Type: application/x-www-form-urlencoded"
//			-d "ReadoutMode=3&ClientID=7&ClientTransactionID=7"
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Readoutmode(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					mySocketFD;
bool				readOutFound;
char				readOutModeString[32];
int					alpacaReadOutModeIdx;
TYPE_IMAGE_TYPE		newImageTypes;

//	CONSOLE_DEBUG(__FUNCTION__);

	readOutFound		=	GetKeyWordArgument(	reqData->contentData,
												"ReadoutMode",
												readOutModeString,
												(sizeof(readOutModeString) -1));

//	CONSOLE_DEBUG_W_STR("readOutModeString\t=", readOutModeString);

	if (readOutFound)
	{
		if (isdigit(readOutModeString[0]))
		{
//			CONSOLE_DEBUG("Processing digit");
			alpacaReadOutModeIdx	=	(TYPE_IMAGE_TYPE)atoi(readOutModeString);
			//*	now we have to translate this ALPACA image mode index to the native internal image mode

			newImageTypes	=	XlateAlpacaImgIdxToIntImgType(alpacaReadOutModeIdx);
//			CONSOLE_DEBUG_W_NUM("alpacaReadOutModeIdx\t=",	alpacaReadOutModeIdx);
//			CONSOLE_DEBUG_W_NUM("newImageTypes\t\t=",	newImageTypes);

			alpacaErrCode	=	SetImageType(newImageTypes);
		}
		else
		{
//			CONSOLE_DEBUG("Processing string");
			alpacaErrCode	=	SetImageType(readOutModeString);
		}
		if (alpacaErrCode != kASCOM_Err_Success)
		{
			CONSOLE_DEBUG_W_NUM("alpacaErrCode\t=",		alpacaErrCode);
			CONSOLE_DEBUG_W_STR("cLastCameraErrMsg\t=",	cLastCameraErrMsg);
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, cLastCameraErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InvalidValue;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Mode not specified");
	}
	mySocketFD	=	reqData->socket;

	JsonResponse_Add_Int32(	mySocketFD,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"Value",
							0,
							INCLUDE_COMMA);
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Readoutmodes(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					mySocketFD;
char				lineBuff[128];

	mySocketFD	=	reqData->socket;
	lineBuff[0]	=	0;
	Read_Readoutmodes(lineBuff, true);

	JsonResponse_Add_ArrayStart(mySocketFD,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString);

	JsonResponse_Add_RawText(	mySocketFD,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								lineBuff);

	JsonResponse_Add_ArrayEnd(	mySocketFD,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								INCLUDE_COMMA);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_SetCCDtemperature(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;

	if (cCansetccdtemperature)
	{
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Setting CCD temp not supported");
		alpacaErrCode	=	kASCOM_Err_NotImplemented;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
//*	ClientTransactionID=136&ClientID=22074&SetCCDTemperature=85
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_SetCCDtemperature(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
bool				setCCDtempFound;
char				setCCDtempString[256];
double				newSetCCDvalue;

//	CONSOLE_DEBUG(__FUNCTION__);
//	CONSOLE_DEBUG_W_STR("contentData\t=",	reqData->contentData);

	if (cCansetccdtemperature)
	{
		setCCDtempFound	=	GetKeyWordArgument(	reqData->contentData,
												"SetCCDTemperature",
												setCCDtempString,
												(sizeof(setCCDtempString) -1));
		if (setCCDtempFound)
		{
			newSetCCDvalue	=	atof(setCCDtempString);
			if ((newSetCCDvalue > -273.15) && (newSetCCDvalue < 99.0))
			{
				alpacaErrCode	=	kASCOM_Err_Success;
			}
			else
			{
				alpacaErrCode	=	kASCOM_Err_InvalidValue;

				CONSOLE_DEBUG_W_DBL("newSetCCDvalue\t=",	newSetCCDvalue);
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "CCD Setpoint Temperature out of bounds");
				CONSOLE_DEBUG(alpacaErrMsg);
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;

			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "New temperature not specified");
			CONSOLE_DEBUG(alpacaErrMsg);
		}
	}
	else
	{
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Setting CCD temp not supported");
		alpacaErrCode	=	kASCOM_Err_NotImplemented;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
bool	CameraDriver::IsCameraIDvalid(const int argCameraID)
{
	return(true);

}

//*****************************************************************************
void	CameraDriver::ResetCamera(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
}



//*****************************************************************************
//*	Start Exposure
//*		Process:
//*	http://192.168.0.201:6800/api/v1.0.0-oas3/camera/0/startexposure Content-Type: -dDuration=0.001&Light=true
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_StartExposure(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
double				currExposure_secs;

	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
//		CONSOLE_DEBUG_W_STR("contentData\t=",	reqData->contentData);

		alpacaErrCode = ProcessExposureOptions(reqData, alpacaErrMsg);
		if(alpacaErrCode != kASCOM_Err_Success)
		{
			return alpacaErrCode;
		}

                if(cNumX > cCameraXsize / cCurrentBinX)
                {
                        char expectedErrMsg[64];
                        sprintf(expectedErrMsg, "X size larger than binned chip size, Bin %dx%d", cCurrentBinX, cCurrentBinX);
                        GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, expectedErrMsg);
                        return kASCOM_Err_InvalidValue;
                }

                if(cNumY > cCameraYsize / cCurrentBinY)
                {
                        char expectedErrMsg[64];
                        sprintf(expectedErrMsg, "X size larger than binned chip size, Bin %dx%d", cCurrentBinX, cCurrentBinX);
                        GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, expectedErrMsg);
                        return kASCOM_Err_InvalidValue;
                }
		if(cStartX > cCameraXsize / cCurrentBinX)
		{
			char expectedErrMsg[64];
			sprintf(expectedErrMsg, "X start outside binned chip size, Bin %dx%d", cCurrentBinX, cCurrentBinY);
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, expectedErrMsg);
			return kASCOM_Err_InvalidValue;
		}

		if(cStartY > cCameraYsize / cCurrentBinY)
		{
			char expectedErrMsg[64];
			sprintf(expectedErrMsg, "Y start outside binned chip size, Bin %dx%d", cCurrentBinX, cCurrentBinX);
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, expectedErrMsg);
			return kASCOM_Err_InvalidValue;
		}


		cAVIfourcc					=	0;
		cFrameRate					=	0;
		cSaveNextImage				=	true;
		cLastexposureduration_us	=	cCurrentExposure_us;
//-?		cNumFramesSaved				=	0;

		//*	save the time we started the exposure
		//*	this really belongs in Start_CameraExposure, but just in case
		gettimeofday(&cLastExposureStartTime, NULL);

//		CONSOLE_DEBUG(__FUNCTION__);
		alpacaErrCode				=	Start_CameraExposure(cCurrentExposure_us);
//		CONSOLE_DEBUG(__FUNCTION__);
		GenerateFileNameRoot();
//		CONSOLE_DEBUG(__FUNCTION__);

		if (alpacaErrCode == 0)
		{
			JsonResponse_Add_String(reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"filenameroot",
									cFileNameRoot,
									INCLUDE_COMMA);

			currExposure_secs	=	(1.0 * cCurrentExposure_us) / 1000000.0;
			JsonResponse_Add_Double(reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"exposure",
									currExposure_secs,
									INCLUDE_COMMA);
		}
		else
		{
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, cLastCameraErrMsg);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_StopExposure(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	alpacaErrCode	=	Stop_Exposure();

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_AbortExposure(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (cCanAbortExposure)
	{
		cInternalCameraState		=	kCameraState_Idle;
		cImageMode					=	kImageMode_Single;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_NotImplemented;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Abort exposure not finished")
	}
	return(alpacaErrCode);
}



//*****************************************************************************
TYPE_EXPOSURE_STATUS	CameraDriver::Check_Exposure(bool verboseFlag)
{
	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG("Houston, we have a problem, we should not be here!!!!!");
	return(kExposure_Unknown);
}

//*****************************************************************************
TYPE_ALPACA_CAMERASTATE	CameraDriver::Read_AlapcaCameraState(void)
{
TYPE_ALPACA_CAMERASTATE	alpacaCameraState;
TYPE_EXPOSURE_STATUS	internalCameraState;

	CONSOLE_DEBUG(__FUNCTION__);
	//*	set a default value
	alpacaCameraState	=	kALPACA_CameraState_Idle;

	internalCameraState	=	Check_Exposure(false);
	switch(internalCameraState)
	{
		case kExposure_Idle:
			alpacaCameraState	=	kALPACA_CameraState_Idle;
			break;

		case kExposure_Working:
			alpacaCameraState	=	kALPACA_CameraState_Exposing;
			break;

		case kExposure_Success:
			alpacaCameraState	=	kALPACA_CameraState_Idle;
			break;

		case kExposure_Failed:
			alpacaCameraState	=	kALPACA_CameraState_Error;
			break;

		case kExposure_Unknown:
		case kExposure_Last:
		default:
			alpacaCameraState	=	kALPACA_CameraState_Idle;
			break;
	}

	return(alpacaCameraState);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Lastexposureduration(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
double				exposureTimeSecs;

	if (cLastexposureduration_us > 0)
	{
		exposureTimeSecs	=	(cLastexposureduration_us * 1.0) / 1000000.0;

		JsonResponse_Add_Double(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								exposureTimeSecs,
								INCLUDE_COMMA);
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InvalidOperation;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "No valid image");
		CONSOLE_DEBUG(alpacaErrMsg);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Lastexposurestarttime(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
char				timeString[64];

//	CONSOLE_DEBUG(__FUNCTION__);

	if (cLastExposureStartTime.tv_sec > 0)
	{
		FormatTimeStringISO8601(	&cLastExposureStartTime,
							timeString);

	//	CONSOLE_DEBUG(timeString);
		JsonResponse_Add_String(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								timeString,
								INCLUDE_COMMA);
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InvalidOperation;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "No valid image");
		CONSOLE_DEBUG(alpacaErrMsg);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_ImageReady(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
bool				imageReady;
int					exposureStatatus;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		exposureStatatus	=	Check_Exposure(false);
		if ( exposureStatatus == kExposure_Success ||
		    (exposureStatatus == kExposure_Idle && cNewImageReadyToDownload))
		{
			imageReady	=	true;
		}
		else
		{
			imageReady	=	false;
		}
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								gValueString,
								imageReady,
								INCLUDE_COMMA);
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Imagearray(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					pixelCount;
int					ii;
int					mySocket;
unsigned char		*pixelPtr;
char				lineBuff[64];
uint32_t			pixelValue;
char				imageTimeString[64];
double				exposureTimeSecs;

//	CONSOLE_DEBUG(__FUNCTION__);

	mySocket	=	reqData->socket;

	//========================================================================================
	//*	record the time the image was taken
	FormatTimeStringISO8601(&cLastExposureStartTime, imageTimeString);
	JsonResponse_Add_String(mySocket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"ImageTime",
							imageTimeString,
							INCLUDE_COMMA);

	//========================================================================================
	//*	record the exposure time
	exposureTimeSecs	=	(cLastexposureduration_us * 1.0) /
							1000000.0;
	JsonResponse_Add_Double(mySocket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"Exposure",
							exposureTimeSecs,
							INCLUDE_COMMA);

	//========================================================================================
	//*	record the sensor temp
	alpacaErrCode	=	Read_SensorTemp();
	JsonResponse_Add_Double(mySocket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							"ccdtemperature",
							cCameraTemp_Dbl,
							INCLUDE_COMMA);


	//*	get the ROI information which has the current image type
	GetImage_ROI_info();
	pixelCount	=	cROIinfo.currentROIwidth * cROIinfo.currentROIheight;
	CONSOLE_DEBUG_W_NUM("pixelCount\t=", pixelCount);

#if 1
	CONSOLE_DEBUG_W_NUM("cNewImageReadyToDownload\t=", cNewImageReadyToDownload);
	if (cNewImageReadyToDownload && (cCameraDataBuffer != NULL))
	{
		//========================================================================================
		//*	record the image type
//+			Read_ImageTypeString(cROIinfo.currentROIimageType, asiImageTypeString);
//+			JsonResponse_Add_String(mySocket,
//+									reqData->jsonTextBuffer,
//+									kMaxJsonBuffLen,
//+									"ImageType",
//+									asiImageTypeString,
//+									INCLUDE_COMMA);
		//*	record the image size
		JsonResponse_Add_Int32(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"xsize",
								cROIinfo.currentROIwidth,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"ysize",
								cROIinfo.currentROIheight,
								INCLUDE_COMMA);

//		CONSOLE_DEBUG(__FUNCTION__);
		//*	arraytype = 2  >> 32 bit interger
		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
//								"ArrayType",
								"Type",
								2,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"Rank",
								2,
								INCLUDE_COMMA);

		JsonResponse_Add_ArrayStart(	mySocket,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										gValueString);
		pixelPtr	=	cCameraDataBuffer;
		CONSOLE_DEBUG_W_NUM("pixelCount\t=", pixelCount);
		for (ii=0; ii<pixelCount; ii++)
		{
			if(ii % cROIinfo.currentROIheight == 0)
			{
				if(ii > 0)
				{
					JsonResponse_Add_ArrayEnd(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									INCLUDE_COMMA);

				}
				JsonResponse_Add_ArrayStart(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								nullptr);
			}
			switch(cROIinfo.currentROIimageType)
			{
				case kImageType_RAW8:
					pixelValue		=	(pixelPtr[0] & 0x00ff);
					pixelPtr		+=	1;
				//	sprintf(lineBuff, "0x%02lX,\n", pixelValue);
					sprintf(lineBuff, "%d,\n", pixelValue);
					break;

				case kImageType_RAW16:
					pixelValue		=	(pixelPtr[0] << 8) +
										(pixelPtr[1]);
					pixelPtr		+=	2;
				//	sprintf(lineBuff, "0x%04X,\n", pixelValue);
					sprintf(lineBuff, "%d,\n", pixelValue);
					break;

				case kImageType_RGB24:
					pixelValue		=	(pixelPtr[0] << 16) +
										(pixelPtr[1] << 8) +
										(pixelPtr[2]);
					pixelPtr		+=	3;
				//	sprintf(lineBuff, "0x%06X,\n", pixelValue);
					sprintf(lineBuff, "%d,\n", pixelValue);
					break;

				case kImageType_Y8:
					break;

				default:
					break;
			}
			JsonResponse_Add_RawText(	mySocket,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										lineBuff);

			if ((ii % 50000) == 0)
			{
				CONSOLE_DEBUG_W_NUM("ii\t\t=", ii);
			}
		}

		CONSOLE_DEBUG_W_NUM("Lines sent\t=", ii);

		JsonResponse_Add_ArrayEnd(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									NO_COMMA);

		JsonResponse_Add_ArrayEnd(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									INCLUDE_COMMA);
		CONSOLE_DEBUG_W_NUM("pixelCount\t=", pixelCount);
	}
	else
	{
		//*	as per Rick B 6/22/2020
		alpacaErrCode	=	kASCOM_Err_InvalidOperation;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "No image to get");
	}
#else
	alpacaErrCode	=	Read_ImageData();
	CONSOLE_DEBUG_W_NUM("Read_ImageData returned", alpacaErrCode);


	if (alpacaErrCode == 0)
	{
		//========================================================================================
		//*	record the image type
//+			Read_ImageTypeString(cROIinfo.currentROIimageType, asiImageTypeString);
//+			JsonResponse_Add_String(mySocket,
//+									reqData->jsonTextBuffer,
//+									kMaxJsonBuffLen,
//+									"ImageType",
//+									asiImageTypeString,
//+									INCLUDE_COMMA);
		//*	record the image size
		JsonResponse_Add_Int32(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"xsize",
								cROIinfo.currentROIwidth,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"ysize",
								cROIinfo.currentROIheight,
								INCLUDE_COMMA);

		CONSOLE_DEBUG(__FUNCTION__);
		if (cCameraDataBuffer != NULL)
		{
		#ifdef _USE_OPENCV_
			CONSOLE_DEBUG("Saving image via OpenCV");
			CreateOpenCVImage(cCameraDataBuffer);
		#else
			CONSOLE_DEBUG("Image save not enabled");
		#endif
			//*	arraytype = 2  >> 32 bit interger
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"ArrayType",
									2,
									INCLUDE_COMMA);

			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"Rank",
									2,
									INCLUDE_COMMA);

			JsonResponse_Add_ArrayStart(	mySocket,
											reqData->jsonTextBuffer,
											kMaxJsonBuffLen,
											gValueString);
			pixelPtr	=	cCameraDataBuffer;
			for (ii=0; ii<pixelCount; ii++)
			{
				switch(cROIinfo.currentROIimageType)
				{
					case kImageType_RAW8:
						pixelValue		=	(pixelPtr[0] & 0x00ff);
						pixelPtr		+=	1;
					//	sprintf(lineBuff, "0x%02lX,\n", pixelValue);
						sprintf(lineBuff, "%d,\n", pixelValue);
						break;

					case kImageType_RAW16:
						pixelValue		=	(pixelPtr[0] << 8) +
											(pixelPtr[1]);
						pixelPtr		+=	2;
						sprintf(lineBuff, "0x%04X,\n", pixelValue);
						break;

					case kImageType_RGB24:
						pixelValue		=	(pixelPtr[0] << 16) +
											(pixelPtr[1] << 8) +
											(pixelPtr[2]);
						pixelPtr		+=	3;
						sprintf(lineBuff, "0x%06X,\n", pixelValue);
						break;

					case kImageType_Y8:
						break;

					default:
						break;
				}
				JsonResponse_Add_RawText(	mySocket,
											reqData->jsonTextBuffer,
											kMaxJsonBuffLen,
											lineBuff);

				if ((ii % 50000) == 0)
				{
					CONSOLE_DEBUG_W_NUM("ii\t\t=", ii);
				}
			}

			CONSOLE_DEBUG_W_NUM("Lines sent\t=", ii);

			JsonResponse_Add_ArrayEnd(	mySocket,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										INCLUDE_COMMA);
		}
		else
		{
			CONSOLE_DEBUG("Failed to allocate buffer");
		}
	}
	else
	{
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to read image-");
		strcat(alpacaErrMsg, cLastCameraErrMsg);
		CONSOLE_DEBUG_W_STR("alpacaErrCode\t=",		alpacaErrMsg);
	}
#endif
	CONSOLE_DEBUG_W_STR(__FUNCTION__, "--exit");
	return(alpacaErrCode);
}

//*****************************************************************************
//*	this always returns 24 bit pixels, in hex they are 0x00RRGGBB
//*	if the image is b/w, it converts the pixel to the RGB grey scale equivalent
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_RGBarray(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					pixelCount;
int					pixelLimit;
int					iii;
int					mySocket;
unsigned char		*pixelPtr;
uint32_t			pixelValue;
char				lineBuff[256];
char				imageTimeString[256];
double				exposureTimeSecs;
TYPE_ASCOM_STATUS	tempSensorErr;

	CONSOLE_DEBUG(__FUNCTION__);
//*	Jun 25,	2020	<MLS> Changed JSON xmit buffer limit to 1475, significant speed improvement

#define		kBuffSize_MaxSpeed	1475

	mySocket	=	reqData->socket;

	//========================================================================================
	//*	record the time the image was taken
	FormatTimeString(&cLastExposureStartTime.tv_sec, imageTimeString);
	JsonResponse_Add_String(mySocket,
							reqData->jsonTextBuffer,
							kBuffSize_MaxSpeed,
							"ImageTime",
							imageTimeString,
							INCLUDE_COMMA);

	//========================================================================================
	//*	record the exposure time
	exposureTimeSecs	=	(cLastexposureduration_us * 1.0) /
							1000000.0;
	JsonResponse_Add_Double(mySocket,
							reqData->jsonTextBuffer,
							kBuffSize_MaxSpeed,
							"Exposure",
							exposureTimeSecs,
							INCLUDE_COMMA);

	//========================================================================================
	//*	record the sensor temp
	tempSensorErr	=	Read_SensorTemp();
	if (tempSensorErr == kASCOM_Err_Success)
	{
		JsonResponse_Add_Double(mySocket,
								reqData->jsonTextBuffer,
								kBuffSize_MaxSpeed,
								"ccdtemperature",
								cCameraTemp_Dbl,
								INCLUDE_COMMA);


	}
	//*	get the ROI information which has the current image type
	GetImage_ROI_info();
	pixelCount	=	cROIinfo.currentROIwidth * cROIinfo.currentROIheight;
	CONSOLE_DEBUG_W_NUM("pixelCount\t=", pixelCount);

	CONSOLE_DEBUG_W_NUM("cNewImageReadyToDownload\t=", cNewImageReadyToDownload);
	if (cNewImageReadyToDownload && (cCameraDataBuffer != NULL))
	{
		//========================================================================================
		//*	record the image type
//+			Read_ImageTypeString(cROIinfo.currentROIimageType, asiImageTypeString);
//+			JsonResponse_Add_String(mySocket,
//+									reqData->jsonTextBuffer,
//+									kBuffSize_MaxSpeed,
//+									"ImageType",
//+									asiImageTypeString,
//+									INCLUDE_COMMA);
		//*	record the image size
		JsonResponse_Add_Int32(mySocket,
								reqData->jsonTextBuffer,
								kBuffSize_MaxSpeed,
								"xsize",
								cROIinfo.currentROIwidth,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(mySocket,
								reqData->jsonTextBuffer,
								kBuffSize_MaxSpeed,
								"ysize",
								cROIinfo.currentROIheight,
								INCLUDE_COMMA);

//		CONSOLE_DEBUG(__FUNCTION__);
		//*	arraytype = 2  >> 32 bit interger
		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kBuffSize_MaxSpeed,
								"ArrayType",
								2,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kBuffSize_MaxSpeed,
								"Rank",
								2,
								INCLUDE_COMMA);

		JsonResponse_Add_ArrayStart(	mySocket,
										reqData->jsonTextBuffer,
										kBuffSize_MaxSpeed,
										gValueString);
		pixelPtr	=	cCameraDataBuffer;
		CONSOLE_DEBUG_W_NUM("pixelCount\t=", pixelCount);

		//====================================================================
		//*	this is broken up the way it is to increase transmission speed
		iii		=	0;
		switch(cROIinfo.currentROIimageType)
		{
			//====================================================================
			case kImageType_RGB24:
				CONSOLE_DEBUG("kImageType_RGB24");
				pixelLimit	=	pixelCount - 1;
				iii	=	0;
				while (iii < pixelLimit)
				{
					pixelValue		=	((pixelPtr[0] & 0x00ff) << 16) +
										((pixelPtr[1] & 0x00ff) << 8) +
										((pixelPtr[2] & 0x00ff));
					pixelPtr		+=	3;
					sprintf(lineBuff, "%d,\n", pixelValue);
					JsonResponse_Add_RawText(	mySocket,
												reqData->jsonTextBuffer,
												kBuffSize_MaxSpeed,
												lineBuff);
					iii++;

					if ((iii % 500000) == 0)
					{
					double	cpuTemp_DegC;
					double	cpuTemp_DegF;
						cpuTemp_DegC	=	CPUstats_GetTemperature(NULL);
						cpuTemp_DegF	=	((cpuTemp_DegC * (9.0/5.0)) + 32);
						CONSOLE_DEBUG_W_DBL("cpuTemp_DegF\t=", cpuTemp_DegF);
					}
				}
				CONSOLE_DEBUG("Done");
				//*	now do the last one WITHOUT the comma
				pixelValue		=	(pixelPtr[0] << 16) +
									(pixelPtr[1] << 8) +
									(pixelPtr[2]);
				sprintf(lineBuff, "%d\n", pixelValue);
				JsonResponse_Add_RawText(	mySocket,
											reqData->jsonTextBuffer,
											kBuffSize_MaxSpeed,
											lineBuff);
				CONSOLE_DEBUG("Done");
				break;

			//====================================================================
			case kImageType_RAW8:
				CONSOLE_DEBUG("kImageType_RAW8");
				pixelLimit	=	pixelCount - 1;
				iii	=	0;
				while (iii < pixelLimit)
				{
					pixelValue		=	(pixelPtr[0] & 0x00ff);
					pixelValue		=	pixelValue * 0x010101;
					pixelPtr		+=	1;
					sprintf(lineBuff, "%d,\n", pixelValue);
					JsonResponse_Add_RawText(	mySocket,
												reqData->jsonTextBuffer,
												kBuffSize_MaxSpeed,
												lineBuff);
					iii++;
				}
				//*	now do the last one WITHOUT the comma
				pixelValue		=	(pixelPtr[0] & 0x00ff);
				pixelValue		=	pixelValue * 0x010101;
				pixelPtr		+=	1;
				sprintf(lineBuff, "%d\n", pixelValue);
				JsonResponse_Add_RawText(	mySocket,
											reqData->jsonTextBuffer,
											kBuffSize_MaxSpeed,
											lineBuff);
				CONSOLE_DEBUG("Done");
				break;

			//====================================================================
			case kImageType_RAW16:
				CONSOLE_DEBUG("kImageType_RAW16");
				pixelLimit	=	pixelCount - 1;
				iii	=	0;
				while (iii < pixelLimit)
				{
					pixelValue		=	(pixelPtr[0] & 0x00ff);
					pixelValue		=	pixelValue * 0x010101;
					pixelPtr		+=	2;
					sprintf(lineBuff, "%d,\n", pixelValue);
					JsonResponse_Add_RawText(	mySocket,
												reqData->jsonTextBuffer,
												kBuffSize_MaxSpeed,
												lineBuff);
					iii++;
				}
				//*	now do the last one WITHOUT the comma
				pixelValue		=	(pixelPtr[0] & 0x00ff);
				pixelValue		=	pixelValue * 0x010101;
				pixelPtr		+=	2;
				sprintf(lineBuff, "%d\n", pixelValue);
				JsonResponse_Add_RawText(	mySocket,
											reqData->jsonTextBuffer,
											kBuffSize_MaxSpeed,
											lineBuff);
				break;

			//====================================================================
			case kImageType_Y8:
				CONSOLE_DEBUG("kImageType_Y8");
				pixelLimit	=	pixelCount - 1;
				iii	=	0;
				while (iii < pixelLimit)
				{
					pixelValue		=	(pixelPtr[0] & 0x00ff);
					pixelValue		=	pixelValue * 0x010101;
					pixelPtr		+=	1;
					sprintf(lineBuff, "%d,\n", pixelValue);
					JsonResponse_Add_RawText(	mySocket,
												reqData->jsonTextBuffer,
												kBuffSize_MaxSpeed,
												lineBuff);
					iii++;
				}
				//*	now do the last one WITHOUT the comma
				pixelValue		=	(pixelPtr[0] & 0x00ff);
				pixelValue		=	pixelValue * 0x010101;
				pixelPtr		+=	1;
				sprintf(lineBuff, "%d\n", pixelValue);
				JsonResponse_Add_RawText(	mySocket,
											reqData->jsonTextBuffer,
											kBuffSize_MaxSpeed,
											lineBuff);
				break;

			//====================================================================
			case kImageType_Invalid:
			case kImageType_last:
				break;

		}

		CONSOLE_DEBUG_W_NUM("Lines sent\t=", iii);

		JsonResponse_Add_ArrayEnd(	mySocket,
									reqData->jsonTextBuffer,
									kBuffSize_MaxSpeed,
									INCLUDE_COMMA);
		CONSOLE_DEBUG_W_NUM("pixelCount\t=", pixelCount);
	}
	else
	{
		//*	as per Rick B 6/22/2020
		alpacaErrCode	=	kASCOM_Err_InvalidOperation;
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "No image available");
	}
//	CONSOLE_DEBUG_W_STR(__FUNCTION__, "--exit");
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Camerastate(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					mySocketFD;
char				stateString[16];

//	CONSOLE_DEBUG(__FUNCTION__);

	mySocketFD			=	reqData->socket;
//	cAlpacaCameraState	=	Check_Exposure(true);
	cAlpacaCameraState	=	Read_AlapcaCameraState();
	JsonResponse_Add_Int32(	mySocketFD,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							cAlpacaCameraState,
							INCLUDE_COMMA);

	switch(cAlpacaCameraState)
	{
		case	kALPACA_CameraState_Idle:		strcpy(stateString,	"Idle");		break;
		case	kALPACA_CameraState_Waiting:	strcpy(stateString,	"Waiting");		break;
		case	kALPACA_CameraState_Exposing:	strcpy(stateString,	"Exposing");	break;
		case	kALPACA_CameraState_Reading:	strcpy(stateString,	"Reading");		break;
		case	kALPACA_CameraState_Download:	strcpy(stateString,	"Download");	break;
		case	kALPACA_CameraState_Error:		strcpy(stateString,	"Error");		break;

		default:								strcpy(stateString,	"unknown");		break;
	}
	JsonResponse_Add_String(	mySocketFD,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"camerastate-string",
								stateString,
								INCLUDE_COMMA);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Fastreadout(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG(__FUNCTION__);
	alpacaErrCode	=	Read_Fastreadout();
	if (alpacaErrCode == 0)
	{
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								cHighSpeedMode,
								INCLUDE_COMMA);
	}
	else
	{
		GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to read FastReadout:, camera-err=");
		strcat(alpacaErrMsg, cLastCameraErrMsg);
	}

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Fastreadout(		TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	return(alpacaErrCode);
}

//*****************************************************************************
//*	this accepts a wide variety of inputs to set various parameters.
//*	Regardless of order on the input data, the RefID is processed first.
//*	thus allowing selection of a standard configuration and then overwriting
//*	select values.  Everything is case sensitive
//*
//*	The only place this is used is in the FITS output.
//*	Later it will be used for Firecapture text output and for xml output
//*
//*	Accepted inputs
//*		RefID		refers to a collection of settings from observatory settings.
//*		Telescope
//*		Instrument
//*		Focuser
//*		Filterwheel
//*		FocalLen
//*		Object
//*		Prefix			file name prefix
//*		Imagetype		Light/Dark
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_TelescopeInfo(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
bool				refIDFound;
char				myRefID[32];

//	CONSOLE_DEBUG(__FUNCTION__);
	strcpy(myRefID, "not specified");
	if (reqData != NULL)
	{
		alpacaErrCode	=	kASCOM_Err_Success;
		refIDFound		=	GetKeyWordArgument(	reqData->contentData,
												"RefID",
												myRefID,
												(sizeof(myRefID) -1));
		if (refIDFound)
		{
			GetTelescopeSettingsByRefID(myRefID, 0, &cTS_info);
	//		CONSOLE_DEBUG_W_STR("cTS_info.refID\t\t=",		cTS_info.refID);
	//		CONSOLE_DEBUG_W_STR("cTS_info.manufacturer\t=",	cTS_info.telescp_manufacturer);
	//		CONSOLE_DEBUG_W_STR("cTS_info.model\t\t=",		cTS_info.telescp_model);
	//		CONSOLE_DEBUG_W_STR("cTS_info.instrument\t=",	cTS_info.instrument);

			if ((strlen(cTS_info.telescp_manufacturer) > 0) || (strlen(cTS_info.telescp_model) > 0))
			{
				strcpy(cTelescopeModel, cTS_info.telescp_manufacturer);
				strcat(cTelescopeModel, "-");
				strcat(cTelescopeModel, cTS_info.telescp_model);
			}
		}

		ProcessTelescopeKeyWord(reqData->contentData,	"Telescope",	cTelescopeModel,		kTelescopeDefMaxStrLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Instrument",	cTS_info.instrument,	kTelescopeDefMaxStrLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Focuser",		cTS_info.focuser,		kTelescopeDefMaxStrLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Filterwheel",	cTS_info.filterwheel,	kTelescopeDefMaxStrLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Object",		cObjectName,			kObjectNameMaxLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Prefix",		cFileNamePrefix,		kFileNamePrefixMaxLen);
		ProcessTelescopeKeyWord(reqData->contentData,	"Suffix",		cFileNameSuffix,		kFileNamePrefixMaxLen);

		ProcessTelescopeKeyWord(reqData->contentData,	"auxtext",		cAuxTextTag,			kAuxiliaryTextMaxLen);

//		CONSOLE_DEBUG_W_STR("cTS_info.instrument\t=",	cTS_info.instrument);

		JsonResponse_Add_String(	reqData->socket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									gValueString,
									myRefID,
									INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_LiveMode(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	JsonResponse_Add_Bool(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							(cImageMode == kImageMode_Live),
							INCLUDE_COMMA);

	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_LiveMode(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
bool				liveModeFound;
char				livemodeString[64];

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		liveModeFound		=	GetKeyWordArgument(	reqData->contentData,
													"Livemode",
													livemodeString,
													(sizeof(livemodeString) -1));
		if (liveModeFound)
		{
			alpacaErrCode	=	kASCOM_Err_Success;
			if (strcasecmp(livemodeString, "true") == 0)
			{
				cImageMode	=	kImageMode_Live;
			}
			else
			{
				//*	turn live mode off and close the live view window
				cImageMode	=	kImageMode_Single;

			#ifdef _USE_OPENCV_
				CONSOLE_DEBUG_W_STR("cvDestroyWindow\t=", cOpenCV_ImgWindowName);
				cvDestroyWindow(cOpenCV_ImgWindowName);
				cCreateOpenCVwindow		=	true;
				cOpenCV_ImgWindowValid	=	false;

//				CONSOLE_DEBUG("cvDestroyWindow");
			#endif	//	_USE_OPENCV_
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "'Livemode' argument not found");
			CONSOLE_DEBUG(alpacaErrMsg);
		}
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								gValueString,
								(cImageMode == kImageMode_Live),
								INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

#ifdef _USE_OPENCV_
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Sidebar(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

//	CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);
	JsonResponse_Add_Bool(	reqData->socket,
							reqData->jsonTextBuffer,
							kMaxJsonBuffLen,
							responseString,
							cDisplaySideBar,
							INCLUDE_COMMA);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Sidebar(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
bool				keywordFound;
char				keywordString[64];

	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);

	if (reqData != NULL)
	{
		CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);
		keywordFound		=	GetKeyWordArgument(	reqData->contentData,
													"Sidebar",
													keywordString,
													(sizeof(keywordString) -1));
		if (keywordFound)
		{
			CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);
			CONSOLE_DEBUG_W_STR("keywordString\t=",		keywordString);
			alpacaErrCode	=	kASCOM_Err_Success;
			if (strcasecmp(keywordString, "true") == 0)
			{
				cDisplaySideBar	=	kSideBar_Left;
				CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);
			}
			else if (strcasecmp(keywordString, "left") == 0)
			{
				cDisplaySideBar	=	kSideBar_Left;
				CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);
			}
			else if (strcasecmp(keywordString, "right") == 0)
			{
				cDisplaySideBar	=	kSideBar_Right;
				CONSOLE_DEBUG_W_NUM("cDisplaySideBar\t=",		cDisplaySideBar);
			}
			else
			{
				cDisplaySideBar	=	false;
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "'Sidebar' argument not found");
			CONSOLE_DEBUG(alpacaErrMsg);
		}
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								gValueString,
								cDisplaySideBar,
								INCLUDE_COMMA);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}
#endif // _USE_OPENCV_


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_ExposureTime(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
double				exposureTimeSecs;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		exposureTimeSecs	=	(cCurrentExposure_us * 1.0) /
								1000000.0;

		JsonResponse_Add_Double(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,			//gValueString,
								exposureTimeSecs,
								INCLUDE_COMMA);
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_ExposureTime(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
double				exposureDuration_secs;
bool				durationFound;
char				duarationString[32];
long				exposureDuration_us;	//*	micro seconds

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		durationFound		=	GetKeyWordArgument(	reqData->contentData,
														"Duration",
														duarationString,
														(sizeof(duarationString) -1));
		if (durationFound)
		{
			exposureDuration_secs	=	atof(duarationString);
			exposureDuration_us		=	exposureDuration_secs * 1000 * 1000;
			if ((exposureDuration_us >= cExposureMin_us) && (exposureDuration_us <= cExposureMax_us))
			{
				cCurrentExposure_us		=	exposureDuration_us;
				alpacaErrCode			=	kASCOM_Err_Success;
	//			CONSOLE_DEBUG_W_DBL("New exposure time (secs)\t=", exposureDuration_secs);
			}
			else
			{
				alpacaErrCode			=	kASCOM_Err_InvalidValue;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Exposure time out of valid range");
			}
		}
		else
		{
			CONSOLE_DEBUG("Duration not specified");
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
void	CameraDriver::SaveNextImage(void)
{
	cSaveNextImage		=	true;
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_SaveNextImage(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		cSaveNextImage		=	true;
		CONSOLE_DEBUG_W_STR("cFileNameRoot before\t=",	cFileNameRoot);
		GenerateFileNameRoot();
		CONSOLE_DEBUG_W_STR("cFileNameRoot after \t=",	cFileNameRoot);
		JsonResponse_Add_String(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filenameroot",
								cFileNameRoot,
								INCLUDE_COMMA);

		alpacaErrCode		=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
//*	startsequence
//*	Standard parameteres (all optional)
//*		Duration			Exposure duration in seconds	Defaults to current setting
//*		Object
//*		Prefix
//*		Suffix
//*
//*	StartSequece parameters (all optional)
//*		Count				Number of frames to to take		Defaults to 5
//*		Delay				Seconds between frames			Defaults to 0
//*		DeltaDuration		Seconds between frames			Defaults to 0
//*
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_StartSequence(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InternalError;
bool				sequenceCntFound;
bool				delayFound;
bool				deltaDurationFound;
char				countString[32];
char				delayString[32];
char				deltaDurationString[32];
int					sequenceCnt;
double				delay_secs;
double				deltaExp_secs;

//	CONSOLE_DEBUG(__FUNCTION__);

	if (reqData != NULL)
	{
		//*	default frame count
		sequenceCnt			=	5;

		//*	All of these fields are optional
		alpacaErrCode = ProcessExposureOptions(reqData, alpacaErrMsg);
		if(alpacaErrCode != kASCOM_Err_Success)
		{
			return alpacaErrCode;
		}


		sequenceCntFound	=	GetKeyWordArgument(	reqData->contentData,
														"Count",
														countString,
														(sizeof(countString) -1));

		delayFound			=	GetKeyWordArgument(	reqData->contentData,
														"Delay",
														delayString,
														(sizeof(delayString) -1));

		deltaDurationFound	=	GetKeyWordArgument(	reqData->contentData,
														"DeltaDuration",
														deltaDurationString,
														(sizeof(deltaDurationString) -1));
		//==============================================
		if (sequenceCntFound)
		{
			sequenceCnt				=	atof(countString);
		}
		if (sequenceCnt <= 0)
		{
			sequenceCnt	=	5;
		}
		//==============================================
		if (delayFound)
		{
			delay_secs			=	atof(delayString);
			cSequenceDelay_us	=	delay_secs * 1000 * 1000;
		}
		else
		{
			cSequenceDelay_us	=	0;
		}
		//==============================================
		if (deltaDurationFound)
		{
			deltaExp_secs			=	atof(deltaDurationString);
			cSeqDeltaExposure_us	=	deltaExp_secs * 1000 * 1000;
		}
		else
		{
			cSeqDeltaExposure_us	=	0;
		}


		cNumFramesRequested	=	sequenceCnt;
		cNumFramesToSave	=	sequenceCnt;
		cNumFramesSaved		=	0;				//*	start sequence
		cImageMode			=	kImageMode_Sequence;
		cSaveNextImage		=	true;
		cImageSeqNumber		=	0;

		alpacaErrCode		=	kASCOM_Err_Success;
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
//*	if buffer size is <= zero, figure out the size
//*****************************************************************************
bool	CameraDriver::AllcateImageBuffer(long bufferSize)
{
uint32_t		myBufferSize;
bool			successFlag;

//	CONSOLE_DEBUG(__FUNCTION__);

	if ((cCameraDataBuffer != NULL) && (bufferSize <= cCameraDataBuffLen))
	{
		//*	everything is OK
//		CONSOLE_DEBUG_W_LONG("everything is OK, current buff size\t=", cCameraDataBuffLen);
		successFlag			=	true;
	}
	else
	{
		if (cCameraDataBuffer != NULL)
		{
			CONSOLE_DEBUG("Freeing existing buffer");
			//*	buffer is not big enough, free it so we can allocate a new one
			free(cCameraDataBuffer);
			cCameraDataBuffer	=	NULL;
			cCameraDataBuffLen	=	0;
		}

		if (bufferSize > 0)
		{
			myBufferSize	=	bufferSize;
		}
		else
		{
			myBufferSize	=	cCameraXsize * cCameraYsize * 4;
		}
		CONSOLE_DEBUG_W_NUM("myBufferSize\t=", myBufferSize);
		cCameraDataBuffer	=	(unsigned char *)malloc(myBufferSize + 128);
		if (cCameraDataBuffer != NULL)
		{
			CONSOLE_DEBUG("cCameraDataBuffer allocated");
			cCameraDataBuffLen	=	myBufferSize;
			successFlag			=	true;
		}
		else
		{
			CONSOLE_DEBUG("cCameraDataBuffer FAILED");
			cCameraDataBuffLen	=	0;
			successFlag			=	false;
		}
	}
//	CONSOLE_DEBUG(__FUNCTION__);
	return(successFlag);
}



#pragma mark -
#pragma mark Video commands
//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_StartVideo(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
char				recordTimeStr[32];
bool				recTimeFound;
#ifdef _USE_OPENCV_
	int		videoIsColor;
	int		fourCC;
	char	filePath[128];
#endif // _USE_OPENCV_

	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
//		DumpRequestStructure(__FUNCTION__, reqData);

		//*	look for parameters in the request
		alpacaErrCode = ProcessExposureOptions(reqData, alpacaErrMsg);
		if(alpacaErrCode != kASCOM_Err_Success)
		{
			return alpacaErrCode;
		}

		recTimeFound	=	GetKeyWordArgument(	reqData->contentData,
												"recordtime",
												recordTimeStr,
												(sizeof(recordTimeStr) -1));
//		CONSOLE_DEBUG_W_NUM("cInternalCameraState\t=", cInternalCameraState);

		switch(cInternalCameraState)
		{
			case kCameraState_Idle:
				CONSOLE_DEBUG("kCameraState_Idle");
				cNumFramesSaved			=	0;		//*	start video
				cNumVideoFramesSaved	=	0;
				cFrameRate				=	0;

				if (recTimeFound)
				{
					cNumFramesToSave	=	0;
					cVideoDuration_secs	=	atof(recordTimeStr);
				}
				else
				{
					cNumFramesToSave	=	300;
					cVideoDuration_secs	=	0;
				}
				CONSOLE_DEBUG_W_DBL("cVideoDuration_secs\t=", cVideoDuration_secs);
				CONSOLE_DEBUG_W_NUM("cNumFramesToSave\t=", cNumFramesToSave);

				alpacaErrCode			=	Start_Video();
				if (alpacaErrCode == 0)
				{
			#ifdef _USE_OPENCV_
					videoIsColor		=	1;
					GenerateFileNameRoot();
					strcpy(filePath, kImageDataDir);
					strcat(filePath, "/");
					strcat(filePath, cFileNameRoot);
					strcat(filePath, ".avi");

					//	http://www.fourcc.org/codecs.php
					switch(cROIinfo.currentROIimageType)
					{
						case kImageType_RGB24:
						//	CV_FOURCC_DEFAULT,
						//	CV_FOURCC('M', 'J', 'L', 'S'),
						//	CV_FOURCC('M', 'J', 'P', 'G'),		//*	MJPG -> motion jpeg
						//	CV_FOURCC('P', 'I', 'M', '1'),		//*	MPEG-1
						//
						//	-1,									//*	user selectable dialog box
						//	fourCC	=	CV_FOURCC('R', 'G', 'B', '2');
							fourCC	=	CV_FOURCC('M', 'P', '4', '2');		//*	MP42 -> MPEG-4  WORKS!!
							videoIsColor		=	1;
							break;

						default:
							fourCC	=	CV_FOURCC('Y', '8', '0', '0');		//*	writes, but cant be read
							videoIsColor		=	0;
							break;
					}
				//	fourCC	=	-1;
					CONSOLE_DEBUG_W_HEX("fourCC\t=", fourCC);
					cAVIfourcc			=	fourCC;
					cOpenCV_videoWriter	=	cvCreateVideoWriter(	filePath,
																	fourCC,
																	30.0,
																	cvSize(cCameraXsize, cCameraYsize),
																	videoIsColor);
					if (cOpenCV_videoWriter == NULL)
					{
						CONSOLE_DEBUG("Failed to create video writer");
						cInternalCameraState	=	kCameraState_Idle;
						alpacaErrCode			=	kASCOM_Err_FailedToTakePicture;
						GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Failed to create video writer (openCv)");
						exit(0);

					}
					//=============================================
					if (cVideoCreateTimeStampFile)
					{
						GenerateFileNameRoot();
						strcpy(filePath, kImageDataDir);
						strcat(filePath, "/");
						strcat(filePath, cFileNameRoot);
						strcat(filePath, ".csv");

						cVideoTimeStampFilePtr	=	fopen(filePath, "w");
						if (cVideoTimeStampFilePtr != NULL)
						{
						//	fprintf(cVideoTimeStampFilePtr, "#Time Stamp File:%s\r\n", filePath);
						//	fprintf(cVideoTimeStampFilePtr, "#------------------------------------\r\n");
							fprintf(cVideoTimeStampFilePtr, "#FrameNum,TimeStamp\r\n");
						}
					}
			#endif // _USE_OPENCV_
				}
				break;

			case kCameraState_TakingPicture:
				alpacaErrCode	=	kASCOM_Err_CameraBusy;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Single frame exposure in progress");
				CONSOLE_DEBUG(alpacaErrMsg);
				break;

			case kCameraState_StartVideo:
				CONSOLE_DEBUG("kCameraState_StartVideo");
				break;

			case kCameraState_TakingVideo:
				CONSOLE_DEBUG("kCameraState_TakingVideo");
				alpacaErrCode	=	kASCOM_Err_CameraBusy;
				GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "Video exposure in progress");
				break;

			default:
				break;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_StopVideo(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		CONSOLE_DEBUG_W_NUM("cNumFramesToSave\t=", cNumFramesToSave);
		cNumFramesToSave	=	1;
		CONSOLE_DEBUG_W_NUM("cNumFramesToSave\t=", cNumFramesToSave);
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
		CONSOLE_DEBUG("kASCOM_Err_InternalError");
		exit(0);
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Set_ExposureTime(int32_t exposureMicrosecs)
{
	cCurrentExposure_us	=	exposureMicrosecs;

	return(kASCOM_Err_Success);
}


//*****************************************************************************
TYPE_ASCOM_STATUS		CameraDriver::Start_CameraExposure(int32_t exposureMicrosecs)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "Needs to be overloaded:-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);

	CONSOLE_DEBUG(cLastCameraErrMsg);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS		CameraDriver::Stop_Exposure(void)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "Not finished-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	CONSOLE_DEBUG(cLastCameraErrMsg);

	return(alpacaErrCode);
}



//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Start_Video(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "Not finished-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	CONSOLE_DEBUG(cLastCameraErrMsg);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Stop_Video(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "Not finished-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	CONSOLE_DEBUG(cLastCameraErrMsg);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Take_Video(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "Not finished-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	CONSOLE_DEBUG(cLastCameraErrMsg);

	return(alpacaErrCode);
}





#pragma mark -


//*****************************************************************************
void	CameraDriver::OutputHTML(TYPE_GetPutRequestData *reqData)
{
char				lineBuffer[256];
double				megaPixels;
int					mySocketFD;

	mySocketFD	=	reqData->socket;

	SocketWriteData(mySocketFD,	"<CENTER>\r\n");
	SocketWriteData(mySocketFD,	"<H2>Camera</H2>\r\n");
	SocketWriteData(mySocketFD,	"<TABLE BORDER=1>\r\n");


	//===============================================================
	SocketWriteData(mySocketFD,	"<TR>\r\n");
//	SocketWriteData(mySocketFD,	"\t<TD>Camera</TD>");
	SocketWriteData(mySocketFD,	"\t<TD>");
	SocketWriteData(mySocketFD,	cDeviceName);
	SocketWriteData(mySocketFD,	"</TD></TR>\r\n");


	//===============================================================
	if (strlen(cDriverversionStr) > 0)
	{
		SocketWriteData(mySocketFD,	"<TR>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD></TD>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD>Library Version</TD>");
		SocketWriteData(mySocketFD,	"\t<TD>");
		SocketWriteData(mySocketFD,	cDriverversionStr);
		SocketWriteData(mySocketFD,	"</TD></TR>\r\n");
	}


	//===============================================================
	if (strlen(cSensorName) > 0)
	{
		SocketWriteData(mySocketFD,	"<TR>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD></TD>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD>Sensor</TD>");
		SocketWriteData(mySocketFD,	"\t<TD>");
		SocketWriteData(mySocketFD,	cSensorName);
		SocketWriteData(mySocketFD,	"</TD></TR>\r\n");
	}

	//===============================================================
	if (strlen(cDeviceManufacturer) > 0)
	{
		SocketWriteData(mySocketFD,	"<TR>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD></TD>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD>Manufacturer</TD>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD>");
		SocketWriteData(mySocketFD,	cDeviceManufacturer);
		SocketWriteData(mySocketFD,	"</TD></TR>\r\n");
	}

	//===============================================================
	if (strlen(cDeviceDescription) > 0)
	{
		SocketWriteData(mySocketFD,	"<TR>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD></TD>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD>Description</TD>\r\n");
		SocketWriteData(mySocketFD,	"\t<TD>");
		SocketWriteData(mySocketFD,	cDeviceDescription);
		SocketWriteData(mySocketFD,	"</TD></TR>\r\n");
	}
	//===============================================================
	SocketWriteData(mySocketFD,	"<TR>\r\n");
	SocketWriteData(mySocketFD,	"\t<TD></TD>\r\n");
	SocketWriteData(mySocketFD,	"\t<TD>Image size</TD>\r\n");


	megaPixels	=	(1.0 * cCameraXsize * cCameraYsize) / (1000.0 * 1000.0);

	sprintf(lineBuffer,	"\t<TD>%d x %d (%2.1f megapixels)</TD>\r\n",	cCameraXsize,
																		cCameraYsize,
																		megaPixels);
	SocketWriteData(mySocketFD,	lineBuffer);
	SocketWriteData(mySocketFD,	"</TR>\r\n");

	//===============================================================
	SocketWriteData(mySocketFD,	"<TR>\r\n");
	SocketWriteData(mySocketFD,	"\t<TD></TD>\r\n");
	SocketWriteData(mySocketFD,	"\t<TD>Pixel size</TD>\r\n");
	sprintf(lineBuffer,	"\t<TD>%3.2f x %3.2f</TD>\r\n",	cPixelSizeX, cPixelSizeX);
	SocketWriteData(mySocketFD,	lineBuffer);
	SocketWriteData(mySocketFD,	"</TR>\r\n");

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>Bit depth</TD><TD>%d</TD></TR>\r\n",
											cBitDepth);
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>IsColorCam</TD><TD>%s</TD></TR>\r\n",
										(cIsColorCam == true)		? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>MechanicalShutter</TD><TD>%s</TD></TR>\r\n",
										(cHasShutter == 1) ? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>ST4Port</TD><TD>%s</TD></TR>\r\n",
											(cSt4Port == 1)		? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>IsCoolerCam</TD><TD>%s</TD></TR>\r\n",
											(cIsCoolerCam == 1)	? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>IsUSB3Host</TD><TD>%s</TD></TR>\r\n",
											(cIsUSB3Host == 1)		? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>IsUSB3Camera</TD><TD>%s</TD></TR>\r\n",
											(cIsUSB3Camera == 1)	? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);

	//===============================================================
	if (cElectronsPerADU > 0.0)
	{
		sprintf(lineBuffer,	"\t<TR><TD></TD><TD>ElecPerADU</TD><TD>%f</TD></TR>\r\n",
											cElectronsPerADU);
		SocketWriteData(mySocketFD,	lineBuffer);
	}

	//===============================================================
	sprintf(lineBuffer,	"\t<TR><TD></TD><TD>IsTriggerCam</TD><TD>%s</TD></TR>\r\n",
											(cIsTriggerCam == 1) ? "Yes" : "No");
	SocketWriteData(mySocketFD,	lineBuffer);


	SocketWriteData(mySocketFD,	"</TABLE>\r\n");
	SocketWriteData(mySocketFD,	"</CENTER>\r\n");
	SocketWriteData(mySocketFD,	"<P>\r\n");


	//===============================================================
	SocketWriteData(reqData->socket,	"<CENTER>\r\n");
	SocketWriteData(reqData->socket,	"<TABLE BORDER=1>\r\n");

	//*	frames read
	sprintf(lineBuffer,	"\t<TR><TD>Total frames taken</TD><TD>%ld</TD></TR>\r\n",
										cFramesRead);
	SocketWriteData(reqData->socket,	lineBuffer);

	if (cImageMode == kImageMode_Live)
	{
		//*	frame rate
		sprintf(lineBuffer,	"\t<TR><TD>Frame rate</TD><TD>%1.1f</TD></TR>\r\n",
											cFrameRate);
		SocketWriteData(reqData->socket,	lineBuffer);

		//*	last exposure time
		sprintf(lineBuffer,	"\t<TR><TD>last exposure time</TD><TD>%d (u-secs)</TD></TR>\r\n",
											cCurrentExposure_us);
		SocketWriteData(reqData->socket,	lineBuffer);
	}

	//*	failure count
	sprintf(lineBuffer,	"\t<TR><TD>Frame Read Failure count</TD><TD>%d</TD></TR>\r\n",
										cExposureFailureCnt);
	SocketWriteData(reqData->socket,	lineBuffer);


	SocketWriteData(reqData->socket,	"</TABLE>\r\n");
	SocketWriteData(reqData->socket,	"</CENTER>\r\n");
	SocketWriteData(reqData->socket,	"<P>\r\n");

}

//*****************************************************************************
//*	this is expected to get overloaded
void	CameraDriver::OutputHTML_Part2(TYPE_GetPutRequestData *reqData)
{
char	lineBuffer[512];



	//===============================================================
	//*	display the most recent jpeg image
	if (strlen(cLastJpegImageName) > 0)
	{
		SocketWriteData(reqData->socket,	"<CENTER>\r\n");
		sprintf(lineBuffer,	"\t<img src=../%s width=75%%>\r\n",	cLastJpegImageName);
		SocketWriteData(reqData->socket,	lineBuffer);
		SocketWriteData(reqData->socket,	"</CENTER>\r\n");
	}
	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG_W_NUM("cCameraID\t=", cCameraID);
	GenerateHTMLcmdLinkTable(reqData->socket, "camera", cDeviceNum, gCameraCmdTable);
}


#pragma mark -
#pragma mark Virtual functions
//*****************************************************************************
int	CameraDriver::GetImage_ROI_info(void)
{
	memset(&cROIinfo, 0, sizeof(TYPE_IMAGE_ROI_Info));

	cROIinfo.currentROIimageType	=	kImageType_RAW8;
	cROIinfo.currentROIwidth		=	cCameraXsize;
	cROIinfo.currentROIheight		=	cCameraYsize;
	cROIinfo.currentROIbin			=	1;
	return(-1);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::SetImageTypeCameraOpen(TYPE_IMAGE_TYPE newImageType)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Write_Gain(const int newGainValue)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_Gain(int *cameraGainValue)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}

//*****************************************************************************
static TYPE_IMAGE_TYPE	GetInternalImageType(const char *imageTypeString)
{
TYPE_IMAGE_TYPE	myImageType;

//	CONSOLE_DEBUG_W_STR("imageTypeString:", imageTypeString);

	if (strcasecmp(imageTypeString, "RAW8") == 0)
	{
		myImageType	=	kImageType_RAW8;
	}
	else if (strcasecmp(imageTypeString, "RAW16") == 0)
	{
		myImageType	=	kImageType_RAW16;
	}
	else if (strcasecmp(imageTypeString, "RGB24") == 0)
	{
		myImageType	=	kImageType_RGB24;
	}
	else if (strcasecmp(imageTypeString, "Y8") == 0)
	{
		myImageType	=	kImageType_Y8;
	}
	else
	{
		myImageType	=	kImageType_Invalid;
		CONSOLE_DEBUG_W_STR("Unsupported image type:", imageTypeString);
	}
	CONSOLE_DEBUG_W_NUM("myImageType:", myImageType);

	return(myImageType);
}


//*****************************************************************************
//*	this routine takes an index and image type string and sets it up in the array
//*	The array has the corresponding internal enum for that image type
//*
//*	Each sub class must call this for each supported image type so that the alpaca
//*	image type commands work
//*****************************************************************************
void	CameraDriver::SetImageTypeIndex(const int alpacaImgTypeIdx, const char *imageTypeString)
{
//	CONSOLE_DEBUG(__FUNCTION__);

	if ((alpacaImgTypeIdx >=0) && (alpacaImgTypeIdx < kMaxImageTypes))
	{
		strcpy(cSupportedImageTypes[alpacaImgTypeIdx].imageTypeString, imageTypeString);
		cSupportedImageTypes[alpacaImgTypeIdx].internalImgageType	=	GetInternalImageType(imageTypeString);
		cSupportedImageTypes[alpacaImgTypeIdx].isValid				=	true;

		CONSOLE_DEBUG_W_NUM("alpacaImgTypeIdx\t=",	alpacaImgTypeIdx);
		CONSOLE_DEBUG_W_NUM("internal\t=",			cSupportedImageTypes[alpacaImgTypeIdx].internalImgageType);
		CONSOLE_DEBUG_W_STR("name\t\t=",			cSupportedImageTypes[alpacaImgTypeIdx].imageTypeString);
	}
	else
	{
		CONSOLE_DEBUG_W_NUM("alpacaImgTypeIdx is out of bounds\t=",	alpacaImgTypeIdx);
		exit(0);
	}
}

//*****************************************************************************
//*	Translate Alpaca image index to internal image type index (TYPE_IMAGE_TYPE)
//*****************************************************************************
TYPE_IMAGE_TYPE	CameraDriver::XlateAlpacaImgIdxToIntImgType(const int alpacaImgTypeIdx)
{
TYPE_IMAGE_TYPE	internalImgType;

	internalImgType	=	kImageType_Invalid;
	if ((alpacaImgTypeIdx >=0) && (alpacaImgTypeIdx < kMaxImageTypes))
	{
		if (cSupportedImageTypes[alpacaImgTypeIdx].isValid)
		{
			internalImgType	=	cSupportedImageTypes[alpacaImgTypeIdx].internalImgageType;
		}
	}
	return(internalImgType);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::SetImageType(TYPE_IMAGE_TYPE newImageType)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::SetImageType(char *newImageTypeString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
TYPE_IMAGE_TYPE		myImageType;

//	CONSOLE_DEBUG(__FUNCTION__);

	myImageType	=	GetInternalImageType(newImageTypeString);
	CONSOLE_DEBUG_W_STR("newImageTypeString\t=",	newImageTypeString);
	CONSOLE_DEBUG_W_NUM("myImageType\t\t=",			myImageType);
	if (myImageType >= 0)
	{
		alpacaErrCode	=	SetImageType(myImageType);
	}
	else
	{
		strcpy(cLastCameraErrMsg, "Unsupported image type-");
		strcat(cLastCameraErrMsg, newImageTypeString);
		CONSOLE_DEBUG_W_STR("cLastCameraErrMsg\t=", cLastCameraErrMsg);
		alpacaErrCode	=	kASCOM_Err_InvalidValue;
	}
	if (alpacaErrCode != 0)
	{
		CONSOLE_DEBUG_W_NUM("alpacaErrCode\t=", alpacaErrCode);
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Cooler_TurnOn(void)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Cooler_TurnOff(void)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}


//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_CoolerPowerLevel(void)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}

//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_CoolerState(bool *coolerOnOff)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	*coolerOnOff	=	false;
	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}


//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_Fastreadout(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_Readoutmodes(char *readOutModeString, bool includeQuotes)

{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					ii;

	readOutModeString[0]	=	0;
	ii						=	0;
	while ((ii<kMaxImageTypes) && (cSupportedImageTypes[ii].isValid))
	{
		strcat(readOutModeString, cSupportedImageTypes[ii].imageTypeString);
		if (cSupportedImageTypes[ii+1].isValid)
		{
			strcat(readOutModeString, ", ");
		}
		ii++;
	}
	return(alpacaErrCode);
}

//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_SensorTemp(void)
{
TYPE_ASCOM_STATUS		alpacaErrCode	=	kASCOM_Err_NotImplemented;

//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}

#pragma mark -
#pragma mark Image data commands
//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Read_ImageData(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	strcpy(cLastCameraErrMsg, "AlpacaPi: Not implemented-");
	strcat(cLastCameraErrMsg, __FILE__);
	strcat(cLastCameraErrMsg, ":");
	strcat(cLastCameraErrMsg, __FUNCTION__);
	return(alpacaErrCode);
}


#pragma mark -

//*****************************************************************************
void	CameraDriver::CheckPulseGuiding(void)
{
struct timeval	currentTime;
uint32_t		deltaSecs;

//	CONSOLE_DEBUG(__FUNCTION__);
	//*	are we pulse guiding, if we are, turn it off after 1 second
	if (cIsPulseGuiding)
	{

		gettimeofday(&currentTime, NULL);
		deltaSecs	=	currentTime.tv_sec - cPulseGuideStartTime.tv_sec;

		if (deltaSecs > 1)
		{
			cIsPulseGuiding	=	false;
		}
	}
}


//*****************************************************************************
int32_t	CameraDriver::RunStateMachine_Idle(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode;
int32_t				delayMicroSecs;
bool				startNextFrame;
struct timeval		currentTime;
uint32_t			currentMilliSecs;
uint32_t			startofLastExp;
uint32_t			elapsedMilliSecs;

	if (cUpdateOtherDevices)
	{
	#ifdef _ENABLE_FITS_

		//*	check on other devices
		#ifdef _ENABLE_FOCUSER_
			UpdateFocuserLink();
		#endif
		#ifdef _ENABLE_ROTATOR_
			UpdateRotatorLink();
		#endif
		#ifdef _ENABLE_FILTERWHEEL_
			UpdateFilterwheelLink();
		#endif
	#endif
		cUpdateOtherDevices	=	false;
	}
	alpacaErrCode	=	kASCOM_Err_Success;
	delayMicroSecs	=	99999999;
	switch(cImageMode)
	{
		case kImageMode_Single:
			//*	do nothing
			break;

		case kImageMode_Sequence:
			//*	prepare for the next image
			if (cNumFramesToSave > 0)
			{
				startNextFrame		=	false;

				//*	calculate time since last picture was STARTED
				gettimeofday(&currentTime, NULL);
				currentMilliSecs	=	Calc_millisFromTimeStruct(&currentTime);
				startofLastExp		=	Calc_millisFromTimeStruct(&cLastExposureStartTime);
				elapsedMilliSecs	=	currentMilliSecs - startofLastExp;

				//*	is it time to start the next image?
				if (elapsedMilliSecs > (cSequenceDelay_us / 1000))
				{
					startNextFrame	=	true;
				}

				if (startNextFrame)
				{
					CONSOLE_DEBUG_W_NUM("Starting next image in sequence", cNumFramesToSave);
					//*	start next image
					cCurrentExposure_us	+=	cSeqDeltaExposure_us;
					cSaveNextImage		=	true;
					alpacaErrCode		=	Start_CameraExposure(cCurrentExposure_us);
					GenerateFileNameRoot();
					cImageSeqNumber++;
					cNumFramesToSave--;
				}
			}
			else
			{
				//*	we are done, revert back to single frame mode
				cImageMode		=	kImageMode_Single;
				cImageSeqNumber	=	0;
			}
			break;

		case kImageMode_Live:
			{
				alpacaErrCode	=	Start_CameraExposure(cCurrentExposure_us);
				GenerateFileNameRoot();
				if (alpacaErrCode != 0)
				{
					CONSOLE_DEBUG("Start_CameraExposure() failed, disabling live mode");
					cImageMode	=	kImageMode_Single;

				}
				delayMicroSecs	=	50000;
			}
			break;

		case kImageMode_Last:
			//*	we should never get here
			cImageMode	=	kImageMode_Single;
			break;
	}
	//*	4/11/2020 debugging segmentation fault
	if (alpacaErrCode != kASCOM_Err_Success)
	{
		CONSOLE_DEBUG_W_NUM("Exit, error code=", alpacaErrCode);
	}

	return(delayMicroSecs);
}

//*****************************************************************************
int	CameraDriver::RunStateMachine_TakingPicture(void)
{
int					exposureState;
TYPE_ASCOM_STATUS	alpacaErrCode;

	exposureState	=	Check_Exposure(true);
//	CONSOLE_DEBUG_W_NUM("Taking picture: exposureState=", exposureState);
	switch(exposureState)
	{
		case kExposure_Idle:
			CONSOLE_DEBUG("Reseting to idle");
			cInternalCameraState	=	kCameraState_Idle;
			cWorkingLoopCnt			=	0;
			break;

		case kExposure_Working:
			cWorkingLoopCnt++;
			if (cWorkingLoopCnt > 70000)
			{
				Check_Exposure(true);
			//	CONSOLE_DEBUG_W_STR("Aborting.... Reseting to idle-", cDeviceManufAbrev);
			//	cInternalCameraState	=	kCameraState_Idle;
				cWorkingLoopCnt			=	0;
			}
			else
			{
			//	usleep(40);
			}
			break;

		case kExposure_Success:
			cFramesRead++;
			if (gVerbose)
			{
				CONSOLE_DEBUG_W_LONG("Done Taking picture, frame#", cFramesRead);
			}

			cWorkingLoopCnt		=	0;
			//*	Extract Image
			alpacaErrCode	=	Read_ImageData();
			if (alpacaErrCode != kASCOM_Err_Success)
			{
				CONSOLE_DEBUG_W_NUM("Read_ImageData returned error", alpacaErrCode);
			}
			//*	record the time the exposure ended
			gettimeofday(&cLastExposureEndTime, NULL);
			cNewImageReadyToDisplay		=	true;
			cNewImageReadyToDownload	=	true;

			if (cImageMode == kImageMode_Live)
			{
			double	secondsOfExposure;

				secondsOfExposure	=	millis() / 1000;
				if (secondsOfExposure < 1.0)
				{
					secondsOfExposure	=	1.0;
				}
				cFrameRate	=	(cFramesRead * 1.0) / secondsOfExposure;
			}
		#ifdef _USE_OPENCV_
			CreateOpenCVImage(cCameraDataBuffer);
//			CONSOLE_DEBUG("Done with jpg and png");
		#endif

			if (cSaveNextImage || cSaveImages)
			{
				SaveImageData();
			}
			else
			{
//				CONSOLE_DEBUG("Image not saved");
			}

			//*	check to see if we are in auto exposure adjustment
			if (cAutoAdjustExposure)
			{
				AutoAdjustExposure();
			}

			cInternalCameraState	=	kCameraState_Idle;
			break;

		case kExposure_Failed:
			cExposureFailureCnt++;
			strcpy(cLastCameraErrMsg, "Failed Taking picture");
			LogEvent(	"camera",
						__FUNCTION__,
						NULL,
						kASCOM_Err_FailedToTakePicture,
						cLastCameraErrMsg);
			cInternalCameraState	=	kCameraState_Idle;
			CONSOLE_DEBUG(cLastCameraErrMsg);
			ResetCamera();
			break;
	}
	return(exposureState);
}


//*****************************************************************************
int32_t	CameraDriver::RunStateMachine(void)
{
int32_t		delayMicroSecs;

//	if (cInternalCameraState != kCameraState_Idle)
//	{
//		CONSOLE_DEBUG_W_NUM("cInternalCameraState\t=",	cInternalCameraState);
//	}
	delayMicroSecs	=	99999999;

	switch(cInternalCameraState)
	{
		case kCameraState_Idle:
			delayMicroSecs	=	RunStateMachine_Idle();
			break;

		case kCameraState_TakingPicture:
			RunStateMachine_TakingPicture();
			delayMicroSecs	=	25000;
			break;

		case kCameraState_StartVideo:
//			CONSOLE_DEBUG("kCameraState_StartVideo");
//			CameraThread_StartVideo(theCamera);
			break;

		case kCameraState_TakingVideo:
			Take_Video();
			delayMicroSecs	=	100;
			break;

		default:
			//*	we should never get here
			break;

	}
#ifdef _USE_OPENCV_
//	if (delayMicroSecs > 500)
	{
		if (cOpenCV_Image != NULL)
		{
			if ((cImageMode == kImageMode_Live) || cDisplayImage)
			{
				if (cDisplaySideBar)
				{
					DisplayLiveImage_wSideBar();
				}
				else
				{
					DisplayLiveImage();
				}
			}
		}
		else if ((cImageMode == kImageMode_Live) || cDisplayImage)
		{
//			CONSOLE_DEBUG("cOpenCV_Image is NULL")
		}
	}
#endif // _USE_OPENCV_
	CheckPulseGuiding();
	RunStateMachine_Device();
	return(delayMicroSecs);
}


//*****************************************************************************
void	CameraDriver::RunStateMachine_Device(void)
{
	//*	this is for sub class use

}

#pragma mark -



//*****************************************************************************
void	FormatTimeStringFileName(struct timeval *tv, char *timeString)
{
struct tm	*linuxTime;
long		milliSecs;

	if ((tv != NULL) && (timeString != NULL))
	{
		linuxTime		=	gmtime(&tv->tv_sec);
		milliSecs		=	tv->tv_usec / 1000;

		sprintf(timeString, "%d-%02d-%02dT%02d_%02d_%02d.%03ld",
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
//*	this is to be called anytime
//*****************************************************************************
void	CameraDriver::GenerateFileNameRoot(void)
{
char		fileNameDateString[64];

//	CONSOLE_DEBUG(__FUNCTION__);

	FormatTimeStringFileName(&cLastExposureStartTime, fileNameDateString);
//	CONSOLE_DEBUG_W_STR("fileNameDateString\t=", fileNameDateString);

	cFileNameRoot[0]	=	0;
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);
	if (strlen(cFileNamePrefix) > 0)
	{
		strcat(cFileNameRoot, cFileNamePrefix);
		strcat(cFileNameRoot, "-");
	}
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);
	strcat(cFileNameRoot, fileNameDateString);
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);

	if (cFN_includeManuf)
	{
		strcat(cFileNameRoot, "-");
		strcat(cFileNameRoot, cDeviceManufAbrev);
	}
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);

	//*	are we supposed to include the serial number int the file name
	if (cFN_includeSerialNum)
	{
		if (strlen(cDeviceSerialNum) > 0)
		{
			strcat(cFileNameRoot, "-");
			strcat(cFileNameRoot, cDeviceSerialNum);
		}
	}
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);

	if (strlen(cFileNameSuffix) > 0)
	{
		strcat(cFileNameRoot, "-");
		strcat(cFileNameRoot, cFileNameSuffix);
	}
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);

	if (cFN_includeRefID && (strlen(cTS_info.refID) > 0))
	{
		strcat(cFileNameRoot, "-");
		strcat(cFileNameRoot, cTS_info.refID);
	}
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);

#ifdef _ENABLE_FILTERWHEEL_
	//*	are we suppose to include the filter name
	if (cFN_includeFilter)
	{
	char	fileName1stChar[4];
	bool	addFilterName;

//		CONSOLE_DEBUG("cFN_includeFilter");

		if (cConnectedFilterWheel != NULL)
		{
			cFilterWheelCurrPos			=	cConnectedFilterWheel->cFilterWheelCurrPos;
			strcpy(cFilterWheelCurrName,	cConnectedFilterWheel->cFilterWheelCurrName);
		}

		if ((cFilterWheelCurrPos >= 0) && (strlen(cFilterWheelCurrName) > 0))
		{
//			CONSOLE_DEBUG_W_NUM("cFilterWheelCurrPos\t=", cFilterWheelCurrPos);
			addFilterName	=	true;
			if (strncasecmp(cFilterWheelCurrName, "None", 4) == 0)
			{
				addFilterName	=	false;
			}
			if (strncasecmp(cFilterWheelCurrName, "Empty", 5) == 0)
			{
				addFilterName	=	false;
			}
			if (addFilterName)
			{
				fileName1stChar[0]	=	cFilterWheelCurrName[0];
				fileName1stChar[1]	=	0;
				strcat(cFileNameRoot, "-");
				strcat(cFileNameRoot, fileName1stChar);
			}
		}
	}
//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);
#endif // _ENABLE_FILTERWHEEL_

//	CONSOLE_DEBUG_W_STR("cFileNameRoot\t=", cFileNameRoot);
}



#pragma mark -


#define	_SORT_FILENAMES_
#ifdef _SORT_FILENAMES_

#define	kMaxFileCnt	200
//*****************************************************************************
typedef struct
{
	char	filename[128];
} TYPE_FILE_ENTRY;

//*****************************************************************************
int	DirSort(const void *e1, const void* e2)
{
int	retValue;
TYPE_FILE_ENTRY	*file1;
TYPE_FILE_ENTRY	*file2;

	file1		=	(TYPE_FILE_ENTRY *)e1;
	file2		=	(TYPE_FILE_ENTRY *)e2;
	retValue	=	strcmp(file1->filename, file2->filename);
	return(retValue);
}
#endif // _SORT_FILENAMES_

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Filelist(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode;
DIR					*directory;
struct dirent		*dir;
int					errorCode;
bool				keepGoing;
bool				firstLine;
int					mySocketFD;
char				lineBuff[512];
#ifdef _SORT_FILENAMES_
TYPE_FILE_ENTRY		files[kMaxFileCnt];
int					fileIdx	=	0;
int					ii;
#endif // _SORT_FILENAMES_

	CONSOLE_DEBUG_W_STR(__FUNCTION__, kImageDataDir);

	mySocketFD	=	reqData->socket;

	directory	=	opendir(kImageDataDir);
	if (directory != NULL)
	{
		JsonResponse_Add_String(mySocketFD,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"Directory",
								kImageDataDir,
								INCLUDE_COMMA);

		JsonResponse_Add_ArrayStart(	mySocketFD,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										gValueString);
		alpacaErrCode	=	kASCOM_Err_Success;
		keepGoing		=	true;
		firstLine		=	true;
		while (keepGoing)
		{
			dir	=	readdir(directory);
			if (dir != NULL)
			{
			//	if ((strcmp(dir->d_name, ".") == 0) || (strcmp(dir->d_name, "..") == 0))
				if (dir->d_name[0] == '.')
				{
					//*	ignore . and ..
				}
				else if (dir->d_type == DT_DIR)
				{
					//*	ignore directories
				}
				else
				{
				#ifdef _SORT_FILENAMES_
					if (fileIdx < kMaxFileCnt)
					{
						strcpy(files[fileIdx].filename, dir->d_name);
						fileIdx++;
					}
					else
					{
						CONSOLE_DEBUG("Exceeded file list max count!!!!!!");
//+						keepGoing	=	false;
					}
				#else
				//	printf("%2d\t%s\r\n", dir->d_type, dir->d_name);
					if (firstLine)
					{
						strcpy(lineBuff, "\r\n\t\t\t\"");
						firstLine	=	false;
					}
					else
					{
						strcpy(lineBuff, "\t\t\t\"");
					}
					strcat(lineBuff, dir->d_name);
					strcat(lineBuff, "\",");
					strcat(lineBuff, "\r\n");
					JsonResponse_Add_RawText(	mySocketFD,
												reqData->jsonTextBuffer,
												kMaxJsonBuffLen,
												lineBuff);
				#endif // _SORT_FILENAMES_
				}
			}
			else
			{
				keepGoing	=	false;
			}
		}
CONSOLE_DEBUG(__FUNCTION__);
	#ifdef _SORT_FILENAMES_
CONSOLE_DEBUG("sorting");
		qsort(files, fileIdx, sizeof(TYPE_FILE_ENTRY), DirSort);
		for (ii=0; ii<fileIdx; ii++)
		{
			lineBuff[0]	=	0;
			if (firstLine)
			{
				strcpy(lineBuff, "\r\n");
				firstLine	=	false;
			}
			strcat(lineBuff, "\t\t\t\"");
			strcat(lineBuff, files[ii].filename);
			strcat(lineBuff, "\",");
			strcat(lineBuff, "\r\n");
			JsonResponse_Add_RawText(	mySocketFD,
										reqData->jsonTextBuffer,
										kMaxJsonBuffLen,
										lineBuff);
		}
		JsonResponse_Add_RawText(	mySocketFD,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"\t\t\t\"END\"\r\n");
	#endif // _SORT_FILENAMES_
CONSOLE_DEBUG(__FUNCTION__);
		JsonResponse_Add_ArrayEnd(	mySocketFD,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									INCLUDE_COMMA);
		errorCode	=	closedir(directory);
		if (errorCode != 0)
		{
			CONSOLE_DEBUG_W_NUM("closedir errorCode\t=", errorCode);
			CONSOLE_DEBUG_W_NUM("errno\t=", errno);
		}
	}
	else
	{
		CONSOLE_DEBUG_W_STR("Failed to open", kImageDataDir);
		CONSOLE_DEBUG_W_NUM("errno\t=", errno);
		LogEvent(	"camera",
					"Failure",
					NULL,
					kASCOM_Err_Success,
					"Failed to open image data directory");
	}

	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_AutoExposure(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								cAutoAdjustExposure,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"stepsize",
								cAutoAdjustStepSz_us,
								INCLUDE_COMMA);
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_AutoExposure(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
char				argumentString[32];
bool				foundKeyWord;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{

		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"autoexposure",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{

			if (strcmp(argumentString, "true") == 0)
			{
				cAutoAdjustExposure	=	true;
			}
			else
			{
				cAutoAdjustExposure	=	false;
			}
			alpacaErrCode	=	kASCOM_Err_Success;
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "argument not specified");
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_DisplayImage(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg, const char *responseString)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	if (reqData != NULL)
	{
		JsonResponse_Add_Bool(	reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								responseString,
								cDisplayImage,
								INCLUDE_COMMA);

	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_DisplayImage(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
char				argumentString[32];
bool				foundKeyWord;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"displayImage",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			if (strcmp(argumentString, "true") == 0)
			{
				cDisplayImage	=	true;
			}
			else
			{
				cDisplayImage	=	false;
			}
		}
		else
		{
			alpacaErrCode	=	kASCOM_Err_InvalidValue;
			GENERATE_ALPACAPI_ERRMSG(alpacaErrMsg, "argument not specified");
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}


//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Put_Filenameoptions(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
char				argumentString[32];
bool				foundKeyWord;

//	CONSOLE_DEBUG(__FUNCTION__);
	if (reqData != NULL)
	{
		//*	look for filter
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"includefilter",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			cFN_includeFilter	=	IsTrueFalse(argumentString);
		}

		//*	look for camera
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"includecamera",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			cFN_includeManuf	=	IsTrueFalse(argumentString);
		}

		//*	look for serial number
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"includeserialnum",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			cFN_includeSerialNum	=	IsTrueFalse(argumentString);
		}

		//*	look for serial number
		foundKeyWord	=	GetKeyWordArgument(	reqData->contentData,
												"includerefid",
												argumentString,
												(sizeof(argumentString) -1));
		if (foundKeyWord)
		{
			cFN_includeRefID	=	IsTrueFalse(argumentString);
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InternalError;
	}
	return(alpacaErrCode);
}



//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriver::Get_Readall(TYPE_GetPutRequestData *reqData, char *alpacaErrMsg)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
int					mySocket;
char				cameraStateString[32];
char				imageModeString[32];
int					exposureState;
char				exposureStateString[32];
char				textBuffer[128];

	alpacaErrCode	=	Read_SensorTemp();

	switch(cInternalCameraState)
	{
		case kCameraState_Idle:				strcpy(cameraStateString,	"Idle");			break;
		case kCameraState_TakingPicture:	strcpy(cameraStateString,	"TakingPicture");	break;
		case kCameraState_StartVideo:		strcpy(cameraStateString,	"StartVideo");		break;
		case kCameraState_TakingVideo:		strcpy(cameraStateString,	"TakingVideo");		break;
		default:							strcpy(cameraStateString,	"UNKNOWN");			break;
	}


	switch(cImageMode)
	{
		case kImageMode_Single:		strcpy(imageModeString,	"Single");		break;
		case kImageMode_Sequence:	strcpy(imageModeString,	"Sequence");	break;
		case kImageMode_Live:		strcpy(imageModeString,	"Live");		break;
		default:					strcpy(imageModeString,	"UNKNOWN");		break;
	}


	exposureState	=	Check_Exposure(true);
	switch(exposureState)
	{
		case kExposure_Idle:	strcpy(exposureStateString,	"Idle");		break;
		case kExposure_Working:	strcpy(exposureStateString,	"Working");		break;
		case kExposure_Success:	strcpy(exposureStateString,	"Success");		break;
		case kExposure_Failed:	strcpy(exposureStateString,	"Failed");		break;
		default:				strcpy(exposureStateString,	"UNKNOWN");		break;
	}


	if (reqData != NULL)
	{
		//*	do the common ones first
		Get_Readall_Common(			reqData, alpacaErrMsg);

		//*	do them in alphabetical order
		Get_binX(					reqData, alpacaErrMsg, "binx");
		Get_binY(					reqData, alpacaErrMsg, "biny");
		Get_Camerastate(			reqData, alpacaErrMsg, "camerastate");
		Get_CCDtemperature(			reqData, alpacaErrMsg, "ccdtemperature");
		Get_Cooleron(				reqData, alpacaErrMsg, "cooleron");
		Get_CoolerPowerLevel(		reqData, alpacaErrMsg, "coolerpower");


		//*	make local copies of the data structure to make the code easier to read
		mySocket	=	reqData->socket;

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"cameraxsize",
								cCameraXsize,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"cameraysize",
								cCameraYsize,
								INCLUDE_COMMA);


		//*	Indicates whether the camera can abort exposures.
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"canabortexposure",
								cCanAbortExposure,
								INCLUDE_COMMA);

		//*	Indicates whether the camera supports asymmetric binning
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"canasymmetricbin",
								cCanAsymmetricBin,
								INCLUDE_COMMA);

		//*	Indicates whether the camera has a fast readout mode.
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"canfastreadout",
								cCanFastReadout,
								INCLUDE_COMMA);

		//*	Indicates whether the camera's cooler power setting can be read.
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"cangetcoolerpower",
								cCanGetCoolerPower,
								INCLUDE_COMMA);

		//*	Returns a flag indicating whether this camera supports pulse guiding
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"canpulseguide",
								cCanPulseGuide,
								INCLUDE_COMMA);

		//*	Returns a flag indicating whether this camera supports setting the CCD temperature
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"cansetccdtemperature",
								cCansetccdtemperature,
								INCLUDE_COMMA);

		//*	Returns a flag indicating whether this camera can stop an exposure that is in progress
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"canstopexposure",
								true,
								INCLUDE_COMMA);


		JsonResponse_Add_Double(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"electronsperadu",
								cElectronsPerADU,
								INCLUDE_COMMA);

		Get_Exposuremax(		reqData,	alpacaErrMsg,	"exposuremax");
		Get_Exposuremin(		reqData,	alpacaErrMsg,	"exposuremin");
		Get_Fastreadout(		reqData,	alpacaErrMsg,	"fastreadout");
		Get_Fullwellcapacity(	reqData,	alpacaErrMsg,	"fullwellcapacity");
		Get_Gain(				reqData,	alpacaErrMsg,	"gain");

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"gainmax",
								cGainMax,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"gainmin",
								cGainMin,
								INCLUDE_COMMA);

		Get_Lastexposureduration(	reqData, 	alpacaErrMsg,	"lastexposureduration");
		Get_Lastexposurestarttime(	reqData,	alpacaErrMsg,	"lastexposurestarttime");
		Get_numX(					reqData,	alpacaErrMsg,	"numx");
		Get_numY(					reqData,	alpacaErrMsg,	"numy");

		//*	Width of CCD chip pixels (microns)
		JsonResponse_Add_Double(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"pixelsizex",
								cPixelSizeX,
								INCLUDE_COMMA);

		//*	Height of CCD chip pixels (microns)
		JsonResponse_Add_Double(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"pixelsizey",
								cPixelSizeY,
								INCLUDE_COMMA);


		Get_Readoutmode(	reqData, alpacaErrMsg, "readoutmode");
		Get_Readoutmodes(	reqData, alpacaErrMsg, "readoutmodes");

		Read_Readoutmodes(textBuffer, false);
		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"readoutmodes-str",
								textBuffer,
								INCLUDE_COMMA);

		//*	Sensor name
		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"sensorname",
								cSensorName,
								INCLUDE_COMMA);

		Get_Sensortype(	reqData,	alpacaErrMsg,	"sensortype");

		Get_startX(					reqData,	alpacaErrMsg,	"startx");
		Get_startY(					reqData,	alpacaErrMsg,	"starty");

		//===============================================================
		JsonResponse_Add_String(reqData->socket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"comment-cmds",
								"Non-standard alpaca commands follow",
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"version",
								gFullVersionString,
								INCLUDE_COMMA);


		Get_ExposureTime(	reqData, alpacaErrMsg, "exposuretime");

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"exposureState",
								exposureStateString,
								INCLUDE_COMMA);


		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"saveNextImage",
								cSaveNextImage,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"saveImages",
								cSaveImages,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"autoexposure",
								cAutoAdjustExposure,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"stepsize",
								cAutoAdjustStepSz_us,
								INCLUDE_COMMA);

		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"frames-read",
								cFramesRead,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"focuserInfoValid",
								cFocuserInfoValid,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"rotatorInfoValid",
								cRotatorInfoValid,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filterWheelInfoValid",
								cFilterWheelInfoValid,
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"fileNamePrefix",
								cFileNamePrefix,
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"fileNameSuffix",
								cFileNameSuffix,
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filenameroot",
								cFileNameRoot,
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"object",
								cObjectName,
								INCLUDE_COMMA);

		//===============================================================
		Get_LiveMode(		reqData,	alpacaErrMsg,	"livemode");
#ifdef _USE_OPENCV_
		Get_Sidebar(		reqData,	alpacaErrMsg,	"sidebar");
#endif // _USE_OPENCV_

		Get_DisplayImage(	reqData,	alpacaErrMsg,	"displayImage");

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filenameincludefilter",
								cFN_includeFilter,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filenameincludecamera",
								cFN_includeManuf,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filenameincludeserialnum",
								cFN_includeSerialNum,
								INCLUDE_COMMA);

		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"filenameincluderefid",
								cFN_includeRefID,
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"refid",
								cTS_info.refID,
								INCLUDE_COMMA);
		if (strlen(cAuxTextTag) > 0)
		{
			JsonResponse_Add_String(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"auxtext",
									cAuxTextTag,
									INCLUDE_COMMA);
		}
		JsonResponse_Add_Int32(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"videoframes",
								cNumVideoFramesSaved,
								INCLUDE_COMMA);

		//*	figure out how much time is remaining on the video
 		if (cVideoDuration_secs > 0)
		{
		int	deltaSecs;
		int	timeRemaining;

			deltaSecs		=	cLastExposureEndTime.tv_sec - cLastExposureStartTime.tv_sec;
			if ((cVideoDuration_secs > deltaSecs) && (deltaSecs > 0))
			{
				timeRemaining	=	cVideoDuration_secs - deltaSecs;
			}
			else
			{
				timeRemaining	=	0;
			}
			if (timeRemaining > (60 * 60 * 2))
			{
				CONSOLE_DEBUG_W_DBL("cVideoDuration_secs\t\t=",				cVideoDuration_secs);
				CONSOLE_DEBUG_W_NUM("deltaSecs\t\t\t\t=",					deltaSecs);
				CONSOLE_DEBUG_W_LONG("cLastExposureEndTime.tv_sec\t=",		cLastExposureEndTime.tv_sec);
				CONSOLE_DEBUG_W_LONG("cLastExposureStartTime.tv_sec\t=",	cLastExposureStartTime.tv_sec);

			//	CONSOLE_DEBUG("ABORT!!!!!!!");
			//	exit(0);
			}
			JsonResponse_Add_Int32(	mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"remainingseconds",
									timeRemaining,
									INCLUDE_COMMA);
		}

		//===============================================================
		//*	all of the debugging stuff last
		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"image-mode",
								imageModeString,
								INCLUDE_COMMA);

		JsonResponse_Add_String(mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"internalCameraState",
								cameraStateString,
								INCLUDE_COMMA);

		//*	write errors to log file if true
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"errorLogging",
								gErrorLogging,
								INCLUDE_COMMA);

		//*	log all commands to log file to match up with Conform
		JsonResponse_Add_Bool(	mySocket,
								reqData->jsonTextBuffer,
								kMaxJsonBuffLen,
								"conformLogging",
								gConformLogging,
								INCLUDE_COMMA);

		//*	color information
	#ifdef _USE_OPENCV_
	uint16_t	myRed;
	uint16_t	myGrn;
	uint16_t	myBlu;
		myRed	=	cSideBarBGcolor.val[2];
		myGrn	=	cSideBarBGcolor.val[1];
		myBlu	=	cSideBarBGcolor.val[0];

		myRed	=	myRed & 0x00ff;
		myGrn	=	myGrn & 0x00ff;
		myBlu	=	myBlu & 0x00ff;
		if ((myRed != 0) || (myGrn != 0) || (myBlu != 0))
		{
			sprintf(textBuffer, "#%02X%02X%02X", myRed, myGrn, myBlu);

//			CONSOLE_DEBUG_W_STR("Background\t=", textBuffer);
			JsonResponse_Add_String(mySocket,
									reqData->jsonTextBuffer,
									kMaxJsonBuffLen,
									"backgroundcolor",
									textBuffer,
									INCLUDE_COMMA);
		}
		else
		{
			CONSOLE_DEBUG("Background color not set");
		}
	#endif // _USE_OPENCV_


		Get_Readall_CPUstats(	reqData, alpacaErrMsg);


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
bool	CameraDriver::GetCmdNameFromMyCmdTable(const int cmdNumber, char *comandName, char *getPut)
{
bool	foundIt;

	foundIt	=	GetCmdNameFromTable(cmdNumber, comandName, gCameraCmdTable, getPut);
	return(foundIt);
}

#endif	//	_ENABLE_CAMERA_
