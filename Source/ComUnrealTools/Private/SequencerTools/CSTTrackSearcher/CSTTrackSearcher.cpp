// Copyright com04 All Rights Reserved.

#include "CSTTrackSearcher.h"
#include "CSTTrackSearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"
#include "Utility/CUTAssetSearcher.h"

#include "AssetData.h"
#include "AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "LevelSequence.h"
#include "MovieScene.h"
#include "MovieSceneFolder.h"
#include "MovieSceneSection.h"
#include "MovieSceneTrack.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Notifications/SProgressBar.h"



#define LOCTEXT_NAMESPACE "CSTTrackSearcher"

////////////////////////////////////
// SCSTTrackSearcher
const FString SCSTTrackSearcher::DirectorySeparateString = FString(" / ");
FString SCSTTrackSearcher::SearchPath = FString("/Game/SandBox/SearchLS");
FString SCSTTrackSearcher::SearchValue = FString("");
ECheckBoxState SCSTTrackSearcher::CheckBoxUsePropertySearch = ECheckBoxState::Checked;


SCSTTrackSearcher::~SCSTTrackSearcher()
{
}

void SCSTTrackSearcher::Construct(const FArguments& InArgs)
{
	SCSTTrackSearcher* Self = this;
	bDirtySearchPath = true;
	bDirtySearchText = true;
	LevelSequenceSearcher.OnSearchEnd.BindLambda([Self]{Self->FinishSearch();});
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Search path
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 4.f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.f, 4.f, 12.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SearchPath", "Path"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("FindPath", "Enter level sequence path to find references..."))
				.InitialText(FText::FromString(SearchPath))
				.OnTextChanged(this, &SCSTTrackSearcher::OnSearchPathChanged)
			]
		]
		
		// Search box
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 4.f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.f, 4.f, 12.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SearchName", "Name"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("Find", "Enter track name, texture name to find references..."))
				.InitialText(FText::FromString(SearchValue))
				.OnTextChanged(this, &SCSTTrackSearcher::OnSearchTextChanged)
			]
		]
		
		// Option
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 4.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)

			// プロパティ検索
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(30.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SCheckBox)
				.IsChecked(CheckBoxUsePropertySearch)
				.OnCheckStateChanged(this, &SCSTTrackSearcher::OnCheckBoxUsePropertySearchChanged)
			]
			// override only
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("UsePropertySearch", "BPのプロパティも検索する"))
			]
		]
		
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 10.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			
			// Asset Check
			+ SHorizontalBox::Slot()
			.MaxWidth(300.f)
			[
				SAssignNew(SearchStartButton, SButton)
				.Text(LOCTEXT("SearchStartButton", "Search Start"))
				.OnClicked(this, &SCSTTrackSearcher::ButtonSearchStartClicked)
				.IsEnabled(false)
			]
			
			+ SHorizontalBox::Slot()
			.MaxWidth(50.f)
					
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCSTTrackSearcher::ButtonCopyClipBoardClicked)
				.IsEnabled(false)
			]
			
			+ SHorizontalBox::Slot()
			.MaxWidth(10.f)
			
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCSTTrackSearcher::ButtonExportTextClicked)
				.IsEnabled(false)
			]
			+ SHorizontalBox::Slot()
			.MaxWidth(10.f)
			
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ExportCsvButton, SButton)
				.Text(LOCTEXT("ExportCsv", "Export CSV"))
				.OnClicked(this, &SCSTTrackSearcher::ButtonExportCsvClicked)
				.IsEnabled(false)
			]
		]
		
		
		// Result tree
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(0.f, 4.f, 0.f, 0.f)
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
			[
				SAssignNew(TreeView, STreeView<TSharedPtr<FCSTTrackSearcherResult>>)
				.ItemHeight(24)
				.TreeItemsSource(&ItemsFound)
				.OnGenerateRow(this, &SCSTTrackSearcher::OnGenerateRow)
				.OnGetChildren(this, &SCSTTrackSearcher::OnGetChildren)
				.OnMouseButtonDoubleClick(this,&SCSTTrackSearcher::OnTreeSelectionDoubleClicked)
			]
		]
		
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 4.f, 0.f, 0.f)
		[
			SNew(SProgressBar)
			.Visibility(this, &SCSTTrackSearcher::GetProgressBarVisibility)
			.Percent(this, &SCSTTrackSearcher::GetProgressBarPercent)
		]
	];
	
	
	
	OnSearchPathChanged(FText::FromString(SearchPath));
	OnSearchTextChanged(FText::FromString(SearchValue));
}

