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
const FName FComUnrealToolsStyle::ViewToolsTabIconBrushName = FName(TEXT("ComUnrealTools.ViewToolsTabIcon"));
const FName FComUnrealToolsStyle::SequencerToolsTabIconBrushName = FName(TEXT("ComUnrealTools.SequencerToolsTabIcon"));
const FName FComUnrealToolsStyle::ObjectToolsTabIconBrushName = FName(TEXT("ComUnrealTools.ObjectToolsTabIcon"));
const FName FComUnrealToolsStyle::BlueprintIconBrushName = FName(TEXT("ComUnrealTools.BlueprintIcon"));
const FName FComUnrealToolsStyle::FolderIconBrushName = FName(TEXT("ComUnrealTools.FolderIcon"));
const FName FComUnrealToolsStyle::LevelSequenceIconBrushName = FName(TEXT("ComUnrealTools.LevelSequenceIcon"));
const FName FComUnrealToolsStyle::TrackIconBrushName = FName(TEXT("ComUnrealTools.TrackIcon"));
const FName FComUnrealToolsStyle::MaterialIconBrushName = FName(TEXT("ComUnrealTools.MaterialIcon"));
const FName FComUnrealToolsStyle::MaterialFunctionIconBrushName = FName(TEXT("ComUnrealTools.MaterialFunctionIcon"));
const FName FComUnrealToolsStyle::MaterialInstanceIconBrushName = FName(TEXT("ComUnrealTools.MaterialInstanceIcon"));
const FName FComUnrealToolsStyle::MaterialScalarIconBrushName = FName(TEXT("ComUnrealTools.MaterialScalarIcon"));
const FName FComUnrealToolsStyle::MaterialVectorIconBrushName = FName(TEXT("ComUnrealTools.MaterialVectorIcon"));
const FName FComUnrealToolsStyle::MaterialTextureIconBrushName = FName(TEXT("ComUnrealTools.MaterialTextureIcon"));
const FName FComUnrealToolsStyle::MaterialSwitchIconBrushName = FName(TEXT("ComUnrealTools.MaterialSwitchIcon"));
const FName FComUnrealToolsStyle::MaterialComponentMaskIconBrushName = FName(TEXT("ComUnrealTools.MaterialComponentMaskIcon"));
const FName FComUnrealToolsStyle::MaterialFontIconBrushName = FName(TEXT("ComUnrealTools.MaterialFontIcon"));
const FName FComUnrealToolsStyle::MaterialVirtualTextureIconBrushName = FName(TEXT("ComUnrealTools.MaterialVirtualTextureIcon"));
const FName FComUnrealToolsStyle::MenuBGBrushName = FName(TEXT("ComUnrealTools.MenuBackground"));
const FName FComUnrealToolsStyle::MenuBG2BrushName = FName(TEXT("ComUnrealTools.MenuBackground2"));
const FName FComUnrealToolsStyle::MenuBGActiveBrushName = FName(TEXT("ComUnrealTools.MenuBackgroundActive"));
TSharedPtr<UMaterial> FComUnrealToolsStyle::ImageOpaqueMaterial;
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
UMaterial* FComUnrealToolsStyle::GetImageOpaqueMaterial()
{
	return ImageOpaqueMaterial.Get();
}

FSlateFontInfo FComUnrealToolsStyle::GetLargeFontStyle()
{
	FSlateFontInfo Font = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;
	Font.Size = 20;
	return Font;
}
FSlateFontInfo FComUnrealToolsStyle::GetMiddleFontStyle()
{
	FSlateFontInfo Font = FCoreStyle::Get().GetWidgetStyle<FTextBlockStyle>("NormalText").Font;
	Font.Size = 12;
	return Font;
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

	Style->Set(UnrealToolsIconBrushName,			new IMAGE_BRUSH(TEXT("UnrealToolsIcon_40x"),		Icon40x40));
	Style->Set(UnrealToolsTabIconBrushName,			new IMAGE_BRUSH(TEXT("UnrealToolsIcon_40x"),		Icon16x16));
	Style->Set(MaterialToolsTabIconBrushName,		new IMAGE_BRUSH(TEXT("MaterialToolsIcon_40x"),		Icon16x16));
	Style->Set(ViewToolsTabIconBrushName,			new IMAGE_BRUSH(TEXT("ViewToolsIcon_40x"),			Icon16x16));
	Style->Set(SequencerToolsTabIconBrushName,		new IMAGE_BRUSH(TEXT("SequencerToolsIcon_40x"),		Icon16x16));
	Style->Set(ObjectToolsTabIconBrushName,			new IMAGE_BRUSH(TEXT("ObjectToolsIcon_40x"),		Icon16x16));
	Style->Set(BlueprintIconBrushName,				new IMAGE_BRUSH(TEXT("BlueprintIcon"),				Icon16x16));
	Style->Set(FolderIconBrushName,					new IMAGE_BRUSH(TEXT("FolderIcon"),					Icon16x16));
	Style->Set(LevelSequenceIconBrushName,			new IMAGE_BRUSH(TEXT("LevelSequenceIcon"),			Icon16x16));
	Style->Set(TrackIconBrushName,					new IMAGE_BRUSH(TEXT("TrackIcon"),					Icon16x16));
	Style->Set(MaterialIconBrushName,				new IMAGE_BRUSH(TEXT("MaterialIcon"),				Icon20x20));
	Style->Set(MaterialFunctionIconBrushName,		new IMAGE_BRUSH(TEXT("MaterialFunctionIcon"),		Icon20x20));
	Style->Set(MaterialInstanceIconBrushName,		new IMAGE_BRUSH(TEXT("MaterialInstanceIcon"),		Icon20x20));
	Style->Set(MaterialScalarIconBrushName,			new IMAGE_BRUSH(TEXT("MaterialScalarIcon"),			Icon20x20));
	Style->Set(MaterialVectorIconBrushName,			new IMAGE_BRUSH(TEXT("MaterialVectorIcon"),			Icon20x20));
	Style->Set(MaterialTextureIconBrushName,		new IMAGE_BRUSH(TEXT("MaterialTextureIcon"),		Icon20x20));
	Style->Set(MaterialSwitchIconBrushName,			new IMAGE_BRUSH(TEXT("MaterialSwitchIcon"),			Icon20x20));
	Style->Set(MaterialComponentMaskIconBrushName,	new IMAGE_BRUSH(TEXT("MaterialComponentMaskIcon"),	Icon20x20));
	Style->Set(MaterialFontIconBrushName,			new IMAGE_BRUSH(TEXT("MaterialFontIcon"),			Icon20x20));
	Style->Set(MaterialVirtualTextureIconBrushName,	new IMAGE_BRUSH(TEXT("MaterialVirtualTextureIcon"),	Icon20x20));
	Style->Set(MenuBGBrushName,						new BOX_BRUSH("MenuBackground",						BGTile));
	Style->Set(MenuBG2BrushName,					new BOX_BRUSH("MenuBackground2",					BGTile));
	Style->Set(MenuBGActiveBrushName,				new BOX_BRUSH("MenuBackgroundActive",				BGTile));

	if (!ImageOpaqueMaterial.IsValid())
	{
		ImageOpaqueMaterial = MakeShareable(Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr, TEXT("/ComUnrealTools/Materials/M_CUTImageOpaque"))));
	}
	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH

