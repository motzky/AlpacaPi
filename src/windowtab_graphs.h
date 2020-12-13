//*****************************************************************************
//#include	"windowtab_graphs.h"



#ifndef	_WINDOWTAB_GRAPHS_H_
#define	_WINDOWTAB_GRAPHS_H_

#ifndef	_CONTROLLER_H_
	#include	"controller.h"
#endif // _CONTROLLER_H_

//*****************************************************************************
enum
{
	kGraphBox_TempGraph	=	0,
	kGraphBox_TempLabel,
	kGraphBox_VoltageGraph,
	kGraphBox_VoltageLabel,

	kGraphBox_AlpacaLogo,
	kGraphBox_AlpacaDrvrVersion,
	kGraphBox_IPaddr,
	kGraphBox_Readall,


	kGraphBox_last
};

#define	kMaxLogEnries	300

//**************************************************************************************
class WindowTabGraph: public WindowTab
{
	public:
		//
		// Construction
		//
				WindowTabGraph(const int xSize, const int ySize, CvScalar backGrndColor);
		virtual	~WindowTabGraph(void);

		virtual	void	SetupWindowControls(void);
		virtual	void	DrawGraphWidget(IplImage *openCV_Image, const int widgitIdx);

				void	DrawGraph(IplImage *openCV_Image, TYPE_WIDGET *graphWidget, double *arrayData, int arrayCount);
				void	LogVoltage(double voltageValue);
				void	LogTemperature(double temp_degC);

				double	cVoltageLog[kMaxLogEnries];
				double	cTemperatureLog[kMaxLogEnries];

};


#endif // _WINDOWTAB_GRAPHS_H_