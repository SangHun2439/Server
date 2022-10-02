#pragma once
#include "JobQueue.h"

class Room : public JobQueue
{
	friend class EnterJob;
	friend class LeaveJob;
	friend class BroadCastJob;

public:
	void	Enter(PlayerRef player);
	void	Leave(PlayerRef player);
	void	Broadcast(SendBufferRef sendBuffer);

private:
	Map<uint64, PlayerRef>	_players;
};
