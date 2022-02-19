// Copyright 2020 Richard W. Van Tassel. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSTOMPWebSocketsModule : public IModuleInterface
{
private:
	/**
	 * Called when WTOMPWebSockets module is loaded
	 * Initialize implementation specific parts of Stomp handling
	 */
	virtual void StartupModule() override;
};
