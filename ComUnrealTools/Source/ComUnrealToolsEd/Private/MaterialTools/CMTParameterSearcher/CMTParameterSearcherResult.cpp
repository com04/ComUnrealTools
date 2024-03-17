// Copyright com04 All Rights Reserved.

#include "CMTParameterSearcherResult.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Widgets/Images/SImage.h"



//////////////////////////////////////////////////////////////////////////
// FCMTParameterSearcherResult

FCMTParameterSearcherResult::FCMTParameterSearcherResult(const FText& InDisplayText, bool bError) 
	: Material(nullptr),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  ValueText(),
	  bError(bError),
	  Type(ECMTParameterSearcherResultType::Other)
{
}

FCMTParameterSearcherResult::FCMTParameterSearcherResult(const FText& InDisplayText, const FText& InValueText, UMaterial* InMaterial) 
	: Material(InMaterial),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  ValueText(InValueText),
	  bError(false),
	  Type(ECMTParameterSearcherResultType::Material)
{
}
FCMTParameterSearcherResult::FCMTParameterSearcherResult(const FText& InDisplayText, const FText& InValueText, UMaterialInstance* InInstance) 
	: Material(nullptr),
	  Instance(InInstance),
	  DisplayText(InDisplayText),
	  ValueText(InValueText),
	  bError(false),
	  Type(ECMTParameterSearcherResultType::MaterialInstance)
{
}

FReply FCMTParameterSearcherResult::OnClick()
{
	// wakeup MaterialEditor
	switch (Type)
	{
	case ECMTParameterSearcherResultType::Material:
		FCUTUtility::WakeupMaterialEditor(Material, nullptr, nullptr);
		break;
	case ECMTParameterSearcherResultType::MaterialInstance:
		FCUTUtility::WakeupMaterialEditor(nullptr, Instance, nullptr);
		break;
	}
	
	return FReply::Handled();
}

TSharedRef<SWidget>	FCMTParameterSearcherResult::CreateIcon() const
{
	FName BrushName;
	switch (Type)
	{
	case ECMTParameterSearcherResultType::Material:
		BrushName = FComUnrealToolsStyle::MaterialIconBrushName;
		break;
	case ECMTParameterSearcherResultType::MaterialInstance:
		BrushName = FComUnrealToolsStyle::MaterialInstanceIconBrushName;
		break;
	default:
		BrushName = GetBackgroundBrushName();
		break;
	}
	
	
	return 	SNew(SImage)
		.Image(FComUnrealToolsStyle::Get().GetBrush(BrushName))
		.ColorAndOpacity(FLinearColor::White);
}

FName	FCMTParameterSearcherResult::GetBackgroundBrushName() const
{
	FName BrushName(FComUnrealToolsStyle::MenuBGBrushName);
	switch (Type)
	{
	case ECMTParameterSearcherResultType::Material:
	case ECMTParameterSearcherResultType::MaterialInstance:
		BrushName = FName(FComUnrealToolsStyle::MenuBG2BrushName);
		break;
	}
	return BrushName;
}

FText FCMTParameterSearcherResult::GetDisplayText() const
{
	return DisplayText;
}

FText FCMTParameterSearcherResult::GetValueText() const
{
	return ValueText;
}
