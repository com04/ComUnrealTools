// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CVTVolumeRendererItem.h"
#include "UnrealTools/CUTDeveloperSettings.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"


class SButton;
template<typename NumericType> class SSpinBox;
class STableViewBase;
template <typename ItemType> class STileView;
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
public:
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
	
	// Always表示設定が切り替えされた時のコールバック
	void OnChangedAlways(TSharedPtr<FCVTVolumeRendererItem> Item);
	// OneShotが押された時のコールバック
	void OnOneShot(TSharedPtr<FCVTVolumeRendererItem> Item);
	// Removeが押された時のコールバック
	void OnRemove(TSharedPtr<FCVTVolumeRendererItem> Item);
	
	// Actor名テキスト更新コールバック
	void OnMatchActorNameCommitted(const FText& Text, ETextCommit::Type CommitType);
	// 既に登録しているクラスか
	static bool FindClassInItemInfos(const TArray<FCVTVolumeRendererItemInfo>& InItemInfos, TSubclassOf<AActor> InClass);
private:
	TSharedPtr<STileView<TSharedPtr<FCVTVolumeRendererItem>>> ItemTileView;
	TSharedPtr<SSpinBox<float>> SpinBoxLineThickness;
	TSharedPtr<SSpinBox<float>> SpinBoxOneShotDuration;
	TSharedPtr<SSpinBox<float>> SpinBoxRenderDistance;
	
	/// 現在選択中のクラス
	TSubclassOf<AActor> SelectedClass;
	/// 追加されたActorリスト
	TArray<TSharedPtr<FCVTVolumeRendererItem>> Items;
	
	/// 設定変更時のDelegateハンドル
	FDelegateHandle OnChangedEditorSettingsHandle;

	/// 外部で設定変更された際の更新処理中
	bool bExternalOnChanged;
	
	// 線の太さ
	CUT_DEVSETTINGS_FLOAT(LineThickness, CVTVolumeRendererLineThickness);
	// OneShotの際の描画時間
	CUT_DEVSETTINGS_FLOAT(OneShotDuration, CVTVolumeRendererOneShotDuration);
	// 表示距離
	CUT_DEVSETTINGS_FLOAT(RenderDistance, CVTVolumeRendererRenderDistance);

	static FString MatchActorName;
	static TArray<FCVTVolumeRendererItemInfo> ItemInfos;
	static const TArray<FLinearColor> DefaultColorList;

	// エディターの環境設定に変更があったか
	static bool bDirtyEditorSettings;
};
