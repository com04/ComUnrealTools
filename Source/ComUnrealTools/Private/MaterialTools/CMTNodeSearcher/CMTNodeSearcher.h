// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/StreamableManager.h"
#include "SlateFwd.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

#include "CMTNodeSearcherResult.h"
#include "MaterialTools/Utility/CMTMaterialSearcher.h"


typedef TSharedPtr<class FCMTNodeSearcherResult> FCMTNodeSearcherResultShare;
typedef STreeView<FCMTNodeSearcherResultShare>  SCMTNodeSearcherTreeViewType;
struct FAssetData;
class SButton;
class UMaterialExpressionComment;

/**
 * Implements the launcher application
 */
class SCMTNodeSearcher
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCMTNodeSearcher) { }
	SLATE_END_ARGS()

public:
	/**
	 * Constructs the application.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(const FArguments& InArgs);


	/** Destructor. */
	~SCMTNodeSearcher();

private:
	
	// Search  --- Begin
	void OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType);
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	// Search  --- End
	
	
	/** Search */
	void SearchStart();
	
	/** search finish callback */
	void FinishSearch();
	
	/** search match MaterialExpressions */
	bool MatchTokensFromAssets(const FAssetData& InAsset,
			const TArray<UMaterialExpression*>& InExpressions, const TArray<UMaterialExpressionComment*>& InCommentExpressions, 
			FCMTNodeSearcherResultShare InRoot);
		
	template<class ExpressionClass, class NameFunction>
	void MatchTokensFromExpressions(
			const TArray<ExpressionClass*>& InExpressions, FCMTNodeSearcherResultShare InRoot,
			FText InPrefix, NameFunction InFunctionGetName);
	
	
	
	// Check box --- Begin
	
	/** "check material instance" checkbox changed callback */
	void OnCheckMaterialInstanceChanged(ECheckBoxState InValue) { CheckMaterialInstance = InValue; }
	
	// Check box --- End
	
	
	
	// Result Tree --- Begin
	
	/** row generate */
	TSharedRef<ITableRow> OnGenerateRow(FCMTNodeSearcherResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable);
	/** tree child */
	void OnGetChildren(FCMTNodeSearcherResultShare InItem, TArray<FCMTNodeSearcherResultShare>& OutChildren);
	/** item double click event */
	void OnTreeSelectionDoubleClicked(FCMTNodeSearcherResultShare Item);
	
	// Result Tree --- End
	
	// Progress Bar --- Begin
	
	/** progress bar is now search? */
	EVisibility GetProgressBarVisibility() const;
	
	/** progress bar progress */
	TOptional<float> GetProgressBarPercent() const;

	// Progress Bar --- End
	
	
	// Button --- Begin
	
	/** SearchStart clicked event */
	FReply ButtonSearchStartClicked();
	
	/** CopyClipboard clicked event */
	FReply ButtonCopyClipBoardClicked();
	
	/** ExportText clicked event */
	FReply ButtonExportTextClicked();
	
	/** ExportCsv clicked event */
	FReply ButtonExportCsvClicked();
	
	// Button --- End
	
	FString GetClipboardText();
	FString GetClipboardCsv();
	void AddClipboardTextFromResult(const FCMTNodeSearcherResultShare& Result, int32 Indent, FString* ExportText);
	void AddClipboardCsvFromResult(const FCMTNodeSearcherResultShare& Result, FString* ExportText);
	
	
	TSharedPtr<SButton> SearchStartButton;
	TSharedPtr<SButton> CopyClipBoardButton;
	TSharedPtr<SButton> ExportTextButton;
	TSharedPtr<SButton> ExportCsvButton;
	
	/** result tree widget instance*/
	TSharedPtr<SCMTNodeSearcherTreeViewType> TreeView;
	
	/** changed SearchPath */
	bool bDirtySearchPath;
	
	/** changed SearchPath */
	bool bDirtySearchText;
	
	/** search string list. from SearchPath */
	TArray<FString> SearchPathTokens;
	
	/** search string list. from SearchValue */
	TArray<FString> SearchTokens;
	
	
	/** Result tree highlighting text (= Search text) */
	FText HighlightText;
	
	
	/** material search utility class */
	FCMTMaterialSearcher MaterialSearcher;
	
	
	/** Find result list */
	TArray<FCMTNodeSearcherResultShare> ItemsFound;
	
	
	
	
	/** search path text */
	static FString SearchPath;
	
	/** search text */
	static FString SearchValue;
	
	/** "check material instance" result */
	static ECheckBoxState CheckMaterialInstance;
};
