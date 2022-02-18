// Copyright com04 All Rights Reserved.

#include "CVTVolumeRendererItemRow.h"
#include "ComUnrealToolsStyle.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Colors/SColorPicker.h"
#include "Widgets/Text/STextBlock.h"


#define LOCTEXT_NAMESPACE "CVTVolumeRendererItemRow"

void SCVTVolumeRendererItemRow::Construct(const FArguments& InArgs, TSharedPtr<FCVTVolumeRendererItem> InItem, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Item = InItem;

	STableRow<TSharedPtr<FCVTVolumeRendererItem>>::Construct(
		STableRow<TSharedPtr<FCVTVolumeRendererItem>>::FArguments()
		.Content()
		[
			SNew(SHorizontalBox)
			
			// data name
			+SHorizontalBox::Slot()
			.MaxWidth(200)
			.VAlign(VAlign_Center)
			.Padding(2,0)
			[
				SNew(STextBlock)
				.Text(this, &SCVTVolumeRendererItemRow::GetClassText)
				.Font(FComUnrealToolsStyle::GetMiddleFontStyle())
				.ColorAndOpacity(FLinearColor::White)
				.AutoWrapText(true)
				.WrapTextAt(200)
				.MinDesiredWidth(200)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.Text(this, &SCVTVolumeRendererItemRow::GetDispAlwaysText)
				.OnClicked(this, &SCVTVolumeRendererItemRow::ButtonDispAlwaysClicked)
				.ButtonColorAndOpacity(this, &SCVTVolumeRendererItemRow::GetAlwaysButtonColor)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("DispOneShot", "一回表示"))
				.OnClicked(this, &SCVTVolumeRendererItemRow::ButtonDispOneShotClicked)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				// ColorPicker
				SNew(SComboButton)
				.ContentPadding(0)
				.HasDownArrow(false)
				.ButtonStyle(FEditorStyle::Get(), "Sequencer.AnimationOutliner.ColorStrip")
				.OnGetMenuContent(this, &SCVTVolumeRendererItemRow::CreateColorPicker)
				.CollapseMenuOnParentFocus(true)
				.ButtonContent()
				[
					SNew(SColorBlock)
					.Color(this, &SCVTVolumeRendererItemRow::GetDisplayColor)
					.ShowBackgroundForAlpha(true)
					.IgnoreAlpha(false)
					.Size(FVector2D(32.0f, 32.0f))
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("Remove", "外す"))
				.OnClicked(this, &SCVTVolumeRendererItemRow::ButtonRemoveClicked)
				.ButtonColorAndOpacity(FLinearColor(0.6f, 0.6f, 1.0f, 1.0f))
			]
		], InOwnerTableView);
}

FText SCVTVolumeRendererItemRow::GetDispAlwaysText() const
{
	if (Item->IsAlways())
	{
		return LOCTEXT("DispAlwaysOFF", "常に表示ＯＦＦ");
	}
	return LOCTEXT("DispAlwaysON", "常に表示ＯＮ　");
}

FText SCVTVolumeRendererItemRow::GetClassText() const
{
	return FText::FromString(Item->GetInfo().Class->GetName());
}

FSlateColor SCVTVolumeRendererItemRow::GetAlwaysButtonColor() const
{
	if (Item->IsAlways())
	{
		return FLinearColor(0.6f, 0.3f, 0.3f, 1.0f);
	}
	return FLinearColor::White;
}
FLinearColor SCVTVolumeRendererItemRow::GetDisplayColor() const
{
	return Item->GetInfo().DisplayColor;
}
void SCVTVolumeRendererItemRow::SetDisplayColor(FLinearColor InColor)
{
	Item->SetInfoDisplayColor(InColor);
}
TSharedRef<SWidget> SCVTVolumeRendererItemRow::CreateColorPicker()
{
	return SNew(SColorPicker)
	.TargetColorAttribute(this, &SCVTVolumeRendererItemRow::GetDisplayColor)
	.UseAlpha(true)
	.DisplayInlineVersion(true)
	.OnColorCommitted(this, &SCVTVolumeRendererItemRow::SetDisplayColor);
}

void SCVTVolumeRendererItemRow::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
}
void SCVTVolumeRendererItemRow::OnMouseLeave(const FPointerEvent& MouseEvent)
{
}

FReply SCVTVolumeRendererItemRow::ButtonDispAlwaysClicked()
{
	Item->SetAlways(!Item->IsAlways());
	Item->OnAlwaysON(Item);
	return FReply::Handled();
}
FReply SCVTVolumeRendererItemRow::ButtonDispOneShotClicked()
{
	Item->OnOneShot(Item);
	return FReply::Handled();
}
FReply SCVTVolumeRendererItemRow::ButtonRemoveClicked()
{
	Item->OnRemove(Item);
	return FReply::Handled();
}


#undef LOCTEXT_NAMESPACE

