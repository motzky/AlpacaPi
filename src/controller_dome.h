//*****************************************************************************
//*	Jan 12,	2021	<MLS> Added _ENABLE_EXTERNAL_SHUTTER_
//*	Jan 12,	2021	<MLS> Added _ENABLE_SLIT_TRACKER_
//*****************************************************************************
//#include	"controller_dome.h"

#ifndef _CONTROLLER_DOME_H_
#define	_CONTROLLER_DOME_H_


//#define	_ENABLE_EXTERNAL_SHUTTER_
#define	_ENABLE_SLIT_TRACKER_


#ifndef _DISCOVERY_LIB_H_
	#include	"discovery_lib.h"
#endif // _DISCOVERY_LIB_H_

#include	"controller.h"

//===========================================
#ifndef	_WINDOWTAB_DOME_H_
	#include	"windowtab_dome.h"
#endif // _WINDOWTAB_DOME_H_
//===========================================
#ifndef	_WINDOWTAB_SLIT_H_
	#include	"windowtab_slit.h"
#endif // _WINDOWTAB_SLIT_H_


//===========================================
#ifndef	_WINDOWTAB_SLIT_GRAPH_H_
	#include	"windowtab_slitgraph.h"
#endif // _WINDOWTAB_SLIT_GRAPH_H_

//===========================================
#ifndef	_WINDOWTAB_ABOUT_H_
	#include	"windowtab_about.h"
#endif // _WINDOWTAB_ABOUT_H_


extern	bool	gUpdateSLitWindow;

//**************************************************************************************
enum
{
	kTab_Dome	=	1,
	kTab_SlitTracker,
	kTab_SlitGraph,
	kTab_About,

	kTab_Count

};

//**************************************************************************************
class ControllerDome: public Controller
{
	public:
		//
		// Construction
		//
						ControllerDome(	const char			*argWindowName,
										TYPE_REMOTE_DEV		*alpacaDevice);


		virtual			~ControllerDome(void);


		virtual	void	SetupWindowControls(void);
		virtual	void	RunBackgroundTasks(void);
	//	virtual	void	DrawGraphWidget(const int widgitIdx);
				bool	AlpacaGetStartupData(void);
				bool	AlpacaGetStatus(void);
		virtual	void	AlpacaDisplayErrorMessage(const char *errorMsgString);

		virtual	void	AlpacaProcessReadAll(			const char	*deviceType,
														const int	deviveNum,
														const char	*keywordString,
														const char	*valueString);
		virtual	void	AlpacaProcessSupportedActions(	const char	*deviceType,
														const int	deviveNum,
														const char	*valueString);

				void	AlpacaProcessSupportedActions_Dome(const int deviveNum, const char *valueString);
				void	AlpacaProcessReadAllDome(		const int	deviceNum,
														const char	*keywordString,
														const char	*valueString);


				bool	AlpacaGetStatus_OneAAT(void);	//*	One At A Time
				void	ToggleSwitchState(const int switchNum);
				void	UpdateDomeAzimuth(const double newAzimuth);

				void	UpdateShutterStatus(const int newShutterStatus);
				void	UpdateShutterAltitude(const double newAltitude);

			//===================================================================
				void	SetAlpacaShutterInfo(TYPE_REMOTE_DEV *alpacaDevice);
				void	SendShutterCommand(const char *shutterCmd);
			#ifdef _ENABLE_EXTERNAL_SHUTTER_
				void	AlpacaGetShutterReadAll(void);
				bool	ShutterSendPutCmd(	const char	*alpacaDevice,
											const char	*alpacaCmd,
											const char	*dataString);
			#endif
			//===================================================================
			#ifdef _ENABLE_SLIT_TRACKER_
				void	SetAlpacaSlitTrackerInfo(TYPE_REMOTE_DEV *alpacaDevice);
				void	AlpacaGetSlitTrackerReadAll(void);
				void	UpdateSlitLog(void);

				void	CloseSlitTrackerDataFile(void);

				WindowTabSlitTracker	*cSlitTrackerTabObjPtr;
				WindowTabSlitGraph		*cSlitGraphTabObjPtr;
			#endif // _ENABLE_SLIT_TRACKER_

				//*	tab information
				WindowTabDome			*cDomeTabObjPtr;
				WindowTabAbout			*cAboutBoxTabObjPtr;

				bool			cCanFindHome;
				bool			cCanPark;
				bool			cCanSetAltitude;
				bool			cCanSetAzimuth;
				bool			cCanSetPark;
				bool			cCanSetShutter;
				bool			cCanSlave;
				bool			cCanSyncAzimuth;
				int				cShutterStatus;

				bool			cAtHome;
				bool			cAtPark;
				bool			cSlaved;
				bool			cSlewing;
				double			cAzimuth_Dbl;
				double			cAltitude_Dbl;

				uint32_t		cUpdateDelta;

			#ifdef _ENABLE_EXTERNAL_SHUTTER_
				//*	shutter device info
				bool				cShutterInfoValid;
				struct sockaddr_in	cShutterDeviceAddress;
				int					cShutterPort;
				int					cShutterAlpacaDevNum;
				bool				cShutterCommFailed;			//*	failed to communicate with shutter
				int					cShutterCommFailCnt;
			#endif // _ENABLE_EXTERNAL_SHUTTER_

			#ifdef _ENABLE_SLIT_TRACKER_
				//*	Slit tracker device info
				bool				cSlitTrackerInfoValid;
				struct sockaddr_in	cSlitTrackerDeviceAddress;
				int					cSlitTrackerPort;
				int					cSlitTrackerAlpacaDevNum;
				bool				cSlitTrackerCommFailed;			//*	failed to communicate with shutter
				struct timeval		cSlitTrackerLastUpdateTime;		//*	last time update
				bool				cLogSlitData;
				FILE				*cSlitDataLogFilePtr;

				double				cGravity_X;
				double				cGravity_Y;
				double				cGravity_Z;
				double				cGravity_T;
				bool				cValidGravity;
				double				cUpAngle_Rad;
				double				cUpAngle_Deg;
			#endif
};


//*****************************************************************************
typedef struct
{
	bool	validData;
	bool	updated;
	double	distanceInches;
	long	readCount;
} TYPE_SLITCLOCK;

#define		kSensorValueCnt	12


extern	TYPE_SLITCLOCK	gSlitDistance[];
extern	int				gSlitLogIdx;


//*****************************************************************************
typedef struct
{
	bool	validData;
	double	distanceInches[kSensorValueCnt];
	double	average20pt[kSensorValueCnt];
} TYPE_SLIT_LOG;

#define	kSlitLogCount	400

extern	TYPE_SLIT_LOG	gSlitLog[kSlitLogCount];

#ifdef _SLIT_TRACKER_DIRECT_
	void	SendSlitTrackerCmd(const char *cmdBuffer);
#endif


#endif // _CONTROLLER_DOME_H_

