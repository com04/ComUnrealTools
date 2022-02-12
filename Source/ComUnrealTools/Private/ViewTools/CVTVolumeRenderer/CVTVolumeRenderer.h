// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CVTVolumeRendererItem.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"


class SButton;
class STableViewBase;
class ITableRow;
class UCUTDeveloperSettings;



/**
 * Implements the launcher application
 */
class SCVTVolumeRenderer
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCVTVolumeRenderer) { }
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
	~SCVTVolumeRenderer();

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
public:
	/** エディタの環境設定での値変更時のコールバック */
	static void OnChangedEditorSettings(UCUTDeveloperSettings* Settings, struct FPropertyChangedEvent& Property);
	/** エディタ終了時の現在環境の保存 */
	static void OnFinalizeEditorSettings(UCUTDeveloperSettings* Settings);

protected:
	
	// Asset Select --------
	const UClass* OnGetClass() const;
	void OnSetClass(const UClass* InClass);
	// -------- Asset Select
	
	// Spin Box  --------
	void OnSpinBoxLineThicknessChanged(float InValue);
	void OnSpinBoxOneShotDurationChanged(float InValue);
	void OnSpinBoxRenderDistanceChanged(float InValue);
	// -------- Spin Box
	
	// Buttons --------
	FReply ButtonClassAddClicked();
	FReply ButtonClearItemsClicked();
	// -------- Buttons
	
	// Result --------
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FCVTVolumeRendererItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	// -------- Result
	
	void AddItem(const FCVTVolumeRendererItemInfo& InInfo);
	// アイテム一つの描画
	void RenderItem(UWorld* InWorld, TSharedPtr<FCVTVolumeRendererItem> InItem, float InDuration);
	
	// AlwaysがONに設定された時のコールバック
	void OnAlwaysON(TSharedPtr<FCVTVolumeRendererItem> Item);
	// OneShotが押された時のコールバック
	void OnOneShot(TSharedPtr<FCVTVolumeRendererItem> Item);
	// Removeが押された時のコールバック
	void OnRemove(TSharedPtr<FCVTVolumeRendererItem> Item);
	
	// 既に登録しているクラスか
	static bool FindClassInItemInfos(const TArray<FCVTVolumeRendererItemInfo>& InItemInfos, TSubclassOf<AActor> InClass);
private:
	TSharedPtr<STileView<TSharedPtr<FCVTVolumeRendererItem>>> ItemTileView;
	
	// 現在選択中のクラス
	TSubclassOf<AActor> SelectedClass;
	// 追加されたActorリスト
	TArray<TSharedPtr<FCVTVolumeRendererItem>> Items;
	
	// AlwaysがONになっているリスト
	TArray<TSharedPtr<FCVTVolumeRendererItem>> RequestAlwaysItems;
	// OneShotが押された～描画が走る前のリスト
	TArray<TSharedPtr<FCVTVolumeRendererItem>> RequestOneShotItems;
	
	// 線の太さ
	static float LineThickness;
	// OneShotの際の描画時間
	static float OneShotDuration;
	static TArray<FCVTVolumeRendererItemInfo> ItemInfos;
	static const TArray<FLinearColor> DefaultColorList;
	// 表示距離
	static float RenderDistance;

	// エディターの環境設定に変更があったか
	static bool bDirtyEditorSettings;
};
