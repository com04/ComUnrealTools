// Copyright com04 All Rights Reserved.

#include "CMTNodeSearcher.h"
#include "CMTNodeSearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "UnrealTools/CUTDeveloperSettings.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionComment.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Notifications/SProgressBar.h"



#define LOCTEXT_NAMESPACE "CMTNodeSearcher"


////////////////////////////////////
// SCMTNodeSearcher


SCMTNodeSearcher::~SCMTNodeSearcher()
{
}

void SCMTNodeSearcher::Construct(const FArguments& InArgs)
{
	SCMTNodeSearcher* Self = this;
	bDirtySearchPath = true;
	bDirtySearchText = true;
	MaterialSearcher.OnSearchEnd.BindLambda([Self]{Self->FinishSearch();});
	
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
				.HintText(LOCTEXT("FindPath", "Enter material path to find references..."))
				.InitialText(FText::FromString(GetSearchPath()))
				.OnTextCommitted(this, &SCMTNodeSearcher::OnSearchPathCommitted)
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
				.HintText(LOCTEXT("Find", "Enter material or node name, texture name to find references..."))
				.InitialText(FText::FromString(GetSearchValue()))
				.OnTextCommitted(this, &SCMTNodeSearcher::OnSearchTextCommitted)
			]
		]
		
		// CheckBox material instance
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 4.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SCheckBox)
				.IsChecked(GetCheckMaterialInstance())
				.OnCheckStateChanged(this, &SCMTNodeSearcher::OnCheckMaterialInstanceChanged)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CheckMaterialInstance", "check material instance"))
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
				.OnClicked(this, &SCMTNodeSearcher::ButtonSearchStartClicked)
				.IsEnabled(false)
			]
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(50.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTNodeSearcher::ButtonCopyClipBoardClicked)
				.IsEnabled(false)
			]
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCMTNodeSearcher::ButtonExportTextClicked)
				.IsEnabled(false)
			]
			// export csv
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportCsvButton, SButton)
				.Text(LOCTEXT("ExportCsv", "Export CSV"))
				.OnClicked(this, &SCMTNodeSearcher::ButtonExportCsvClicked)
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
				SAssignNew(TreeView, SCMTNodeSearcherTreeViewType)
				.ItemHeight(24)
				.TreeItemsSource(&ItemsFound)
				.OnGenerateRow(this, &SCMTNodeSearcher::OnGenerateRow)
				.OnGetChildren(this, &SCMTNodeSearcher::OnGetChildren)
				.OnMouseButtonDoubleClick(this,&SCMTNodeSearcher::OnTreeSelectionDoubleClicked)
			]
		]
		
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 4.f, 0.f, 0.f)
		[
			SNew(SProgressBar)
			.Visibility(this, &SCMTNodeSearcher::GetProgressBarVisibility)
			.Percent(this, &SCMTNodeSearcher::GetProgressBarPercent)
		]
	];
	
	
	
	OnSearchPathCommitted(FText::FromString(GetSearchPath()), ETextCommit::OnEnter);
	OnSearchTextCommitted(FText::FromString(GetSearchValue()), ETextCommit::OnEnter);
}



// Search path --- Begin

/** text change event */
void SCMTNodeSearcher::OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchPath(Text.ToString());
	bDirtySearchPath = true;
	
	SearchStartButton->SetEnabled((!GetSearchPath().IsEmpty() && !GetSearchValue().IsEmpty()));
}

/** text change event */
void SCMTNodeSearcher::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchValue(Text.ToString());
	bDirtySearchText = true;

	SearchStartButton->SetEnabled((!GetSearchPath().IsEmpty() && !GetSearchValue().IsEmpty()));
}

/** Search */
void SCMTNodeSearcher::SearchStart()
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
		MaterialSearcher.SearchStart(SearchPathTokens, TArray<FString>(),
				true,
				true, (GetCheckMaterialInstance() == ECheckBoxState::Checked),
				true);
	}
	else
	{
		MaterialSearcher.Reset();
		FinishSearch();
	}
}

