#pragma once

#include "BusPacket.h"
#include "BankState.hpp"
#include "Transaction.h"
#include "SystemConfiguration.h"
#include "SimulatorObject.h"

using namespace std;

namespace DRAMSim
{
	class CommandQueue : public SimulatorObject
	{
		CommandQueue();
		ostream& _dramsimLog;
	public:
		//typedefs
		typedef vector<BusPacket *> BusPacket1D;
		typedef vector<BusPacket1D> BusPacket2D;
		typedef vector<BusPacket2D> BusPacket3D;

		//functions
		CommandQueue(vector<vector<BankState>>& states, ostream& dramsim_log);
		virtual ~CommandQueue();

		void enqueue(BusPacket* newBusPacket);
		bool pop(BusPacket** busPacket);
		bool hasRoomFor(unsigned numberToEnqueue, unsigned rank, unsigned bank);
		bool isIssuable(BusPacket* busPacket);
		bool isEmpty(unsigned rank);
		void needRefresh(unsigned rank);
		void print();
		void update() override; //SimulatorObject requirement
		vector<BusPacket *>& getCommandQueue(unsigned rank, unsigned bank);

		//fields

		BusPacket3D _queues; // 3D array of BusPacket pointers
		vector<vector<BankState>>& _bankStates;
	private:
		void nextRankAndBank(unsigned& rank, unsigned& bank);
		//fields
		unsigned _nextBank;
		unsigned _nextRank;

		unsigned _nextBankPre;
		unsigned _nextRankPre;

		unsigned _refreshRank;
		bool _refreshWaiting;

		vector<vector<unsigned>> _tFawCountdown;
		vector<vector<unsigned>> _rowAccessCounters;

		bool _sendAct;
	};
}
