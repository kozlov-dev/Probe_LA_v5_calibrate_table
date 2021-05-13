//---------------------------------------------------------------------------
#pragma hdrstop

#include "Probe.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

// Поток опроса состояния кнопок
DWORD ProbeSleepTime	= 200;

DWORD WINAPI ProbeGetButtonState(void *param) {
	Probe *ptr	= (Probe*)param;

	while (true) {
		ptr->updateBtnState();
		Sleep(ProbeSleepTime);
	}
}
//---------------------------------------------------------------------------

Probe::Probe() {
	threadGetButtonState	= NULL;
	mutexGetButtonState     = NULL;
	reset();
}

Probe::~Probe() {
	stopAutoButtonStateCheck();
}

void Probe::reset() {
    bufLen	= 0;
	memset(buf, 0x00, SERIAL_BUF_SIZE);

	stopAutoButtonStateCheck();
	callbackBtnState	= NULL;
	serial.reset();


}

//---------------------------------------------------------------------------

bool Probe::open(const std::string &name, DWORD baudRate) {
	return serial.open(name, baudRate);
}

bool Probe::isOpened() const {
	return serial.isOpened();
}

//---------------------------------------------------------------------------

bool Probe::getStatus(bool &Relay, uint16_t &DAC_A, uint16_t &DAC_B) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	uint16_t val;

	buf[0]	= 0x05;
	bufLen	= 1;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	if (bufLen >= 6 && buf[0] == 0x05) {
		Relay	= buf[1] == 0x01;

		memcpy(&val, buf + 2, sizeof(uint16_t));
		DAC_A	= val;

		memcpy(&val, buf + 2 + sizeof(uint16_t), sizeof(uint16_t));
		DAC_B	= val;

		return true;
	}

	return false;
}

bool Probe::setRelay(bool Relay) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x01;
	buf[1]	= Relay ? 0x01 : 0x00;
	bufLen	= 2;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	return (bufLen >= 2 && buf[0] == 0x01 && buf[1] == 0x00);
}

bool Probe::setDAC_A(uint16_t DAC_A) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x02;

	if (DAC_A > 0x0FFF)	DAC_A	= 0x0FFF;
	memcpy(buf + 1, &DAC_A, sizeof(uint16_t));

	bufLen	= 1 + sizeof(uint16_t);

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	return (bufLen >= 2 && buf[0] == 0x02 && buf[1] == 0x00);
}

bool Probe::setDAC_B(uint16_t DAC_B) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x03;

	if (DAC_B > 0x0FFF)	DAC_B	= 0x0FFF;
	memcpy(buf + 1, &DAC_B, sizeof(uint16_t));

	bufLen	= 1 + sizeof(uint16_t);

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	return (bufLen >= 2 && buf[0] == 0x03 && buf[1] == 0x00);
}

//-------------------------------------------
bool Probe::setTableCount(u8 mode, i16 min, i16 max, u16 count){

	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x0C;

//	if (DAC_B > 0x0FFF)	DAC_B	= 0x0FFF;
	memcpy(buf + 1, &mode, sizeof(u8));
	memcpy(buf + 2, &min, sizeof(i16));
	memcpy(buf + 4, &max, sizeof(i16));
	memcpy(buf + 6, &count, sizeof(u16));

	bufLen	= 8;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	return (bufLen >= 2 && buf[0] == 0x0C && buf[1] == 0x00);

};

