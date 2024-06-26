﻿// Copyright com04 All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "SlateBasics.h"

/**  */
class FComUnrealToolsStyle
{
public:

	static void Initialize();

	static void Shutdown();

	/** reloads textures used by slate renderer */
	static void ReloadTextures();

	/** @return The Slate style set for the Shooter game */
	static const ISlateStyle& Get();

	static FName GetStyleSetName();
	static UMaterial* GetImageOpaqueMaterial();
	static FSlateFontInfo GetLargeFontStyle();
	static FSlateFontInfo GetMiddleFontStyle();

	static const FName UnrealToolsIconBrushName;
	static const FName UnrealToolsTabIconBrushName;
	static const FName MaterialToolsTabIconBrushName;
	static const FName ViewToolsTabIconBrushName;
	static const FName SequencerToolsTabIconBrushName;
	static const FName ObjectToolsTabIconBrushName;

	static const FName BlueprintIconBrushName;
	static const FName FolderIconBrushName;
	static const FName LevelSequenceIconBrushName;
	static const FName TrackIconBrushName;
	static const FName MaterialIconBrushName;
	static const FName MaterialFunctionIconBrushName;
	static const FName MaterialInstanceIconBrushName;
	static const FName MaterialScalarIconBrushName;
	static const FName MaterialVectorIconBrushName;
	static const FName MaterialTextureIconBrushName;
	static const FName MaterialSwitchIconBrushName;
	static const FName MaterialComponentMaskIconBrushName;
	static const FName MaterialFontIconBrushName;
	static const FName MaterialVirtualTextureIconBrushName;
	static const FName MenuBGBrushName;
	static const FName MenuBG2BrushName;
	static const FName MenuBGActiveBrushName;


private:
	static TSharedRef<class FSlateStyleSet> Create();
	static TWeakObjectPtr<UMaterial> ImageOpaqueMaterial;

private:

	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};