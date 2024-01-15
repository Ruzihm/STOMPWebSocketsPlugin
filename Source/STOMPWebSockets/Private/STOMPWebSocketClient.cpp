// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.


#include "STOMPWebSocketClient.h"
#include "WebSocketsModule.h"
#include "STOMPWebSocketMessage.h"
#include "Stomp/Public/IStompMessage.h"
#include "Stomp/Public/StompModule.h"
#include "Stomp/Public/IStompClient.h"

// Sets default values for this component's properties
USTOMPWebSocketClient::USTOMPWebSocketClient()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}

// Called when the game starts
void USTOMPWebSocketClient::BeginPlay()
{
	Super::BeginPlay();

	FStompModule* stompModule = &FStompModule::Get();
	StompClient = stompModule->CreateClient(Url, AuthToken);

	StompClient->OnConnected().AddUObject(this, &USTOMPWebSocketClient::HandleOnConnected);
	StompClient->OnConnectionError().AddUObject(this, &USTOMPWebSocketClient::HandleOnConnectionError);
	StompClient->OnError().AddUObject(this, &USTOMPWebSocketClient::HandleOnError);
	StompClient->OnClosed().AddUObject(this, &USTOMPWebSocketClient::HandleOnClosed);
}

void USTOMPWebSocketClient::SetUrl(FString NewUrl)
{
	Url = NewUrl;
}

FString USTOMPWebSocketClient::GetUrl()
{
	return Url;
}

void USTOMPWebSocketClient::SetAuthToken(FString NewAuthToken)
{
	AuthToken = NewAuthToken;
}

FString USTOMPWebSocketClient::GetAuthToken()
{
	return AuthToken;
}

// Called every frame
void USTOMPWebSocketClient::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

/**
* Initiate a client connection to the server.
* Use this after setting up event handlers or to reconnect after connection errors.
*
* @param Header custom headers to send with the initial CONNECT command.
*/
void USTOMPWebSocketClient::Connect(const TMap<FName, FString>& Header)
{
	StompClient->Connect(Header);
}

/**
 * Disconnect from the server.
 *
 * @param Header custom headers to send with the DISCONNECT command.
 */
void USTOMPWebSocketClient::Disconnect(const TMap<FName, FString>& Header)
{
	StompClient->Disconnect(Header);
}

/**
 * Inquire if this instance is connected to a server.
 */
bool USTOMPWebSocketClient::IsConnected()
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
FString USTOMPWebSocketClient::Subscribe(const FString& Destination, const FSTOMPSubscriptionEvent& EventCallback, const FSTOMPRequestCompleted& CompletionCallback)
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
void USTOMPWebSocketClient::Unsubscribe(FString Subscription, const FSTOMPRequestCompleted& CompletionCallback)
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
void USTOMPWebSocketClient::SendBinary(const FString& Destination, const TArray<uint8>& Body, const TMap<FName, FString>& Header, 
	const FSTOMPRequestCompleted& CompletionCallback)
{
	StompClient->Send(Destination, Body, Header,
		FStompRequestCompleted::CreateLambda([CompletionCallback](bool bSuccess, const FString& Error)->void {
			CompletionCallback.ExecuteIfBound(bSuccess, Error);
		})
	);
}

void USTOMPWebSocketClient::HandleOnConnected(const FString& ProtocolVersion, const FString& SessionId, const FString& ServerString) 
{
	this->OnConnected.Broadcast(ProtocolVersion, SessionId, ServerString);
}

void USTOMPWebSocketClient::HandleOnConnectionError(const FString& Error)
{
	this->OnConnectionError.Broadcast(Error);
}

void USTOMPWebSocketClient::HandleOnError(const FString& Error)
{
	this->OnError.Broadcast(Error);
}

void USTOMPWebSocketClient::HandleOnClosed(const FString& Reason)
{
	this->OnClosed.Broadcast(Reason);
}
