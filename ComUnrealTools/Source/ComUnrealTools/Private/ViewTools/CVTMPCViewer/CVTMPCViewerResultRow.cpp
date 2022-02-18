// Copyright com04 All Rights Reserved.

#include "CVTMPCViewerResultRow.h"
#include "CVTMPCViewer.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "ViewTools/CVTMPCViewerWatch/CVTMPCViewerWatch.h"

#include "Widgets/SWidget.h"


#define LOCTEXT_NAMESPACE "CVTMPCViewerResultRow"


void SCVTMPCViewerResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Info = InArgs._Info;
	SetBorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName));
	SMultiColumnTableRow<TSharedPtr<FCVTMPCViewerResult>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SCVTMPCViewerResultRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == FName(*SCVTMPCViewer::GetHeaderParameterNameText().ToString()))
	{
		return	SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0,0)
				[
					// icon
					SNew(SImage)
					.Image(FComUnrealToolsStyle::Get().GetBrush(FName(Info->bIsScalar ? FComUnrealToolsStyle::MaterialScalarIconBrushName : FComUnrealToolsStyle::MaterialVectorIconBrushName)))
					.ColorAndOpacity(FLinearColor::White)
				]
				+SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				.AutoWidth()
				.Padding(0,0)
				[
					// text
					SNew(STextBlock)
					.Text(FText(FText::FromName(Info->ParameterName)))
				];
	}
	else if (ColumnName == FName(*SCVTMPCViewer::GetHeaderParameterDefaultValueText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(this, &SCVTMPCViewerResultRow::GetParameterDefaultValueText)
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTMPCViewer::GetHeaderParameterValueText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(this, &SCVTMPCViewerResultRow::GetParameterValueText)
					.ColorAndOpacity(this, &SCVTMPCViewerResultRow::GetParameterValueColor)
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTMPCViewer::GetHeaderWatchText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(SButton)
					.Text(LOCTEXT("MPCWatch", "ウォッチに入れる"))
					.OnClicked(this, &SCVTMPCViewerResultRow::ButtonWatchClicked)
				];
		BorderColor2List.Add(Border);
		return Border;
	}

	return SNullWidget::NullWidget;
}


void SCVTMPCViewerResultRow::OnMouseEnter(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
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
void SCVTMPCViewerResultRow::OnMouseLeave(const FPointerEvent& InMouseEvent)
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

FText SCVTMPCViewerResultRow::GetParameterDefaultValueText() const
{
	FText RetText;
	if (Info.IsValid())
	{
		RetText = Info->GetParameterDefaultValueText();
	}
	return RetText;
}
FText SCVTMPCViewerResultRow::GetParameterValueText() const
{
	FText RetText;
	if (Info.IsValid())
	{
		RetText = Info->GetParameterValueText();
	}
	return RetText;
}

FSlateColor SCVTMPCViewerResultRow::GetParameterValueColor() const
{
	const FLinearColor DefaultValueColor = FLinearColor::White;
	const FLinearColor ChangedValueColor = FLinearColor(0.8f, 0.2f, 0.2f, 1.0f);
	FLinearColor RetColor = DefaultValueColor;
	if (Info.IsValid())
	{
		if (Info->bIsScalar)
		{
			if (!FMath::IsNearlyEqual(Info->ScalarValue, Info->DefaultScalarValue))
			{
				RetColor = ChangedValueColor;
			}
		}
		else
		{
			if (!Info->VectorValue.Equals(Info->DefaultVectorValue))
			{
				RetColor = ChangedValueColor;
			}
		}
	}
	return RetColor;
}
/* ウォッチに入れるボタン */
FReply SCVTMPCViewerResultRow::ButtonWatchClicked()
{
	if (Info.IsValid())
	{
		SCVTMPCViewerWatch::AddWatchList(Info->Collection, Info->ParameterName, Info->bIsScalar);
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
