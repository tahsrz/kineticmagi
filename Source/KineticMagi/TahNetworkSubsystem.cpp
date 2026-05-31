#include "TahNetworkSubsystem.h"

#include "Async/Async.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "SocketSubsystem.h"
#include "Sockets.h"

void UTahNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	bIsListening = false;
	ConnectionSocket = nullptr;
}

void UTahNetworkSubsystem::Deinitialize()
{
	Disconnect();
	Super::Deinitialize();
}

void UTahNetworkSubsystem::ConnectToServer(const FString& IpAddress, int32 Port)
{
	if (bIsListening)
	{
		return;
	}

	FIPv4Address Address;
	if (!FIPv4Address::Parse(IpAddress, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("TahNetworkSubsystem: Invalid IPv4 address '%s'."), *IpAddress);
		return;
	}

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!SocketSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("TahNetworkSubsystem: Socket subsystem unavailable."));
		return;
	}

	TSharedRef<FInternetAddr> Addr = SocketSubsystem->CreateInternetAddr();
	Addr->SetIp(Address.Value);
	Addr->SetPort(Port);

	ConnectionSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("TahNetworkSocket"), false);
	if (!ConnectionSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("TahNetworkSubsystem: Failed to create socket."));
		return;
	}

	if (!ConnectionSocket->Connect(*Addr))
	{
		UE_LOG(LogTemp, Error, TEXT("TahNetworkSubsystem: Failed to connect to %s:%d."), *IpAddress, Port);
		SocketSubsystem->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
		return;
	}

	bIsListening = true;
	ListenerTask = Async(EAsyncExecution::Thread, [this]()
	{
		TArray<uint8> ReceiveBuffer;
		ReceiveBuffer.SetNumUninitialized(65536);

		while (bIsListening)
		{
			uint32 PendingBytes = 0;
			if (ConnectionSocket && ConnectionSocket->HasPendingData(PendingBytes))
			{
				int32 BytesRead = 0;
				if (ConnectionSocket->Recv(ReceiveBuffer.GetData(), ReceiveBuffer.Num(), BytesRead) && BytesRead > 0)
				{
					TArray<uint8> DataCopy;
					DataCopy.SetNumUninitialized(BytesRead);
					FMemory::Memcpy(DataCopy.GetData(), ReceiveBuffer.GetData(), BytesRead);
					HandleReceivedRawBytes(DataCopy);
				}
			}

			FPlatformProcess::Sleep(0.01f);
		}
	});
}

void UTahNetworkSubsystem::Disconnect()
{
	bIsListening = false;

	if (ListenerTask.IsValid())
	{
		ListenerTask.Wait();
	}

	if (ConnectionSocket)
	{
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
	}
}

void UTahNetworkSubsystem::HandleReceivedRawBytes(const TArray<uint8>& RawBuffer)
{
	FSocialInteractionPayload ParsedPayload;
	if (!TahBinary::TryParseSocialPayload(RawBuffer, ParsedPayload))
	{
		return;
	}

	AsyncTask(ENamedThreads::GameThread, [this, ParsedPayload]()
	{
		if (IsValid(this))
		{
			OnSocialPayloadReceived.Broadcast(ParsedPayload);
		}
	});
}
