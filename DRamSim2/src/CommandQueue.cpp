#include "CommandQueue.h"
#include "MemoryController.h"
#include <assert.h>

using namespace DRAMSim;

CommandQueue::CommandQueue(vector<vector<BankState>>& states, ostream& dramsim_log_)
	: _dramsimLog(dramsim_log_)
	, _bankStates(states)
	, _nextBank(0)
	, _nextRank(0)
	, _nextBankPre(0)
	, _nextRankPre(0)
	, _refreshRank(0)
	, _refreshWaiting(false)
	, _sendAct(true)
{
	//set here to avoid compile errors
	currentClockCycle = 0;

	//use numBankQueus below to create queue structure
	size_t numBankQueues;
	if (queuingStructure == PerRank)
	{
		numBankQueues = 1;
	}
	else if (queuingStructure == PerRankPerBank)
	{
		numBankQueues = NUM_BANKS;
	}
	else
	{
		ERROR("== Error - Unknown queuing structure");
		exit(0);
	}

	//vector of counters used to ensure rows don't stay open too long
	_rowAccessCounters = vector<vector<unsigned>>(NUM_RANKS, vector<unsigned>(NUM_BANKS, 0));

	//create queue based on the structure we want
	BusPacket1D actualQueue;
	BusPacket2D perBankQueue = BusPacket2D();
	_queues = BusPacket3D();
	for (size_t rank = 0; rank < NUM_RANKS; rank++)
	{
		//this loop will run only once for per-rank and NUM_BANKS times for per-rank-per-bank
		for (size_t bank = 0; bank < numBankQueues; bank++)
		{
			actualQueue = BusPacket1D();
			perBankQueue.push_back(actualQueue);
		}
		_queues.push_back(perBankQueue);
	}


	//FOUR-bank activation window
	//	this will count the number of activations within a given window
	//	(decrementing counter)
	//
	//countdown vector will have decrementing counters starting at tFAW
	//  when the 0th element reaches 0, remove it
	_tFawCountdown.reserve(NUM_RANKS);
	for (size_t i = 0; i < NUM_RANKS; i++)
	{
		//init the empty vectors here so we don't seg fault later
		_tFawCountdown.push_back(vector<unsigned>());
	}
}

CommandQueue::~CommandQueue()
{
	//ERROR("COMMAND QUEUE destructor");
	size_t bankMax = NUM_RANKS;
	if (queuingStructure == PerRank)
	{
		bankMax = 1;
	}
	for (size_t r = 0; r < NUM_RANKS; r++)
	{
		for (size_t b = 0; b < bankMax; b++)
		{
			for (size_t i = 0; i < _queues[r][b].size(); i++)
			{
				delete(_queues[r][b][i]);
			}
			_queues[r][b].clear();
		}
	}
}
//Adds a command to appropriate queue
void CommandQueue::enqueue(BusPacket* newBusPacket)
{
	unsigned rank = newBusPacket->rank;
	unsigned bank = newBusPacket->bank;
	if (queuingStructure == PerRank)
	{
		_queues[rank][0].push_back(newBusPacket);
		if (_queues[rank][0].size() > CMD_QUEUE_DEPTH)
		{
			ERROR("== Error - Enqueued more than allowed in command queue");
			ERROR("						Need to call .hasRoomFor(int numberToEnqueue, unsigned rank, unsigned bank) first");
			exit(0);
		}
	}
	else if (queuingStructure == PerRankPerBank)
	{
		_queues[rank][bank].push_back(newBusPacket);
		if (_queues[rank][bank].size() > CMD_QUEUE_DEPTH)
		{
			ERROR("== Error - Enqueued more than allowed in command queue");
			ERROR("						Need to call .hasRoomFor(int numberToEnqueue, unsigned rank, unsigned bank) first");
			exit(0);
		}
	}
	else
	{
		ERROR("== Error - Unknown queuing structure");
		exit(0);
	}
}

