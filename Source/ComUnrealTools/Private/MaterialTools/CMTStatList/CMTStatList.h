// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MaterialTools/Utility/CMTMaterialSearcher.h"

#include "Containers/Array.h"
#include "Engine/StreamableManager.h"
#include "SlateFwd.h"
#include "Templates/SharedPointer.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SHeaderRow.h"



struct FAssetData;
struct FCMTStatListResult;
class UMaterial;
class UMaterialInterface;


/**
 * Implements the launcher application
 */
class SCMTStatList
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCMTStatList) { }
	SLATE_END_ARGS()

public:
	/**
	 * Constructs the application.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(const FArguments& InArgs);


	/** Destructor. */
	~SCMTStatList();

public:

	static const FText& GetHeaderNameText();
	static FName GetHeaderNameTextName();
	static const FText& GetHeaderDomainText();
	static FName GetHeaderDomainTextName();
	static const FText& GetHeaderShadingModelText();
	static FName GetHeaderShadingModelTextName();
	static const FText& GetHeaderBlendModeText();
	static FName GetHeaderBlendModeTextName();
	static const FText& GetHeaderInstructionsText();
	static FName GetHeaderInstructionsTextName();
	static const FText& GetHeaderTextureNumText();
	static FName GetHeaderTextureNumTextName();
	static const FText& GetHeaderTextureSizeText();
	static FName GetHeaderTextureSizeTextName();
	static const FText& GetHeaderRenderAfterDOFText();
	static FName GetHeaderRenderAfterDOFTextName();
	static const FText& GetHeaderPositionOffsetText();
	static FName GetHeaderPositionOffsetTextName();
	static const FText& GetHeaderDepthOffsetText();
	static FName GetHeaderDepthOffsetTextName();
	static const FText& GetHeaderRefractionText();
	static FName GetHeaderRefractionTextName();

protected:

	// SCompoundWidget overrides

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	
	// Search box --- Begin
	
	/** text commit event */
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	
	/** search finish callback */
	void FinishSearch();
	
	/** result material infomation setup */
	void SetupMaterialResult(TSharedPtr<FCMTStatListResult>& Result, UMaterialInterface* SelfMaterial, UMaterial* MasterMaterial);

	// Search box --- End
	
	
	// Check box --- Begin
	
	/** "check material instance" checkbox changed callback */
	void OnCheckMaterialInstanceChanged(ECheckBoxState InValue) { CheckMaterialInstance = InValue; }
	
	// Check box --- End
	
	
	
	// Result list --- Begin
	
	/** create header one column widget */
	SHeaderRow::FColumn::FArguments CreateHeaderColumn(const FText& Name, int32 Width);
	
	/** Create row widget event */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FCMTStatListResult> Item, const TSharedRef<STableViewBase>& OwnerTable);

	/** get header sort setting */
	EColumnSortMode::Type GetHeaderSortMode(const FName ColumnName) const;
	
	/** get header sort setting */
	EColumnSortPriority::Type GetHeaderSortPriority(const FName ColumnName) const;
	
	/** header sort setting change event */
	void OnSortHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode);

	/** list sort */
	void SortList();
	
	/** check two args sort */
	int32 CheckSortResult(const TSharedPtr<FCMTStatListResult>& InsertResult, const TSharedPtr<FCMTStatListResult>& CheckResult, const FName& SortName);
	// Result list --- End
	
	
	
	// Progress Bar --- Begin
	
	/** progress bar is now search? */
	EVisibility GetProgressBarVisibility() const;
	
	/** progress bar progress */
	TOptional<float> GetProgressBarPercent() const;

	// Progress Bar --- End
	
	// Button --- Begin
	// Button --- Begin
	
	/** SearchStart clicked event */
	FReply ButtonSearchStartClicked();
	
	/** CopyClipboard clicked event */
	FReply ButtonCopyClipBoardClicked();
	
	/** ExportText clicked event */
	FReply ButtonExportTextClicked();
	
	/** ExportCSV clicked event */
	FReply ButtonExportCsvClicked();
	
	// Button --- End
	
	FString GetClipboardText();
	void AddClipboardTextFromStatListResult(const TSharedPtr<FCMTStatListResult>& Result, FString* ExportText);
	
	
	/** list sort name. EColumnSortPriority::Max */
	TArray<FName> 					ListSortName;
	
	/** list sort direction. EColumnSortPriority::Max */
	TArray<EColumnSortMode::Type>	ListSortMode;
	
	/** search string list. from SearchValue */
	TArray<FString> SearchTokens;
	
	/** material search utility class */
	FCMTMaterialSearcher MaterialSearcher;
	
	
	TSharedPtr<SButton> SearchStartButton;
	TSharedPtr<SButton> CopyClipBoardButton;
	TSharedPtr<SButton> ExportTextButton;
	TSharedPtr<SButton> ExportCsvButton;
	
	
	TSharedPtr<SListView<TSharedPtr<FCMTStatListResult>>> ResultView;
	TArray<TSharedPtr<FCMTStatListResult>> ResultList;
	
	
	/** search text */
	static FString SearchValue;
	
	/** "check material instance" result */
	static ECheckBoxState CheckMaterialInstance;
};
