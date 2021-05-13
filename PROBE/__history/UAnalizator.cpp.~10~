//---------------------------------------------------------------------------

#pragma hdrstop

#include "UAnalizator.h"
#include "Registry.hpp"
//---------------------------------------------------------------------------
#pragma package(smart_init)



extern USignals*	Signals;
extern Log			console;
//extern Rows*	rows;
i32 VAL1[23] = {0,100,200,300,400,500,1000,1500,2000,2500,
				3000,3500,4000,4500,5000,5500,6000,6500,7000,7500,
				8000,8500,9000};
i32 LOW1[23] = {2157,2167,2200,2223,2246,2267,2374,2492,2606,2720,
				2828,2954,3064,3178,3293,3407,3522,3636,3702,3746,
				3760,3786,3788};
i32 LOW2[23] = {2147,2177,2190,2213,2236,2257,2364,2482,2596,2710,
				2838,2944,3054,3168,3283,3397,3512,3626,3692,3736,
				3750,3776,3778};

i32 VAL2[33] =	{9000,10000,11000,12000,12500,13000,13500,14000,14500,15000,
				15500,16000,16500,17000,17500,18000,18500,19000,19500,20000,
				20500,21000,21500,22000,22500,23000,24000,24500,25000,25500,
				26000,26500,27000};
i32 HI1[33] = 	{2805,2875,2945,3015,3055,3095,3125,3155,3185,3215,
				3255,3290,3325,3360,3395,3430,3465,3500,3535,3570,
				3605,3640,3670,3690,3705,3715,3740,3750,3755,3760,
				3765,3770,3775};
i32 HI2[33] = 	{2795,2865,2935,3005,3045,3085,3115,3145,3175,3205,
				3245,3280,3315,3350,3385,3420,3455,3490,3525,3560,
				3595,3630,3660,3680,3695,3705,3730,3740,3745,3750,
				3755,3760,3765};

std::map<i32, pair<i32, i32> >	v12;
std::map<i32, pair<i32, i32> >  v27;





//==============================================================================
void	callback(u8 a, u8 b, u8 c){

	HList list(true);
	list->Add(IntToStr(a));
	list->Add(IntToStr(b));
	list->Add(IntToStr(c));

	(*Signals)["Probe.Perform"]->Message(list->Text);
};
//==============================================================================
Analizator::Analizator(){

	_ols[0]	= 1;
	_ols[1]	= 1;

	_contact[0]	= 31;
	_contact[1]	= 32;

	_level[0]	= 400;
	_level[1]	= 2400;

	_target_id	= 0;

	_comparator_on	= true;
	bool			_relay;
	initialize_maps();

	HList list			= get_COM_ports();
//	load_port_info();
//	std::string	address	= get_first(list);
//	connect(address);

};

//-------------------------------------
Analizator::~Analizator(){

	disconnect();
};

//-------------------------------------
void	__fastcall Analizator::connect(std::string address, i32 baudrate){

	if(address == "")	return;

	_address	= address.c_str();
	_probe.open(address, baudrate);
	_probe.runAutoButtonStateCheck(200, callback);

	if(is_connected())	save_port_info();

	message("Probe.Config");
};

//-------------------------------------
void	__fastcall Analizator::disconnect(){

	_probe.reset();
};

//-------------------------------------
bool	__fastcall Analizator::is_connected(){

	if(!_probe.isOpened())	return false;

	std::string	id;
	return (_probe.getDeviceID(id) && id == DEFAULT_ID);

};

//-------------------------------------
bool 	__fastcall Analizator::getDeviceID(String &ID){

	std::string id;
	bool res	= _probe.getDeviceID(id);
	ID	= AnsiString(id.c_str());
	return res;
};

//-------------------------------------
bool	__fastcall Analizator::getEvent(u8 ch, u16& event){

	return ch == 0? _probe.getLowTime(event): _probe.getHiTime(event);
};

//-------------------------------------
void	__fastcall Analizator::comparator_ON(){

	_comparator_on	= true;
};

//-------------------------------------
void	__fastcall Analizator::comparator_OFF(){

	_comparator_on	= false;
};

//-------------------------------------
bool	__fastcall Analizator::used_comparator(){

	return _comparator_on;
};

//-------------------------------------
void	__fastcall Analizator::state(bool& relay, u16& dac_a, u16& dac_b){

	_probe.getStatus(relay, dac_a, dac_b);
};

//-------------------------------------
u8		__fastcall Analizator::calculate(bool low, bool high){

	if(low && high)	return 1;
	else if(high)	return 4;
	else if(low)	return 2;
	else			return 0;
};

