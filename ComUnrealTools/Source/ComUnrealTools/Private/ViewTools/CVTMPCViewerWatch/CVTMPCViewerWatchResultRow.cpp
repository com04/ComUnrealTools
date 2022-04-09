// Copyright com04 All Rights Reserved.

#include "CVTMPCViewerWatchResultRow.h"
#include "CVTMPCViewerWatch.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "ViewTools/CVTMPCViewer/CVTMPCViewerResult.h"
#include "Utility/CUTUtility.h"

#include "Materials/MaterialParameterCollection.h"
#include "Widgets/SWidget.h"


#define LOCTEXT_NAMESPACE "CVTMPCViewerWatchResultRow"


void SCVTMPCViewerWatchResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Info = InArgs._Info;
	SetBorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName));
	SMultiColumnTableRow<TSharedPtr<FCVTMPCViewerResult>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SCVTMPCViewerWatchResultRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == FName(*SCVTMPCViewerWatch::GetHeaderMPCNameText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text((Info->Collection.IsValid()) ? FText::FromString(FCUTUtility::NormalizePathText(Info->Collection->GetPathName())) : LOCTEXT("NoMPC", "MPCが存在しません"))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTMPCViewerWatch::GetHeaderParameterNameText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Left)
				[
					SNew(SHorizontalBox)
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
						SNew(STextBlock)
						.Text(FText(FText::FromName(Info->ParameterName)))
					]
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTMPCViewerWatch::GetHeaderParameterDefaultValueText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(this, &SCVTMPCViewerWatchResultRow::GetParameterDefaultValueText)
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTMPCViewerWatch::GetHeaderParameterValueText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(this, &SCVTMPCViewerWatchResultRow::GetParameterValueText)
					.ColorAndOpacity(this, &SCVTMPCViewerWatchResultRow::GetParameterValueColor)
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTMPCViewerWatch::GetHeaderWatchEraseText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(SButton)
					.Text(LOCTEXT("MPCWatch", "ウォッチ解除"))
					.OnClicked(this, &SCVTMPCViewerWatchResultRow::ButtonWatchClearClicked)
				];
		BorderColor1List.Add(Border);
		return Border;
	}

	return SNullWidget::NullWidget;
}


void SCVTMPCViewerWatchResultRow::OnMouseEnter(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
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
void SCVTMPCViewerWatchResultRow::OnMouseLeave(const FPointerEvent& InMouseEvent)
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

FText SCVTMPCViewerWatchResultRow::GetParameterDefaultValueText() const
{
	FText RetText;
	if (Info.IsValid())
	{
		RetText = Info->GetParameterDefaultValueText();
	}
	return RetText;
}
FText SCVTMPCViewerWatchResultRow::GetParameterValueText() const
{
	FText RetText;
	if (Info.IsValid())
	{
		RetText = Info->GetParameterValueText();
	}
	return RetText;
}

FSlateColor SCVTMPCViewerWatchResultRow::GetParameterValueColor() const
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
/* ウォッチ解除ボタン */
FReply SCVTMPCViewerWatchResultRow::ButtonWatchClearClicked()
{
	if (Info.IsValid())
	{
		SCVTMPCViewerWatch::RemoveWatchList(Info->Collection, Info->ParameterName, Info->bIsScalar);
	}
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
