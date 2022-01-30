// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CVTMPCViewerWatch.h"

#include "Containers/Array.h"
#include "SlateFwd.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Views/STableRow.h"

class SBorder;
class STableViewBase;
class SWidget;

/** Implements a row widget for result list. */
class SCVTMPCViewerWatchResultRow : public SMultiColumnTableRow<TSharedPtr<FCVTMPCViewerResult>>
{
public:

	SLATE_BEGIN_ARGS(SCVTMPCViewerWatchResultRow)	{}
		SLATE_ARGUMENT(TSharedPtr<FCVTMPCViewerResult>, Info)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;

	
private:
	FText GetParameterDefaultValueText() const;
	FText GetParameterValueText() const;
	FSlateColor GetParameterValueColor() const;
	/* ウォッチ解除ボタン */
	FReply ButtonWatchClearClicked();

private:

	/** Result to display */
	TSharedPtr<FCVTMPCViewerResult> Info;
	
	TArray<TSharedPtr<SBorder>>  BorderColor1List;
	TArray<TSharedPtr<SBorder>>  BorderColor2List;
};
