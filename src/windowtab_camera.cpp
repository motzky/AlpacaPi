//*****************************************************************************
//*		windowtab_camera.cpp		(c) 2020 by Mark Sproul
//*
//*	Description:	C++ Client to talk to Alpaca devices
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
//*	Mar  1,	2020	<MLS> Started on windowtab_camera.cpp
//*	Mar  6,	2020	<MLS> Added alpaca camera state to window
//*	Mar  6,	2020	<MLS> Added filter wheel display
//*	Mar  9,	2020	<MLS> Added SetTempartueEnable()
//*	Mar  9,	2020	<MLS> Added "R" indicator in IP field if "readall" is available
//*	Mar 16,	2020	<MLS> Added error message box to camera display
//*	Apr  5,	2020	<MLS> Added ToggleDisplayImage()
//*	Dec 26,	2020	<MLS> Started on image download for camera controller
//*	Dec 27,	2020	<MLS> Added DownloadImage()
//*****************************************************************************

#ifdef _ENABLE_CTRL_CAMERA_


#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"

#include	"controller.h"
#include	"controller_camera.h"
#include	"windowtab.h"
#include	"windowtab_camera.h"
#include	"controller_image.h"


#define	kAboutBoxHeight	100

//**************************************************************************************
WindowTabCamera::WindowTabCamera(	const int	xSize,
									const int	ySize,
									CvScalar	backGrndColor,
									const char	*windowName,
									const char	*deviceName,
									const bool	hasFilterWheel)
	:WindowTab(xSize, ySize, backGrndColor, windowName)
{
//	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cAlpacaDeviceName, "");
 //	memset(&cAlpacaDevInfo, 0, sizeof(TYPE_REMOTE_DEV));

	cForce8BitRead	=	false;
	cHasFilterWheel	=	hasFilterWheel;
	strcpy(cAlpacaDeviceName, deviceName);

	strcpy(cDownLoadedFileNameRoot, "unknown");

	SetupWindowControls();
}

//**************************************************************************************
// Destructor
//**************************************************************************************
WindowTabCamera::~WindowTabCamera(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
}