FReply SCSTTrackSearcher::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}


// Search path --- Begin

/** text change event */
void SCSTTrackSearcher::OnSearchPathChanged(const FText& Text)
{
	SearchPath = Text.ToString();
	bDirtySearchPath = true;
	
	SearchStartButton->SetEnabled((!SearchPath.IsEmpty() && !SearchValue.IsEmpty()));
}

/** text change event */
void SCSTTrackSearcher::OnSearchTextChanged(const FText& Text)
{
	SearchValue = Text.ToString();
	bDirtySearchText = true;
	
	SearchStartButton->SetEnabled((!SearchPath.IsEmpty() && !SearchValue.IsEmpty()));
}
// Search  --- End

void SCSTTrackSearcher::OnCheckBoxUsePropertySearchChanged(ECheckBoxState InValue)
{
	CheckBoxUsePropertySearch = InValue;
}

/** Search */
void SCSTTrackSearcher::SearchStart()
{
	// 前回の検索結果をクリアする
	for (auto It(ItemsFound.CreateIterator()); It; ++It)
	{
		TreeView->SetItemExpansion(*It, false);
	}
	ItemsFound.Empty();		

	HighlightText = FText::FromString(SearchValue);

	// search path parse
	if (bDirtySearchPath)
	{
		FCUTUtility::SplitStringTokens(SearchPath, &SearchPathTokens);
		bDirtySearchPath = false;
	}
	
	// search text parse
	if (bDirtySearchText)
	{
		FCUTUtility::SplitStringTokens(SearchValue, &SearchTokens);
		bDirtySearchText = false;
	}
	
	// search
	if (SearchTokens.Num() > 0)
	{
		TArray<UClass*> SearchClass = {ULevelSequence::StaticClass()};
		LevelSequenceSearcher.SearchStart(SearchPathTokens, TArray<FString>(),
				true, SearchClass);
	}
	else
	{
		LevelSequenceSearcher.Reset();
		FinishSearch();
	}
}


