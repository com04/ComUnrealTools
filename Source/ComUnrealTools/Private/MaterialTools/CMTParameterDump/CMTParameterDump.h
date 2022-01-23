// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"


typedef TSharedPtr<class FCMTParameterDumpResult> FCMTParameterDumpResultShare;
typedef STreeView<FCMTParameterDumpResultShare>  SCMTParameterDumpViewType;

struct FAssetData;
class SButton;
class SCheckBox;
class SScrollBox;
class SHorizontalBox;
class FAssetThumbnail;
class FAssetThumbnailPool;
class UMaterialInterface;
class UMaterialFunction;

/**
 * Implements the launcher application
 */
class SCMTParameterDump
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCMTParameterDump) { }
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
	~SCMTParameterDump();

public:

protected:
	
	// Asset Select --------
	
	/** EntryBox asset filtering */
	bool ShouldFilterAsset(const FAssetData& AssetData);
	/** EntryBox get display path */
	FString GetObjectPath() const;
	/** EntryBox change object event */
	void OnObjectChanged(const FAssetData& AssetData);
	
	// -------- Asset Select
	
	// Check box --- Begin
	
	/** "check material instance" checkbox changed callback */
	void OnDispOverrideOnlyStateChanged(ECheckBoxState InValue) { DispOverrideOnlyState = InValue; }
	
	// Check box --- End
	
	// Buttons --------
	
	/** AssetCheckButton clicked event */
	FReply ButtonAssetCheckClicked();
	
	/** CopyClipboard clicked event */
	FReply ButtonCopyClipBoardClicked();
	
	/** ExportText clicked event */
	FReply ButtonExportTextClicked();
	
	/** ExportCsv clicked event */
	FReply ButtonExportCsvClicked();
	
	// -------- Buttons
	
	
	// Result Tree --------
	
	/** row generate */
	TSharedRef<ITableRow> OnGenerateRow(FCMTParameterDumpResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable);
	/** tree child */
	void OnGetChildren(FCMTParameterDumpResultShare InItem, TArray<FCMTParameterDumpResultShare>& OutChildren);
	/** item double click event */
	void OnTreeSelectionDoubleClicked(FCMTParameterDumpResultShare Item);
	
	// -------- Result Tree
	
	void SetupClipboardText();
	void SetupCsvText();

private:
	
	
	/** result tree widget instance*/
	TSharedPtr<SCMTParameterDumpViewType> TreeView;
	
	/* Buttons */
	TSharedPtr<SButton> AssetCheckButton;
	TSharedPtr<SButton> CopyClipBoardButton;
	TSharedPtr<SButton> ExportTextButton;
	TSharedPtr<SButton> ExportCsvButton;
	
	
	/** check material */
	UMaterialInterface* SelectedMaterial;
	
	/** Find result list */
	TArray<FCMTParameterDumpResultShare> ItemsFound;
	
	
	/** チェックした時点のマテリアル名 */
	FString CheckMaterialName;
	/** Export Text */
	FString TextClipboard;
	/** Export Csv */
	FString CsvClipboard;
	
	/** "disp override only" result */
	static ECheckBoxState DispOverrideOnlyState;
};
