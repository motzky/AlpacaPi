//*****************************************************************************
//*		windowtab_ml_single..cpp		(c) 2020 by Mark Sproul
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
//*	Feb 24,	2020	<MLS> Created windowtab_focuser.cpp
//*	Feb 26,	2020	<MLS> Added NiteCrawler logo in memory so .png file is not needed
//*	Apr 24,	2020	<MLS> Changed name to windowtab_ml_single.cpp
//*	May 21,	2020	<MLS> Focuser store points working on MoonLite single controller
//*	Jun 19,	2020	<MLS> Finished HOME button operation
//*****************************************************************************

#ifdef _ENABLE_CTRL_FOCUSERS_


#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"


#include	"json_parse.h"
#include	"controller.h"
#include	"controller_focus.h"


#include	"windowtab_ml_single.h"

#include	"nitecrawler_image.h"
#include	"focuser_common.h"


IplImage	*gMoonLiteImage	=	NULL;


//*******************************************7*******************************************
WindowTabMLsingle::WindowTabMLsingle(	const int	xSize,
										const int	ySize,
										CvScalar	backGrndColor,
										const int	comMode,
										const char	*windowName)
	:WindowTab(xSize, ySize, backGrndColor, windowName)
{
int		iii;
	CONSOLE_DEBUG(__FUNCTION__);

	if (gMoonLiteImage == NULL)
	{
	//	gNiteCrawlerImgPtr	=	GetNiteCrawlerImage();
		gMoonLiteImage		=	GetMoonLiteImage();

		if (gNiteCrawlerImgPtr != NULL)
		{
		//	WriteOutImageAsCode(gNiteCrawlerImgPtr);
		//	CONSOLE_DEBUG_W_NUM("nc image width\t=", gNiteCrawlerImgPtr->width);
		//	CONSOLE_DEBUG_W_NUM("nc image height\t=", gNiteCrawlerImgPtr->height);
		}
	}


	cComMode			=	comMode;
	cPreviousBtnClicked	=	-1;
	for (iii=0; iii<kStorePtCnt; iii++)
	{
		cStorePoints[iii]	=	-1;
	}
	iii	=	0;
	cButtonNumValues[iii++]	=	5000;
	cButtonNumValues[iii++]	=	1000;
	cButtonNumValues[iii++]	=	100;
	cButtonNumValues[iii++]	=	10;
	cButtonNumValues[iii++]	=	1;
	cButtonNumValues[iii++]	=	-1;
	cButtonNumValues[iii++]	=	-10;
	cButtonNumValues[iii++]	=	-100;
	cButtonNumValues[iii++]	=	-1000;
	cButtonNumValues[iii++]	=	-5000;

	SetupWindowControls();
}

//**************************************************************************************
// Destructor
//**************************************************************************************
WindowTabMLsingle::~WindowTabMLsingle(void)
{
//	CONSOLE_DEBUG(__FUNCTION__);
}