/** search finish callback */
void SCSTTrackSearcher::FinishSearch()
{
	const TArray<FAssetData>& LevelSequenceAsset = LevelSequenceSearcher.GetAssets(0);
	for (const FAssetData& AssetData : LevelSequenceAsset)
	{
		FString AssetPathString = AssetData.ObjectPath.ToString();

		ULevelSequence* LevelSequence = FindObject<ULevelSequence>(NULL, *AssetPathString);
		if (LevelSequence == nullptr) continue;
		

		UMovieScene* MovieScene = LevelSequence->MovieScene;
		if (MovieScene)
		{
			TArray<FGuid> FinishFindGuids;
			TSharedPtr<FCSTTrackSearcherResult> RootSearchResult(new FCSTTrackSearcherResult(FText::FromName(AssetData.PackageName), LevelSequence));
		
			// ここでの構築
			// |- フォルダ
			// |- BP
			// |- トラック
			
			// フォルダ
			const TArray<UMovieSceneFolder*>& Folders = MovieScene->GetRootFolders();
			for (UMovieSceneFolder* Folder : Folders)
			{
				SearchMovieSceneFolder(MovieScene, Folder, RootSearchResult, FString(), FinishFindGuids);
			}
			
			// トップに配置されたBP
			const TArray<FMovieSceneBinding>& Bindings = MovieScene->GetBindings();
			for (const FMovieSceneBinding& Binding : Bindings)
			{
				SearchMovieSceneBinding(MovieScene, &Binding, RootSearchResult, FString(), FinishFindGuids);
			}
			
			// トップに配置されたTrack
			const TArray<UMovieSceneTrack*>& Tracks = MovieScene->GetMasterTracks();
			for (UMovieSceneTrack* Track : Tracks)
			{
				SearchMovieSceneTrack(MovieScene, Track, RootSearchResult, FString(), FinishFindGuids);
			}

			if (RootSearchResult->GetChildren().Num() > 0)
			{
				ItemsFound.Add(RootSearchResult);
			}
		}
	}
	
	// not found
	if (ItemsFound.Num() == 0)
	{
		ItemsFound.Add(TSharedPtr<FCSTTrackSearcherResult>(new FCSTTrackSearcherResult(LOCTEXT("ResultNoResults", "No Results found"))));
		CopyClipBoardButton->SetEnabled(false);
		ExportTextButton->SetEnabled(false);
		ExportCsvButton->SetEnabled(false);
	}
	else
	{
		CopyClipBoardButton->SetEnabled(true);
		ExportTextButton->SetEnabled(true);
		ExportCsvButton->SetEnabled(true);
	}

	TreeView->RequestTreeRefresh();

	for (const TSharedPtr<FCSTTrackSearcherResult>& Item : ItemsFound)
	{
		TreeView->SetItemExpansion(Item, true);
	}
}
void SCSTTrackSearcher::SearchMovieSceneFolder(UMovieScene* InMovieScene, const UMovieSceneFolder* InFolder, TSharedPtr<FCSTTrackSearcherResult> InParentResult, const FString& InParentString, TArray<FGuid>& OutFindGuids)
{
	if (!InFolder)
	{
		return;
	}
	
	// ここでの構築
	// |- フォルダ名
	// |- フォルダ名 / 子フォルダ名
	// |- フォルダ名 / BP名
	// |- フォルダ名 / 子トラック
	
	// フォルダ名
	const FString FolderName = InParentString + InFolder->GetFolderName().ToString();
	if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, FolderName))
	{
		TSharedPtr<FCSTTrackSearcherResult> NewResult(new FCSTTrackSearcherResult(FText::FromString(FolderName), InParentResult, InFolder));
		InParentResult->AddChild(NewResult);
	}
	
	const FString FolderParentName = FolderName + DirectorySeparateString;

	// 子フォルダ
	const TArray<UMovieSceneFolder*>& ChildFolders = InFolder->GetChildFolders();
	for (UMovieSceneFolder* ChildFolder : ChildFolders)
	{
		SearchMovieSceneFolder(InMovieScene, ChildFolder, InParentResult, FolderParentName, OutFindGuids);
	}
	
	// フォルダ内のBP
	const TArray<FGuid>& ChildBindingGuids = InFolder->GetChildObjectBindings();
	for (const FGuid& ChildBindingGuid : ChildBindingGuids)
	{
		FMovieSceneBinding* ChildBinding = InMovieScene->FindBinding(ChildBindingGuid);
		if (ChildBinding)
		{
			SearchMovieSceneBinding(InMovieScene, ChildBinding, InParentResult, FolderParentName, OutFindGuids);
		}
	}
	
	// 子トラック
	const TArray<UMovieSceneTrack*>& ChildTracks = InFolder->GetChildMasterTracks();
	for (UMovieSceneTrack* ChildTrack : ChildTracks)
	{
		SearchMovieSceneTrack(InMovieScene, ChildTrack, InParentResult, FolderParentName, OutFindGuids);
	}
}
void SCSTTrackSearcher::SearchMovieSceneBinding(UMovieScene* InMovieScene, const FMovieSceneBinding* InBinding, TSharedPtr<FCSTTrackSearcherResult> InParentResult, const FString& InParentString, TArray<FGuid>& OutFindGuids)
{
	if (!InBinding)
	{
		return;
	}

	const FGuid& BindingGuid = InBinding->GetObjectGuid();
	if (OutFindGuids.Find(BindingGuid) != INDEX_NONE)
	{
		return;
	}
	OutFindGuids.Add(BindingGuid);
	
	// ここでの構築
	// |- BP名
	//   |- プロパティ
	//   |- Track名
	
	
	// BP内のトラックに有るかを先に調べる
	TArray<UMovieSceneTrack*> FindTracks;
	const TArray<UMovieSceneTrack*>& Tracks = InBinding->GetTracks();
	for (UMovieSceneTrack* Track : Tracks)
	{
		const FString TrackName = Track->GetDisplayName().ToString();
		if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, TrackName))
		{
			FindTracks.Add(Track);
		}
	}
	
	// BP名
	TArray<FString> FindProperty;
	TSharedPtr<FCSTTrackSearcherResult> ObjectResult = InParentResult;
	{
		TArray<FString> ObjectNames;

		FMovieSceneSpawnable* Spawnable = InMovieScene->FindSpawnable(BindingGuid);
		if (Spawnable)
		{
			// Track上で見える名前
			ObjectNames.Add(Spawnable->GetName());
		}
		// 内部的な名前
		ObjectNames.Add(InBinding->GetName());

		// BPのプロパティ
		if ((CheckBoxUsePropertySearch == ECheckBoxState::Checked) && Spawnable->GetObjectTemplate())
		{
			UObject* SpwanableObject = Spawnable->GetObjectTemplate();
			for (TFieldIterator<FProperty> PropertyIterator(SpwanableObject->GetClass()); PropertyIterator; ++PropertyIterator)
			{
				FCUTUtility::SearchProperty(SpwanableObject, *PropertyIterator, SearchTokens, false, [&FindProperty](const FProperty& InProperty, const FString& ValueString)
						{
							FindProperty.Add(ValueString);
						});
			}
		}
		for (const FString& Name : ObjectNames)
		{
			// Track側に一致が有る or プロパティ側に一致が有る or このBP自体がマッチしているか
			if ((FindTracks.Num() > 0) || (FindProperty.Num() > 0) || FCUTUtility::StringMatchesSearchTokens(SearchTokens, Name))
			{
				ObjectResult = MakeShareable(new FCSTTrackSearcherResult(FText::FromString(InParentString + Name), InParentResult, BindingGuid));
				InParentResult->AddChild(ObjectResult);
				break;
			}
		}
	}
	
	// BPのプロパティ
	for (const FString& PropertyString : FindProperty)
	{
		TSharedPtr<FCSTTrackSearcherResult> NewResult(new FCSTTrackSearcherResult(FText::FromString(PropertyString), ObjectResult, ECSTTrackSearcherResultType::Property));
		ObjectResult->AddChild(NewResult);
	}
	// Track名
	for (UMovieSceneTrack* Track : FindTracks)
	{
		const FString TrackName = Track->GetDisplayName().ToString();
		TSharedPtr<FCSTTrackSearcherResult> NewResult(new FCSTTrackSearcherResult(FText::FromString(TrackName), ObjectResult, Track));
		ObjectResult->AddChild(NewResult);
	}
}
void SCSTTrackSearcher::SearchMovieSceneTrack(UMovieScene* InMovieScene, const UMovieSceneTrack* InTrack, TSharedPtr<FCSTTrackSearcherResult> InParentResult, const FString& InParentString, TArray<FGuid>& OutFindGuids)
{
	if (!InTrack)
	{
		return;
	}
	const FGuid TrackGuid = InTrack->FindObjectBindingGuid();
	if (OutFindGuids.Find(TrackGuid) != INDEX_NONE)
	{
		return;
	}
	OutFindGuids.Add(TrackGuid);
	
	const FString TrackName = InTrack->GetDisplayName().ToString();
	if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, TrackName))
	{
		TSharedPtr<FCSTTrackSearcherResult> NewResult(new FCSTTrackSearcherResult(FText::FromString(InParentString + TrackName), InParentResult, InTrack));
		InParentResult->AddChild(NewResult);
	}
}
// Search box --- End