/** search finish callback */
void SCMTNodeSearcher::FinishSearch()
{
	const TArray<FAssetData>& MaterialAsset = MaterialSearcher.GetMaterialAssets();
	const TArray<FAssetData>& MaterialFunctionAsset = MaterialSearcher.GetMaterialFunctionAssets();
	const TArray<FAssetData>& MaterialInstanceAsset = MaterialSearcher.GetMaterialInstanceAssets();
	
	for (auto It = MaterialAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->GetSoftObjectPath().ToString();
		
		UMaterial* Material = FindObject<UMaterial>(NULL, *AssetPathString);
		if (Material == nullptr) continue;
		
		// get expression
		TArray<UMaterialExpression*> MaterialExpressions;
		if (Material->GetAllReferencedExpressions(MaterialExpressions, nullptr) == false) continue;
		
		FCMTNodeSearcherResultShare RootSearchResult(new FCMTNodeSearcherResult(FText::FromName(It->PackageName), Material));
		
		MatchTokensFromAssets(*It, MaterialExpressions, Material->GetEditorComments(), RootSearchResult);
				
	}
	
	for (auto It = MaterialFunctionAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->GetObjectPathString();
		
		UMaterialFunction* MaterialFunction = FindObject<UMaterialFunction>(NULL, *AssetPathString);
		if (MaterialFunction == nullptr) continue;
		
		FCMTNodeSearcherResultShare RootSearchResult(new FCMTNodeSearcherResult(FText::FromName(It->PackageName), MaterialFunction));

		MatchTokensFromAssets(*It, MaterialFunction->GetExpressions(), MaterialFunction->GetEditorComments(), RootSearchResult);
	}
	
	for (auto It = MaterialInstanceAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->GetObjectPathString();
		
		UMaterialInstance* MaterialInstance = FindObject<UMaterialInstance>(NULL, *AssetPathString);
		if (MaterialInstance == nullptr) continue;
		
		FCMTNodeSearcherResultShare RootSearchResult(new FCMTNodeSearcherResult(FText::FromName(It->PackageName), MaterialInstance));

		bool bFind = MatchTokensFromAssets(*It, {}, {}, RootSearchResult);
		bool bFindTexture = false;
		for (auto ItTexture = MaterialInstance->TextureParameterValues.CreateConstIterator(); ItTexture; ++ItTexture)
		{
			UTexture* Texture = ItTexture->ParameterValue;
			if (Texture)
			{
				FText TextureName =  FText::FromName(Texture->GetFName());
				if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, TextureName.ToString()))
				{
					FCMTNodeSearcherResultShare ExpressionResult(new FCMTNodeSearcherResult(FText::FromString(FString("Texture: ") + TextureName.ToString()), RootSearchResult, Texture));
					RootSearchResult->AddChild(ExpressionResult);
					bFindTexture = true;
				}
			}
		}
		if ((bFind == false) && bFindTexture)
		{
			ItemsFound.Add(RootSearchResult);
		}
	}
	
	
	// not found
	if (ItemsFound.Num() == 0)
	{
		ItemsFound.Add(FCMTNodeSearcherResultShare(new FCMTNodeSearcherResult(LOCTEXT("ResultNoResults", "No Results found"))));
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

	for (auto It(ItemsFound.CreateIterator()); It; ++It)
	{
		TreeView->SetItemExpansion(*It, true);
	}
}

