// Copyright com04 All Rights Reserved.

#include "ComUnrealTools.h"

#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY(LogComUnrealTools)

#define LOCTEXT_NAMESPACE "FComUnrealToolsModule"

void FComUnrealToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
}

void FComUnrealToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComUnrealToolsModule, ComUnrealTools)