//**************************************************************************************
void	WindowTabCamera::SetupWindowControls(void)
{
int			yLoc;
int			yLocSave;
int			yLocClm4;
int			iii;
char		textBuff[32];
IplImage	*logoImage;

//	CONSOLE_DEBUG(__FUNCTION__);


	for (iii=0; iii<kCameraBox_last; iii++)
	{
		SetWidgetTextColor(iii, CV_RGB(255,	255,	255));
	}

	//------------------------------------------
	yLoc			=	cTabVertOffset;



	//------------------------------------------
	SetWidget(		kCameraBox_Title,		0,			yLoc,		cFullWidthBtn,	cTitleHeight);
	SetWidgetFont(	kCameraBox_Title, kFont_Medium);
	yLoc			+=	cTitleHeight;
	yLoc			+=	2;

	SetBGcolorFromWindowName(kCameraBox_Title);


	//------------------------------------------
	SetWidget(			kCameraBox_Size,		0,			yLoc,	cFullWidthBtn,	cSmallBtnHt	);
	SetWidgetFont(		kCameraBox_Size, 	kFont_Medium);
	SetWidgetType(		kCameraBox_Size,	kWidgetType_RadioButton);
	SetWidgetChecked(	kCameraBox_Size, true);

	//*	the logo goes the same vertical placement as the size button
	SetWidget(		kCameraBox_Logo,	(cWidth - cLogoWidth),	yLoc,	cLogoWidth,	cLogoHeight	);
	SetWidgetType(	kCameraBox_Logo,	kWidgetType_Image);
	//*	now figure out which logo belongs
	logoImage	=	NULL;
	CONSOLE_DEBUG_W_STR("cAlpacaDeviceName=", cAlpacaDeviceName);
	if (strcasestr(cAlpacaDeviceName, "ZWO") != NULL)
	{
		logoImage		=	cvLoadImage("zwo-logo.png",		CV_LOAD_IMAGE_COLOR);
	}
	else if (strcasestr(cAlpacaDeviceName, "Atik") != NULL)
	{
		logoImage		=	cvLoadImage("atik-logo.png",	CV_LOAD_IMAGE_COLOR);
	}
	else if ((strcasestr(cAlpacaDeviceName, "toup") != NULL) || (strcasestr(cAlpacaDeviceName, "GCMOS") != NULL))
	{
		logoImage		=	cvLoadImage("touptek-logo.png",	CV_LOAD_IMAGE_COLOR);
	}
	else if (strcasestr(cAlpacaDeviceName, "QHY") != NULL)
	{
		logoImage		=	cvLoadImage("qhy-logo.png",		CV_LOAD_IMAGE_COLOR);
	}
	SetWidgetImage(kCameraBox_Logo, logoImage);

	yLoc			+=	cLogoHeight;
	yLoc			+=	2;

	//=======================================================
	//*	exposure with slider
	SetWidget(		kCameraBox_Exposure_Label,	cClm1_offset,	yLoc,		cClmWidth,			cRadioBtnHt	);
	SetWidget(		kCameraBox_Exposure_Slider,	cClm2_offset,	yLoc,		(3 * cClmWidth),	cRadioBtnHt	);
	SetWidget(		kCameraBox_Exposure,		cClm5_offset,	yLoc,		cClmWidth,			cRadioBtnHt	);

	SetWidget(				kCameraBox_Exposure_Up,		cClm6_offset+5,	yLoc -2,	cSmIconSize,		cSmIconSize);
	SetWidget(				kCameraBox_Exposure_Down,	cClm6_offset+5,	yLoc + cSmIconSize,	cSmIconSize,cSmIconSize);
	SetWidgetBGColor(		kCameraBox_Exposure_Up,		CV_RGB(255,	255,	255));
	SetWidgetBGColor(		kCameraBox_Exposure_Down,	CV_RGB(255,	255,	255));

	SetWidgetBorderColor(	kCameraBox_Exposure_Up,		CV_RGB(0,	0,	0));
	SetWidgetBorderColor(	kCameraBox_Exposure_Down,	CV_RGB(0,	0,	0));
	SetWidgetIcon(			kCameraBox_Exposure_Up,		kIcon_UpArrow);
	SetWidgetIcon(			kCameraBox_Exposure_Down,	kIcon_DownArrow);
	SetWidgetTextColor(		kCameraBox_Exposure_Up,		CV_RGB(255,	0,	0));
	SetWidgetTextColor(		kCameraBox_Exposure_Down,	CV_RGB(255,	0,	0));



	SetWidgetType(	kCameraBox_Exposure_Slider,	kWidgetType_Slider);
	SetWidgetFont(	kCameraBox_Exposure_Label,	kFont_Small);
	SetWidgetFont(	kCameraBox_Exposure,		kFont_Small);
	SetWidgetFont(	kCameraBox_Exposure_Slider,	kFont_Small);
	SetWidgetText(	kCameraBox_Exposure_Label,	"Exp");
	SetWidgetSliderLimits(	kCameraBox_Exposure_Slider,	0.001, 1000);
	SetWidgetSliderValue(	kCameraBox_Exposure_Slider,	123.0);
	yLoc			+=	cSmIconSize * 2;
	yLoc			+=	2;

	yLoc			+=	8;

	//=======================================================
	//*	gain with slider
	SetWidget(		kCameraBox_Gain_Label,	cClm1_offset,	yLoc,		cClmWidth,			cRadioBtnHt	);
	SetWidget(		kCameraBox_Gain_Slider,	cClm2_offset,	yLoc,		(3 * cClmWidth),	cRadioBtnHt	);
	SetWidget(		kCameraBox_Gain,		cClm5_offset,	yLoc,		cClmWidth,			cRadioBtnHt	);
	SetWidget(		kCameraBox_Gain_Up,		cClm6_offset+5,	yLoc -2,	cSmIconSize,		cSmIconSize);
	SetWidget(		kCameraBox_Gain_Down,	cClm6_offset+5,	yLoc + cSmIconSize,	cSmIconSize,		cSmIconSize);
	SetWidgetBGColor(kCameraBox_Gain_Up,	CV_RGB(255,	255,	255));
	SetWidgetBGColor(kCameraBox_Gain_Down,	CV_RGB(255,	255,	255));

	SetWidgetBorderColor(kCameraBox_Gain_Up,	CV_RGB(0,	0,	0));
	SetWidgetBorderColor(kCameraBox_Gain_Down,	CV_RGB(0,	0,	0));

	SetWidgetIcon(		kCameraBox_Gain_Up,		kIcon_UpArrow);
	SetWidgetIcon(		kCameraBox_Gain_Down,	kIcon_DownArrow);
	SetWidgetTextColor(	kCameraBox_Gain_Up,		CV_RGB(255,	0,	0));
	SetWidgetTextColor(	kCameraBox_Gain_Down,	CV_RGB(255,	0,	0));


	SetWidgetType(	kCameraBox_Gain_Slider,	kWidgetType_Slider);
	SetWidgetFont(	kCameraBox_Gain_Label,	kFont_Small);
	SetWidgetFont(	kCameraBox_Gain_Slider,	kFont_Small);
	SetWidgetFont(	kCameraBox_Gain,		kFont_Small);
	SetWidgetText(	kCameraBox_Gain_Label,	"Gain");
	yLoc			+=	cSmIconSize * 2;
	yLoc			+=	5;

	//=======================================================
	//*	Camera state (idle, waiting, etc)
	SetWidget(					kCameraBox_State,	cClm1_offset,	yLoc,		cClmWidth * 2,		cRadioBtnHt	);
	SetWidgetFont(				kCameraBox_State,	kFont_Small);
	SetWidgetJustification  (	kCameraBox_State,	kJustification_Left);
	yLoc			+=	cRadioBtnHt;
	yLoc			+=	5;


	//=======================================================
	//*	Readout modes
	yLocSave	=	yLoc;
	for (iii=0; iii<kMaxReadOutModes; iii++)
	{
		SetWidget(		(kCameraBox_ReadMode0 + iii),	2,			yLoc,		cWidth/4,		cRadioBtnHt	);
		SetWidgetFont(	(kCameraBox_ReadMode0 + iii),	kFont_RadioBtn);
		SetWidgetType(	(kCameraBox_ReadMode0 + iii),	kWidgetType_RadioButton);
		SetWidgetValid(	(kCameraBox_ReadMode0 + iii),	false);


		yLoc			+=	cRadioBtnHt;
		yLoc			+=	1;
	}
	SetWidgetText(	kCameraBox_ReadMode0,	"opt1");
	SetWidgetText(	kCameraBox_ReadMode1,	"opt2");
	SetWidgetText(	kCameraBox_ReadMode2,	"opt3");
	SetWidgetText(	kCameraBox_ReadMode3,	"opt4");

	SetWidgetOutlineBox(kCameraBox_ReadModeOutline, kCameraBox_ReadMode0, kCameraBox_ReadMode4);
	yLoc			+=	4;

	//=======================================================
	//*	Live mode
	SetWidget(		kCameraBox_LiveMode,	cClm3_offset,	yLocSave,		cWidth/4,		cRadioBtnHt	);
	SetWidgetType(	kCameraBox_LiveMode,	kWidgetType_CheckBox);
	SetWidgetText(	kCameraBox_LiveMode,	"LIVE");
	SetWidgetFont(	kCameraBox_LiveMode,	kFont_Medium);
	yLocSave	+=	cRadioBtnHt;
	yLocSave	+=	2;

	//=======================================================
	//*	Live mode
	SetWidget(		kCameraBox_SideBar,	cClm3_offset,	yLocSave,		cWidth/4,		cRadioBtnHt	);
	SetWidgetType(	kCameraBox_SideBar,	kWidgetType_CheckBox);
	SetWidgetText(	kCameraBox_SideBar,	"SideBar");
	SetWidgetFont(	kCameraBox_SideBar,	kFont_Medium);
	yLocSave	+=	cRadioBtnHt;
	yLocSave	+=	2;


	//=======================================================
	//*	Auto exposure
	SetWidget(		kCameraBox_AutoExposure,	cClm3_offset,	yLocSave,	cWidth/4,		cRadioBtnHt	);
	SetWidgetType(	kCameraBox_AutoExposure,	kWidgetType_CheckBox);
	SetWidgetText(	kCameraBox_AutoExposure,	"AUTO EXP");
	SetWidgetFont(	kCameraBox_AutoExposure,	kFont_Medium);
	yLocSave	+=	cRadioBtnHt;
	yLocSave	+=	2;

	//=======================================================
	//*	Display Image
	SetWidget(		kCameraBox_DisplayImage,	cClm3_offset,	yLocSave,	cWidth/4,		cRadioBtnHt	);
	SetWidgetType(	kCameraBox_DisplayImage,	kWidgetType_CheckBox);
	SetWidgetText(	kCameraBox_DisplayImage,	"Display Image");
	SetWidgetFont(	kCameraBox_DisplayImage,	kFont_Medium);

	//=======================================================
	//*	Filename
	SetWidget(		kCameraBox_Filename,	cClm1_offset,	yLoc,		cFullWidthBtn,		cRadioBtnHt	);
	SetWidgetFont(	kCameraBox_Filename,	kFont_Medium);
	SetWidgetText(	kCameraBox_Filename,	"-----");
	yLoc			+=	cRadioBtnHt;
	yLoc			+=	2;


	//=======================================================
	//*	Error messages
	SetWidget(			kCameraBox_ErrorMsg,	cClm1_offset,	yLoc,	cFullWidthBtn,		cRadioBtnHt * 2	);
	SetWidgetType(		kCameraBox_ErrorMsg, 	kWidgetType_MultiLineText);
	SetWidgetFont(		kCameraBox_ErrorMsg,	kFont_Medium);
	SetWidgetText(		kCameraBox_ErrorMsg,	"-----");
	SetWidgetTextColor(	kCameraBox_ErrorMsg,	CV_RGB(255,	0,	0));
	yLoc			+=	cRadioBtnHt * 2;
	yLoc			+=	2;



	yLoc			+=	10;

	SetWidget(			kCameraBox_Reset,			cClm1_offset,	yLoc,	cLrgBtnWidth,	cLrgBtnHeight);
	SetWidgetType(		kCameraBox_Reset,			kWidgetType_Button);
	SetWidgetText(		kCameraBox_Reset,			"Reset");
	SetWidgetFont(		kCameraBox_Reset,			kFont_Medium);
	SetWidgetBGColor(	kCameraBox_Reset,			CV_RGB(255,	255,	255));
	SetWidgetTextColor(	kCameraBox_Reset,			CV_RGB(255,	0,	0));

	SetWidget(			kCameraBox_StartExposure,	cClm3_offset,	yLoc,	((cClmWidth * 2) -4),	cLrgBtnHeight);
	SetWidgetType(		kCameraBox_StartExposure,	kWidgetType_Button);
	SetWidgetText(		kCameraBox_StartExposure,	"Start Exposure");
	SetWidgetFont(		kCameraBox_StartExposure,	kFont_Medium);
	SetWidgetBGColor(	kCameraBox_StartExposure,	CV_RGB(255,	255,	255));
	SetWidgetTextColor(	kCameraBox_StartExposure,	CV_RGB(255,	0,	0));

	SetWidget(			kCameraBox_DownloadImage,	cClm5_offset,	yLoc,	((cClmWidth * 2) -4),	cLrgBtnHeight);
	SetWidgetType(		kCameraBox_DownloadImage,	kWidgetType_Button);
	SetWidgetText(		kCameraBox_DownloadImage,	"Download Image");
	SetWidgetFont(		kCameraBox_DownloadImage,	kFont_Medium);
	SetWidgetBGColor(	kCameraBox_DownloadImage,	CV_RGB(255,	255,	255));
	SetWidgetTextColor(	kCameraBox_DownloadImage,	CV_RGB(255,	0,	0));

	yLoc			+=	cLrgBtnHeight;
	yLoc			+=	4;

	//=======================================================
	SetWidget(			kCameraBox_Btn_8Bit,	cClm3_offset,	yLoc,	((cClmWidth * 2) -4),	cRadioBtnHt);
	SetWidgetType(		kCameraBox_Btn_8Bit,	kWidgetType_CheckBox);
	SetWidgetText(		kCameraBox_Btn_8Bit,	"Force 8 Bit");
	SetWidgetFont(		kCameraBox_Btn_8Bit,	kFont_RadioBtn);
//	SetWidgetBGColor(	kCameraBox_Btn_8Bit,	CV_RGB(255,	255,	255));
//	SetWidgetTextColor(	kCameraBox_Btn_8Bit,	CV_RGB(255,	0,	0));


	SetWidget(			kCameraBox_DownloadRGBarray,	cClm5_offset,	yLoc,	((cClmWidth * 2) -4),	cLrgBtnHeight);
	SetWidgetType(		kCameraBox_DownloadRGBarray,	kWidgetType_Button);
	SetWidgetText(		kCameraBox_DownloadRGBarray,	"DL RGBarray");
	SetWidgetFont(		kCameraBox_DownloadRGBarray,	kFont_Medium);
	SetWidgetBGColor(	kCameraBox_DownloadRGBarray,	CV_RGB(255,	255,	255));
	SetWidgetTextColor(	kCameraBox_DownloadRGBarray,	CV_RGB(255,	0,	0));

	yLoc			+=	cLrgBtnHeight;
	yLoc			+=	4;


	yLocClm4		=	yLoc;
	//=======================================================
	//*	Filter wheel (if present)
	if (cHasFilterWheel)
	{
	int	slotNum;
	int	filterWhlWidth;
	int		fnWidgetNum;	//*	filter name
	int		foWidgetNum;	//*	filter offset

		filterWhlWidth	=	(cClmWidth * 3) - 6;
		SetWidget(		kCameraBox_FilterWheelName,	cClm1_offset,	yLoc,	filterWhlWidth,		cRadioBtnHt	);
		SetWidgetFont(	kCameraBox_FilterWheelName,	kFont_RadioBtn);
		SetWidgetText(	kCameraBox_FilterWheelName,	"Filter wheel");
		yLoc			+=	cRadioBtnHt;
		yLoc			+=	1;

		slotNum	=	1;
//		for (iii=kCameraBox_FilterWheel1; iii<=kCameraBox_FilterWheel8; iii++)
		for (iii=0; iii<8; iii++)
		{
			fnWidgetNum	=	kCameraBox_FilterWheel1 + iii;
			foWidgetNum	=	kCameraBox_FilterOffset1 + iii;
			SetWidget(		fnWidgetNum,	2,				yLoc,	cWidth/4,		cRadioBtnHt	);
			SetWidgetFont(	fnWidgetNum,	kFont_RadioBtn);
			SetWidgetType(	fnWidgetNum,	kWidgetType_RadioButton);
			SetWidgetTextColor(fnWidgetNum, CV_RGB(255,	255,	255));
			sprintf(textBuff, "Slot #%d", slotNum);
			SetWidgetText(	fnWidgetNum,	textBuff);
			slotNum++;

			SetWidget(		foWidgetNum,	cClm3_offset,	yLoc,	cClmWidth,		cRadioBtnHt	);
			SetWidgetFont(	foWidgetNum,	kFont_RadioBtn);
			SetWidgetTextColor(foWidgetNum, CV_RGB(255,	255,	255));
			SetWidgetNumber(foWidgetNum, 0);


			yLoc			+=	cRadioBtnHt;
			yLoc			+=	1;
		}
		SetWidgetOutlineBox(	kCameraBox_FilterWheelOutline,
								kCameraBox_FilterWheelName,
								kCameraBox_FilterWheel8);
	}
	yLoc			+=	4;
	yLoc			+=	4;

	//=======================================================
	//*	Temperature
	SetWidget(		kCameraBox_Temp_Label,	cClm4_offset,	yLocClm4,		cClmWidth,		cRadioBtnHt	);
	SetWidget(		kCameraBox_Temperature,	cClm5_offset,	yLocClm4,		cClmWidth * 2,	cRadioBtnHt	);
	SetWidgetFont(	kCameraBox_Temp_Label,	kFont_Small);
	SetWidgetFont(	kCameraBox_Temperature,	kFont_Small);
	SetWidgetText(	kCameraBox_Temp_Label,	"Temp");
	yLocClm4	+=	cRadioBtnHt;
	yLocClm4	+=	2;
	//*	Cooler check box
	SetWidget(		kCameraBox_CoolerChkBox,	cClm4_offset + 2,	yLocClm4,	cClmWidth,	cRadioBtnHt	);
	SetWidgetType(	kCameraBox_CoolerChkBox,	kWidgetType_CheckBox);
	SetWidgetText(	kCameraBox_CoolerChkBox,	"Camera Cooler");
	SetWidgetFont(	kCameraBox_CoolerChkBox,	kFont_Medium);
	yLocClm4	+=	cRadioBtnHt;
	yLocClm4	+=	2;

	yLoc			+=	cRadioBtnHt;
	yLoc			+=	2;
	if (yLocClm4 > yLoc)
	{
		yLoc	=	yLocClm4;
	}


#ifdef _TEMP_GRAPH_
	SetWidget(		kCameraBox_TempGraph,	cClm1_offset+ 3,	yLoc,		cWidth - 8,		75	);
	SetWidgetType(	kCameraBox_TempGraph,	kWidgetType_Graph);

	SetWidgetOutlineBox(kCameraBox_TempOutline, kCameraBox_Temp_Label, kCameraBox_TempGraph);

	yLoc			+=	100;
	yLoc			+=	4;
#endif // _TEMP_GRAPH_


	SetAlpacaLogo(kCameraBox_AlpacaLogo, kCameraBox_LastCmdString);

	//=======================================================
	//*	IP address
	SetIPaddressBoxes(kCameraBox_IPaddr, kCameraBox_Readall, kCameraBox_AlpacaDrvrVersion, -1);
}

