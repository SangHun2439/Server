#pragma once

class Player
{
public:
	uint64					_playerId = 0;
	string					_name;
	Protocol::PlayerType	_type = Protocol::PLAYER_TYPE_NONE;
	GameSessionRef			_ownerSession; // Ref Cycle
};

