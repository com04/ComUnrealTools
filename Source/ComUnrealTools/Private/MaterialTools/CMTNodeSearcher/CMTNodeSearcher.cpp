// Copyright com04 All Rights Reserved.

#include "CMTNodeSearcher.h"
#include "CMTNodeSearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetData.h"
#include "AssetRegistryModule.h"
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

/**
 * Add clip board string from MaterialData.
 */
void AddClipboardTextFromNodeSearcherResult(const FCMTNodeSearcherResultShare& Result, int32 Indent, FString* ExportText)
{
	FString NodeComment;
	if (!Result->GetCommentText().IsEmpty())
	{
		NodeComment += FString::Printf(TEXT("     // Node Comment:%s"), *Result->GetCommentText());
	}
	
	*ExportText += FString::Printf(TEXT("%s- %s%s\n"), *FString::ChrN(Indent, TEXT(' ')), *Result->GetDisplayString().ToString(), *NodeComment);
	
	for (auto It = Result->GetChildren().CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromNodeSearcherResult(*It, Indent + 2, ExportText);
	}
}


////////////////////////////////////
// SCMTNodeSearcher
FString SCMTNodeSearcher::SearchPath = FString("/Game/");
FString SCMTNodeSearcher::SearchValue = FString("");
ECheckBoxState SCMTNodeSearcher::CheckMaterialInstance = ECheckBoxState::Checked;


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
				.Text(LOCTEXT("SearchPath", "Path"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("FindPath", "Enter material path to find references..."))
				.InitialText(FText::FromString(SearchPath))
				.OnTextChanged(this, &SCMTNodeSearcher::OnSearchPathChanged)
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
				.Text(LOCTEXT("SearchName", "Name"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("Find", "Enter material or node name, texture name to find references..."))
				.InitialText(FText::FromString(SearchValue))
				.OnTextChanged(this, &SCMTNodeSearcher::OnSearchTextChanged)
				.OnTextCommitted(this, &SCMTNodeSearcher::OnSearchTextCommitted)
			]
		]
		
		// CheckBox material instance
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.f, 4.f, 12.f, 0.f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CheckMaterialInstance", "check material instance"))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked(CheckMaterialInstance)
				.OnCheckStateChanged(this, &SCMTNodeSearcher::OnCheckMaterialInstanceChanged)
			]
			
			+ SHorizontalBox::Slot()
			.MaxWidth(50.f)
				
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTNodeSearcher::ButtonCopyClipBoardClicked)
			]
			
			+ SHorizontalBox::Slot()
			.MaxWidth(10.f)
			
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCMTNodeSearcher::ButtonExportTextClicked)
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
	
	
	
	OnSearchPathChanged(FText::FromString(SearchPath));
	OnSearchTextChanged(FText::FromString(SearchValue));
}

FReply SCMTNodeSearcher::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}


// Search path --- Begin

/** text change event */
void SCMTNodeSearcher::OnSearchPathChanged(const FText& Text)
{
	SearchPath = Text.ToString();
	bDirtySearchPath = true;
}

/** text commit event */
void SCMTNodeSearcher::OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (MaterialSearcher.IsAsyncLoading())  return;
	
	OnSearchPathChanged(Text);
	SearchStart();
}

// Search path --- End



// Search box --- Begin

/** text change event */
void SCMTNodeSearcher::OnSearchTextChanged(const FText& Text)
{
	SearchValue = Text.ToString();
	bDirtySearchText = true;
}

/** text commit event */
void SCMTNodeSearcher::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType != ETextCommit::OnEnter) return;
	if (MaterialSearcher.IsAsyncLoading())  return;
	
	OnSearchTextChanged(Text);
	SearchStart();
}

/** Search */
void SCMTNodeSearcher::SearchStart()
{
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
		// 前回の検索結果をクリアする
		for (auto It(ItemsFound.CreateIterator()); It; ++It)
		{
			TreeView->SetItemExpansion(*It, false);
		}
		ItemsFound.Empty();		
	
		HighlightText = FText::FromString(SearchValue);
		MatchTokens();
	}
}


/** search match */
void SCMTNodeSearcher::MatchTokens()
{
	MaterialSearcher.SearchStart(SearchPathTokens, TArray<FString>(),
			true,
			true, (CheckMaterialInstance == ECheckBoxState::Checked),
			true);
}

