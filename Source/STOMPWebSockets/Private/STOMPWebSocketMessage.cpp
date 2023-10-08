// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.

#include "STOMPWebSocketMessage.h"
#include "IStompMessage.h"

void USTOMPWebSocketMessage::Ack(const TMap<FName, FString>& Header, const FSTOMPRequestCompleted& CompletionCallback)
{
	
	MyMessage->Ack(Header, FStompRequestCompleted::CreateLambda([CompletionCallback](bool bSuccess, const FString& Error)->void {
		CompletionCallback.Execute(bSuccess, Error);
	}));
}

void USTOMPWebSocketMessage::Nack(const TMap<FName, FString>& Header, const FSTOMPRequestCompleted& CompletionCallback)
{
	MyMessage->Nack(Header, FStompRequestCompleted::CreateLambda([CompletionCallback](bool bSuccess, const FString& Error)->void {
		CompletionCallback.Execute(bSuccess, Error);
	}));
}


const TMap<FName, FString>& USTOMPWebSocketMessage::GetHeader() const
{
	return MyMessage->GetHeader();
}

FString USTOMPWebSocketMessage::GetBodyAsString() const
{
	return MyMessage->GetBodyAsString();
}

const TArray<uint8> USTOMPWebSocketMessage::GetRawBody() const
{
	return TArray<uint8>(MyMessage->GetRawBody(), MyMessage->GetRawBodyLength());
}

int32 USTOMPWebSocketMessage::GetRawBodyLength() const
{
	return MyMessage->GetRawBodyLength();
}


FString USTOMPWebSocketMessage::GetSubscriptionId() const
{
	return MyMessage->GetSubscriptionId();
}

FString USTOMPWebSocketMessage::GetDestination() const
{
	return MyMessage->GetDestination();
}

FString USTOMPWebSocketMessage::GetMessageId() const
{
	return MyMessage->GetMessageId();
}

FString USTOMPWebSocketMessage::GetAckId() const
{
	return MyMessage->GetAckId();
}