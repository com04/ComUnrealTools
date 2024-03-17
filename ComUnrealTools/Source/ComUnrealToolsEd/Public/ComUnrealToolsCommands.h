// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"
#include "ComUnrealToolsStyle.h"

class FComUnrealToolsCommands : public TCommands<FComUnrealToolsCommands>
{
public:

	FComUnrealToolsCommands()
		: TCommands<FComUnrealToolsCommands>(TEXT("ComUnrealTools"), NSLOCTEXT("Contexts", "ComUnrealTools", "ComUnrealTools Plugin"), NAME_None, FComUnrealToolsStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
