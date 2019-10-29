#pragma once

#include "SystemConfiguration.h"
#include "BusPacket.h"

namespace DRAMSim
{
	enum CurrentBankState
	{
		Idle,
		RowActive,
		Precharging,
		Refreshing,
		PowerDown
	};

	class BankState
	{
		ostream& _dramsimLog;
	public:
		//Fields
		CurrentBankState currentBankState;
		unsigned openRowAddress;
		uint64_t nextRead;
		uint64_t nextWrite;
		uint64_t nextActivate;
		uint64_t nextPrecharge;
		uint64_t nextPowerUp;

		BusPacketType lastCommand;
		unsigned stateChangeCountdown;

		//Functions
		BankState(ostream& dramsim_log_)
			: _dramsimLog(dramsim_log_)
			, currentBankState(Idle)
			, openRowAddress(0)
			, nextRead(0)
			, nextWrite(0)
			, nextActivate(0)
			, nextPrecharge(0)
			, nextPowerUp(0)
			, lastCommand(READ)
			, stateChangeCountdown(0)
		{
		}

		void print()
		{
			PRINT(" == Bank State ");
			if (currentBankState == Idle)
			{
				PRINT("    State : Idle");
			}
			else if (currentBankState == RowActive)
			{
				PRINT("    State : Active");
			}
			else if (currentBankState == Refreshing)
			{
				PRINT("    State : Refreshing");
			}
			else if (currentBankState == PowerDown)
			{
				PRINT("    State : Power Down");
			}

			PRINT("    OpenRowAddress : " << openRowAddress);
			PRINT("    nextRead       : " << nextRead);
			PRINT("    nextWrite      : " << nextWrite);
			PRINT("    nextActivate   : " << nextActivate);
			PRINT("    nextPrecharge  : " << nextPrecharge);
			PRINT("    nextPowerUp    : " << nextPowerUp);
		}
	};
}
