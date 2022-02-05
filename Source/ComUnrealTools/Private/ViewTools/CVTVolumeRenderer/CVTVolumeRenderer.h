// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CVTVolumeRendererItem.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STileView.h"


class SButton;
class ITableRow;
class STableViewBase;



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

protected:
	
	// Asset Select --------
	const UClass* OnGetClass() const;
	void OnSetClass(const UClass* InClass);
	// -------- Asset Select
	
	// Spin Box  --------
	void OnSpinBoxLineThicknessChanged(float InValue);
	void OnSpinBoxOneShotDurationChanged(float InValue);
	// -------- Spin Box
	
	// Buttons --------
	FReply ButtonClassAddClicked();
	FReply ButtonClearItemsClicked();
	// -------- Buttons
	
	// Result --------
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FCVTVolumeRendererItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	// -------- Result
	
	void AddItem(const FCVTVolumeRendererItemInfo& InInfo);
	void RenderItem(UWorld* InWorld, TSharedPtr<FCVTVolumeRendererItem> InItem, float InDuration);
	void OnAlwaysON(TSharedPtr<FCVTVolumeRendererItem> Item);
	void OnOneShot(TSharedPtr<FCVTVolumeRendererItem> Item);
	
private:
	TSharedPtr<STileView<TSharedPtr<FCVTVolumeRendererItem>>> ItemTileView;
	
	TSubclassOf<AActor> SelectedClass;
	TArray<TSharedPtr<FCVTVolumeRendererItem>> Items;
	
	TArray<TSharedPtr<FCVTVolumeRendererItem>> RequestAlwaysItems;
	TArray<TSharedPtr<FCVTVolumeRendererItem>> RequestOneShotItems;
	
	static float LineThickness;
	static float OneShotDuration;
	static TArray<FCVTVolumeRendererItemInfo> ItemInfos;
	static const TArray<FLinearColor> DefaultColorList;
};
