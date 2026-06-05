// Fill out your copyright notice in the Description page of Project Settings.


#include "TCPClientSubsystem.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"
#include "Interfaces/IPv4/IPv4Address.h"

#include "UserPacket_generated.h"

void UTCPClientSubsystem::Deinitialize()
{
	Disconnect();

	Super::Deinitialize();
}

bool UTCPClientSubsystem::Connect(const FString& Host, int32 Port)
{
	//WSStartup
	//OS별 소켓 생성 시스템
	ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	//socket
	//SOCKADDR

	//hostent
	//getaddrinfo()
	FAddressInfoResult AddrInfo = SocketSubSystem->GetAddressInfo(*Host, nullptr, EAddressInfoFlags::Default, NAME_None);

	//SOCKADDR(IP)
	TSharedRef<FInternetAddr> ServerAddr = AddrInfo.Results[0].Address;

	//Port
	ServerAddr->SetPort(Port);

	//socket()
	ServerSocket = SocketSubSystem->CreateSocket(NAME_Stream, TEXT("TCPClient"), ServerAddr->GetProtocolType());

	//connect()
	if (!ServerSocket->Connect(*ServerAddr))
	{
		UE_LOG(LogTemp, Warning, TEXT("Connect Failed."));
		return false;
	}

	ServerSocket->SetNonBlocking(false);

	OnTCPConnected.Broadcast();

	return true;
}


void UTCPClientSubsystem::Disconnect()
{
	ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	//closesocket
	ServerSocket->Close();
	SocketSubSystem->DestroySocket(ServerSocket);

	ServerSocket = nullptr;


	OnTCPDisconnected.Broadcast();
}

bool UTCPClientSubsystem::IsConncted() const
{
	return ServerSocket != nullptr && ServerSocket->GetConnectionState() == SCS_Connected;
}


void UTCPClientSubsystem::SendLogin(const FString& UserID, const FString& Password)
{
	flatbuffers::FlatBufferBuilder Builder;

	const FTCHARToUTF8 UserUTF8(UserID);
	const FTCHARToUTF8 PasswordUTF8(Password);

	auto LoginData = UserPacket::CreateC2S_LoginDirect(
		Builder,
		UserUTF8.Get(),
		PasswordUTF8.Get()
	);

	auto PacketData = UserPacket::CreatePacketData(
		Builder,
		UserPacket::PacketType_C2S_Login,
		LoginData.Union()
	);

	Builder.Finish(PacketData);

	SendAll(Builder.GetBufferPointer(), Builder.GetSize());
}

void UTCPClientSubsystem::SendSignUp(const FString& UserID, const FString& Password)
{
}

void UTCPClientSubsystem::RecvAll()
{
	if (!IsConncted())
	{
		return;
	}

	uint32 Pending = 0;
	if (!ServerSocket->HasPendingData(Pending) || Pending <= 0)
	{
		return;
	}

	//1. 2byte 헤더 받기
	uint16 NetPacketSize = 0; //BigEndian
	uint16 PacketSize = 0; //LitteleEndian
	int32 TotalRecvBytes = 0;
	int32 RecvBytes = 0;
	//2바이트 받았냐?
	while (TotalRecvBytes < (int32)sizeof(NetPacketSize))
	{
		if (!ServerSocket->Recv((uint8*)&NetPacketSize + TotalRecvBytes, sizeof(NetPacketSize) - TotalRecvBytes, RecvBytes) || RecvBytes == 0)
		{
			Disconnect();
			break;
		}
		TotalRecvBytes += RecvBytes;
	}
	PacketSize = NETWORK_ORDER16(NetPacketSize);


	//Body
	RecvBuffer.SetNumUninitialized(PacketSize);
	TotalRecvBytes = 0;
	RecvBytes = 0;
	while (TotalRecvBytes < (int32)(PacketSize))
	{
		if (!ServerSocket->Recv(RecvBuffer.GetData() + TotalRecvBytes, PacketSize - TotalRecvBytes, RecvBytes) || RecvBytes == 0)
		{
			Disconnect();
			break;
		}
		TotalRecvBytes += RecvBytes;
	}

	DispatchPacket();
	RecvBuffer.Reset();

}

bool UTCPClientSubsystem::SendAll(const uint8* Body, uint32 BodyLength)
{
	//char Buffer[2 + BodyLength];
	TArray<uint8> Packet;

	Packet.Reserve(2 + BodyLength);
	Packet.Add((uint8)((BodyLength >> 8) & 0xFF));
	Packet.Add((uint8)((BodyLength) & 0xFF));
	Packet.Append(Body, BodyLength);

	int32 SentTotalBytes = 0;
	while (SentTotalBytes < Packet.Num())
	{
		int32 SentBytes = 0;
		if (!ServerSocket->Send(Packet.GetData() + SentTotalBytes, Packet.Num() - SentTotalBytes, SentBytes) || SentBytes < 0)
		{
			return false;
		}
		SentTotalBytes += SentBytes;
	}

	return true;
}

void UTCPClientSubsystem::DispatchPacket()
{
	//flatbuffer, -> Extract

	const auto UserPacketData = UserPacket::GetPacketData(RecvBuffer.GetData());
	switch (UserPacketData->data_type())
	{
	case UserPacket::PacketType_S2C_Login:
	{
		//Delegate로 바꿈
		const auto* LoginData = UserPacketData->data_as_S2C_Login();
		
		FString Message = UTF8_TO_TCHAR(LoginData->message()->c_str());

		UE_LOG(LogTemp, Warning, TEXT("Login %d %s"), LoginData->client_socket_id(),  *Message);
	}
	break;
	case UserPacket::PacketType_S2C_Spawn:
	{
	}
	break;
	case UserPacket::PacketType_S2C_Move:
	{

	}
	break;
	case UserPacket::PacketType_S2C_Destroy:
	{
	}
	break;

	case UserPacket::PacketType_S2C_ChangeColor:
	{
	}
	break;

	case UserPacket::PacketType_S2C_Signup:
	{
		auto SignupPacket = UserPacketData->data_as_S2C_Signup();

		{

		}
	}
	break;
	}
}

TStatId UTCPClientSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UTCPClientSubsystem, STATGROUP_Tickables);
}

void UTCPClientSubsystem::Tick(float DeltaTime)
{
	RecvAll();
}