//**************************************************************************************
void	WindowTabMLsingle::SetupWindowControls(void)
{
int		yLoc;
int		yLocSave;
int		yloc2;
int		btnIdx;
int		rightSideX;
int		rightSideWidth;
int		myButtonX;
int		myButtonWidth;
char	numberString[32];

	CONSOLE_DEBUG(__FUNCTION__);

	//------------------------------------------
	yLoc			=	cTabVertOffset;

	//------------------------------------------
	if (gMoonLiteImage != NULL)
	{
		SetWidget(kMLsingle_logo,		0,			yLoc,
									gMoonLiteImage->width,
									gMoonLiteImage->height);

//		yLoc			+=	gMoonLiteImage->height;

		SetWidgetImage(kMLsingle_logo, gMoonLiteImage);
	}
	else
	{
		SetWidget(kMLsingle_logo,		0,			yLoc,		cWidth,		kLogoHeight	);
//		yLoc			+=	kLogoHeight;
	}
	yLoc			+=	kLogoHeight;
	yLoc			+=	2;
	SetWidgetType(kMLsingle_logo, kWidgetType_Image);

	//==========================================
	SetWidget(		kMLsingle_Model,			0,				yLoc,		cWidth,	cBtnHeight);
	SetWidgetFont(	kMLsingle_Model, kFont_Medium);
	yLoc	+=	cBtnHeight;
	yLoc	+=	2;

	SetBGcolorFromWindowName(kMLsingle_Model);

	//==========================================
	SetWidget(			kMLsingle_Temperature,	0,				yLoc,		cWidth,	cBtnHeight);
	SetWidgetFont(		kMLsingle_Temperature,	kFont_Medium);
	SetWidgetTextColor(	kMLsingle_Temperature,	Color16BitTo24Bit(0x07ff));
	yLoc			+=	cBoxHeight;
	yLoc			+=	2;

	yloc2	=	yLoc;

	myButtonX		=	8;
	myButtonWidth	=	(cClmWidth * 4) - 4;
	//==========================================
	SetWidget(			kMLsingle_focValue,		myButtonX,	yLoc,	myButtonWidth,	cBoxHeight);
	yLoc			+=	cBoxHeight;
	yLoc			+=	2;

	//==========================================
	SetWidget(			kMLsingle_focDesired,	myButtonX,	yLoc,	myButtonWidth,	cBoxHeight);
	yLoc			+=	cBoxHeight;
	yLoc			+=	2;

	yLocSave		=	yLoc;
	//==========================================
	myButtonWidth	=	(cClmWidth * 2) - 5;
	btnIdx			=	kMLsingle_Store;
	while (btnIdx <= kMLsingle_SP09)
	{
		SetWidget(				btnIdx,		cClm1_offset + 5,	yLoc,	myButtonWidth,	cSmallBtnHt);
		SetWidgetType(			btnIdx,		kWidgetType_Button);
		SetWidgetFont(			btnIdx,		kFont_Small);
		sprintf(numberString, "SP%d", 	(btnIdx - kMLsingle_Store));
		SetWidgetText(			btnIdx,		numberString);
		SetWidgetBGColor(		btnIdx,		CV_RGB(255, 255, 255));
		SetWidgetBorderColor(	btnIdx,		CV_RGB(0, 0, 0));
		SetWidgetTextColor(		btnIdx,		CV_RGB(0, 0, 0));

		yLoc	+=	cSmallBtnHt;
		yLoc	+=	2;

		btnIdx++;
	}
	SetWidgetText(			kMLsingle_Store,		"Store");


	yLoc			=	yLocSave;

	//==========================================
	//*	this will use the buttons in the order they are defined to set up the widgets
	//*	the above order MUST match
	btnIdx	=	kMLsingle_Foc_p5000;
	while (btnIdx <= kMLsingle_Foc_m5000)
	{
		SetWidget(				btnIdx,		cClm3_offset + 5,	yLoc,	myButtonWidth,	cSmallBtnHt);
		SetWidgetType(			btnIdx,		kWidgetType_Button);
		SetWidgetFont(			btnIdx,		kFont_Small);
		sprintf(numberString, "%+d", cButtonNumValues[btnIdx - kMLsingle_Foc_p5000]);
		SetWidgetText(			btnIdx,		numberString);
		SetWidgetBGColor(		btnIdx,		CV_RGB(255, 255, 255));
		SetWidgetBorderColor(	btnIdx,		CV_RGB(0, 0, 0));
		SetWidgetTextColor(		btnIdx,		CV_RGB(0, 0, 0));

		yLoc	+=	cSmallBtnHt;
		yLoc	+=	2;

		btnIdx++;
	}
	CONSOLE_DEBUG_W_NUM("btnIdx\t",	btnIdx);

	SetWidgetOutlineBox(kMLsingle_GOTO_outline, kMLsingle_focValue, kMLsingle_Foc_m5000);

	//================================================================
	//*	right hand of screen stuff

	rightSideX		=	cClm5_offset + 10;
	rightSideWidth	=	(cBtnWidth * 2) - 10;

	SetWidget(			kMLsingle_CurPosLabel,	rightSideX,	yloc2,		rightSideWidth,	cBoxHeight);
	SetWidgetFont(		kMLsingle_CurPosLabel,	kFont_Medium);
	SetWidgetText(		kMLsingle_CurPosLabel,	"POS");
	SetWidgetTextColor(	kMLsingle_CurPosLabel,	CV_RGB(255, 0, 0));
	SetWidgetBorder(	kMLsingle_CurPosLabel,	false);
	yloc2			+=	cBoxHeight;
	yloc2			+=	2;

	SetWidget(			kMLsingle_NewPosLabel,	rightSideX,	yloc2,		rightSideWidth,	cBoxHeight);
	SetWidgetFont(		kMLsingle_NewPosLabel,	kFont_Medium);
	SetWidgetText(		kMLsingle_NewPosLabel,	"NEW");
	SetWidgetTextColor(	kMLsingle_NewPosLabel,	CV_RGB(255, 0, 0));
	SetWidgetBorder(	kMLsingle_NewPosLabel,	false);
	yloc2			+=	cBoxHeight;
	yloc2			+=	2;

	yloc2			+=	12;

	//==========================================
	SetWidget(			kMLsingle_StopMotors,	rightSideX,	yloc2,		rightSideWidth,	cBtnHeight);
	SetWidgetType(		kMLsingle_StopMotors,	kWidgetType_Button);
	SetWidgetFont(		kMLsingle_StopMotors,	kFont_Medium);
	SetWidgetText(		kMLsingle_StopMotors,	"STOP");
	SetWidgetTextColor(	kMLsingle_StopMotors,	CV_RGB(255, 255, 255));
	SetWidgetBGColor(	kMLsingle_StopMotors,	CV_RGB(255, 0, 0));
	SetWidgetBorderColor(kMLsingle_StopMotors,	CV_RGB(255, 255, 255));

	yloc2			+=	cBtnHeight;
	yloc2			+=	2;


	//==========================================
	SetWidget(			kMLsingle_Home,			rightSideX,	yloc2,		rightSideWidth,	cBtnHeight);
	SetWidgetType(		kMLsingle_Home,			kWidgetType_Button);
	SetWidgetFont(		kMLsingle_Home,			kFont_Small);
	SetWidgetText(		kMLsingle_Home,			"HOME");
	SetWidgetTextColor(	kMLsingle_Home,			CV_RGB(0, 0, 0));
	SetWidgetBGColor(	kMLsingle_Home,			CV_RGB(255, 255, 255));
	yloc2			+=	cBtnHeight;
	yloc2			+=	2;

	CONSOLE_DEBUG(__FUNCTION__);


	//*	set default text strings
	SetWidgetText(	kMLsingle_Model,		"HiRes Stepper");
	SetWidgetText(	kMLsingle_focDesired,	"-----");
	SetWidgetText(	kMLsingle_focValue,		"-----");
	SetWidgetText(	kMLsingle_Temperature,	"-----");
	SetWidgetText(	kMLsingle_IPaddr,		"-----");


	switch(cComMode)
	{
		case kNCcomMode_Alpaca:
			SetAlpacaLogo(kMLsingle_AlpacaLogo, kMLsingle_LastCmdString);
			SetIPaddressBoxes(kMLsingle_IPaddr, kMLsingle_Readall, kMLsingle_AlpacaDrvrVersion, -1);
			break;

		case kNCcomMode_USB:
			SetAlpacaLogo(-1, kMLsingle_LastCmdString);
			SetIPaddressBoxes(	kMLsingle_IPaddr,
								kMLsingle_Readall,
								kMLsingle_AlpacaDrvrVersion,
								kMLsingle_Connect);

//			SetWidgetText(	kMLsingle_AlpacaDrvrVersion,		gFullVersionString);

			break;

		default:
			CONSOLE_DEBUG_W_NUM("cComMode not set=", cComMode);
			exit(0);
			break;
	}

}