bool SCMTNodeSearcher::MatchTokensFromAssets(const FAssetData& InAsset,
		const TConstArrayView<UMaterialExpression*>& InExpressions, const TConstArrayView<TObjectPtr<UMaterialExpressionComment>>& InCommentExpressions, 
		FCMTNodeSearcherResultShare InRoot)
{
	auto FunctionGetExpressionName = [](UMaterialExpression* Expression){return FText::FromName(Expression->GetFName());};
	auto FunctionGetCommentName = [](UMaterialExpressionComment* Expression){return FText::FromString(Expression->Text);};
	const FText CommentNodePrefix = FText(LOCTEXT("CommentNodePrefix", "Comment: "));
	MatchTokensFromExpressions(InExpressions, InRoot, FText(), FunctionGetExpressionName);
	MatchTokensFromExpressions(InCommentExpressions, InRoot, CommentNodePrefix, FunctionGetCommentName);
	
	bool bFind = (InRoot->GetChildren().Num() > 0);
	// no match expressions.
	if (bFind == false)
	{
		// find Asset name
		if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, InAsset.AssetName.ToString()))
		{
			bFind = true;
		}
	}

	if (bFind)
	{
		ItemsFound.Add(InRoot);
	}
	
	return bFind;
}
template<class ExpressionClass, class NameFunction>
void SCMTNodeSearcher::MatchTokensFromExpressions(
		const TConstArrayView<ExpressionClass>& InExpressions, FCMTNodeSearcherResultShare InRoot,
		FText InPrefix, NameFunction InFunctionGetName)
{
	for (auto InExpression : InExpressions)
	{
		auto Expression = &(*InExpression);
		const FText NodeName = InFunctionGetName(Expression);
		FString NodeSearchString = NodeName.ToString();
		
		NodeSearchString = NodeSearchString.Replace(TEXT(" "), TEXT(""));

		// match node name?
		bool bNodeMatchesSearch = FCUTUtility::StringMatchesSearchTokens(SearchTokens, NodeSearchString);

		// use MatchesSearchQuery
		if (!bNodeMatchesSearch)
		{
			bool bMatchesAllTokens = true;
			for (int32 Index = 0; Index < SearchTokens.Num(); ++Index)
			{
				if (!Expression->MatchesSearchQuery(*SearchTokens[Index]))
				{
					bMatchesAllTokens = false;
					break;
				}
			}
			if (bMatchesAllTokens)
			{
				bNodeMatchesSearch = true;
			}
		}
		
		// caption check (Scene Texture)
		if (!bNodeMatchesSearch)
		{
			TArray<FString> Caption;
			Expression->GetCaption(Caption);
			
			for (auto ItCaption = Caption.CreateConstIterator(); ItCaption; ++ItCaption)
			{
				if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, *ItCaption))
				{
					bNodeMatchesSearch = true;
					break;
				}
			}
		}
		
		// Custom Node check
		if (!bNodeMatchesSearch)
		{
			UMaterialExpressionCustom* ExpressionCustom = Cast<UMaterialExpressionCustom>(Expression);
			if (ExpressionCustom)
			{
				if (FCUTUtility::StringMatchesSearchTokens(SearchTokens, ExpressionCustom->Code))
				{
					bNodeMatchesSearch = true;
				}
			}
		}
		
		// find!!
		if (bNodeMatchesSearch)
		{
			FString DisplayString = InPrefix.ToString();
			{
				FString NodeNameString = NodeName.ToString();
				int32 ExpressionIndex = NodeNameString.Find(TEXT("MaterialExpression"));
				if (ExpressionIndex >= 0)
				{
					NodeNameString.RemoveAt(0, 18);
				}
				DisplayString += NodeNameString;
			}
			
			// function call
			UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(Expression);
			if (FunctionCall)
			{
				if (FunctionCall->MaterialFunction)
				{
					DisplayString += FString(TEXT("    ->    ")) + FunctionCall->MaterialFunction->GetFName().ToString();
				}
			}
			// texture sample
			UMaterialExpressionTextureSample* TextureSample = Cast<UMaterialExpressionTextureSample>(Expression);
			if (TextureSample)
			{
				if (TextureSample->Texture)
				{
					DisplayString += FString(TEXT("    ->    ")) + TextureSample->Texture->GetFName().ToString();
					
				}
			}
			
			FCMTNodeSearcherResultShare ExpressionResult(new FCMTNodeSearcherResult(FText::FromString(DisplayString), InRoot, Expression));
			InRoot->AddChild(ExpressionResult);
		}
	}
}

// Search box --- End




// Result Tree --- Begin

