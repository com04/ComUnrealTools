// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "UnrealTools/CUTDeveloperSettings.h"


class SImage;
class UTextureRenderTarget2D;
struct FCVTRenderTargetViewerResult;


/**
 * Implements the launcher application
 */
class SCVTRenderTargetViewer
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCVTRenderTargetViewer) { }
	SLATE_END_ARGS()

public:
	/**
	 * Constructs the application.
	 *
	 * @param InArgs The Slate argument list.
	 * @param ConstructUnderMajorTab The major tab which will contain the session front-end.
	 * @param ConstructUnderWindow The window in which this widget is being constructed.
	 */
	void Construct(const FArguments& InArgs);

	/** Destructor. */
	~SCVTRenderTargetViewer();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
public:
	static FText GetHeaderNameText();
	static FText GetHeaderFormatText();
	static FText GetHeaderSizeText();
	static FText GetHeaderImageText();
	static FVector2D GetPreviewSizeV2();
	static bool IsPreviewWithoutAlpha();

protected:
	// 使用中のRenderTargetを検索して結果に反映する
	void SearchRenderTargets();
	// リザルト欄のPreview表示を現在の設定で更新する
	void RefreshResultPreview();
	
	// Check Box  --------
	void OnCheckPreviewWithoutAlphaChanged(ECheckBoxState InValue);
	// -------- Check Box
	// Spin Box  --------
	void OnSpinBoxPreviewSizeChanged(int32 InValue);
	// -------- Spin Box
	
	// Button  --------
	FReply ButtonRefreshClicked();
	// -------- Button
	
	// Result List --------
	SHeaderRow::FColumn::FArguments CreateHeaderColumn(const FText& Name, int32 Width);
	TSharedRef<ITableRow> OnGenerateRowResultList(TSharedPtr<FCVTRenderTargetViewerResult> Item, const TSharedRef<STableViewBase>& OwnerTable);
	// -------- Result List

private:
	// 結果の表ウィジェット
	TSharedPtr<SListView<TSharedPtr<FCVTRenderTargetViewerResult>>> ResultView;
	// 結果の表の値リスト
	TArray<TSharedPtr<FCVTRenderTargetViewerResult>> ResultList;
	
	// アルファ無しでプレビュー
	CUT_DEVSETTINGS_CHECKBOX(CheckPreviewWithoutAlpha, CVTRenderTargetViewerPreviewWithoutAlpha);
	// プレビューサイズ
	CUT_DEVSETTINGS_INT(PreviewSize, CVTRenderTargetViewerPreviewSize);
};
