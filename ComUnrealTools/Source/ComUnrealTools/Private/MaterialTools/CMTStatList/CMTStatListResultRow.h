// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CMTStatListResult.h"

#include "Containers/Array.h"
#include "SlateFwd.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Views/STableRow.h"

class SBorder;
class STableViewBase;
class SWidget;

/** Implements a row widget for result list. */
class SCMTStatListResultRow : public SMultiColumnTableRow<TSharedPtr<FCMTStatListResult>>
{
public:

	SLATE_BEGIN_ARGS(SCMTStatListResultRow)	{}
		SLATE_ARGUMENT(TSharedPtr<FCMTStatListResult>, Info)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;
private:

	/** Result to display */
	TSharedPtr<FCMTStatListResult> Info;
	
	TArray<TSharedPtr<SBorder>>  BorderColor1List;
	TArray<TSharedPtr<SBorder>>  BorderColor2List;
};
