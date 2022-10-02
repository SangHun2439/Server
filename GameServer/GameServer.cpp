#include "pch.h"
#include "ServerGlobal.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include "ClientPacketHandler.h"
#include "Protocol.pb.h"
#include "Room.h"
#include "DBConnectionPool.h"
#include "DBBind.h"
#include "XMLParser.h"
#include "DBSynchronizer.h"

enum
{
	WORKER_TICK = 64,
};

void	DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->Dispatch(10);

		ThreadManager::DistributeReservedJobs();

		ThreadManager::DoGlobalQueueWork();
	}
}

char sendData[1000] = "Hello World";

int main()
{
	ServerGlobal::Init();
	
	ASSERT_CRASH(GDBConnectionPool->Connect(1, L"Driver={ODBC Driver 17 for SQL Server};Server=(localdb)\\ProjectModels;Database=ServerDB;Trusted_Connection=Yes;"));

	DBConnection* dbConn = GDBConnectionPool->Pop();
	DBSynchronizer dbSync(*dbConn);
	dbSync.Synchronize(L"GameDB.xml");

	ClientPacketHandler::Init();
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		5);
	
	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; ++i)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					DoWorkerJob(service);
				}
			});
	}

	DoWorkerJob(service);
	
	GThreadManager->Join();
}