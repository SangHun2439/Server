#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "Memory.h"
#include "SocketUtils.h"

ThreadManager* GThreadManager = nullptr;
Memory* GMemory = nullptr;
DeadLockProfiler* GDeadLockProfiler = nullptr;

class CoreGlobal
{
public:
	CoreGlobal()
	{
		GThreadManager = new ThreadManager();
		GMemory = new Memory();
		GDeadLockProfiler = new DeadLockProfiler();
		SocketUtils::init();
	}
	~CoreGlobal()
	{
		delete GThreadManager;
		delete GMemory;
		delete GDeadLockProfiler;
		SocketUtils::Clear();
	}
}	GCoreGlobal;