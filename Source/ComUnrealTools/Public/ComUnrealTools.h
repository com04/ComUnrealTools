// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class UCUTDeveloperSettings;

class FComUnrealToolsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
private:
	static const FName MaterialToolsTabName;
	static const FName ViewToolsTabName;

	/** spawn tab event */
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	/** Editor Setting */
	UCUTDeveloperSettings* DeveloperSettings = nullptr;

};
