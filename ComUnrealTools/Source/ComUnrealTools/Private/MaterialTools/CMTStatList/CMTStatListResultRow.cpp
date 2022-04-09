// Copyright com04 All Rights Reserved.

#include "CMTStatListResultRow.h"
#include "CMTStatList.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/MaterialInstance.h"
#include "Widgets/SWidget.h"




void SCMTStatListResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Info = InArgs._Info;
	SetBorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName));
	SMultiColumnTableRow<TSharedPtr<FCMTStatListResult>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SCMTStatListResultRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == SCMTStatList::GetHeaderNameTextName())
	{
		return	SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0,0)
				[
					// icon
					SNew(SImage)
					.Image(FComUnrealToolsStyle::Get().GetBrush(FName(Info->IsInstance ? FComUnrealToolsStyle::MaterialInstanceIconBrushName : FComUnrealToolsStyle::MaterialIconBrushName)))
					.ColorAndOpacity(FLinearColor::White)
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0,0)
				[
					// text
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->Name))
				];
	}
	else if (ColumnName == SCMTStatList::GetHeaderDomainTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->Domain))
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderShadingModelTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->ShadingModel))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderBlendModeTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(Info->BlendMode)
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderInstructionsTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::AsNumber(Info->Instruction))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderTextureNumTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::AsNumber(Info->TextureNum))
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderTextureSizeTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::AsNumber(Info->TextureSize))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderRenderAfterDOFTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->UseRenderAfterDOF ? "use" : "no"))
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderPositionOffsetTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->UsePositionOffset ? "use" : "no"))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderDepthOffsetTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->UseDepthOffset ? "use" : "no"))
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == SCMTStatList::GetHeaderRefractionTextName())
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.ToolTipText(Info->Tooltip)
					.Text(FText::FromString(Info->UseRefraction ? "use" : "no"))
				];
		BorderColor1List.Add(Border);
		return Border;
	}

	return SNullWidget::NullWidget;
}


void SCMTStatListResultRow::OnMouseEnter(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	// change active border
	const FSlateBrush* Brush = FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGActiveBrushName);
	SetBorderImage(Brush);
	for (auto ItBorder = BorderColor1List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush);
	}
	for (auto ItBorder = BorderColor2List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush);
	}
	return SMultiColumnTableRow::OnMouseEnter(InMyGeometry, InMouseEvent);
}
void SCMTStatListResultRow::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	// restore border
	const FSlateBrush* Brush = FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName);
	const FSlateBrush* Brush2 = FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName);
	SetBorderImage(Brush);
	for (auto ItBorder = BorderColor1List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush);
	}
	for (auto ItBorder = BorderColor2List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush2);
	}
	
	return SMultiColumnTableRow::OnMouseLeave(InMouseEvent);
}

FReply SCMTStatListResultRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(Info->Material);
	UMaterial* Material = Cast<UMaterial>(Info->Material);
	if (MaterialInstance)
	{
		FCUTUtility::WakeupMaterialEditor(nullptr, MaterialInstance, nullptr);
	}
	else if (Material)
	{
		FCUTUtility::WakeupMaterialEditor(Material, nullptr, nullptr);
	}
	
	return SMultiColumnTableRow::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
}
