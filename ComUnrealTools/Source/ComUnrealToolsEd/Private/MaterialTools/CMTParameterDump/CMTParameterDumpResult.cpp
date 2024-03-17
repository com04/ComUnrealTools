// Copyright com04 All Rights Reserved.

#include "CMTParameterDumpResult.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Widgets/Images/SImage.h"



//////////////////////////////////////////////////////////////////////////
// FCMTParameterDumpResult

FCMTParameterDumpResult::FCMTParameterDumpResult(const FString& InParameterNameString, const FString& InParameterValueString, ECMTParameterDumpResultType InType, UTexture* InTexture) 
	: ParameterNameString(InParameterNameString),
	  ParameterValueString(InParameterValueString),
	  Type(InType),
	  Texture(InTexture)
{
}

FReply FCMTParameterDumpResult::OnClick()
{
	switch (Type)
	{
	case ECMTParameterDumpResultType::Texture:
		if (IsValid(Texture))
		{
			FCUTUtility::WakeupTextureEditor(Texture);
		}
		break;
	}
	
	return FReply::Handled();
}

TSharedRef<SWidget>	FCMTParameterDumpResult::CreateIcon() const
{
	FName BrushName;
	switch (Type)
	{
	case ECMTParameterDumpResultType::Scalar:
		BrushName = FName(FComUnrealToolsStyle::MaterialScalarIconBrushName);
		break;
	case ECMTParameterDumpResultType::Vector:
		BrushName = FName(FComUnrealToolsStyle::MaterialVectorIconBrushName);
		break;
	case ECMTParameterDumpResultType::Texture:
		BrushName = FName(FComUnrealToolsStyle::MaterialTextureIconBrushName);
		break;
	case ECMTParameterDumpResultType::Switch:
		BrushName = FName(FComUnrealToolsStyle::MaterialSwitchIconBrushName);
		break;
	case ECMTParameterDumpResultType::ComponentMask:
		BrushName = FName(FComUnrealToolsStyle::MaterialComponentMaskIconBrushName);
		break;
	case ECMTParameterDumpResultType::Font:
		BrushName = FName(FComUnrealToolsStyle::MaterialFontIconBrushName);
		break;
	case ECMTParameterDumpResultType::VirtualTexture:
		BrushName = FName(FComUnrealToolsStyle::MaterialVirtualTextureIconBrushName);
		break;
	default:
		BrushName = GetBackgroundBrushName();
		break;
	}
	
	
	return 	SNew(SImage)
		.Image(FComUnrealToolsStyle::Get().GetBrush(BrushName))
		.ColorAndOpacity(FLinearColor::White);
}

FName	FCMTParameterDumpResult::GetBackgroundBrushName() const
{
	FName BrushName(FComUnrealToolsStyle::MenuBGBrushName);
	return BrushName;
}

FString FCMTParameterDumpResult::GetParamNameString() const
{
	return ParameterNameString;
}

FString FCMTParameterDumpResult::GetTypeString() const
{
	FString RetValue;
	switch (Type)
	{
	case ECMTParameterDumpResultType::Scalar:
		RetValue = FString(TEXT("Scalar"));
		break;
	case ECMTParameterDumpResultType::Vector:
		RetValue = FString(TEXT("Vector"));
		break;
	case ECMTParameterDumpResultType::Texture:
		RetValue = FString(TEXT("Texture"));
		break;
	case ECMTParameterDumpResultType::Switch:
		RetValue = FString(TEXT("StaticSwitch"));
		break;
	case ECMTParameterDumpResultType::ComponentMask:
		RetValue = FString(TEXT("StaticComponentMask"));
		break;
	case ECMTParameterDumpResultType::Font:
		RetValue = FString(TEXT("Font"));
		break;
	case ECMTParameterDumpResultType::VirtualTexture:
		RetValue = FString(TEXT("VirtualTexture"));
		break;
	}
	return RetValue;
}

FString FCMTParameterDumpResult::GetParameterValueString() const
{
	return ParameterValueString;
}