//**************************************************************************************
void	WindowTabCamera::SetTempartueDisplayEnable(bool enabled)
{

	SetWidgetValid(kCameraBox_Temp_Label,	enabled);
	SetWidgetValid(kCameraBox_Temperature,	enabled);
	SetWidgetValid(kCameraBox_CoolerChkBox,	enabled);
#ifdef _TEMP_GRAPH_
	SetWidgetValid(kCameraBox_TempGraph,	enabled);
	SetWidgetValid(kCameraBox_TempOutline,	enabled);
#endif // _TEMP_GRAPH_
}


//******************************************************************************
void	WindowTabCamera::DrawGraphWidget(IplImage *openCV_Image, const int widgitIdx)
{
CvRect		myCVrect;

	myCVrect.x		=	cWidgetList[widgitIdx].left;
	myCVrect.y		=	cWidgetList[widgitIdx].top;
	myCVrect.width	=	cWidgetList[widgitIdx].width;
	myCVrect.height	=	cWidgetList[widgitIdx].height;


	cvRectangleR(	openCV_Image,
					myCVrect,
					cWidgetList[widgitIdx].bgColor,			//	CvScalar color,
					CV_FILLED,								//	int thickness CV_DEFAULT(1),
					8,										//	int line_type CV_DEFAULT(8),
					0);										//	int shift CV_DEFAULT(0));

//	cvRectangleR(	openCV_Image,
//					myCVrect,
//					cWidgetList[widgitIdx].boarderColor,	//	CvScalar color,
//					1,										//	int thickness CV_DEFAULT(1),
//					8,										//	int line_type CV_DEFAULT(8),
//					0);										//	int shift CV_DEFAULT(0));

	switch(widgitIdx)
	{

		default:
			CONSOLE_DEBUG_W_NUM("widgitIdx\t",	widgitIdx);
			break;
	}
}



