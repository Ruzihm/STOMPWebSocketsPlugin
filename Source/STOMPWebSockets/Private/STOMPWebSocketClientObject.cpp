// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.


#include "STOMPWebSocketClientObject.h"
#include "WebSocketsModule.h"
#include "STOMPWebSocketMessage.h"
#include "Stomp/Public/IStompMessage.h"
#include "Stomp/Public/StompModule.h"
#include "Stomp/Public/IStompClient.h"

// Called when the game starts
void USTOMPWebSocketClientObject::Initialize()
{
	FStompModule* stompModule = &FStompModule::Get();
	StompClient = stompModule->CreateClient(Url, AuthToken);

	StompClient->OnConnected().AddUObject(this, &USTOMPWebSocketClientObject::HandleOnConnected);
	StompClient->OnConnectionError().AddUObject(this, &USTOMPWebSocketClientObject::HandleOnConnectionError);
	StompClient->OnError().AddUObject(this, &USTOMPWebSocketClientObject::HandleOnError);
	StompClient->OnClosed().AddUObject(this, &USTOMPWebSocketClientObject::HandleOnClosed);
}

void USTOMPWebSocketClientObject::SetUrl(FString NewUrl)
{
	Url = NewUrl;
}


FString USTOMPWebSocketClientObject::GetUrl()
{
	return Url;
}

void USTOMPWebSocketClientObject::SetAuthToken(FString NewAuthToken)
{
	AuthToken = NewAuthToken;
}

FString USTOMPWebSocketClientObject::GetAuthToken()
{
	return AuthToken;
}

/**
	 * Initiate a client connection to the server.
	 * Use this after setting up event handlers or to reconnect after connection errors.
	 *
	 * @param Header custom headers to send with the initial CONNECT command.
	 */
void USTOMPWebSocketClientObject::Connect(const TMap<FName, FString>& Header)
{
	StompClient->Connect(Header);
}

/**
 * Disconnect from the server.
 *
 * @param Header custom headers to send with the DISCONNECT command.
 */
void USTOMPWebSocketClientObject::Disconnect(const TMap<FName, FString>& Header)
{
	StompClient->Disconnect(Header);
}

/**
 * Inquire if this instance is connected to a server.
 */
bool USTOMPWebSocketClientObject::IsConnected()
{
	return StompClient->IsConnected();
}

/**
 * Subscribe to an event
 * @param Destination Destination endpoint to subscribe to.
 * @param EventCallback Delegate called when events arrive on this subscription.
 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
 * @return a handle to the active subscription. Can be passed to Unsubscribe to unsubscribe from the end point.
 */
FString USTOMPWebSocketClientObject::Subscribe(const FString& Destination, const FSTOMPSubscriptionEventObject& EventCallback, const FSTOMPRequestCompleted& CompletionCallback)
{
	return StompClient->Subscribe(Destination, 
		FStompSubscriptionEvent::CreateLambda([this, EventCallback](const IStompMessage& Message)->void {
			USTOMPWebSocketMessage* msg = NewObject<USTOMPWebSocketMessage>(this);
			msg->MyMessage = &Message;
			EventCallback.ExecuteIfBound(msg);
			msg->ConditionalBeginDestroy();
		}),
		FStompRequestCompleted::CreateLambda([CompletionCallback](bool bSuccess, const FString& Error)->void {
			CompletionCallback.ExecuteIfBound(bSuccess, Error);
		})
	);
}


/**
 * Unsubscribe from an event
 * @param Subscription The id returned from the call to Subscribe.
 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
 * @param ResponseCallback Delegate called when the request has been completed.
 */
void USTOMPWebSocketClientObject::Unsubscribe(FString Subscription, const FSTOMPRequestCompletedObject& CompletionCallback)
{
	StompClient->Unsubscribe(Subscription,
		FStompRequestCompleted::CreateLambda([CompletionCallback](bool bSuccess, const FString& Error)->void {
			CompletionCallback.ExecuteIfBound(bSuccess, Error);
		})
	);
}

/**
 * Emit an event to a destination
 * @param Destination The destination endoint of the event.
 * @param Body The event body as a binary blob.
 * @param Header Custom header values to send along with the data.
 * @param CompletionCallback Delegate called when the request has been acknowledged by the server or if there is an error.
 */
void USTOMPWebSocketClientObject::SendBinary(const FString& Destination, const TArray<uint8>& Body, const TMap<FName, FString>& Header,
	const FSTOMPRequestCompletedObject& CompletionCallback)
{
	StompClient->Send(Destination, Body, Header,
		FStompRequestCompleted::CreateLambda([CompletionCallback](bool bSuccess, const FString& Error)->void {
			CompletionCallback.ExecuteIfBound(bSuccess, Error);
		})
	);
}

void USTOMPWebSocketClientObject::HandleOnConnected(const FString& ProtocolVersion, const FString& SessionId, const FString& ServerString)
{
	this->OnConnected.Broadcast(ProtocolVersion, SessionId, ServerString);
}

void USTOMPWebSocketClientObject::HandleOnConnectionError(const FString& Error)
{
	this->OnConnectionError.Broadcast(Error);
}
void USTOMPWebSocketClientObject::HandleOnError(const FString& Error)
{
	this->OnError.Broadcast(Error);
}
void USTOMPWebSocketClientObject::HandleOnClosed(const FString& Reason)
{
	this->OnClosed.Broadcast(Reason);
}