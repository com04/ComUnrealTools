// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CVTRenderTargetViewerResult.h"

#include "Containers/Array.h"
#include "SlateFwd.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Views/STableRow.h"

class SBorder;
class STableViewBase;
class SWidget;
class UMaterialInstanceDynamic;

/** Implements a row widget for result list. */
class SCVTRenderTargetViewerResultRow : public SMultiColumnTableRow<TSharedPtr<FCVTRenderTargetViewerResult>>
{
public:

	SLATE_BEGIN_ARGS(SCVTRenderTargetViewerResultRow)	{}
		SLATE_ARGUMENT(TSharedPtr<FCVTRenderTargetViewerResult>, Info)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	END_SLATE_FUNCTION_BUILD_OPTIMIZATION

	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

	// Previewの画像表示を現在の設定で更新する
	void RefreshPreviewImage();
	
private:
	FSlateImageBrush* CreateSlateImageBrush();

private:

	/** Result to display */
	TSharedPtr<FCVTRenderTargetViewerResult> Info;
	TSharedPtr<SImage> PreviewImage;
	
	// 不透明表示用マテリアル
	UPROPERTY()
	UMaterialInstanceDynamic* OpaqueImage;
	
	TArray<TSharedPtr<SBorder>>  BorderColor1List;
	TArray<TSharedPtr<SBorder>>  BorderColor2List;
};