//*****************************************************************************
void	WindowTabCamera::ProcessButtonClick(const int buttonIdx)
{
bool		validData;
char		dataString[64];
int			fwPosition;

	switch(buttonIdx)
	{
		case kCameraBox_Gain_Up:
			BumpGain(1);
			break;

		case kCameraBox_Gain_Down:
			BumpGain(-1);
			break;

		case kCameraBox_Exposure_Up:
			BumpExposure(0.001);
			break;

		case kCameraBox_Exposure_Down:
			BumpExposure(-0.001);
			break;

		case kCameraBox_ReadMode0:
		case kCameraBox_ReadMode1:
		case kCameraBox_ReadMode2:
		case kCameraBox_ReadMode3:
		case kCameraBox_ReadMode4:
			sprintf(dataString, "ReadoutMode=%d", (buttonIdx - kCameraBox_ReadMode0));
			validData	=	AlpacaSendPutCmd(	"camera", "readoutmode",	dataString);
			ForceUpdate();
			break;

		case kCameraBox_LiveMode:
			ToggleLiveMode();
			break;

		case kCameraBox_SideBar:
			ToggleSideBar();
			break;

		case kCameraBox_AutoExposure:
			ToggleAutoExposure();
			break;

		case kCameraBox_DisplayImage:
			ToggleDisplayImage();
			break;

		case kCameraBox_CoolerChkBox:
			ToggleCooler();
			break;

		case kCameraBox_Reset:
			validData	=	AlpacaSendPutCmd(	"camera", "abortexposure",	NULL);
			if (validData == false)
			{
				CONSOLE_DEBUG("abortexposure failed");
			}
			break;

		case kCameraBox_StartExposure:
			CONSOLE_DEBUG(__FUNCTION__);
			StartExposure();
			break;

		case kCameraBox_DownloadImage:
			DownloadImage(false);	//*	false -> imageArray
			break;

		case kCameraBox_Btn_8Bit:
			cForce8BitRead	=	!cForce8BitRead;
			SetWidgetChecked(kCameraBox_Btn_8Bit, cForce8BitRead);
			ForceUpdate();
			break;

		case kCameraBox_DownloadRGBarray:
			DownloadImage(true);	//*	true -> Use RGBarray
			break;

		case kCameraBox_FilterWheel1:
		case kCameraBox_FilterWheel2:
		case kCameraBox_FilterWheel3:
		case kCameraBox_FilterWheel4:
		case kCameraBox_FilterWheel5:
		case kCameraBox_FilterWheel6:
		case kCameraBox_FilterWheel7:
		case kCameraBox_FilterWheel8:
			//*	alpaca is a zero based index for the filter number
			//*	number is 0 to N-1
			fwPosition	=	buttonIdx - kCameraBox_FilterWheel1;
			sprintf(dataString, "Position=%d", fwPosition);
	//		CONSOLE_DEBUG_W_STR("dataString\t=",	dataString);
			validData	=	AlpacaSendPutCmd(	"filterwheel", "position",	dataString);
			if (validData)
			{
				ForceUpdate();
			}
			else
			{
				CONSOLE_DEBUG("Error setting filter wheel position");
			}
			break;

		default:
			CONSOLE_DEBUG(__FUNCTION__);
			CONSOLE_DEBUG_W_NUM("buttonIdx\t",	buttonIdx);

			break;

	}
	DisplayLastAlpacaCommand();
}

