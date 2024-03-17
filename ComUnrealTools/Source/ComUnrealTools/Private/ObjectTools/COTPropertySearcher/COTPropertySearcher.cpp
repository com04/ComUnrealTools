// Copyright com04 All Rights Reserved.

#include "COTPropertySearcher.h"
#include "COTPropertySearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "UnrealTools/CUTDeveloperSettings.h"
#include "Utility/CUTAssetSearcher.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "LevelSequence.h"
#include "MovieScene.h"
#include "MovieSceneFolder.h"
#include "MovieSceneSection.h"
#include "MovieSceneTrack.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Notifications/SProgressBar.h"




#define LOCTEXT_NAMESPACE "COTPropertySearcher"

////////////////////////////////////
// SCOTPropertySearcher
const FString SCOTPropertySearcher::DirectorySeparateString = FString(" / ");


SCOTPropertySearcher::~SCOTPropertySearcher()
{
}

void SCOTPropertySearcher::Construct(const FArguments& InArgs)
{
	SCOTPropertySearcher* Self = this;
	bDirtySearchPath = true;
	bDirtySearchText = true;
	AssetSearcher.OnSearchEnd.BindLambda([Self]{Self->FinishSearch();});
	
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
				.Text(LOCTEXT("SearchPath", "Search Path"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("FindPath", "Enter level sequence path to find references..."))
				.InitialText(FText::FromString(GetSearchPath()))
				.OnTextCommitted(this, &SCOTPropertySearcher::OnSearchPathCommitted)
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
				.Text(LOCTEXT("SearchName", "Search Name"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("Find", "Enter property value..."))
				.InitialText(FText::FromString(GetSearchValue()))
				.OnTextCommitted(this, &SCOTPropertySearcher::OnSearchTextCommitted)
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
				.OnClicked(this, &SCOTPropertySearcher::ButtonSearchStartClicked)
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
				.OnClicked(this, &SCOTPropertySearcher::ButtonCopyClipBoardClicked)
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
				.OnClicked(this, &SCOTPropertySearcher::ButtonExportTextClicked)
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
				.OnClicked(this, &SCOTPropertySearcher::ButtonExportCsvClicked)
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
				SAssignNew(TreeView, STreeView<TSharedPtr<FCOTPropertySearcherResult>>)
				.ItemHeight(24)
				.TreeItemsSource(&ItemsFound)
				.OnGenerateRow(this, &SCOTPropertySearcher::OnGenerateRow)
				.OnGetChildren(this, &SCOTPropertySearcher::OnGetChildren)
				.OnMouseButtonDoubleClick(this,&SCOTPropertySearcher::OnTreeSelectionDoubleClicked)
			]
		]
		
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 4.f, 0.f, 0.f)
		[
			SNew(SProgressBar)
			.Visibility(this, &SCOTPropertySearcher::GetProgressBarVisibility)
			.Percent(this, &SCOTPropertySearcher::GetProgressBarPercent)
		]
	];
	
	
	
	OnSearchPathCommitted(FText::FromString(GetSearchPath()), ETextCommit::OnEnter);
	OnSearchTextCommitted(FText::FromString(GetSearchValue()), ETextCommit::OnEnter);
}

FReply SCOTPropertySearcher::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}


// Search path --- Begin

/** text change event */
void SCOTPropertySearcher::OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchPath(Text.ToString());
	bDirtySearchPath = true;
	
	SearchStartButton->SetEnabled((!GetSearchPath().IsEmpty() && !GetSearchValue().IsEmpty()));
}

/** text change event */
void SCOTPropertySearcher::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchValue(Text.ToString());
	bDirtySearchText = true;
	
	SearchStartButton->SetEnabled((!GetSearchPath().IsEmpty() && !GetSearchValue().IsEmpty()));
}
// Search  --- End

/** Search */
void SCOTPropertySearcher::SearchStart()
{
	// 前回の検索結果をクリアする
	for (auto It(ItemsFound.CreateIterator()); It; ++It)
	{
		TreeView->SetItemExpansion(*It, false);
	}
	ItemsFound.Empty();		

	HighlightText = FText::FromString(GetSearchValue());

	// search path parse
	if (bDirtySearchPath)
	{
		FCUTUtility::SplitStringTokens(GetSearchPath(), &SearchPathTokens);
		bDirtySearchPath = false;
	}
	
	// search text parse
	if (bDirtySearchText)
	{
		FCUTUtility::SplitStringTokens(GetSearchValue(), &SearchTokens);
		bDirtySearchText = false;
	}
	
	// search
	if (SearchTokens.Num() > 0)
	{
		TArray<UClass*> SearchClass = {UBlueprint::StaticClass(), UMaterial::StaticClass()};
		AssetSearcher.SearchStart(SearchPathTokens, TArray<FString>(),
				true, SearchClass);
	}
	else
	{
		AssetSearcher.Reset();
		FinishSearch();
	}
}


