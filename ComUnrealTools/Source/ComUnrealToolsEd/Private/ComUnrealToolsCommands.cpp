// Copyright com04 All Rights Reserved.

#include "ComUnrealToolsCommands.h"

#define LOCTEXT_NAMESPACE "FComUnrealToolsModule"

void FComUnrealToolsCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "ComUnrealTools", "Bring up ComUnrealTools window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
