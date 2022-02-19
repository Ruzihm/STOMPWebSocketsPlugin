// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.

#include "STOMPWebSockets.h"
#include "StompModule.h"

#define LOCTEXT_NAMESPACE "FSTOMPWebSocketsModule"
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSTOMPWebSocketsModule, STOMPWebSockets)

void FSTOMPWebSocketsModule::StartupModule()
{
	FModuleManager::LoadModuleChecked<FStompModule>("Stomp");
}