TSharedRef<ITableRow> SCMTNodeSearcher::OnGenerateRow(FCMTNodeSearcherResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	// create comment text
	FText CommentText = FText::GetEmpty();
	if(!InItem->GetCommentText().IsEmpty())
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("Comment"), InItem->GetCommentText());

		CommentText = FText::Format(LOCTEXT("NodeComment", "Node Comment:[{Comment}]"), Args);
	}
	
	return SNew(STableRow< TSharedPtr<FCMTNodeSearcherResultShare> >, OwnerTable)
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
				
				// comments
				+SHorizontalBox::Slot()
				.FillWidth(1)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(2,0)
				[
					SNew(STextBlock)
					.Text(CommentText)
					.ColorAndOpacity(FLinearColor::Yellow)
					.HighlightText(HighlightText)
				]
			]
		];
}
void SCMTNodeSearcher::OnGetChildren(FCMTNodeSearcherResultShare InItem, TArray<FCMTNodeSearcherResultShare>& OutChildren)
{
	OutChildren += InItem->GetChildren();
}

void SCMTNodeSearcher::OnTreeSelectionDoubleClicked(FCMTNodeSearcherResultShare Item)
{
	if(Item.IsValid())
	{
		Item->OnClick();
	}
}

// Result Tree --- End

// Progress Bar --- Begin

EVisibility SCMTNodeSearcher::GetProgressBarVisibility() const
{
	return MaterialSearcher.IsAsyncLoading() ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SCMTNodeSearcher::GetProgressBarPercent() const
{
	if (!MaterialSearcher.IsAsyncLoading())
	{
		return 1.0f;
	}
	
	return MaterialSearcher.GetProgress();
}

/* SearchStart clicked event */
FReply SCMTNodeSearcher::ButtonSearchStartClicked()
{
	SearchStart();
	
	return FReply::Handled();
}

/* CopyClipboard clicked event */
FReply SCMTNodeSearcher::ButtonCopyClipBoardClicked()
{
	FString ClipBoard = GetClipboardText();
	
	FCUTUtility::ExportClipboard(ClipBoard);
	
	return FReply::Handled();
}

/* ExportText clicked event */
FReply SCMTNodeSearcher::ButtonExportTextClicked()
{
	FString ClipBoard = GetClipboardText();
	
	FCUTUtility::ExportTxt("NodeSearcher", "CMTNodeSearcher.txt", ClipBoard, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

/* ExportCsv clicked event */
FReply SCMTNodeSearcher::ButtonExportCsvClicked()
{
	FString ClipBoard = GetClipboardCsv();
	
	FCUTUtility::ExportTxt("NodeSearcher", "CMTNodeSearcher.csv", ClipBoard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}

FString SCMTNodeSearcher::GetClipboardText()
{
	FString RetString;
	RetString = FString::Printf(TEXT("Search Path: %s\n"), *GetSearchPath());
	RetString += FString::Printf(TEXT("Search Name: %s\n\n"), *GetSearchValue());
	
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromResult(*It, 0, &RetString);
	}
	return RetString;
}

FString SCMTNodeSearcher::GetClipboardCsv()
{
	FString RetString;
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardCsvFromResult(*It, &RetString);
	}
	return RetString;
}

/**
 * Add clip board string
 */
void SCMTNodeSearcher::AddClipboardTextFromResult(const FCMTNodeSearcherResultShare& Result, int32 Indent, FString* ExportText)
{
	FString NodeComment;
	if (!Result->GetCommentText().IsEmpty())
	{
		NodeComment += FString::Printf(TEXT("     // Node Comment:%s"), *Result->GetCommentText().ToString());
	}
	
	*ExportText += FString::Printf(TEXT("%s- %s%s\n"), *FString::ChrN(Indent, TEXT(' ')), *Result->GetDisplayText().ToString(), *NodeComment);
	
	for (auto It = Result->GetChildren().CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromResult(*It, Indent + 2, ExportText);
	}
}

void SCMTNodeSearcher::AddClipboardCsvFromResult(const FCMTNodeSearcherResultShare& Result, FString* ExportText)
{
	*ExportText += FString::Printf(TEXT("%s,//%s\n"), *Result->GetDisplayText().ToString(), *Result->GetCommentText().ToString());
	
	for (auto It = Result->GetChildren().CreateConstIterator() ; It ; ++It)
	{
		*ExportText += FString::Printf(TEXT("%s,%s,//%s\n"), *Result->GetDisplayText().ToString(), *((*It)->GetDisplayText().ToString()), *(*It)->GetCommentText().ToString());
	}
}


// Progress Bar --- End


#undef LOCTEXT_NAMESPACE