//-------------------------------------
void	__fastcall Analizator::initialize_maps(){

	if(FileExists(DEFAULT_CALIBRATE_TABLE))	load_map_from_file(DEFAULT_CALIBRATE_TABLE);
	else{
		inner_map_initialize();
		save_map_to_file(DEFAULT_CALIBRATE_TABLE);
    }
};

//-------------------------------------
void	__fastcall Analizator::inner_map_initialize(){

/*	u32 count_12V	= 23;
	u32 count_27V	= 33;

	low.clear();
	hi.clear();

	for(int i = 0; i < count_12V; i++){
		pair<i32, i32>				codes(LOW1[i], LOW2[i]);
		pair<i32, pair<i32, i32> >	item(VAL1[i], codes);
		low.insert(item);
	}

	for(int i = 0; i < count_27V; i++){
		pair<i32, i32>				codes(HI1[i], HI2[i]);
		pair<i32, pair<i32, i32> >	item(VAL2[i], codes);
		hi.insert(item);
	}       */
	v12.clear();
	v27.clear();

	pair<i32, i32>				codes_0(0, 0);
	pair<i32, i32>				codes_1(4096, 4096);
	pair<i32, pair<i32, i32> >	item_0(V12_MIN, codes_0);
	pair<i32, pair<i32, i32> >	item_1(V12_MAX, codes_1);
	pair<i32, pair<i32, i32> >	item_2(V27_MIN, codes_0);
	pair<i32, pair<i32, i32> >	item_3(V27_MAX, codes_1);

	v12.insert(item_0);
	v12.insert(item_1);

	v27.insert(item_2);
	v27.insert(item_3);

};

//-------------------------------------
void	__fastcall Analizator::save_map_to_file(String filename){

	HList list(true);
	std::map<i32, pair<i32, i32> >::iterator it;

	list->Add("//12V");
	for(it = low.begin(); it != low.end(); it++){
		String volt		= IntToStr(it->first);
		String code1	= IntToStr(it->second.first);
		String code2	= IntToStr(it->second.second);
		String  str	= volt + "\t" + code1 + "\t" + code2;
		list->Add(str);
	}
	list->Add("//27V");
	for(it = hi.begin(); it != hi.end(); it++){
		String volt		= IntToStr(it->first);
		String code1	= IntToStr(it->second.first);
		String code2	= IntToStr(it->second.second);
		String  str	= volt + "\t" + code1 + "\t" + code2;
		list->Add(str);
	}

	list->SaveToFile(filename);
};

//-------------------------------------
void	__fastcall Analizator::load_map_from_file(String filename){

	if(!FileExists(filename))	return;
	HList list(true);

	list->LoadFromFile(filename);
	std::map<i32, pair<i32, i32> >*		pointer	= NULL;
	try{
		for(int i = 0; i < list->Count; i++){
			if(list->Strings[i] == "//12V")			pointer = &low;
			else if(list->Strings[i] == "//27V")	pointer	= &hi;
			else{
				HList tri	= wrap(list->Strings[i]);
				pair<i32, i32>				codes(StrToInt(tri->Strings[1]), StrToInt(tri->Strings[2]));
				pair<i32, pair<i32, i32> >	item(StrToInt(tri->Strings[0]), codes);
				pointer->insert(item);
			}
		}
	}catch(...){
		low.clear();
		hi.clear();
	};


};

//-------------------------------------
HList	__fastcall Analizator::wrap(String rules){

	std::set<wchar_t>			separators;
	std::set<wchar_t>::iterator	it;
	separators.insert(L'\t');
	separators.insert(L' ');

	String buffer = EmptyStr;
	HList list(true);

	for(int  i = 1; i <= rules.Length(); i++){
		it = separators.find(rules[i]);
		if(it == separators.end()){
			buffer += rules[i];
		}else{
			if(buffer != EmptyStr){
				list->Add(Trim(buffer));
				buffer	= EmptyStr;
			}
        }
	}
	if(buffer != EmptyStr)		list->Add(Trim(buffer));

	return list;
};

//-------------------------------------
i32		__fastcall Analizator::volt_to_code(u8 ch, i32 volt, bool relay){


/*	std::map<i32, pair<i32, i32> >*		pointer	= (volt < 9000)? &low: &hi;

	if(volt > 27000)		return 3780;

	std::map<i32, pair<i32, i32> >::iterator	it1	= pointer->upper_bound(volt);
	std::map<i32, pair<i32, i32> >::iterator	it2	= it1--;

	if(it1 == pointer->end() || it2 == pointer->end()) return 3780;
	i32 code[2] = {ch? it1->second.second: it1->second.first,ch? it2->second.second: it2->second.first};
	r64 k = 1.0*(code[1] - code[0])/(it2->first - it1->first);
	i32 c =	code[0] - k*it1->first;
	i32 d = 1.0*volt*k + c;

	return d;      */
	i32 d = -1;
	if(!relay){
		r64 k = 1.0*(CODE_MAX - CODE_MIN)/(V27_MAX - V27_MIN);
		i32 c =	CODE_MIN - k*V27_MIN;
		d = 1.0*volt*k + c;
	}else{
		r64 k = 1.0*(CODE_MAX - CODE_MIN)/(V12_MAX - V12_MIN);
		i32 c =	CODE_MIN - k*V12_MIN;
		d = 1.0*volt*k + c;
	}

	return d;
};