/** search finish callback */
void SCOTPropertySearcher::FinishSearch()
{
	for (int32 AssetIndex = 0 ; AssetIndex < AssetSearcher.GetMaxAssetsIndex() ; ++AssetIndex)
	{
		const TArray<FAssetData>& Assets = AssetSearcher.GetAssets(AssetIndex);
		for (const FAssetData& AssetData : Assets)
		{
			FString AssetPathString = AssetData.GetObjectPathString();

			UObject* Object = FindObject<UObject>(NULL, *AssetPathString);
			if (Object == nullptr) continue;
		
			TSharedPtr<FCOTPropertySearcherResult> RootSearchResult;

			UObject* TargetObject = Object;
			if (UBlueprint* BPObject = Cast<UBlueprint>(TargetObject))
			{
				TargetObject = BPObject->GeneratedClass->ClassDefaultObject;
				RootSearchResult = MakeShareable(new FCOTPropertySearcherResult(FText::FromName(AssetData.PackageName), BPObject));
			}
			else
			{
				UMaterial* MaterialObject = Cast<UMaterial>(TargetObject);
				RootSearchResult = MakeShareable(new FCOTPropertySearcherResult(FText::FromName(AssetData.PackageName), MaterialObject));
			}

			const bool bUseDisplayNameInPropertySearch = UCUTDeveloperSettings::Get()->bUseDisplayNameInPropertySearch;
			for (TFieldIterator<FProperty> PropertyIterator(TargetObject->GetClass()); PropertyIterator; ++PropertyIterator)
			{
				FCUTUtility::SearchProperty(TargetObject, *PropertyIterator, SearchTokens, true, bUseDisplayNameInPropertySearch, [RootSearchResult](const FProperty& InProperty, const FString& ValueString)
						{
							TSharedPtr<FCOTPropertySearcherResult> NewResult(new FCOTPropertySearcherResult(FText::FromString(ValueString), RootSearchResult));
							RootSearchResult->AddChild(NewResult);
						});
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
		ItemsFound.Add(TSharedPtr<FCOTPropertySearcherResult>(new FCOTPropertySearcherResult(LOCTEXT("ResultNoResults", "No Results found"))));
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

	for (const TSharedPtr<FCOTPropertySearcherResult>& Item : ItemsFound)
	{
		TreeView->SetItemExpansion(Item, true);
	}
}
// Search box --- End




// Result Tree --- Begin

TSharedRef<ITableRow> SCOTPropertySearcher::OnGenerateRow(TSharedPtr<FCOTPropertySearcherResult> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<FCOTPropertySearcherResult>>, OwnerTable)
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
void SCOTPropertySearcher::OnGetChildren(TSharedPtr<FCOTPropertySearcherResult> InItem, TArray<TSharedPtr<FCOTPropertySearcherResult>>& OutChildren)
{
	OutChildren += InItem->GetChildren();
}

void SCOTPropertySearcher::OnTreeSelectionDoubleClicked(TSharedPtr<FCOTPropertySearcherResult> Item)
{
	if(Item.IsValid())
	{
		Item->OnClick();
	}
}

// Result Tree --- End

// Progress Bar --- Begin

EVisibility SCOTPropertySearcher::GetProgressBarVisibility() const
{
	return AssetSearcher.IsAsyncLoading() ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SCOTPropertySearcher::GetProgressBarPercent() const
{
	if (!AssetSearcher.IsAsyncLoading())
	{
		return 1.0f;
	}
	
	return AssetSearcher.GetProgress();
}

/* SearchSteart clicked event */
FReply SCOTPropertySearcher::ButtonSearchStartClicked()
{
	SearchStart();
	return FReply::Handled();
}

/* CopyClipboard clicked event */
FReply SCOTPropertySearcher::ButtonCopyClipBoardClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportClipboard(Clipboard);
	
	return FReply::Handled();
}

/* ExportText clicked event */
FReply SCOTPropertySearcher::ButtonExportTextClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportTxt("COTPropertySearcher", "COTPropertySearcher.txt", Clipboard, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

/* ExportCsv clicked event */
FReply SCOTPropertySearcher::ButtonExportCsvClicked()
{
	FString Clipboard = GetClipboardCsvText();
	FCUTUtility::ExportTxt("COTPropertySearcher", "COTPropertySearcher.csv", Clipboard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}

FString SCOTPropertySearcher::GetClipboardText()
{
	FString RetString;
	RetString = FString::Printf(TEXT("Search Path: %s\n"), *GetSearchPath());
	RetString += FString::Printf(TEXT("Search Name: %s\n\n"), *GetSearchValue());
	
	
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromNodeSearcherResult(*It, 0, &RetString);
	}
	return RetString;
}
FString SCOTPropertySearcher::GetClipboardCsvText()
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
void SCOTPropertySearcher::AddClipboardTextFromNodeSearcherResult(const TSharedPtr<FCOTPropertySearcherResult>& Result, int32 Indent, FString* ExportText)
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
void SCOTPropertySearcher::AddClipboardCsvTextFromNodeSearcherResult(const TSharedPtr<FCOTPropertySearcherResult>& Result, FString* ExportText)
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

