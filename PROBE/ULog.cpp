//---------------------------------------------------------------------------

#pragma hdrstop

#include "ULog.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)

extern USignals*	Signals;
//==============================================================================
Log::Log(){

	_init	= false;
	_data 	= EmptyStr;
};

//-----------------------------------
Log::~Log(){


};

//-----------------------------------
void	__fastcall Log::init(){

	_init	= true;
};

//-----------------------------------
void	__fastcall Log::close(){

	_init	= false;
};

//-----------------------------------
Log& Log::operator<<(String message){

	if(_init)	_data += message;
	return *this;
};

//-----------------------------------
Log& Log::operator<<(const Endl* pointer){

	if(_init){
		(*Signals)["Log.Change"]->Execute(_data);
		_data = EmptyStr;
	}
	return *this;
};