//Removes the next item from the command queue based on the system's
//command scheduling policy
bool CommandQueue::pop(BusPacket** busPacket)
{
	//this can be done here because pop() is called every clock cycle by the parent MemoryController
	//	figures out the sliding window requirement for tFAW
	//
	//deal with tFAW book-keeping
	//	each rank has it's own counter since the restriction is on a device level
	for (size_t i = 0; i < NUM_RANKS; i++)
	{
		//decrement all the counters we have going
		for (size_t j = 0; j < _tFawCountdown[i].size(); j++)
		{
			_tFawCountdown[i][j]--;
		}

		//the head will always be the smallest counter, so check if it has reached 0
		if (_tFawCountdown[i].size() > 0 && _tFawCountdown[i][0] == 0)
		{
			_tFawCountdown[i].erase(_tFawCountdown[i].begin());
		}
	}

	/* Now we need to find a packet to issue. When the code picks a packet, it will set
		 *busPacket = [some eligible packet]
		 
		 First the code looks if any refreshes need to go
		 Then it looks for data packets
		 Otherwise, it starts looking for rows to close (in open page)
	*/

	if (rowBufferPolicy == ClosePage)
	{
		bool sendingREF = false;
		//if the memory controller set the flags signaling that we need to issue a refresh
		if (_refreshWaiting)
		{
			bool foundActiveOrTooEarly = false;
			//look for an open bank
			for (size_t b = 0; b < NUM_BANKS; b++)
			{
				vector<BusPacket *>& queue = getCommandQueue(_refreshRank, b);
				//checks to make sure that all banks are idle
				if (_bankStates[_refreshRank][b].currentBankState == RowActive)
				{
					foundActiveOrTooEarly = true;
					//if the bank is open, make sure there is nothing else
					// going there before we close it
					for (size_t j = 0; j < queue.size(); j++)
					{
						BusPacket* packet = queue[j];
						if (packet->row == _bankStates[_refreshRank][b].openRowAddress &&
							packet->bank == b)
						{
							if (packet->busPacketType != ACTIVATE && isIssuable(packet))
							{
								*busPacket = packet;
								queue.erase(queue.begin() + j);
								sendingREF = true;
							}
							break;
						}
					}

					break;
				}
				//	NOTE: checks nextActivate time for each bank to make sure tRP is being
				//				satisfied.	the next ACT and next REF can be issued at the same
				//				point in the future, so just use nextActivate field instead of
				//				creating a nextRefresh field
				if (_bankStates[_refreshRank][b].nextActivate > currentClockCycle)
				{
					foundActiveOrTooEarly = true;
					break;
				}
			}

			//if there are no open banks and timing has been met, send out the refresh
			//	reset flags and rank pointer
			if (!foundActiveOrTooEarly && _bankStates[_refreshRank][0].currentBankState != PowerDown)
			{
				*busPacket = new BusPacket(REFRESH, 0, 0, 0, _refreshRank, 0, nullptr, _dramsimLog);
				_refreshRank = -1;
				_refreshWaiting = false;
				sendingREF = true;
			}
		} // refreshWaiting

		//if we're not sending a REF, proceed as normal
		if (!sendingREF)
		{
			bool foundIssuable = false;
			unsigned startingRank = _nextRank;
			unsigned startingBank = _nextBank;
			do
			{
				vector<BusPacket *>& queue = getCommandQueue(_nextRank, _nextBank);
				//make sure there is something in this queue first
				//	also make sure a rank isn't waiting for a refresh
				//	if a rank is waiting for a refesh, don't issue anything to it until the
				//		refresh logic above has sent one out (ie, letting banks close)
				if (!queue.empty() && !((_nextRank == _refreshRank) && _refreshWaiting))
				{
					if (queuingStructure == PerRank)
					{

						//search from beginning to find first issuable bus packet
						for (size_t i = 0; i < queue.size(); i++)
						{
							if (isIssuable(queue[i]))
							{
								//check to make sure we aren't removing a read/write that is paired with an activate
								if (i > 0 && queue[i - 1]->busPacketType == ACTIVATE &&
									queue[i - 1]->physicalAddress == queue[i]->physicalAddress)
									continue;

								*busPacket = queue[i];
								queue.erase(queue.begin() + i);
								foundIssuable = true;
								break;
							}
						}
					}
					else
					{
						if (isIssuable(queue[0]))
						{

							//no need to search because if the front can't be sent,
							// then no chance something behind it can go instead
							*busPacket = queue[0];
							queue.erase(queue.begin());
							foundIssuable = true;
						}
					}
				}

				//if we found something, break out of do-while
				if (foundIssuable)
					break;

				//rank round robin
				if (queuingStructure == PerRank)
				{
					_nextRank = (_nextRank + 1) % NUM_RANKS;
					if (startingRank == _nextRank)
					{
						break;
					}
				}
				else
				{
					nextRankAndBank(_nextRank, _nextBank);
					if (startingRank == _nextRank && startingBank == _nextBank)
					{
						break;
					}
				}
			}
			while (true);

			//if we couldn't find anything to send, return false
			if (!foundIssuable)
				return false;
		}
	}
	else if (rowBufferPolicy == OpenPage)
	{
		bool sendingREForPRE = false;
		if (_refreshWaiting)
		{
			bool sendREF = true;
			//make sure all banks idle and timing met for a REF
			for (size_t b = 0; b < NUM_BANKS; b++)
			{
				//if a bank is active we can't send a REF yet
				if (_bankStates[_refreshRank][b].currentBankState == RowActive)
				{
					sendREF = false;
					bool closeRow = true;
					//search for commands going to an open row
					vector<BusPacket *>& refreshQueue = getCommandQueue(_refreshRank, b);

					for (size_t j = 0; j < refreshQueue.size(); j++)
					{
						BusPacket* packet = refreshQueue[j];
						//if a command in the queue is going to the same row . . .
						if (_bankStates[_refreshRank][b].openRowAddress == packet->row &&
							b == packet->bank)
						{
							// . . . and is not an activate . . .
							if (packet->busPacketType != ACTIVATE)
							{
								closeRow = false;
								// . . . and can be issued . . .
								if (isIssuable(packet))
								{
									//send it out
									*busPacket = packet;
									refreshQueue.erase(refreshQueue.begin() + j);
									sendingREForPRE = true;
								}
								break;
							}
							//command is an activate
							//if we've encountered another act, no other command will be of interest
							break;
						}
					}

					//if the bank is open and we are allowed to close it, then send a PRE
					if (closeRow && currentClockCycle >= _bankStates[_refreshRank][b].nextPrecharge)
					{
						_rowAccessCounters[_refreshRank][b] = 0;
						*busPacket = new BusPacket(PRECHARGE, 0, 0, 0, _refreshRank, b, nullptr, _dramsimLog);
						sendingREForPRE = true;
					}
					break;
				}
				//	NOTE: the next ACT and next REF can be issued at the same
				//				point in the future, so just use nextActivate field instead of
				//				creating a nextRefresh field
				if (_bankStates[_refreshRank][b].nextActivate > currentClockCycle) //and this bank doesn't have an open row
				{
					sendREF = false;
					break;
				}
			}

			//if there are no open banks and timing has been met, send out the refresh
			//	reset flags and rank pointer
			if (sendREF && _bankStates[_refreshRank][0].currentBankState != PowerDown)
			{
				*busPacket = new BusPacket(REFRESH, 0, 0, 0, _refreshRank, 0, nullptr, _dramsimLog);
				_refreshRank = -1;
				_refreshWaiting = false;
				sendingREForPRE = true;
			}
		}

		if (!sendingREForPRE)
		{
			unsigned startingRank = _nextRank;
			unsigned startingBank = _nextBank;
			bool foundIssuable = false;
			do // round robin over queues
			{
				vector<BusPacket *>& queue = getCommandQueue(_nextRank, _nextBank);
				//make sure there is something there first
				if (!queue.empty() && !((_nextRank == _refreshRank) && _refreshWaiting))
				{
					//search from the beginning to find first issuable bus packet
					for (size_t i = 0; i < queue.size(); i++)
					{
						BusPacket* packet = queue[i];
						if (isIssuable(packet))
						{
							//check for dependencies
							bool dependencyFound = false;
							for (size_t j = 0; j < i; j++)
							{
								BusPacket* prevPacket = queue[j];
								if (prevPacket->busPacketType != ACTIVATE &&
									prevPacket->bank == packet->bank &&
									prevPacket->row == packet->row)
								{
									dependencyFound = true;
									break;
								}
							}
							if (dependencyFound)
								continue;

							*busPacket = packet;

							//if the bus packet before is an activate, that is the act that was
							//	paired with the column access we are removing, so we have to remove
							//	that activate as well (check i>0 because if i==0 then theres nothing before it)
							if (i > 0 && queue[i - 1]->busPacketType == ACTIVATE)
							{
								_rowAccessCounters[(*busPacket)->rank][(*busPacket)->bank]++;
								// i is being returned, but i-1 is being thrown away, so must delete it here 
								delete (queue[i - 1]);

								// remove both i-1 (the activate) and i (we've saved the pointer in *busPacket)
								queue.erase(queue.begin() + i - 1, queue.begin() + i + 1);
							}
							else // there's no activate before this packet
							{
								//or just remove the one bus packet
								queue.erase(queue.begin() + i);
							}

							foundIssuable = true;
							break;
						}
					}
				}

				//if we found something, break out of do-while
				if (foundIssuable)
					break;

				//rank round robin
				if (queuingStructure == PerRank)
				{
					_nextRank = (_nextRank + 1) % NUM_RANKS;
					if (startingRank == _nextRank)
					{
						break;
					}
				}
				else
				{
					nextRankAndBank(_nextRank, _nextBank);
					if (startingRank == _nextRank && startingBank == _nextBank)
					{
						break;
					}
				}
			}
			while (true);

			//if nothing was issuable, see if we can issue a PRE to an open bank
			//	that has no other commands waiting
			if (!foundIssuable)
			{
				//search for banks to close
				bool sendingPRE = false;
				unsigned startingRank = _nextRankPre;
				unsigned startingBank = _nextBankPre;

				do // round robin over all ranks and banks
				{
					vector<BusPacket *>& queue = getCommandQueue(_nextRankPre, _nextBankPre);
					bool found = false;
					//check if bank is open
					if (_bankStates[_nextRankPre][_nextBankPre].currentBankState == RowActive)
					{
						for (size_t i = 0; i < queue.size(); i++)
						{
							//if there is something going to that bank and row, then we don't want to send a PRE
							if (queue[i]->bank == _nextBankPre &&
								queue[i]->row == _bankStates[_nextRankPre][_nextBankPre].openRowAddress)
							{
								found = true;
								break;
							}
						}

						//if nothing found going to that bank and row or too many accesses have happend, close it
						if (!found || _rowAccessCounters[_nextRankPre][_nextBankPre] == TOTAL_ROW_ACCESSES)
						{
							if (currentClockCycle >= _bankStates[_nextRankPre][_nextBankPre].nextPrecharge)
							{
								sendingPRE = true;
								_rowAccessCounters[_nextRankPre][_nextBankPre] = 0;
								*busPacket = new BusPacket(PRECHARGE, 0, 0, 0, _nextRankPre, _nextBankPre, nullptr, _dramsimLog);
								break;
							}
						}
					}
					nextRankAndBank(_nextRankPre, _nextBankPre);
				}
				while (!(startingRank == _nextRankPre && startingBank == _nextBankPre));

				//if no PREs could be sent, just return false
				if (!sendingPRE)
					return false;
			}
		}
	}

	//sendAct is flag used for posted-cas
	//  posted-cas is enabled when AL>0
	//  when sendAct is true, when don't want to increment our indexes
	//  so we send the column access that is paid with this act
	if (AL > 0 && _sendAct)
	{
		_sendAct = false;
	}
	else
	{
		_sendAct = true;
		nextRankAndBank(_nextRank, _nextBank);
	}

	//if its an activate, add a tfaw counter
	if ((*busPacket)->busPacketType == ACTIVATE)
	{
		_tFawCountdown[(*busPacket)->rank].push_back(tFAW);
	}

	return true;
}

