// Copyright com04 All Rights Reserved.


#include "CMTTextureFindResultAssetPath.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"

#define LOCTEXT_NAMESPACE "CMTTextureFindResultWidgetAssetPath"


void SCMTTextureFindResultWidgetAssetPath::Construct(const FArguments& InArgs)
{
	Texture = InArgs._Texture.Get();
	Material = Cast<UMaterial>(InArgs._Material.Get());
	MaterialInstance = Cast<UMaterialInstance>(InArgs._Material.Get());
	MaterialFunction = InArgs._MaterialFunction.Get();
	
	SetPadding(FMargin(0.0f, 0.0f, 6.0f, 0.0f));
	SetVisibility(EVisibility::Visible);
	SetBorderBackgroundColor(FLinearColor::Transparent);
	
	FName IconBrushName = FCUTUtility::GetIconBrushName(Material, MaterialInstance, MaterialFunction);
	FLinearColor IconColor = FLinearColor::Transparent;
	if (IconBrushName != NAME_None)
	{
		IconColor = FLinearColor::White;
	}
	
	// Path text
	FText PathText;
	{
		FString AssetPath = FCUTUtility::NormalizePathText(InArgs._AssetPath.Get());
		
		FString ExpressionName = InArgs._ExpressionName.Get();
		if (ExpressionName.IsEmpty())
		{
			PathText = FText::FromString(AssetPath);
		}
		else
		{
			FFormatNamedArguments Args;
			Args.Add(TEXT("0"), FText::FromString(AssetPath));
			Args.Add(TEXT("1"), FText::FromString(ExpressionName));
			PathText = FText::Format(LOCTEXT("Path", "{0} - {1}"), Args);
		}
	}
	
	ChildSlot
	[
		SNew(SHorizontalBox)
		
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0,0)
		[
			// icon
			SNew(SImage)
			.Image(FComUnrealToolsStyle::Get().GetBrush(IconBrushName))
			.ColorAndOpacity(IconColor)
		]
		
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0,0)
		[
			SNew(STextBlock)
			.Text(PathText)
		]
	];
}

void SCMTTextureFindResultWidgetAssetPath::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SetBorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGActiveBrushName));
	SetBorderBackgroundColor(FLinearColor::White);
	
	SBorder::OnMouseEnter(MyGeometry, MouseEvent);
}
void SCMTTextureFindResultWidgetAssetPath::OnMouseLeave(const FPointerEvent& MouseEvent) 
{
	SetBorderImage(FCoreStyle::Get().GetBrush("Border"));
	SetBorderBackgroundColor(FLinearColor::Transparent);
	
	SBorder::OnMouseLeave(MouseEvent);
}

FReply SCMTTextureFindResultWidgetAssetPath::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (Material || MaterialInstance || MaterialFunction)
	{
		FCUTUtility::WakeupMaterialEditor(Material, MaterialInstance, MaterialFunction);
	}
	else if (Texture)
	{
		FCUTUtility::WakeupTextureEditor(Texture);
	}
	return SBorder::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}

#undef LOCTEXT_NAMESPACE