/** search finish callback */
void SCMTNodeSearcher::FinishSearch()
{
	const TArray<FAssetData>& MaterialAsset = MaterialSearcher.GetMaterialAssets();
	const TArray<FAssetData>& MaterialFunctionAsset = MaterialSearcher.GetMaterialFunctionAssets();
	const TArray<FAssetData>& MaterialInstanceAsset = MaterialSearcher.GetMaterialInstanceAssets();
	
	for (auto It = MaterialAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterial* Material = FindObject<UMaterial>(NULL, *AssetPathString);
		if (Material == nullptr) continue;
		
		// get expression
		TArray<UMaterialExpression*> MaterialExpressions;
		if (Material->GetAllReferencedExpressions(MaterialExpressions, nullptr) == false) continue;
		
		FCMTNodeSearcherResultShare RootSearchResult(new FCMTNodeSearcherResult(FText::FromName(It->PackageName), Material));
		
		MatchTokensFromAssets(*It, MaterialExpressions, Material->EditorComments, RootSearchResult);
				
	}
	
	for (auto It = MaterialFunctionAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterialFunction* MaterialFunction = FindObject<UMaterialFunction>(NULL, *AssetPathString);
		if (MaterialFunction == nullptr) continue;
		
		FCMTNodeSearcherResultShare RootSearchResult(new FCMTNodeSearcherResult(FText::FromName(It->PackageName), MaterialFunction));

		MatchTokensFromAssets(*It, MaterialFunction->FunctionExpressions, MaterialFunction->FunctionEditorComments, RootSearchResult);
	}
	
	for (auto It = MaterialInstanceAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterialInstance* MaterialInstance = FindObject<UMaterialInstance>(NULL, *AssetPathString);
		if (MaterialInstance == nullptr) continue;
		
		FCMTNodeSearcherResultShare RootSearchResult(new FCMTNodeSearcherResult(FText::FromName(It->PackageName), MaterialInstance));

		TArray<UMaterialExpression*> MaterialExpressions;
		TArray<UMaterialExpressionComment*> MaterialExpressionComments;
		bool bFind = MatchTokensFromAssets(*It, MaterialExpressions, MaterialExpressionComments, RootSearchResult);
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
	}

	TreeView->RequestTreeRefresh();

	for (auto It(ItemsFound.CreateIterator()); It; ++It)
	{
		TreeView->SetItemExpansion(*It, true);
	}
	
	
	// clipboard
	AddClipboardText();
	
}

bool SCMTNodeSearcher::MatchTokensFromAssets(const FAssetData& InAsset,
		const TArray<UMaterialExpression*>& InExpressions, const TArray<UMaterialExpressionComment*>& InCommentExpressions, 
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
		const TArray<ExpressionClass*>& InExpressions, FCMTNodeSearcherResultShare InRoot,
		FText InPrefix, NameFunction InFunctionGetName)
{
	for (auto ItExpression = InExpressions.CreateConstIterator(); ItExpression; ++ItExpression)
	{
		const FText NodeName = InFunctionGetName(*ItExpression);
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
				if (!(*ItExpression)->MatchesSearchQuery(*SearchTokens[Index]))
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
			(*ItExpression)->GetCaption(Caption);
			
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
			UMaterialExpressionCustom* ExpressionCustom = Cast<UMaterialExpressionCustom>(*ItExpression);
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
			UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(*ItExpression);
			if (FunctionCall)
			{
				if (FunctionCall->MaterialFunction)
				{
					DisplayString += FString(TEXT("    ->    ")) + FunctionCall->MaterialFunction->GetFName().ToString();
				}
			}
			// texture sample
			UMaterialExpressionTextureSample* TextureSample = Cast<UMaterialExpressionTextureSample>(*ItExpression);
			if (TextureSample)
			{
				if (TextureSample->Texture)
				{
					DisplayString += FString(TEXT("    ->    ")) + TextureSample->Texture->GetFName().ToString();
					
				}
			}
			
			FCMTNodeSearcherResultShare ExpressionResult(new FCMTNodeSearcherResult(FText::FromString(DisplayString), InRoot, *ItExpression));
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
		Args.Add(TEXT("Comment"), FText::FromString(InItem->GetCommentText()));

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
					.Text(InItem.Get(), &FCMTNodeSearcherResult::GetDisplayString)
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

/* CopyClipboard clicked event */
FReply SCMTNodeSearcher::ButtonCopyClipBoardClicked()
{
	FCUTUtility::ExportClipboard(TextClipboard);
	
	return FReply::Handled();
}

/* ExportText clicked event */
FReply SCMTNodeSearcher::ButtonExportTextClicked()
{
	FCUTUtility::ExportTxt("NodeSearcher", "CMTNodeSearcher.txt", TextClipboard, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

void SCMTNodeSearcher::AddClipboardText()
{
	TextClipboard = FString::Printf(TEXT("Search Path: %s\n"), *SearchPath);
	TextClipboard += FString::Printf(TEXT("Search Name: %s\n\n"), *SearchValue);
	
	
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromNodeSearcherResult(*It, 0, &TextClipboard);
	}
}



// Progress Bar --- End

#undef LOCTEXT_NAMESPACE