//check if a rank/bank queue has room for a certain number of bus packets
bool CommandQueue::hasRoomFor(unsigned numberToEnqueue, unsigned rank, unsigned bank)
{
	vector<BusPacket *>& queue = getCommandQueue(rank, bank);
	return (CMD_QUEUE_DEPTH - queue.size() >= numberToEnqueue);
}

//prints the contents of the command queue
void CommandQueue::print()
{
	if (queuingStructure == PerRank)
	{
		PRINT(endl << "== Printing Per Rank Queue");
		for (size_t i = 0; i < NUM_RANKS; i++)
		{
			PRINT(" = Rank " << i << "  size : " << _queues[i][0].size());
			for (size_t j = 0; j < _queues[i][0].size(); j++)
			{
				PRINTN("    "<< j << "]");
				_queues[i][0][j]->print();
			}
		}
	}
	else if (queuingStructure == PerRankPerBank)
	{
		PRINT("\n== Printing Per Rank, Per Bank Queue");

		for (size_t i = 0; i < NUM_RANKS; i++)
		{
			PRINT(" = Rank " << i);
			for (size_t j = 0; j < NUM_BANKS; j++)
			{
				PRINT("    Bank "<< j << "   size : " << _queues[i][j].size());

				for (size_t k = 0; k < _queues[i][j].size(); k++)
				{
					PRINTN("       " << k << "]");
					_queues[i][j][k]->print();
				}
			}
		}
	}
}

