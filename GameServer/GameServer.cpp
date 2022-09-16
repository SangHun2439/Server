#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include "CoreMacro.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "RefCounting.h"
#include "Memory.h"
#include "Allocator.h"

#include <thread>
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>

using TL = TypeList<class Player, class Knight, class Mage>;

class Player
{
public:
	Player() { INIT_TL(Player) }
	virtual ~Player() {}
	DECLARE_TL
};

class Knight : public Player
{
public:
	Knight() { INIT_TL(Knight) }

};

class Mage : public Player
{
public:
	Mage() { INIT_TL(Mage) }
};

int main()
{
	{
		Player* player = new Player();
		bool canCast = CanCast<Knight*>(player);
		Knight* knight = TypeCast<Knight*>(player);
		delete player;
	}

	{
		shared_ptr<Player> player = MakeShared<Player>();

		bool canCast = CanCast<Knight>(player);
		shared_ptr<Knight> knight = TypeCast<Knight>(player);
	}
	
}