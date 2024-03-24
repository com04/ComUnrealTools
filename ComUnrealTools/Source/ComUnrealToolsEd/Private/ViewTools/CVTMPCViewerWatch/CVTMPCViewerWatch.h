// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"


typedef TSharedPtr<struct FCVTMPCViewerResult> FCVTMPCViewerWatchResultShare;
typedef STreeView<FCVTMPCViewerWatchResultShare>  SCVTMPCViewerWatchViewType;

struct FAssetData;
class SButton;
class SCheckBox;
class SScrollBox;
class SHorizontalBox;
class UCUTDeveloperSettings;
class UMaterialParameterCollection;

/**
 * Implements the launcher application
 */
class SCVTMPCViewerWatch
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCVTMPCViewerWatch) { }
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
	~SCVTMPCViewerWatch();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
public:
	/** エディタ終了時の現在環境の保存 */
	static void OnFinalizeEditorSettings(UCUTDeveloperSettings* Settings);

	static const FText& GetHeaderMPCNameText();
	static const FText& GetHeaderParameterNameText();
	static const FText& GetHeaderParameterDefaultValueText();
	static const FText& GetHeaderParameterValueText();
	static const FText& GetHeaderWatchEraseText();

	// ウォッチリストに追加
	static void AddWatchList(TWeakObjectPtr<UMaterialParameterCollection> InCollection, FName InParameterName, bool bInIsScaler);
	// ウォッチリストから削除
	static void RemoveWatchList(TWeakObjectPtr<UMaterialParameterCollection> InCollection, FName InParameterName, bool bInIsScaler);
protected:
	
	FReply ButtonClearWatchClicked();
	
	// Result List --------
	SHeaderRow::FColumn::FArguments CreateHeaderColumn(const FText& Name, int32 Width);
	TSharedRef<ITableRow> OnGenerateRowResultList(TSharedPtr<FCVTMPCViewerResult> Item, const TSharedRef<STableViewBase>& OwnerTable);
	// -------- Result List
	

private:

	// 結果の表ウィジェット
	TSharedPtr<SListView<FCVTMPCViewerWatchResultShare>> ResultView;

	// ウォッチするリスト
	static TArray<FCVTMPCViewerWatchResultShare> ResultList;
	static bool bRequestRefreshResult;

	// エディターの環境設定に変更があったか
	static bool bDirtyEditorSettings;
};
