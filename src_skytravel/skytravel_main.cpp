//*****************************************************************************
//*		skytravel_main.cpp		(c) 2021 by Mark Sproul
//*
//*		This is the main routine for creating controller windows
//*
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
//*	Jan  9,	2020	<MLS> Created skytravel_main.cpp
//*****************************************************************************


#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<sys/time.h>

#include "opencv/highgui.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"



#include	"discovery_lib.h"
#include	"cpu_stats.h"

//#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"


#include	"alpaca_defs.h"
#include	"discoverythread.h"

#include	"controller.h"
#include	"controller_skytravel.h"


bool	gKeepRunning;
char	gColorOverRide	=	0;
char	gFullVersionString[128];
char	gFirstArgString[256];


//*****************************************************************************
static void	ProcessCmdLineArgs(int argc, char **argv)
{
int		ii;
char	theChar;

	strcpy(gFirstArgString, "");

	CONSOLE_DEBUG(__FUNCTION__);
	ii	=	1;
	while (ii<argc)
	{
		if (argv[ii][0] == '-')
		{
			theChar	=	argv[ii][1];
			switch(theChar)
			{
				//*	-c	force color
				//*	-cr		means red
				case 'c':
					if (isalpha(argv[ii][2]))
					{
						gColorOverRide	=	argv[ii][2];
					}
					break;

				//*	-i ip address
				case 'i':
					break;

			}
		}
		else
		{
			strcpy(gFirstArgString, argv[ii]);
		}
		ii++;
	}
}


//*****************************************************************************
int main(int argc, char *argv[])
{
int					iii;
int					objectsCreated;
int					activeObjCnt;
int					keyPressed;

	objectsCreated	=	0;
	gColorOverRide	=	0;

	CONSOLE_DEBUG(__FUNCTION__);

	ProcessCmdLineArgs(argc, argv);

	sprintf(gFullVersionString, "%s - %s build #%d", kApplicationName, kVersionString, kBuildNumber);


	CPUstats_ReadOSreleaseVersion();
	CPUstats_ReadInfo();



	new ControllerSkytravel("SkyTravel");
	objectsCreated++;

	StartDiscoveryQuerryThread();


	if (objectsCreated > 0)
	{
		gKeepRunning	=	true;
		activeObjCnt	=	objectsCreated;

		while (gKeepRunning && (activeObjCnt > 0))
		{
			activeObjCnt	=	0;
			for (iii=0; iii<kMaxControllers; iii++)
			{
				if (gControllerList[iii] != NULL)
				{
					activeObjCnt++;
					gControllerList[iii]->HandleWindow();
			//		usleep(100);
					keyPressed	=	cvWaitKey(25);
					if (keyPressed > 0)
					{
						Controller_HandleKeyDown(keyPressed);
					}

					if (gControllerList[iii]->cKeepRunning == false)
					{
						CONSOLE_DEBUG_W_NUM("Deleting control #", iii);
						CONSOLE_DEBUG_W_STR("Deleting window", gControllerList[iii]->cWindowName);
						delete gControllerList[iii];
					}
				}
			}
		}
		CONSOLE_DEBUG("Closing all windows");
		for (iii=0; iii<kMaxControllers; iii++)
		{
			if (gControllerList[iii] != NULL)
			{
				CONSOLE_DEBUG_W_STR("Deleting window", gControllerList[iii]->cWindowName);
				delete gControllerList[iii];
				cvWaitKey(10);
			//	sleep(2);
			}
		}
		CONSOLE_DEBUG("Clean exit");
	}
	else
	{
		CONSOLE_DEBUG("No devices found");
	}
}


//*****************************************************************************
//*	this steps through the Controller Object List to see if there is a window by this name
//*****************************************************************************
bool	CheckForOpenWindowByName(const char *windowName)
{
int		iii;
bool	windowExists;

	windowExists	=	false;
	for (iii=0; iii<kMaxControllers; iii++)
	{
		if (gControllerList[iii] != NULL)
		{
			if (strcmp(gControllerList[iii]->cWindowName, windowName) == 0)
			{
				windowExists	=	true;
				break;
			}
		}
	}
	return(windowExists);
}

