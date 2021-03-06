//*****************************************************************************
//#include	"controller_skytravel.h"


#include	"controller.h"

#ifndef	_WINDOWTAB_IMAGE_H_
	#include	"windowtab_image.h"
#endif

#ifndef	_WINDOWTAB_ABOUT_H_
	#include	"windowtab_about.h"
#endif // _WINDOWTAB_ABOUT_H_
#ifndef	_WINDOWTAB_DOME_H_
	#include	"windowtab_dome.h"
#endif // _WINDOWTAB_DOME_H_

#ifndef	_WINDOWTAB_SKYTRAVEL_H_
	#include	"windowtab_skytravel.h"
#endif // _WINDOWTAB_SKYTRAVEL_H_

#ifndef	_WINDOWTAB_ALPACALIST_H_
	#include	"windowtab_alpacalist.h"
#endif // _WINDOWTAB_ALPACALIST_H_

//**************************************************************************************
enum
{
	kTab_SkyTravel	=	1,
	kTab_Dome,
	kTab_AlpacaList,
	kTab_About,

	kTab_Count

};



//**************************************************************************************
class ControllerSkytravel: public Controller
{
	public:
		//
		// Construction
		//
				ControllerSkytravel(const char	*argWindowName);


		virtual	~ControllerSkytravel(void);


		virtual	void	SetupWindowControls(void);
//		virtual	void	ProcessButtonClick(const int buttonIdx);
		virtual	void	RunBackgroundTasks(void);
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
				bool	AlpacaGetStartupData(void);
				bool	AlpacaGetStatus_OneAAT(void);	//*	One At A Time

		virtual	void	RefreshWindow(void);

		//====================================================
		//*	tab information
				WindowTabSkyTravel	*cSkyTravelTabOjbPtr;
				WindowTabDome		*cDomeTabObjPtr;
				WindowTabAlpacaList	*cAlpacaListObjPtr;
				WindowTabAbout		*cAboutBoxTabObjPtr;
		//====================================================
		//*	alpaca device information
				bool				LookForIPaddress(void);
				bool				AlpacaGetDomeStatus(void);

				bool				cDomeAddressValid;
				sockaddr_in			cDomeIpAddress;
				int					cDomeIpPort;
				int					cDomeAlpacaDeviceNum;
				bool				cReadStartup_Dome;
				bool				cDomeHas_readall;
				//----------------------------------------------------------
				//*	these are copied direct from controller_dome
				void			UpdateDomeAzimuth(const double newAzimuth);
				void			UpdateShutterStatus(const int newShutterStatus);
				void			UpdateShutterAltitude(const double newAltitude);
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


				bool				cTelescopeAddressValid;
				sockaddr_in			cTelescopeIpAddress;
				int					cTelescopeIpPort;
				int					cTelescopeAlpacaDeviceNum;

};




