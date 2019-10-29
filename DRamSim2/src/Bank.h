#pragma once

#include "SystemConfiguration.h"
#include "SimulatorObject.h"
#include "BankState.h"
#include "BusPacket.h"
#include <iostream>

namespace DRAMSim
{
class Bank
{
	struct DataStruct
	{
		unsigned row;
		void *data;
		struct DataStruct *next;
	};

public:
	//functions
	Bank(ostream &dramsim_log_);
	void read(BusPacket *busPacket);
	void write(const BusPacket *busPacket);

	//fields
	BankState currentState;

private:
	// private member
	std::vector<DataStruct *> _rowEntries;
	ostream &_dramsimLog; 

	static DataStruct *searchForRow(unsigned row, DataStruct *head);
};
}


