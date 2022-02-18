// Copyright com04 All Rights Reserved.

#include "COTPropertySearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Widgets/Images/SImage.h"



//////////////////////////////////////////////////////////////////////////
// FCOTPropertySearcherResult

FCOTPropertySearcherResult::FCOTPropertySearcherResult(const FText& InDisplayText) 
	: Parent(nullptr),
	  ValueObject(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(),
	  Type(ECOTPropertySearcherResultType::Other)
{
}

FCOTPropertySearcherResult::FCOTPropertySearcherResult(const FText& InDisplayText, UBlueprint* InBlueprint) 
	: Parent(nullptr),
	  ValueObject(InBlueprint),
	  DisplayText(InDisplayText),
	  OptionText(),
	  Type(ECOTPropertySearcherResultType::Blueprint)
{
}
FCOTPropertySearcherResult::FCOTPropertySearcherResult(const FText& InDisplayText, UMaterial* InMaterial) 
	: Parent(nullptr),
	  ValueObject(InMaterial),
	  DisplayText(InDisplayText),
	  OptionText(),
	  Type(ECOTPropertySearcherResultType::Material)
{
}
FCOTPropertySearcherResult::FCOTPropertySearcherResult(const FText& InDisplayText, TSharedPtr<FCOTPropertySearcherResult> InParent)
	: Parent(InParent),
	  ValueObject(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(),
	  Type(ECOTPropertySearcherResultType::Property)
{
}

FReply FCOTPropertySearcherResult::OnClick()
{
	if (WakeupEditor())
	{
	}
	
	return FReply::Handled();
}
bool FCOTPropertySearcherResult::WakeupEditor()
{
	if (Type == ECOTPropertySearcherResultType::Blueprint)
	{
		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(ValueObject);
		return true;
	}
	else
	{
		if (Parent.IsValid())
		{
			return Parent.Pin()->WakeupEditor();
		}
	}
	return false;
}

TSharedRef<SWidget>	FCOTPropertySearcherResult::CreateIcon() const
{
	FName BrushName = FComUnrealToolsStyle::MenuBGBrushName;
	switch (Type)
	{
	case ECOTPropertySearcherResultType::Blueprint:
		BrushName = FComUnrealToolsStyle::BlueprintIconBrushName;
		break;
	case ECOTPropertySearcherResultType::Material:
		BrushName = FComUnrealToolsStyle::MaterialIconBrushName;
		break;
	}
	
	
	return 	SNew(SImage)
		.Image(FComUnrealToolsStyle::Get().GetBrush(BrushName))
		.ColorAndOpacity(FLinearColor::White);
}

FName	FCOTPropertySearcherResult::GetBackgroundBrushName() const
{
	FName BrushName(FComUnrealToolsStyle::MenuBGBrushName);
	switch (Type)
	{
	case ECOTPropertySearcherResultType::Blueprint:
	case ECOTPropertySearcherResultType::Material:
		BrushName = FName(FComUnrealToolsStyle::MenuBG2BrushName);
		break;
	}
	return BrushName;
}


FText FCOTPropertySearcherResult::GetDisplayText() const
{
	return DisplayText;
}
FText FCOTPropertySearcherResult::GetOptionText() const
{
	return OptionText;
}
FString FCOTPropertySearcherResult::GetTypeString() const
{
	FString RetString;
	switch (GetType())
	{
	case ECOTPropertySearcherResultType::Blueprint:
		RetString = FString(TEXT("BP"));
		break;
	case ECOTPropertySearcherResultType::Material:
		RetString = FString(TEXT("Material"));
		break;
	case ECOTPropertySearcherResultType::Property:
		RetString = FString(TEXT("Property"));
		break;
	}
	return RetString;
}


void FCOTPropertySearcherResult::AddChild(TSharedPtr<FCOTPropertySearcherResult> Child)
{
	Children.Add(Child);
}