//-------------------------------------------
bool Probe::setTablePacket(u8 mode, u16 offset, u16 count, u16 code[]){

	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x0A;

//	if (DAC_B > 0x0FFF)	DAC_B	= 0x0FFF;
	memcpy(buf + 1, &mode, 		sizeof(u8));
	memcpy(buf + 2, &offset, 	sizeof(u16));
	memcpy(buf + 4, &count, 	sizeof(u16));
	memcpy(buf + 6, code, 		count*sizeof(u16));

	bufLen	= 6 + count*sizeof(u16);
//
	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	u8 	command			= 0x00;
	u8 	rcv_mode		= 0x00;
	u16 rcv_offset 	= 0x00;
	u16 rcv_count  	= 0x00;
	u8  rcv_state	= 0x01;

	if(bufLen < 7)	return false;

	memcpy(&command, 	buf , 	 sizeof(u8));
	memcpy(&rcv_mode,   buf + 1, sizeof(u8));
	memcpy(&rcv_offset, buf + 2, sizeof(u16));
	memcpy(&rcv_count,  buf + 4, sizeof(u16));
	memcpy(&rcv_state,  buf + 6, sizeof(u8));

	return (command == 0x0A && mode == rcv_mode && offset == rcv_offset && count == rcv_count && rcv_state == 0x00);
};


bool Probe::getADC(uint16_t &ADC) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x04;
	bufLen	= 1;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	if (bufLen >= 3 && buf[0] == 0x04) {
		memcpy(&ADC, buf + 1, sizeof(uint16_t));
		return true;
	}

	return false;
}




bool Probe::getDeviceID(std::string &ID) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x07;
	bufLen	= 1;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	if (bufLen >= 2 && buf[0] == 0x07) {
		int idLen	= buf[1];
		char id[256];

		id[0]	= 0x00;
		if (idLen + 2 <= bufLen) {
			memcpy(id, buf + 2, idLen);
			id[idLen]   = 0x00;
		}

        ID	= id;
		return true;
	}

	return false;
}

//----------------------------------------------
bool Probe::getHiTime(uint16_t& time){

	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x08;
	bufLen	= 1;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}
	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	if (bufLen >= 3 && buf[0] == 0x08) {
		time    = buf[2];
		time = time<<8;
		time|= buf[1];

		return true;
	}

	return false;
};

//-----------------------------------------------
bool Probe::getLowTime(uint16_t& time){

	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x09;
	bufLen	= 1;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}
	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	if (bufLen >= 3 && buf[0] == 0x09) {
		time    = buf[2];
		time = time<<8;
		time|= buf[1];

		return true;
	}

	return false;
};
//-----------------------------------------------

bool Probe::getButtonState(uint8_t &Run, uint8_t &Up, uint8_t &Down) {
	if (mutexGetButtonState)	WaitForSingleObject(mutexGetButtonState, INFINITE);

	buf[0]	= 0x06;
	bufLen	= 1;

	if (!serial.sendAndReceive(buf, bufLen, true)) {
		if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);
		return false;
	}

	if (mutexGetButtonState)	ReleaseMutex(mutexGetButtonState);

	if (bufLen >= 4 && buf[0] == 0x06) {
		Run		= buf[1];
		Up		= buf[2];
		Down	= buf[3];
		return true;
	}

    return false;
}

//---------------------------------------------------------------------------

std::string Probe::getLastError() const {
	return serial.getLastError();
}

//---------------------------------------------------------------------------

void Probe::runAutoButtonStateCheck(uint16_t askPeriod, void (*callbackBState)(uint8_t Run, uint8_t Up, uint8_t Down)) {
	if (threadGetButtonState)   return;

	if (askPeriod >= 10)    ProbeSleepTime	= askPeriod;
	callbackBtnState	= callbackBState;

	mutexGetButtonState		= CreateMutex(NULL, FALSE, L"ProbeGetButtonState");
	threadGetButtonState	= CreateThread(NULL, 0, ProbeGetButtonState, (LPVOID)this, 0, NULL);
}

void Probe::stopAutoButtonStateCheck() {
	if (!threadGetButtonState)  return;

	TerminateThread(threadGetButtonState, 0);
	CloseHandle(threadGetButtonState);
	threadGetButtonState	= NULL;

	CloseHandle(mutexGetButtonState);
    mutexGetButtonState	= NULL;

	callbackBtnState	= NULL;
}

void Probe::updateBtnState() {
	uint8_t Run, Up, Down;

	if (!getButtonState(Run, Up, Down))  return;
	if (callbackBtnState)   callbackBtnState(Run, Up, Down);
}

