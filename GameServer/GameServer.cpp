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

#include "SocketUtils.h"
#include "Listener.h"
#include "IocpCore.h"

int main()
{
	Listener listener;
	listener.StartAccept(NetAddress(L"127.0.0.1", 7777));

	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					GIocpCore.Dispatch();
				}
			});
	}
	GThreadManager->Join();
}