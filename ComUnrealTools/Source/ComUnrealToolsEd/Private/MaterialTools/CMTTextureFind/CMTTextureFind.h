﻿// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MaterialTools/Utility/CMTMaterialSearcher.h"
#include "UnrealTools/CUTDeveloperSettings.h"

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
class FCMTTextureFindResultMaterialData;

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
	void OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType);
	/** text change event */
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
	void SearchTexture(FCMTTextureFindResultData* TextureData, UMaterialInterface* TargetMaterial);
	void SearchTexture(FCMTTextureFindResultData* TextureData, UMaterialFunction* TargetMaterial,
		TSharedPtr<FCMTTextureFindResultMaterialData> StackRoot,
		TSharedPtr<FCMTTextureFindResultMaterialData> StackEnd);
	TSharedPtr<FCMTTextureFindResultMaterialData> DeepCopyMaterialData(TSharedPtr<FCMTTextureFindResultMaterialData> Base);
	void GetTextureForMaterialFunction(TArray<UTexture*>* OutputTextures, UMaterialFunction* TargetMaterial);
	
	
	/** collect clipboard text */
	FString GetClipboardText();
	void AddClipboardText(const FCMTTextureFindResultData& TextureData);
	void AddClipboardTextFromMaterialData(const TSharedPtr<FCMTTextureFindResultMaterialData>& MaterialData, int32 Indent, FString* ExportText);
	
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
	
	/** 検索結果はアセット検索か。falseでディレクトリ検索	*/
	bool bResultSearchAsset;
	
	FString ClipboardText;
	
	/** search path text */
	CUT_DEVSETTINGS_STRING(SearchPath, CMTTextureFindSearchPath);
	
	/** search name text */
	CUT_DEVSETTINGS_STRING(SearchName, CMTTextureFindSearchName);
	
	/** "Check one asset" result */
	CUT_DEVSETTINGS_CHECKBOX(CheckBoxOneAsset, CMTTextureFindOneAsset);
	
	/** "Check Material" result */
	CUT_DEVSETTINGS_CHECKBOX(CheckBoxDirectoryMaterial, CMTTextureFindDirectoryMaterial);
	
	/** "Check Mateiral Function" result */
	CUT_DEVSETTINGS_CHECKBOX(CheckBoxDirectoryMaterialFunction, CMTTextureFindDirectoryMaterialFunction);
	
};
