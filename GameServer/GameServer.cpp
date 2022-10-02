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

	{
		auto query = L"									\
			DROP TABLE IF EXISTS [dbo].[Gold];			\
			CREATE TABLE [dbo].[Gold]					\
			(											\
				[id] INT NOT NULL PRIMARY KEY IDENTITY,	\
				[gold] INT NULL,						\
				[name] NVARCHAR(50) NULL,				\
				[createDate] DATETIME NULL				\
			);";

		DBConnection* dbConn = GDBConnectionPool->Pop();
		ASSERT_CRASH(dbConn->Execute(query));
		GDBConnectionPool->Push(dbConn);
	}

	for (int32 i = 0; i < 3; ++i)
	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<3, 0> dbBind(*dbConn, L"INSERT INTO [dbo].[Gold]([gold], [name], [createDate]) VALUES(?, ?, ?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);

		WCHAR name[100] = L"상훈";
		dbBind.BindParam(1, name);

		TIMESTAMP_STRUCT ts = {2022, 10, 2};
		dbBind.BindParam(2, ts);

		dbBind.Execute();

		GDBConnectionPool->Push(dbConn);
	}

	{
		DBConnection* dbConn = GDBConnectionPool->Pop();

		DBBind<1, 4> dbBind(*dbConn, L"SELECT id, gold, name, createDate FROM [dbo].[Gold] WHERE gold = (?)");

		int32 gold = 100;
		dbBind.BindParam(0, gold);

		int32 outId = 0;
		dbBind.BindCol(0, outId);

		int32 outGold = 0;
		dbBind.BindCol(1, outGold);

		WCHAR outName[100];
		dbBind.BindCol(2, outName);

		TIMESTAMP_STRUCT outDate {};
		dbBind.BindCol(3, outDate);

		dbBind.Execute();

		wcout.imbue(locale("kor"));
		while (dbConn->Fetch())
		{
			wcout << "Id: " << outId << " Gold : " << outGold;
			wcout << " Name : " << outName << " CreateDate : " << outDate.year << "/" << outDate.month << "/" << outDate.day << endl;
		}
		GDBConnectionPool->Push(dbConn);
	}

	ClientPacketHandler::Init();
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IocpCore>(),
		MakeShared<GameSession>,
		100);
	
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