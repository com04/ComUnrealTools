// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CVTVolumeRendererItem.h"

#include "Widgets/Views/STableRow.h"



/* Added Item */
class SCVTVolumeRendererItemRow : public STableRow<TSharedPtr<FCVTVolumeRendererItem>>
{
public:

	void Construct(const FArguments& InArgs, TSharedPtr<FCVTVolumeRendererItem> InItem, const TSharedRef<STableViewBase>& InOwnerTableView);


	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	
private:
	FText GetDispAlwaysText() const;
	FText GetClassText() const;
	FSlateColor GetAlwaysButtonColor() const;
	FLinearColor GetDisplayColor() const;
	void SetDisplayColor(FLinearColor InColor);
	TSharedRef<SWidget> CreateColorPicker();
	
	/* 常に表示ボタン */
	FReply ButtonDispAlwaysClicked();
	/* １回表示ボタン */
	FReply ButtonDispOneShotClicked();
	/* 解除ボタン */
	FReply ButtonRemoveClicked();

private:

	/** Item to display */
	TSharedPtr<FCVTVolumeRendererItem> Item;
};