//*****************************************************************************
void	WindowTabCamera::ForceUpdate(void)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->cForceAlpacaUpdate	=	true;
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}


//*****************************************************************************
void	WindowTabCamera::BumpGain(const int howMuch)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->BumpGain(howMuch);
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}

//*****************************************************************************
void	WindowTabCamera::BumpExposure(const double howMuch)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->BumpExposure(howMuch);
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}


//*****************************************************************************
void	WindowTabCamera::ToggleLiveMode(void)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->ToggleLiveMode();
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}

//*****************************************************************************
void	WindowTabCamera::ToggleSideBar(void)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->ToggleSideBar();
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}



//*****************************************************************************
void	WindowTabCamera::ToggleAutoExposure(void)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->ToggleAutoExposure();
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}

//*****************************************************************************
void	WindowTabCamera::ToggleDisplayImage(void)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->ToggleDisplayImage();
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}




//*****************************************************************************
void	WindowTabCamera::ToggleCooler(void)
{
ControllerCamera	*myCameraController;

//	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->ToggleCooler();
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}


//*****************************************************************************
void	WindowTabCamera::StartExposure(void)
{
ControllerCamera	*myCameraController;

	CONSOLE_DEBUG(__FUNCTION__);
	myCameraController	=	(ControllerCamera *)cParentObjPtr;

	if (myCameraController != NULL)
	{
		myCameraController->StartExposure();
	}
	else
	{
		CONSOLE_DEBUG("myCameraController is NULL");
	}
}