// Result Tree --- Begin

TSharedRef<ITableRow> SCSTTrackSearcher::OnGenerateRow(TSharedPtr<FCSTTrackSearcherResult> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FCSTTrackSearcherResult>>, OwnerTable)
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(InItem->GetBackgroundBrushName()))
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SNew(SHorizontalBox)
				
				// icon
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					InItem->CreateIcon()
				]
				
				// data name
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(2,0)
				[
					SNew(STextBlock)
					.Text(InItem->GetDisplayText())
					.ColorAndOpacity(FLinearColor::White)
					.HighlightText(HighlightText)
				]
				
				// option
				+SHorizontalBox::Slot()
				.FillWidth(1)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(2,0)
				[
					SNew(STextBlock)
					.Text(InItem->GetOptionText())
					.ColorAndOpacity(FLinearColor::Yellow)
					.HighlightText(HighlightText)
				]
			]
		];
}
void SCSTTrackSearcher::OnGetChildren(TSharedPtr<FCSTTrackSearcherResult> InItem, TArray<TSharedPtr<FCSTTrackSearcherResult>>& OutChildren)
{
	OutChildren += InItem->GetChildren();
}

void SCSTTrackSearcher::OnTreeSelectionDoubleClicked(TSharedPtr<FCSTTrackSearcherResult> Item)
{
	if(Item.IsValid())
	{
		Item->OnClick();
	}
}

