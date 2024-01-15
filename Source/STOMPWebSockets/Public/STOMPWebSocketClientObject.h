// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "STOMPWebSocketClientObject.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FSTOMPRequestCompletedObject, bool, bSuccess, const FString&, Error);
DECLARE_DYNAMIC_DELEGATE_OneParam(FSTOMPSubscriptionEventObject, class USTOMPWebSocketMessage*, Message);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSTOMPConnectedEventObject, const FString&, ProtocolVersion, const FString&, SessionId, const FString&, ServerString);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSTOMPConnectionErrorEventObject, const FString&, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSTOMPErrorEventObject, const FString&, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSTOMPClosedEventObject, const FString&, Reason);


UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(DisplayName = "STOMP Web Socket Client Object"))
class STOMPWEBSOCKETS_API USTOMPWebSocketClientObject : public UObject
{
	GENERATED_BODY()

protected:
	TSharedPtr<class IStompClient> StompClient;


private:
	//Wrappers for client events
	void HandleOnConnected(const FString& ProtocolVersion, const FString& SessionId, const FString& ServerString);
	void HandleOnConnectionError(const FString& Error);
	void HandleOnError(const FString& Error);
	void HandleOnClosed(const FString& Reason);
	UPROPERTY(BlueprintSetter = SetUrl, BlueprintGetter = GetUrl, Category = "Online|STOMP over Websockets")
	FString Url;
	UPROPERTY(BlueprintSetter = SetAuthToken, BlueprintGetter = GetAuthToken, Category = "Online|STOMP over Websockets")
	FString AuthToken;

public:	
	UFUNCTION(BlueprintCallable, BlueprintSetter, Category = "Online|STOMP over Websockets")
	void SetUrl(FString NewUrl);

	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Online|STOMP over Websockets")
	FString GetUrl();

	UFUNCTION(BlueprintCallable, BlueprintSetter, Category = "Online|STOMP over Websockets")
	void SetAuthToken(FString NewAuthToken);

	UFUNCTION(BlueprintCallable, BlueprintGetter, Category = "Online|STOMP over Websockets")
	FString GetAuthToken();

	/**
	 * Initialize STOMP controller
	 */
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets")
	void Initialize();

	/**
	 * Initiate a client connection to the server.
	 * Use this after setting up event handlers or to reconnect after connection errors.
	 *
	 * @param Header custom headers to send with the initial CONNECT command.
	 */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Header"), Category = "Online|STOMP over Websockets")
	void Connect(const TMap<FName, FString>& Header);

	/**
	 * Disconnect from the server.
	 *
	 * @param Header custom headers to send with the DISCONNECT command.
	 */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Header"), Category = "Online|STOMP over Websockets")
	void Disconnect(const TMap<FName, FString>& Header);

	/**
	 * Inquire if this instance is connected to a server.
	 */
	UFUNCTION(BlueprintCallable, Category = "Online|STOMP over Websockets")
	bool IsConnected();

	/**
	 * Subscribe to an event
	 * @param Destination Destination endpoint to subscribe to.
	 * @param EventCallback Delegate called when events arrive on this subscription.
	 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
	 * @return a handle to the active subscription. Can be passed to Unsubscribe to unsubscribe from the end point.
	 */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "CompletionCallback"), Category = "Online|STOMP over Websockets")
	FString Subscribe(const FString& Destination, const FSTOMPSubscriptionEventObject& EventCallback, const FSTOMPRequestCompleted& CompletionCallback);

	/**
	 * Unsubscribe from an event
	 * @param Subscription The id returned from the call to Subscribe.
	 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
	 * @param ResponseCallback Delegate called when the request has been completed.
	 */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "CompletionCallback"), Category = "Online|STOMP over Websockets")
	void Unsubscribe(FString Subscription, const FSTOMPRequestCompletedObject& CompletionCallback);

	/**
	 * Emit an event to a destination
	 * @param Destination The destination endoint of the event.
	 * @param Body The event body as string. It will be encoded as UTF8 before sending to the Stomp server.
	 * @param Header Custom header values to send along with the data.
	 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
	 */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Header,CompletionCallback"), Category = "Online|STOMP over Websockets")
	void SendString(const FString& Destination, const FString& Body, const TMap<FName, FString>& Header, const FSTOMPRequestCompletedObject& CompletionCallback)
	{
		FTCHARToUTF8 Convert(*Body);
		TArray<uint8> Encoded;
		Encoded.Append((uint8*)Convert.Get(), Convert.Length());
		SendBinary(Destination, Encoded, Header, CompletionCallback);
	}

	/**
	 * Emit an event to a destination
	 * @param Destination The destination endoint of the event.
	 * @param Body The event body as a binary blob.
	 * @param Header Custom header values to send along with the data.
	 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
	 */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Header,CompletionCallback"), Category = "Online|STOMP over Websockets")
	void SendBinary(const FString& Destination, const TArray<uint8>& Body, const TMap<FName, FString>& Header, const FSTOMPRequestCompletedObject& CompletionCallback);

	/**
	 * Delegate called when a connection been established successfully.
	 * @param ProtocoVersion The protocol version supported by the server
	 * @param SessionId A unique connection identifier. Can be empty depending on the server implementation.
	 * @param ServerString A server version string if returned from the server, otherwise empty string.
	 *
	 */
	UPROPERTY(BlueprintAssignable)
	FSTOMPConnectedEventObject OnConnected;


	/**
	 * Delegate called when a connection could not be established.
	 *
	 */
	UPROPERTY(BlueprintAssignable)
	FSTOMPConnectionErrorEventObject OnConnectionError;

	/**
	 * Delegate called when an error is received from the server.
	 *
	 */
	UPROPERTY(BlueprintAssignable)
	FSTOMPErrorEventObject OnError;

	/**
	 * Delegate called when a connection has been closed.
	 *
	 */
	UPROPERTY(BlueprintAssignable)
	FSTOMPClosedEventObject OnClosed;
};