/** 
 * return a reference to the queue for a given rank, bank. Since we
 * don't always have a per bank queuing structure, sometimes the bank
 * argument is ignored (and the 0th index is returned 
 */
vector<BusPacket *>& CommandQueue::getCommandQueue(unsigned rank, unsigned bank)
{
	if (queuingStructure == PerRankPerBank)
	{
		return _queues[rank][bank];
	}
	if (queuingStructure == PerRank)
	{
		return _queues[rank][0];
	}
	ERROR("Unknown queue structure");
	abort();
}

//checks if busPacket is allowed to be issued
bool CommandQueue::isIssuable(BusPacket* busPacket)
{
	switch (busPacket->busPacketType)
	{
		case REFRESH: break;
		case ACTIVATE:
		{
			if ((_bankStates[busPacket->rank][busPacket->bank].currentBankState == Idle ||
					_bankStates[busPacket->rank][busPacket->bank].currentBankState == Refreshing) &&
				currentClockCycle >= _bankStates[busPacket->rank][busPacket->bank].nextActivate &&
				_tFawCountdown[busPacket->rank].size() < 4)
			{
				return true;
			}
			return false;
		}
		break;
		case WRITE:
		case WRITE_P:
		{
			if (_bankStates[busPacket->rank][busPacket->bank].currentBankState == RowActive &&
				currentClockCycle >= _bankStates[busPacket->rank][busPacket->bank].nextWrite &&
				busPacket->row == _bankStates[busPacket->rank][busPacket->bank].openRowAddress &&
				_rowAccessCounters[busPacket->rank][busPacket->bank] < TOTAL_ROW_ACCESSES)
			{
				return true;
			}
			return false;
		}
			break;
		case READ_P:
		case READ:
		{
			if (_bankStates[busPacket->rank][busPacket->bank].currentBankState == RowActive &&
				currentClockCycle >= _bankStates[busPacket->rank][busPacket->bank].nextRead &&
				busPacket->row == _bankStates[busPacket->rank][busPacket->bank].openRowAddress &&
				_rowAccessCounters[busPacket->rank][busPacket->bank] < TOTAL_ROW_ACCESSES)
			{
				return true;
			}
			return false;
		}
			break;
		case PRECHARGE:
		{
			if (_bankStates[busPacket->rank][busPacket->bank].currentBankState == RowActive &&
				currentClockCycle >= _bankStates[busPacket->rank][busPacket->bank].nextPrecharge)
			{
				return true;
			}
			return false;
		}
		break;
		default:
			ERROR("== Error - Trying to issue a crazy bus packet type : ");
			busPacket->print();
			exit(0);
	}
	return false;
}

