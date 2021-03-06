//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "USignals.h"
#include "UAnalizator.h"
#include "UCalibrate.h"
#include "UPsu.h"
#include "UMgqs.h"
#include "ULog.h"

//---------------------------------------------------------------------------
USEFORM("UFProbeTest.cpp", Form3);
//---------------------------------------------------------------------------
USignals*		Signals;
Analizator* 	analizator;
Calibrate*		calibrate;
Psu*			psu;
Mgqs*			mgqs;
Log				console;

u32	access_count	= 0;
u32	access_error	= 0;

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{
	try
	{
        FormatSettings.DecimalSeparator	= '.';
		Signals		= new USignals();
		analizator	= new Analizator();
		calibrate	= new Calibrate();
		psu			= new Psu();
		mgqs		= new Mgqs();
		console.init();

		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm3), &Form3);
		Application->Run();

//		delete Form3;
		console.close();
//		delete analizator;
//		delete Signals;
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
