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

int main()
{
	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([]()
			{
				Vector<int32> v(10);
				Map<int32, int32> m;
				this_thread::sleep_for(10ms);
			});
	}
	GThreadManager->Join();
}