// Result Tree --- End

// Progress Bar --- Begin

EVisibility SCSTTrackSearcher::GetProgressBarVisibility() const
{
	return LevelSequenceSearcher.IsAsyncLoading() ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SCSTTrackSearcher::GetProgressBarPercent() const
{
	if (!LevelSequenceSearcher.IsAsyncLoading())
	{
		return 1.0f;
	}
	
	return LevelSequenceSearcher.GetProgress();
}

/* SearchSteart clicked event */
FReply SCSTTrackSearcher::ButtonSearchStartClicked()
{
	SearchStart();
	return FReply::Handled();
}

/* CopyClipboard clicked event */
FReply SCSTTrackSearcher::ButtonCopyClipBoardClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportClipboard(Clipboard);
	
	return FReply::Handled();
}

/* ExportText clicked event */
FReply SCSTTrackSearcher::ButtonExportTextClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportTxt("CSTTrackSearcher", "CSTTrackSearcher.txt", Clipboard, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

/* ExportCsv clicked event */
FReply SCSTTrackSearcher::ButtonExportCsvClicked()
{
	FString Clipboard = GetClipboardCsvText();
	FCUTUtility::ExportTxt("CSTTrackSearcher", "CSTTrackSearcher.csv", Clipboard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}

FString SCSTTrackSearcher::GetClipboardText()
{
	FString RetString;
	RetString = FString::Printf(TEXT("Search Path: %s\n"), *SearchPath);
	RetString += FString::Printf(TEXT("Search Name: %s\n\n"), *SearchValue);
	
	
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromNodeSearcherResult(*It, 0, &RetString);
	}
	return RetString;
}
FString SCSTTrackSearcher::GetClipboardCsvText()
{
	FString RetString;
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardCsvTextFromNodeSearcherResult(*It, &RetString);
	}
	return RetString;
}

/**
 * Add clip board string 
 */
void SCSTTrackSearcher::AddClipboardTextFromNodeSearcherResult(const TSharedPtr<FCSTTrackSearcherResult>& Result, int32 Indent, FString* ExportText)
{
	FString TypeString = Result->GetTypeString();
	if (!TypeString.IsEmpty())
	{
		TypeString.Append(TEXT(": "));
	}
	*ExportText += FString::Printf(TEXT("%s- %s%s\n"), *FString::ChrN(Indent, TEXT(' ')), *TypeString, *Result->GetDisplayText().ToString());
	
	for (auto It = Result->GetChildren().CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromNodeSearcherResult(*It, Indent + 2, ExportText);
	}
}



/**
 * Add clip board string 
 */
void SCSTTrackSearcher::AddClipboardCsvTextFromNodeSearcherResult(const TSharedPtr<FCSTTrackSearcherResult>& Result, FString* ExportText)
{
	FString TypeString = Result->GetTypeString();
	*ExportText += FString::Printf(TEXT("%s,%s\n"), *TypeString, *Result->GetDisplayText().ToString());
	
	for (auto It = Result->GetChildren().CreateConstIterator() ; It ; ++It)
	{
		AddClipboardCsvTextFromNodeSearcherResult(*It, ExportText);
	}
}




// Progress Bar --- End

#undef LOCTEXT_NAMESPACE

