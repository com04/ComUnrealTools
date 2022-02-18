// Copyright com04 All Rights Reserved.


#include "CMTTextureFindResultAssetPathRange.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"

#define LOCTEXT_NAMESPACE "CMTTextureFindResultWidgetAssetPathRange"


/**  Get slate parameter from materials */
void GetMaterialInfo(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction,
		FName* OutIconBrushName, FLinearColor* OutIconColor, FText* OutPathText)
{
	FName IconBrushName = FCUTUtility::GetIconBrushName(Material, MaterialInstance, MaterialFunction);
	FLinearColor IconColor = FLinearColor::Transparent;
	FString PathText;
	if (IconBrushName != NAME_None)
	{
		IconColor = FLinearColor::White;
	}
	
	if (Material)
	{
		PathText = GetPathNameSafe(Material);
	}
	else if (MaterialInstance)
	{
		PathText = GetPathNameSafe(MaterialInstance);
	}
	else if (MaterialFunction)
	{
		PathText = GetPathNameSafe(MaterialFunction);
	}
	
	if (OutIconBrushName)
	{
		*OutIconBrushName = IconBrushName;
	}
	if (OutIconColor)
	{
		*OutIconColor = IconColor;
	}
	if (OutPathText)
	{
		*OutPathText = FText::FromString(FCUTUtility::NormalizePathText(PathText));
	}
}

void SCMTTextureFindResultWidgetAssetPathRange::Construct(const FArguments& InArgs)
{
	TopMaterial = Cast<UMaterial>(InArgs._TopMaterial.Get());
	TopMaterialInstance = Cast<UMaterialInstance>(InArgs._TopMaterial.Get());
	TopMaterialFunction = InArgs._TopMaterialFunction.Get();
	
	EndMaterial = Cast<UMaterial>(InArgs._EndMaterial.Get());
	EndMaterialInstance = Cast<UMaterialInstance>(InArgs._EndMaterial.Get());
	EndMaterialFunction = InArgs._EndMaterialFunction.Get();
	
	SetPadding(FMargin(0.0f, 0.0f, 6.0f, 0.0f));
	SetVisibility(EVisibility::Visible);
	SetBorderBackgroundColor(FLinearColor::Transparent);
	
	// Top Material setup
	FName TopIconBrushName;
	FLinearColor TopIconColor;
	FText TopPathText;
	GetMaterialInfo(TopMaterial, TopMaterialInstance, TopMaterialFunction,
			&TopIconBrushName, &TopIconColor, &TopPathText);
	
	// EndMaterial setup
	FName EndIconBrushName;
	FLinearColor EndIconColor;
	FText EndPathText;
	GetMaterialInfo(EndMaterial, EndMaterialInstance, EndMaterialFunction,
			&EndIconBrushName, &EndIconColor, &EndPathText);
	
	
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
			.Image(FComUnrealToolsStyle::Get().GetBrush(TopIconBrushName))
			.ColorAndOpacity(TopIconColor)
			.OnMouseButtonDown(this, &SCMTTextureFindResultWidgetAssetPathRange::OnTopMouseButtonDown)
		]
		
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0,0)
		[
			SNew(STextBlock)
			.Text(TopPathText)
			.OnDoubleClicked(this, &SCMTTextureFindResultWidgetAssetPathRange::OnTopMouseButtonDoubleClick)
		]
		
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0,0)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("   to   ")))
		]
		
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0,0)
		[
			// icon
			SNew(SImage)
			.Image(FComUnrealToolsStyle::Get().GetBrush(EndIconBrushName))
			.ColorAndOpacity(EndIconColor)
		]
		
		+SHorizontalBox::Slot()
		.VAlign(VAlign_Center)
		.AutoWidth()
		.Padding(0,0)
		[
			SNew(STextBlock)
			.Text(EndPathText)
		]
	];
}

void SCMTTextureFindResultWidgetAssetPathRange::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SetBorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGActiveBrushName));
	SetBorderBackgroundColor(FLinearColor::White);
	
	SBorder::OnMouseEnter(MyGeometry, MouseEvent);
}
void SCMTTextureFindResultWidgetAssetPathRange::OnMouseLeave(const FPointerEvent& MouseEvent) 
{
	SetBorderImage(FCoreStyle::Get().GetBrush("Border"));
	SetBorderBackgroundColor(FLinearColor::Transparent);
	
	SBorder::OnMouseLeave(MouseEvent);
}

FReply SCMTTextureFindResultWidgetAssetPathRange::OnTopMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	OnTopMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	return FReply::Handled();
}
FReply SCMTTextureFindResultWidgetAssetPathRange::OnTopMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent)
{
	if (TopMaterial || TopMaterialInstance || TopMaterialFunction)
	{
		FCUTUtility::WakeupMaterialEditor(TopMaterial, TopMaterialInstance, TopMaterialFunction);
	}
	return FReply::Handled();
}
FReply SCMTTextureFindResultWidgetAssetPathRange::OnEndMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	OnEndMouseButtonDoubleClick();
	return FReply::Handled();
}
FReply SCMTTextureFindResultWidgetAssetPathRange::OnEndMouseButtonDoubleClick()
{
	if (EndMaterial || EndMaterialInstance || EndMaterialFunction)
	{
		FCUTUtility::WakeupMaterialEditor(EndMaterial, EndMaterialInstance, EndMaterialFunction);
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE


