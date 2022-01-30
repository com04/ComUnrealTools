// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"


typedef TSharedPtr<struct FCVTMPCViewerResult> FCVTMPCViewerResultShare;
typedef STreeView<FCVTMPCViewerResultShare>  SCVTMPCViewerViewType;

struct FAssetData;
class SButton;
class SCheckBox;
class SScrollBox;
class SHorizontalBox;
class UMaterialParameterCollection;

/**
 * Implements the launcher application
 */
class SCVTMPCViewer
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCVTMPCViewer) { }
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
	~SCVTMPCViewer();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
public:
	static const FText& GetHeaderParameterNameText();
	static const FText& GetHeaderParameterDefaultValueText();
	static const FText& GetHeaderParameterValueText();
	static const FText& GetHeaderWatchText();

protected:
	
	// Asset Select --------
	
	/** アセット選択のクラスフィルター */
	bool ShouldFilterAsset(const FAssetData& AssetData);
	/** 選択したアセット表示パス */
	FString GetObjectPath() const;
	/** アセット選択変更イベント */
	void OnObjectChanged(const FAssetData& AssetData);
	void SetupMPCResult();
	
	// -------- Asset Select
	
	
	// Text --------
	FText GetAssetPathText() const;
	FText GetRuntimeChangedText() const;
	
	// -------- Text
	
	
	// Buttons --------
	/* ウォッチに入れるボタン */
	FReply ButtonMPCWatchClicked();
	// -------- Buttons
	
	
	// Result List --------
	SHeaderRow::FColumn::FArguments CreateHeaderColumn(const FText& Name, int32 Width);
	TSharedRef<ITableRow> OnGenerateRowResultList(TSharedPtr<FCVTMPCViewerResult> Item, const TSharedRef<STableViewBase>& OwnerTable);
	// -------- Result List
	

private:

	TSharedPtr<SHorizontalBox> MPCNoChangedBox;
	// 結果の表ウィジェット
	TSharedPtr<SListView<TSharedPtr<FCVTMPCViewerResult>>> ResultView;
	// 結果の表の値リスト
	TArray<TSharedPtr<FCVTMPCViewerResult>> ResultList;

	// 
	FText AssetPathText;
	
	// PIE終了して値をリセットしたか
	bool bPIEFinishReset;

	static TWeakObjectPtr<UMaterialParameterCollection> SelectedMPC;
	
};