//*****************************************************************************
void	WindowTabCamera::DownloadImage(const bool useRGBarray)
{
ControllerCamera	*myCameraController;
IplImage			*myDownLoadedImage;
int					liveDispalyWidth;
int					liveDisplayHeight;
int					reduceFactor;
char				textBuf[128];
double				download_MBytes;
double				download_MB_per_sec;
double				download_seconds;
char				fileName[256];
int					quality[3] = {16, 200, 0};
int					openCVerr;

	CONSOLE_DEBUG(__FUNCTION__);


	myDownLoadedImage	=	NULL;
	myCameraController	=	(ControllerCamera *)cParentObjPtr;
	if (myCameraController != NULL)
	{
//		CONSOLE_DEBUG("Starting download");
		if (useRGBarray)
		{
			myDownLoadedImage	=	myCameraController->DownloadImage_rgbarray();
		}
		else
		{
			myDownLoadedImage	=	myCameraController->DownloadImage(cForce8BitRead);
		}
		if (myDownLoadedImage != NULL)
		{
			CONSOLE_DEBUG("Download complete");
			CONSOLE_DEBUG_W_NUM("myDownLoadedImage->width\t=",	myDownLoadedImage->width);
			CONSOLE_DEBUG_W_NUM("myDownLoadedImage->height\t=",	myDownLoadedImage->height);

			//======================================
			//*	save the image
			strcpy(fileName, cDownLoadedFileNameRoot);
			strcat(fileName, ".jpg");

			CONSOLE_DEBUG_W_STR("Saving image as", fileName);
			openCVerr	=	cvSaveImage(fileName, myDownLoadedImage, quality);
			if (openCVerr == 0)
			{
			int		openCVerrorCode;
			char	*errorMsgPtr;

				CONSOLE_DEBUG_W_NUM("Error saving file\t=", openCVerr);
				openCVerrorCode	=	cvGetErrStatus();
				CONSOLE_DEBUG_W_NUM("openCVerrorCode\t=", openCVerrorCode);
				errorMsgPtr	=	(char *)cvErrorStr(openCVerrorCode);
				CONSOLE_DEBUG_W_STR("errorMsgPtr\t=", errorMsgPtr);
			}
#ifdef _ENABLE_CTRL_IMAGE_
			//*	this will open a new window with the image displayed
			new ControllerImage(cDownLoadedFileNameRoot, myDownLoadedImage);
#else
			cvReleaseImage(&myDownLoadedImage);
#endif // _ENABLE_CTRL_IMAGE_


			download_MBytes		=	1.0 * myCameraController->cLastDownload_Bytes / (1024.0 * 1024.0);
			download_seconds	=	1.0 * myCameraController->cLastDownload_Millisecs / 1000.0;
			download_MB_per_sec	=	download_MBytes / download_seconds;

			sprintf(textBuf,	"%2.2f mbytes in %2.2f sec =%2.2f mbytes/sec",
								download_MBytes,
								download_seconds,
								download_MB_per_sec
								);


			SetWidgetType(kCameraBox_ErrorMsg, kWidgetType_Text);
			SetWidgetText(kCameraBox_ErrorMsg, textBuf);
		}
		else
		{
			SetWidgetType(kCameraBox_ErrorMsg, kWidgetType_Text);
			SetWidgetText(kCameraBox_ErrorMsg, "Failed to download image, no image exists");
			CONSOLE_DEBUG("Failed to download image");
		}
	}
}

//*****************************************************************************
void	WindowTabCamera::SetReceivedFileName(const char *newFileName)
{
	strcpy(cDownLoadedFileNameRoot, newFileName);
}

#endif // _ENABLE_CTRL_CAMERA_
