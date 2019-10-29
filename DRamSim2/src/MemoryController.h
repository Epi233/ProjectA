#pragma once


//MemoryController.h
//
//Header file for memory controller object
//

#include "SimulatorObject.h"
#include "Transaction.h"
#include "SystemConfiguration.h"
#include "CommandQueue.h"
#include "BusPacket.h"
#include "BankState.hpp"
#include "Rank.h"
#include "CSVWriter.h"
#include <map>

using namespace std;

namespace DRAMSim
{
	class MemorySystem;

	class MemoryController : public SimulatorObject
	{
	public:
		//functions
		MemoryController(MemorySystem* ms, CSVWriter& csvOut_, ostream& dramsim_log_);
		virtual ~MemoryController();

		bool addTransaction(Transaction* trans);
		bool WillAcceptTransaction();
		void returnReadData(const Transaction* trans);
		void receiveFromBus(BusPacket* bpacket);
		void attachRanks(vector<Rank *>* ranks);
		void update() override;
		void printStats(bool finalStats = false);
		void resetStats(); 


		//fields
		vector<Transaction *> transactionQueue;
	private:
		ostream& dramsim_log;
		vector<vector<BankState>> bankStates;
		//functions
		void insertHistogram(unsigned latencyValue, unsigned rank, unsigned bank);

		//fields
		MemorySystem* parentMemorySystem;

		CommandQueue commandQueue;
		BusPacket* poppedBusPacket;
		vector<unsigned> refreshCountdown;
		vector<BusPacket *> writeDataToSend;
		vector<unsigned> writeDataCountdown;
		vector<Transaction *> returnTransaction;
		vector<Transaction *> pendingReadTransactions;
		map<unsigned, unsigned> latencies; // latencyValue -> latencyCount
		vector<bool> powerDown;

		vector<Rank *>* ranks;

		//output file
		CSVWriter& csvOut; 

		// these packets are counting down waiting to be transmitted on the "bus"
		BusPacket* outgoingCmdPacket;
		unsigned cmdCyclesLeft;
		BusPacket* outgoingDataPacket;
		unsigned dataCyclesLeft;

		uint64_t totalTransactions;
		vector<uint64_t> grandTotalBankAccesses;
		vector<uint64_t> totalReadsPerBank;
		vector<uint64_t> totalWritesPerBank;

		vector<uint64_t> totalReadsPerRank;
		vector<uint64_t> totalWritesPerRank;


		vector<uint64_t> totalEpochLatency;

		unsigned channelBitWidth;
		unsigned rankBitWidth;
		unsigned bankBitWidth;
		unsigned rowBitWidth;
		unsigned colBitWidth;
		unsigned byteOffsetWidth;


		unsigned refreshRank;

	public:
		// energy values are per rank -- SST uses these directly, so make these public 
		vector<uint64_t> backgroundEnergy;
		vector<uint64_t> burstEnergy;
		vector<uint64_t> actpreEnergy;
		vector<uint64_t> refreshEnergy;
	};
}