//-------------------------------------
i32		__fastcall Analizator::code_to_volt(u8 ch, bool relay ,u32 code){

	if(ch == 0){

	}else{

	}
};

//-------------------------------------
void	__fastcall	Analizator::save_port_info(){

	HList	list(true);
	list->Add(Address);
	list->SaveToFile(DEFAULT_PORT);
};

//-------------------------------------
void	__fastcall	Analizator::load_port_info(){

	if(FileExists(DEFAULT_PORT)){
		HList list(true);
		list->LoadFromFile(DEFAULT_PORT);
		AnsiString address	= list->Strings[0];
		connect(address.c_str());
	}
};

//-------------------------------------
u8		__fastcall 	Analizator::get_ols(u8 index){

	return index > 1? 0: _ols[index];
};

//-------------------------------------
u8		__fastcall 	Analizator::get_contact(u8 index){

	return index > 1? 0: _contact[index];
};

//-------------------------------------
i32		__fastcall  Analizator::get_level(u8 index){

	return index > 1? 0: _level[index];
};

//-------------------------------------
bool	__fastcall  Analizator::get_relay(){

	return _relay;
};

//-------------------------------------
void	__fastcall	Analizator::set_ols(u8 index, u8 ols){

	if(index < 2)	_ols[index] = ols;
};

//-------------------------------------
void	__fastcall	Analizator::set_contact(u8 index, u8 contact){

	if(index < 2)	_contact[index] = contact;
};

//-------------------------------------
bool	__fastcall	Analizator::set_level(u8 index, i32 level){

//	try{
		if(index > 1)	return false;

		_level[index] = level;

		switch(index){
			case 0:	return _probe.setDAC_A(volt_to_code(index, level));
			case 1:	return _probe.setDAC_B(volt_to_code(index, level));
		}
//	}__finally{
//		message("Probe.Config");
//	}
};

//-------------------------------------
bool	__fastcall	Analizator::set_code(u8 index, i32 code){

//	try{
		if(index > 1)	return false;
		if(code < CODE_MIN || code > CODE_MAX)	return false;

		switch(index){
			case 0:	return _probe.setDAC_A(code);
			case 1:	return _probe.setDAC_B(code);
		}
//	}__finally{
//		message("Probe.Config");
//	}
};

//-------------------------------------
bool	__fastcall	Analizator::set_relay(bool enable){

//	try{
		_relay = enable;
		return	_probe.setRelay(_relay);
//	}__finally{
//		message("Probe.Config");
//	}

};

//-------------------------------------
HList			Analizator::get_COM_ports(){

	String KeyName	= "\\Hardware\\DeviceMap\\SerialComm";

	HList keys(true);
	HList values(true);
	HPtr<TRegistry>	registry(true);

	try{
		registry->RootKey	= HKEY_LOCAL_MACHINE;
		registry->OpenKeyReadOnly(KeyName);
		registry->GetValueNames(keys);

		for (int i = 0; i < keys->Count; i++) {
			values->Add(registry->ReadString(keys->Strings[i]));
		}

	}catch(...){
		console<<"Не удалось получить список СОМ портов."<<endl;
	}
	return values;

};

//-------------------------------------
bool Analizator::setTableCount(u8 mode, i16 min, i16 max, u16 count){

	_probe.setTableCount(mode,min,max,count);
};

//-------------------------------------
bool Analizator::setTablePacket(u8 mode, u16 offset, u16 count, u16 code[]){

	_probe.setTablePacket(mode,offset,count,code);
};

//-------------------------------------
std::string		Analizator::get_first(HList com_ports){

	for(int  i = 0; i < com_ports->Count; i++){
		std::string address	= AnsiString(com_ports->Strings[i]).c_str();
		if(_probe.open(address, 9600)){

			std::string	id;
			if(_probe.getDeviceID(id) && id == DEFAULT_ID){
				_probe.reset();
				return address;
			}
		}
	}
	return "";
};

//-------------------------------------
void	__fastcall	Analizator::message(String event,String data){

	if(!_lock)	(*Signals)[event]->Execute(data);
};

//-------------------------------------
void	__fastcall	Analizator::lock(){

	_lock	= true;
};

//-------------------------------------
void	__fastcall 	Analizator::unlock(){

	_lock	= false;
};
