#include "pch.h"
#include "Room.h"
#include "Player.h"
#include "GameSession.h"

void Room::Enter(PlayerRef player)
{
	_players[player->_playerId] = player;
}

void Room::Leave(PlayerRef player)
{
	_players.erase(player->_playerId);
}

void Room::Broadcast(SendBufferRef sendBuffer)
{
	for (auto& p : _players)
	{
		p.second->_ownerSession->Send(sendBuffer);
	}
}
