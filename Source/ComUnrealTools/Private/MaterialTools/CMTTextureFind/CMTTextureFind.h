// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MaterialTools/Utility/CMTMaterialSearcher.h"

#include "SlateFwd.h"
#include "Styling/SlateTypes.h"
#include "Widgets/SCompoundWidget.h"


struct FAssetData;
class FAssetThumbnail;
class FAssetThumbnailPool;
class SButton;
class SCheckBox;
class SHorizontalBox;
class SScrollBox;
class UMaterialFunction;
class UMaterialInterface;

class FCMTTextureFindResultData;

/**
 * Implements the launcher application
 */
class SCMTTextureFind
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCMTTextureFind) { }
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
	~SCMTTextureFind();

public:

protected:

	/** EntryBox asset filtering */
	bool ShouldFilterAsset(const FAssetData& AssetData);
	/** EntryBox get display path */
	FString GetObjectPath() const;
	/** EntryBox change object event */
	void OnObjectChanged(const FAssetData& AssetData);
	
	/** Material Thumbnail double click */
	FReply OnMaterialThumbnailMouseDoubleClick(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent);
	
	/** AssetCheckButton clicked event */
	FReply ButtonAssetCheckClicked();
	
	/** CopyClipboard clicked event */
	FReply ButtonCopyClipBoardClicked();
	
	/** ExportText clicked event */
	FReply ButtonExportTextClicked();
	
	
	// Search box --- Begin
	
	/** text change event */
	void OnSearchPathChanged(const FText& Text);
	/** text commit event */
	void OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType);
	
	/** text change event */
	void OnSearchTextChanged(const FText& Text);
	/** text commit event */
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	
	// Search box --- End
	
	
	// Check box --- Begin
	
	/** "Check one asset" checkbox changed callback */
	void OnCheckOneAssetChanged(ECheckBoxState InValue);
	/** "Check directory" checkbox changed callback */
	void OnCheckDirectoryChanged(ECheckBoxState InValue);
	/** "check material" checkbox changed callback */
	void OnCheckDirectoryMaterialChanged(ECheckBoxState InValue);
	/** "check material function" checkbox changed callback */
	void OnCheckDirectoryMaterialFunctionChanged(ECheckBoxState InValue);
	
	// Check box --- End
	
private:
	/** "Asset Check" Button check status enabled */
	void CheckAssetCheckButton();
	
	/** search finish callback */
	void FinishSearch();
	
	/** search  material */
	void SearchTextureInMaterial(TArray<UMaterialInterface*>& TargetMaterial,
			TArray<UMaterialFunction*>& TargetMaterialFunction);
	
	
	/** collect clipboard text */
	void AddClipboardText(const FCMTTextureFindResultData& TextureData);
	
private:
	
	/** search string list. from SearchPath */
	TArray<FString> SearchPathTokens;
	
	/** search string list. from SearchValue */
	TArray<FString> SearchTokens;
	
	/** OneAsset CheckBox */
	TSharedPtr<SCheckBox> CheckBoxOneAssetWidget;
	/** Directory CheckBox */
	TSharedPtr<SCheckBox> CheckBoxDirectoryWidget;
	/** OneAsset HorizontalBox */
	TSharedPtr<SHorizontalBox> BoxOneAssetWidget;
	/** Directory HorizontalBox */
	TSharedPtr<SHorizontalBox> BoxDirectoryWidget;
	
	
	TSharedPtr<FAssetThumbnailPool> AssetThumbnailPool;
	TSharedPtr<FAssetThumbnail> SelectedThumbnail;
	
	TSharedPtr<SButton> AssetCheckButton;
	TSharedPtr<SButton> CopyClipBoardButton;
	TSharedPtr<SButton> ExportTextButton;
	
	TSharedPtr<SScrollBox> ResultBox;
	
	UMaterialInterface* SelectedMaterial;
	UMaterialFunction* SelectedMaterialFunction;
	
	
	/** material search utility class */
	FCMTMaterialSearcher MaterialSearcher;
	
	/** Export Text */
	FString TextClipboard;
	
	
	/** search path text */
	static FString SearchPath;
	
	/** search name text */
	static FString SearchName;
	
	/** "Check one asset" result */
	static ECheckBoxState CheckBoxOneAsset;
	
	/** "Check directory" result */
	static ECheckBoxState CheckBoxDirectory;
	
	/** "Check Material" result */
	static ECheckBoxState CheckBoxDirectoryMaterial;
	
	/** "Check Mateiral Function" result */
	static ECheckBoxState CheckBoxDirectoryMaterialFunction;
	
};
