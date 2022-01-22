// Copyright com04 All Rights Reserved.

#include "ComUnrealToolsStyle.h"
#include "ComUnrealTools.h"

#include "Brushes/SlateBoxBrush.h"
#include "Brushes/SlateImageBrush.h"
#include "Interfaces/IPluginManager.h"
#include "Slate/SlateGameResources.h"


const FName FComUnrealToolsStyle::UnrealToolsIconBrushName = FName(TEXT("ComUnrealTools.OpenPluginWindow"));
const FName FComUnrealToolsStyle::UnrealToolsTabIconBrushName = FName(TEXT("ComUnrealTools.TabIcon"));
const FName FComUnrealToolsStyle::MaterialToolsTabIconBrushName = FName(TEXT("ComUnrealTools.MaterialToolsTabIcon"));
const FName FComUnrealToolsStyle::MaterialIconBrushName = FName(TEXT("ComUnrealTools.MaterialIcon"));
const FName FComUnrealToolsStyle::MaterialFunctionIconBrushName = FName(TEXT("ComUnrealTools.MaterialFunctionIcon"));
const FName FComUnrealToolsStyle::MaterialInstanceIconBrushName = FName(TEXT("ComUnrealTools.MaterialInstanceIcon"));
const FName FComUnrealToolsStyle::MenuBGBrushName = FName(TEXT("ComUnrealTools.MenuBackground"));
const FName FComUnrealToolsStyle::MenuBG2BrushName = FName(TEXT("ComUnrealTools.MenuBackground2"));
const FName FComUnrealToolsStyle::MenuBGActiveBrushName = FName(TEXT("ComUnrealTools.MenuBackgroundActive"));
TSharedPtr<FSlateStyleSet> FComUnrealToolsStyle::StyleInstance = NULL;

void FComUnrealToolsStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FComUnrealToolsStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FComUnrealToolsStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("ComUnrealToolsStyle"));
	return StyleSetName;
}

void FComUnrealToolsStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FComUnrealToolsStyle::Get()
{
	return *StyleInstance;
}


#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BOX_BRUSH(RelativePath, ...) FSlateBoxBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)
#define BORDER_BRUSH(RelativePath, ...) FSlateBorderBrush(Style->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

TSharedRef<FSlateStyleSet> FComUnrealToolsStyle::Create()
{
	const FVector2D Icon16x16(16.0f, 16.0f);
	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);
	const FMargin BGTile(8.0f / 64.0f);

	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(GetStyleSetName()));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("ComUnrealTools")->GetBaseDir() / TEXT("Resources"));

	Style->Set(UnrealToolsIconBrushName,		new IMAGE_BRUSH(TEXT("UnrealToolsIcon_40x"),		Icon40x40));
	Style->Set(UnrealToolsTabIconBrushName,		new IMAGE_BRUSH(TEXT("UnrealToolsIcon_40x"),		Icon16x16));
	Style->Set(MaterialToolsTabIconBrushName,	new IMAGE_BRUSH(TEXT("MaterialToolsIcon_40x"),		Icon16x16));
	Style->Set(MaterialIconBrushName,			new IMAGE_BRUSH(TEXT("MaterialIcon"),				Icon20x20));
	Style->Set(MaterialFunctionIconBrushName,	new IMAGE_BRUSH(TEXT("MaterialFunctionIcon"),		Icon20x20));
	Style->Set(MaterialInstanceIconBrushName,	new IMAGE_BRUSH(TEXT("MaterialInstanceIcon"),		Icon20x20));
	Style->Set(MenuBGBrushName,					new BOX_BRUSH("MenuBackground",						BGTile));
	Style->Set(MenuBG2BrushName,				new BOX_BRUSH("MenuBackground2",					BGTile));
	Style->Set(MenuBGActiveBrushName,			new BOX_BRUSH("MenuBackgroundActive",				BGTile));

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH

