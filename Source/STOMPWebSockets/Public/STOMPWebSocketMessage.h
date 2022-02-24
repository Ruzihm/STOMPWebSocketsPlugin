// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STOMPWebSocketClient.h"
#include "STOMPWebSocketClientObject.h"
#include "STOMPWebSocketMessage.generated.h"

class IStompMessage;


UCLASS(meta = (DisplayName="STOMP Web Socket Message"))
class USTOMPWebSocketMessage : public UObject
{
	GENERATED_BODY()

	friend USTOMPWebSocketClient;
	friend USTOMPWebSocketClientObject;

private:
	const IStompMessage* MyMessage;

public:
	virtual ~USTOMPWebSocketMessage()
	{ }

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Header,CompletionCallback"), Category = "Online|STOMP over Websockets|Messages")
	void Ack(const TMap<FName, FString>& Header, const FSTOMPRequestCompleted& CompletionCallback);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Header,CompletionCallback"), Category = "Online|STOMP over Websockets|Messages")
	void Nack(const TMap<FName, FString>& Header, const FSTOMPRequestCompleted& CompletionCallback);

	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		const TMap<FName, FString>& GetHeader() const;
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		FString GetBodyAsString() const;
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		const TArray<uint8> GetRawBody() const;
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		int32 GetRawBodyLength() const;

	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		FString GetSubscriptionId() const;
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		FString GetDestination() const;
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		FString GetMessageId() const;
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets|Messages")
		FString GetAckId() const;
};