//figures out if a rank's queue is empty
bool CommandQueue::isEmpty(unsigned rank)
{
	if (queuingStructure == PerRank)
	{
		return _queues[rank][0].empty();
	}
	if (queuingStructure == PerRankPerBank)
	{
		for (size_t i = 0; i < NUM_BANKS; i++)
		{
			if (!_queues[rank][i].empty())
				return false;
		}
		return true;
	}
	DEBUG("Invalid Queueing Stucture");
	abort();
}

//tells the command queue that a particular rank is in need of a refresh
void CommandQueue::needRefresh(unsigned rank)
{
	_refreshWaiting = true;
	_refreshRank = rank;
}

void CommandQueue::nextRankAndBank(unsigned& rank, unsigned& bank)
{
	if (schedulingPolicy == RankThenBankRoundRobin)
	{
		rank++;
		if (rank == NUM_RANKS)
		{
			rank = 0;
			bank++;
			if (bank == NUM_BANKS)
			{
				bank = 0;
			}
		}
	}
		//bank-then-rank round robin
	else if (schedulingPolicy == BankThenRankRoundRobin)
	{
		bank++;
		if (bank == NUM_BANKS)
		{
			bank = 0;
			rank++;
			if (rank == NUM_RANKS)
			{
				rank = 0;
			}
		}
	}
	else
	{
		ERROR("== Error - Unknown scheduling policy");
		exit(0);
	}
}

void CommandQueue::update()
{
	//do nothing since pop() is effectively update(),
	//needed for SimulatorObject
	//TODO: make CommandQueue not a SimulatorObject
}
