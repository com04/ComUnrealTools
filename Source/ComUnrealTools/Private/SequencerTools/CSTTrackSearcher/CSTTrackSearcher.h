// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/StreamableManager.h"
#include "SlateFwd.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

#include "CSTTrackSearcherResult.h"
#include "MaterialTools/Utility/CMTMaterialSearcher.h"


struct FAssetData;
struct FMovieSceneBinding;
class SButton;
class UMovieScene;
class UMovieSceneFolder;
class UMovieSceneTrack;
class UMaterialExpressionComment;

/**
 * Implements the launcher application
 */
class SCSTTrackSearcher
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCSTTrackSearcher) { }
	SLATE_END_ARGS()

public:
	/**
	 * Constructs the application.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(const FArguments& InArgs);


	/** Destructor. */
	~SCSTTrackSearcher();

public:

protected:

	// SCompoundWidget overrides

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	
	// Search  --- Begin
	void OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType);
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	// Search  --- End
	
	// Check box --- Begin
	void OnCheckBoxUsePropertySearchChanged(ECheckBoxState InValue);
	// Check box --- End
	
	/** Search */
	void SearchStart();
	
	/** search finish callback */
	void FinishSearch();
	
	void SearchMovieSceneFolder(UMovieScene* InMovieScene, const UMovieSceneFolder* InFolder, TSharedPtr<FCSTTrackSearcherResult> InParentResult, const FString& InParentString, TArray<FGuid>& OutFindGuids);
	void SearchMovieSceneBinding(UMovieScene* InMovieScene, const FMovieSceneBinding* InBinding, TSharedPtr<FCSTTrackSearcherResult> InParentResult, const FString& InParentString, TArray<FGuid>& OutFindGuids);
	void SearchMovieSceneTrack(UMovieScene* InMovieScene, const UMovieSceneTrack* InTrack, TSharedPtr<FCSTTrackSearcherResult> InParentResult, const FString& InParentString, TArray<FGuid>& OutFindGuids);
	
	
	
	
	// Result Tree --- Begin
	
	/** row generate */
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FCSTTrackSearcherResult> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	/** tree child */
	void OnGetChildren(TSharedPtr<FCSTTrackSearcherResult> InItem, TArray<TSharedPtr<FCSTTrackSearcherResult>>& OutChildren);
	/** item double click event */
	void OnTreeSelectionDoubleClicked(TSharedPtr<FCSTTrackSearcherResult> Item);
	
	// Result Tree --- End
	
	// Progress Bar --- Begin
	
	/** progress bar is now search? */
	EVisibility GetProgressBarVisibility() const;
	
	/** progress bar progress */
	TOptional<float> GetProgressBarPercent() const;

	// Progress Bar --- End
	
	
	// Button --- Begin
	
	/** SearchSteart clicked event */
	FReply ButtonSearchStartClicked();
	
	/** CopyClipboard clicked event */
	FReply ButtonCopyClipBoardClicked();
	
	/** ExportText clicked event */
	FReply ButtonExportTextClicked();
	
	/** ExportCsv clicked event */
	FReply ButtonExportCsvClicked();
	
	// Button --- End
	
	FString GetClipboardText();
	FString GetClipboardCsvText();
	void AddClipboardTextFromNodeSearcherResult(const TSharedPtr<FCSTTrackSearcherResult>& Result, int32 Indent, FString* ExportText);
	void AddClipboardCsvTextFromNodeSearcherResult(const TSharedPtr<FCSTTrackSearcherResult>& Result, FString* ExportText);
	
	
	
	/** result tree widget instance*/
	TSharedPtr<STreeView<TSharedPtr<FCSTTrackSearcherResult>>> TreeView;
	
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
	
	
	/** Asset search utility class */
	FCUTAssetSearcher LevelSequenceSearcher;
	
	
	/** Find result list */
	TArray<TSharedPtr<FCSTTrackSearcherResult>> ItemsFound;
	
	
	TSharedPtr<SButton> SearchStartButton;
	TSharedPtr<SButton> CopyClipBoardButton;
	TSharedPtr<SButton> ExportTextButton;
	TSharedPtr<SButton> ExportCsvButton;
	
	
	/** 表示上のディレクトリ階層区切り文字 */
	static const FString DirectorySeparateString;

	/** search path text */
	static FString SearchPath;
	
	/** search text */
	static FString SearchValue;
	
	static ECheckBoxState CheckBoxUsePropertySearch;
	
};