//*****************************************************************************
void	WindowTabMLsingle::ProcessButtonClick(const int buttonIdx)
{
ControllerFocus	*focusController;
int				prevDesiredFocusPosition;
int				currFocuserPosition;
int				myFocuserDesiredPotion;
int				btnValueIdx;
bool			alpacaCmdSent;
int				setPointIdx;

//	CONSOLE_DEBUG(__FUNCTION__);
//	CONSOLE_DEBUG_W_NUM("buttonIdx\t",	buttonIdx);
	alpacaCmdSent	=	false;
	if (cParentObjPtr != NULL)
	{
		focusController				=	(ControllerFocus*)cParentObjPtr;
		prevDesiredFocusPosition	=	focusController->cFocuserDesiredPos;
		currFocuserPosition			=	focusController->cFocuserPosition;

		myFocuserDesiredPotion		=	prevDesiredFocusPosition;
		switch(buttonIdx)
		{
			case kMLsingle_Store:
				SetWidgetText(kMLsingle_LastCmdString, "Select Set Point Button");
				CONSOLE_DEBUG_W_NUM("buttonIdx\t",	buttonIdx);
				break;

			case kMLsingle_SP01:
			case kMLsingle_SP02:
			case kMLsingle_SP03:
			case kMLsingle_SP04:
			case kMLsingle_SP05:
			case kMLsingle_SP06:
			case kMLsingle_SP07:
			case kMLsingle_SP08:
			case kMLsingle_SP09:
				setPointIdx			=	buttonIdx - kMLsingle_SP01;
				if (cPreviousBtnClicked == kMLsingle_Store)
				{
					//*	save the current location
					CONSOLE_DEBUG("Saving location");
					CONSOLE_DEBUG_W_NUM("buttonIdx\t",	buttonIdx);

					cStorePoints[setPointIdx]	=	currFocuserPosition;

					SetWidgetNumber(buttonIdx, currFocuserPosition);
					SetWidgetText(kMLsingle_LastCmdString, "Saved");
				}
				else
				{
					if (cStorePoints[setPointIdx] >= 0)
					{
						focusController->SendMoveFocuserCommand(cStorePoints[setPointIdx]);
						alpacaCmdSent	=	true;
					}
					else
					{
						SetWidgetText(kMLsingle_LastCmdString, "Set point not valid");
					}
				}
				break;

			case kMLsingle_Foc_m1:
			case kMLsingle_Foc_p1:
			case kMLsingle_Foc_m10:
			case kMLsingle_Foc_p10:
			case kMLsingle_Foc_m100:
			case kMLsingle_Foc_p100:
			case kMLsingle_Foc_m1000:
			case kMLsingle_Foc_p1000:
			case kMLsingle_Foc_m5000:
			case kMLsingle_Foc_p5000:
				btnValueIdx	=	buttonIdx - kMLsingle_Foc_p5000;
				if (btnValueIdx >= 0)
				{
					myFocuserDesiredPotion	+=	cButtonNumValues[btnValueIdx];
					if (myFocuserDesiredPotion < 0)
					{
						myFocuserDesiredPotion	=	0;
					}

					//*	do we need to update the focuser position
					if (myFocuserDesiredPotion != prevDesiredFocusPosition)
					{
						CONSOLE_DEBUG("Calling SendMoveFocuserCommand()");
						focusController->SendMoveFocuserCommand(myFocuserDesiredPotion);
						alpacaCmdSent	=	true;
					}
				}
				else
				{
					CONSOLE_DEBUG("out of bounds");
					exit(0);
				}
				break;


			case kMLsingle_StopMotors:
				focusController->SendStopMotorsCommand();
				alpacaCmdSent	=	true;
				break;

			case kMLsingle_Home:
				focusController->SendMoveFocuserCommand(0);
				alpacaCmdSent	=	true;
				break;

			case kMLsingle_Connect:
				if (focusController->cUSBportOpen)
				{
					CONSOLE_DEBUG("DisConnect");
					focusController->CloseUSBport();
				}
				else
				{
					CONSOLE_DEBUG("Connect");
					focusController->OpenUSBport();
				}
				break;

			default:
				CONSOLE_DEBUG(__FUNCTION__);
				CONSOLE_DEBUG_W_NUM("buttonIdx\t",	buttonIdx);
				break;
		}
		cPreviousBtnClicked	=	buttonIdx;

		if (alpacaCmdSent)
		{
			DisplayLastAlpacaCommand();
		}
	}
}

//*****************************************************************************
void	WindowTabMLsingle::ProcessDoubleClick(const int buttonIdx)
{
	CONSOLE_DEBUG_W_NUM("Double click on button #", buttonIdx);
	//*	this routine should be overloaded
}


#endif	//	_ENABLE_CTRL_FOCUSERS_
