// Copyright com04 All Rights Reserved.

#include "CMTTextureFind.h"
#include "CMTTextureFindResult.h"
#include "CMTTextureFindResultData.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetData.h"
#include "AssetThumbnail.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "CMTTextureFind"




////////////////////////////////////
// SCMTTextureFind

FString SCMTTextureFind::SearchPath = FString("/Game/");
FString SCMTTextureFind::SearchName = FString("");
ECheckBoxState SCMTTextureFind::CheckBoxOneAsset = ECheckBoxState::Checked;
ECheckBoxState SCMTTextureFind::CheckBoxDirectory = ECheckBoxState::Unchecked;
ECheckBoxState SCMTTextureFind::CheckBoxDirectoryMaterial = ECheckBoxState::Checked;
ECheckBoxState SCMTTextureFind::CheckBoxDirectoryMaterialFunction = ECheckBoxState::Unchecked;


SCMTTextureFind::~SCMTTextureFind()
{
}

void SCMTTextureFind::Construct(const FArguments& InArgs)
{
	const int32 SelectedThumbnailSize = 80;
	SelectedMaterial = nullptr;
	SelectedMaterialFunction = nullptr;
	bResultSearchAsset = true;
	
	AssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(1));
	SelectedThumbnail = MakeShareable(new FAssetThumbnail(nullptr, SelectedThumbnailSize, SelectedThumbnailSize, AssetThumbnailPool));
	SelectedThumbnail->GetViewportRenderTargetTexture(); // Access the texture once to trigger it to render
	
	TArray<const UClass*> ClassFilters;
	ClassFilters.Add(UMaterialInterface::StaticClass());
	ClassFilters.Add(UMaterialFunction::StaticClass());
	
	SCMTTextureFind* Self = this;
	MaterialSearcher.OnSearchEnd.BindLambda([Self]{Self->FinishSearch();});
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Find target asset
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SVerticalBox)
			
			// check box
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
			
				// check box
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(CheckBoxOneAssetWidget, SCheckBox)
					.IsChecked(CheckBoxOneAsset)
					.OnCheckStateChanged(this, &SCMTTextureFind::OnCheckOneAssetChanged)
				]
				
				// Asset property
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SelectAsset", "check one asset"))
				]
			]
		
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(BoxOneAssetWidget, SHorizontalBox)
			
				// Asset Thumbnail
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(20.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(SBorder)
					.Padding(4.0f)
					.BorderImage(FEditorStyle::GetBrush("PropertyEditor.AssetThumbnailShadow"))
					.OnMouseDoubleClick(this, &SCMTTextureFind::OnMaterialThumbnailMouseDoubleClick)
					[
						SNew(SBox)
						.WidthOverride(SelectedThumbnailSize)
						.HeightOverride(SelectedThumbnailSize)
						[
							SNew(SBorder)
							.Padding(1)
							.BorderImage(FEditorStyle::GetBrush("AssetThumbnail", ".Border"))
							.BorderBackgroundColor(FLinearColor::Black)
							[
								SelectedThumbnail->MakeThumbnailWidget()
							]
						]
					]
				]
			
				// Asset property
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SObjectPropertyEntryBox)
					.OnShouldFilterAsset(this, &SCMTTextureFind::ShouldFilterAsset)
					.NewAssetFactories(PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses(ClassFilters))
					.ObjectPath(this, &SCMTTextureFind::GetObjectPath)
					.OnObjectChanged(this, &SCMTTextureFind::OnObjectChanged)
				]
				
				// Spacer
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.HAlign(HAlign_Fill)
				[
					SNew(SSpacer)
				]
			]
		]
		// Find target directory
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SVerticalBox)
			
			// check box
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 4.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
			
				// check box
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SAssignNew(CheckBoxDirectoryWidget, SCheckBox)
					.IsChecked(CheckBoxDirectory)
					.OnCheckStateChanged(this, &SCMTTextureFind::OnCheckDirectoryChanged)
				]
				
				// Asset property
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("SelectDirectory", "check directory"))
				]
			]
		
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(BoxDirectoryWidget, SHorizontalBox)
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.FillWidth(1.0f)
				.Padding(20.0f, 0.0f, 0.0f, 0.0f)
				[
					// text box "Path"
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 4.0f, 12.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("SearchPath", "Search Path"))
						]
						
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSearchBox)
							.HintText(LOCTEXT("Find", "Enter material path to find textures..."))
							.InitialText(FText::FromString(SearchPath))
							.OnTextCommitted(this, &SCMTTextureFind::OnSearchPathCommitted)
						]
					]
					
					// text box "Name"
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(0.0f, 4.0f, 12.0f, 0.0f)
						[
							SNew(STextBlock)
							.Text(LOCTEXT("SearchName", "Material Name"))
						]
						
						+ SHorizontalBox::Slot()
						.FillWidth(1.0f)
						[
							SNew(SSearchBox)
							.HintText(LOCTEXT("Find", "Enter material name to find textures..."))
							.InitialText(FText::FromString(SearchName))
							.OnTextCommitted(this, &SCMTTextureFind::OnSearchTextCommitted)
						]
					]
					
					// check box "material / material instance"
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f)
					[
						SNew(SHorizontalBox)
					
						// check box
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
							.IsChecked(CheckBoxDirectoryMaterial)
							.OnCheckStateChanged(this, &SCMTTextureFind::OnCheckDirectoryMaterialChanged)
						]
						
						// Asset property
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("CheckMaterial", "check material / material instance"))
						]
					]
					
					// check box "material function"
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.0f, 2.0f)
					[
						SNew(SHorizontalBox)
					
						// check box
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(SCheckBox)
							.IsChecked(CheckBoxDirectoryMaterialFunction)
							.OnCheckStateChanged(this, &SCMTTextureFind::OnCheckDirectoryMaterialFunctionChanged)
						]
						
						// Asset property
						+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							SNew(STextBlock)
							.Text(LOCTEXT("CheckMaterialFunction", "check material function"))
						]
					]
				]
			]
		]
		
		// Button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 5.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
		
			// Asset Check
			+ SHorizontalBox::Slot()
			.MaxWidth(300.0f)
			[
				SAssignNew(AssetCheckButton, SButton)
				.Text(LOCTEXT("AssetCheckButton", "Asset Check"))
				.OnClicked(this, &SCMTTextureFind::ButtonAssetCheckClicked)
				.IsEnabled(false)
			]
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(50.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTTextureFind::ButtonCopyClipBoardClicked)
				.IsEnabled(false)
			]
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCMTTextureFind::ButtonExportTextClicked)
				.IsEnabled(false)
			]
		]
		
		// Result
		+ SVerticalBox::Slot()
		.MaxHeight(500.f)
		.AutoHeight()
		.Padding(0.0f, 10.0f, 0.0f, 0.0f)
		[
			SAssignNew(ResultBox, SScrollBox)
		]
	];
	
	if (CheckBoxOneAsset == ECheckBoxState::Checked)
	{
		OnCheckOneAssetChanged(ECheckBoxState::Checked);
	}
	else
	{
		OnCheckDirectoryChanged(ECheckBoxState::Checked);
	}
	OnSearchPathCommitted(FText::FromString(SearchPath), ETextCommit::OnEnter);
	OnSearchTextCommitted(FText::FromString(SearchName), ETextCommit::OnEnter);
	CheckAssetCheckButton();
}

/** EntryBox asset filtering */
bool SCMTTextureFind::ShouldFilterAsset(const FAssetData& AssetData)
{
	UObject* AssetObject = AssetData.GetAsset();
	if (Cast<UMaterialInterface>(AssetObject))
	{
		return false;
	}
	else if (Cast<UMaterialFunction>(AssetObject))
	{
		return false;
	}
	return true;
}

/* EntryBox get display path */
FString SCMTTextureFind::GetObjectPath() const
{
	if (SelectedMaterial)
	{
		return SelectedMaterial->GetPathName();
	}
	if (SelectedMaterialFunction)
	{
		return SelectedMaterialFunction->GetPathName();
	}
    return FString("");
}

/* EntryBox change object event */
void SCMTTextureFind::OnObjectChanged(const FAssetData& AssetData)
{
	SelectedMaterial = Cast<UMaterialInterface>(AssetData.GetAsset());
	if (SelectedMaterial == nullptr)
	{
		SelectedMaterialFunction = Cast<UMaterialFunction>(AssetData.GetAsset());
	}
	
	CheckAssetCheckButton();
	SelectedThumbnail->SetAsset(AssetData);
	
	ResultBox->ClearChildren();
	CopyClipBoardButton->SetEnabled(false);
	ExportTextButton->SetEnabled(false);
	
}

/* Material Thumbnail double click */
FReply SCMTTextureFind::OnMaterialThumbnailMouseDoubleClick(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent)
{
	if (SelectedMaterial || SelectedMaterialFunction)
	{
		UMaterial* Material = Cast<UMaterial>(SelectedMaterial);
		UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(SelectedMaterial);
		FCUTUtility::WakeupMaterialEditor(Material, MaterialInstance, SelectedMaterialFunction);
	}
	return FReply::Handled();
}


/* AssetCheckButton clicked event */
FReply SCMTTextureFind::ButtonAssetCheckClicked()
{
	// ResultBox
	ResultBox->ClearChildren();
	CopyClipBoardButton->SetEnabled(false);
	ExportTextButton->SetEnabled(false);

	if (CheckBoxOneAsset == ECheckBoxState::Checked)
	{
		TArray<UMaterialInterface*> Materials;
		TArray<UMaterialFunction*> MaterialFunctions;
		Materials.Add(SelectedMaterial);
		MaterialFunctions.Add(SelectedMaterialFunction);
		bResultSearchAsset = true;
		SearchTextureInMaterial(Materials, MaterialFunctions);
	}
	else
	{
		bool CheckMaterial = CheckBoxDirectoryMaterial == ECheckBoxState::Checked;
		bool CheckMaterialFunction = CheckBoxDirectoryMaterialFunction == ECheckBoxState::Checked;
		
		// search path parse
		FCUTUtility::SplitStringTokens(SearchPath, &SearchPathTokens);
		
		// search text parse
		FCUTUtility::SplitStringTokens(SearchName, &SearchTokens);
		
		MaterialSearcher.SearchStart(SearchPathTokens, SearchTokens,
				true,
				CheckMaterial, CheckMaterial,
				CheckMaterialFunction);
	}
	
	return FReply::Handled();
}

/* CopyClipboard clicked event */
FReply SCMTTextureFind::ButtonCopyClipBoardClicked()
{
	FCUTUtility::ExportClipboard(ClipboardText);
	
	return FReply::Handled();
}

/* ExportText clicked event */
FReply SCMTTextureFind::ButtonExportTextClicked()
{
	FCUTUtility::ExportTxt("TextureFind", "CMTTextureFind.txt", ClipboardText, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

/** text change event */
void SCMTTextureFind::OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SearchPath = Text.ToString();
	CheckAssetCheckButton();
}
/** text change event */
void SCMTTextureFind::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SearchName = Text.ToString();
	CheckAssetCheckButton();
}

/** "Check one asset" checkbox changed callback */
void SCMTTextureFind::OnCheckOneAssetChanged(ECheckBoxState InValue)
{
	CheckBoxOneAsset = ECheckBoxState::Checked;
	CheckBoxDirectory = ECheckBoxState::Unchecked;
	BoxOneAssetWidget->SetEnabled(true);
	BoxDirectoryWidget->SetEnabled(false);
	CheckBoxOneAssetWidget->SetIsChecked(CheckBoxOneAsset);
	CheckBoxDirectoryWidget->SetIsChecked(CheckBoxDirectory);
	CheckAssetCheckButton();
}

/** "Check directory" checkbox changed callback */
void SCMTTextureFind::OnCheckDirectoryChanged(ECheckBoxState InValue)
{
	CheckBoxOneAsset = ECheckBoxState::Unchecked;
	CheckBoxDirectory = ECheckBoxState::Checked;
	BoxOneAssetWidget->SetEnabled(false);
	BoxDirectoryWidget->SetEnabled(true);
	CheckBoxOneAssetWidget->SetIsChecked(CheckBoxOneAsset);
	CheckBoxDirectoryWidget->SetIsChecked(CheckBoxDirectory);
	CheckAssetCheckButton();
}

/** "check material" checkbox changed callback */
void SCMTTextureFind::OnCheckDirectoryMaterialChanged(ECheckBoxState InValue)
{
	CheckBoxDirectoryMaterial = InValue;
}

/** "check material function" checkbox changed callback */
void SCMTTextureFind::OnCheckDirectoryMaterialFunctionChanged(ECheckBoxState InValue)
{
	CheckBoxDirectoryMaterialFunction = InValue;
}

/** "Asset Check" Button check status enabled */
void SCMTTextureFind::CheckAssetCheckButton()
{
	bool Enable = true;
	if (CheckBoxOneAsset == ECheckBoxState::Checked)
	{
		Enable = (SelectedMaterial != nullptr) || (SelectedMaterialFunction != nullptr);
	}
	else
	{
		Enable = (!SearchPath.IsEmpty() && !SearchName.IsEmpty());
	}
	AssetCheckButton->SetEnabled(Enable);
}

/** search finish callback */
void SCMTTextureFind::FinishSearch()
{
	const TArray<FAssetData>& MaterialAsset = MaterialSearcher.GetMaterialAssets();
	const TArray<FAssetData>& MaterialFunctionAsset = MaterialSearcher.GetMaterialFunctionAssets();
	const TArray<FAssetData>& MaterialInstanceAsset = MaterialSearcher.GetMaterialInstanceAssets();
	
	TArray<UMaterialInterface*> Materials;
	TArray<UMaterialFunction*> MaterialFunctions;
	
	for (auto It = MaterialAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterial* Material = FindObject<UMaterial>(NULL, *AssetPathString);
		if (Material == nullptr) continue;
		
		Materials.Add(Material);
	}
	
	for (auto It = MaterialFunctionAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterialFunction* MaterialFunction = FindObject<UMaterialFunction>(NULL, *AssetPathString);
		if (MaterialFunction == nullptr) continue;
		
		MaterialFunctions.Add(MaterialFunction);
	}
	
	for (auto It = MaterialInstanceAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterialInstance* MaterialInstance = FindObject<UMaterialInstance>(NULL, *AssetPathString);
		if (MaterialInstance == nullptr) continue;
		
		Materials.Add(MaterialInstance);
	}
	
	bResultSearchAsset = false;
	SearchTextureInMaterial(Materials, MaterialFunctions);
}

/** search material */
void SCMTTextureFind::SearchTextureInMaterial(TArray<UMaterialInterface*>& TargetMaterial,
		TArray<UMaterialFunction*>& TargetMaterialFunction)
{
	TArray<UTexture*> Textures;
	
	for (auto It = TargetMaterial.CreateIterator() ; It ; ++It)
	{
		if (*It == nullptr)
		{
			continue;
		}
		// check instance
		UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(*It);
		UMaterialInterface* MasterMaterial = *It;
		if (MaterialInstance)
		{
			MasterMaterial = FCUTUtility::GetMasterMaterial(MaterialInstance);
		}
		if (!IsValid(MasterMaterial))
		{
			continue;
		}
		
		TArray<UTexture*> TmpTextures;
		MasterMaterial->GetUsedTextures(TmpTextures, EMaterialQualityLevel::High, false, ERHIFeatureLevel::SM5, false);
		for (auto ItTexture = TmpTextures.CreateIterator() ; ItTexture ; ++ItTexture)
		{
			bool Find = false;
			FString TexturePath = GetPathNameSafe(*ItTexture);
			for (auto ItAddedTexture = Textures.CreateIterator() ; ItAddedTexture ; ++ItAddedTexture)
			{
				if (TexturePath == GetPathNameSafe(*ItAddedTexture))
				{
					Find = true;
					break;
				}
			}
			if (Find == false)
			{
				Textures.AddUnique(*ItTexture);
			}
		}
	}
	
	if (bResultSearchAsset)
	{
		ClipboardText = FString::Printf(TEXT("Search Asset: %s\n\n"), *FCUTUtility::NormalizePathText(GetObjectPath()));
	}
	else
	{
		ClipboardText = FString::Printf(TEXT("Search Path: %s\n"), *SearchPath);
		ClipboardText += FString::Printf(TEXT("Search Name: %s\n\n"), *SearchName);
	}
	
	for (auto It = TargetMaterialFunction.CreateIterator() ; It ; ++It)
	{
		if (*It == nullptr)
		{
			continue;
		}
		TArray<UTexture*> TmpTextures;
		GetTextureForMaterialFunction(&TmpTextures, *It);
		for (auto ItTexture = TmpTextures.CreateIterator() ; ItTexture ; ++ItTexture)
		{
			bool Find = false;
			FString TexturePath = GetPathNameSafe(*ItTexture);
			for (auto ItAddedTexture = Textures.CreateIterator() ; ItAddedTexture ; ++ItAddedTexture)
			{
				if (TexturePath == GetPathNameSafe(*ItAddedTexture))
				{
					Find = true;
					break;
				}
			}
			if (Find == false)
			{
				Textures.AddUnique(*ItTexture);
			}
		}
	}
	
	// find used asset
	for (int32 Index = 0 ; Index < Textures.Num() ; ++Index)
	{
		FCMTTextureFindResultData TextureData;
		TextureData.Texture = Textures[Index];
		TextureData.TexturePath = GetPathNameSafe(Textures[Index]);
		
		for (auto It = TargetMaterial.CreateIterator() ; It ; ++It)
		{
			SearchTexture(&TextureData, *It);
		}
		for (auto It = TargetMaterialFunction.CreateIterator() ; It ; ++It)
		{
			TSharedPtr<FCMTTextureFindResultMaterialData> Stack(new FCMTTextureFindResultMaterialData);
			Stack->Material = nullptr;
			SearchTexture(&TextureData, *It, Stack, Stack);
		}
		
		AddClipboardText(TextureData);
		
		ResultBox->AddSlot()
		[
			SNew(FCMTTextureFindResult)
			.TextureResultData(TextureData)
		];
	}
	
	
	bool ResultEnabled = Textures.Num() > 0;
	CopyClipBoardButton->SetEnabled(ResultEnabled);
	ExportTextButton->SetEnabled(ResultEnabled);

	// not found.
	if (ResultEnabled == false)
	{
		ResultBox->AddSlot()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("TextureNotFound", "Texture not found."))
		];
	}
}
/**
 * search texture in Material/MaterialInstance
 *
 * @Param[in/out] TextureData Search texture data
 * @Param[in] TargetMaterial Search TextureData in this material
 */
void SCMTTextureFind::SearchTexture(FCMTTextureFindResultData* TextureData, UMaterialInterface* TargetMaterial)
{
	if (TargetMaterial == nullptr)
	{
		return;
	}
	UMaterial* Material = Cast<UMaterial>(TargetMaterial);
	UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(TargetMaterial);
	TArray<UMaterialExpression*> MaterialExpressions;
	
	if (Material)
	{
		Material->GetAllReferencedExpressions(MaterialExpressions, nullptr);
	
	}
	else if (MaterialInstance)
	{
		Material = MaterialInstance->GetMaterial();
		if (Material)
		{
			Material->GetAllReferencedExpressions(MaterialExpressions, nullptr);
		}
	}
	
	for (int32 Index = 0 ; Index < MaterialExpressions.Num() ; ++Index)
	{
		UMaterialExpression* Expression = MaterialExpressions[Index];
		UTexture* Texture = Cast<UTexture>(Expression->GetReferencedTexture());
		if (Texture)
		{
			if (GetPathNameSafe(Texture) == TextureData->TexturePath)
			{
				TSharedPtr<FCMTTextureFindResultMaterialData> MaterialData(new FCMTTextureFindResultMaterialData());
				MaterialData->Material = TargetMaterial;
				MaterialData->MaterialName = Expression->GetFName().ToString();
				TextureData->Referenced.Add(MaterialData);
			}
		}
		else
		{
			
			UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(Expression);
			if (FunctionCall)
			{
				UMaterialFunction* MaterialFunction = Cast<UMaterialFunction>(FunctionCall->MaterialFunction);
				
				TSharedPtr<FCMTTextureFindResultMaterialData> Stack(new FCMTTextureFindResultMaterialData);
				Stack->Material = TargetMaterial;
				Stack->MaterialName = Expression->GetFName().ToString();
				SearchTexture(TextureData, MaterialFunction, Stack, Stack);
			}
		}
	}
}
/**
 * search texture in MaterialFunction
 *
 * @Param[in/out] TextureData Search texture data
 * @Param[in] TargetMaterial Search TextureData in this material
 */
void SCMTTextureFind::SearchTexture(FCMTTextureFindResultData* TextureData, UMaterialFunction* TargetMaterial,
		TSharedPtr<FCMTTextureFindResultMaterialData> StackRoot,
		TSharedPtr<FCMTTextureFindResultMaterialData> StackEnd)
{
	if (TargetMaterial == nullptr)
	{
		return;
	}
	const TArray<UMaterialExpression*>& MaterialExpressions = TargetMaterial->FunctionExpressions;

	// insert child
	TSharedPtr<FCMTTextureFindResultMaterialData> Function(new FCMTTextureFindResultMaterialData());
	Function->Function = TargetMaterial;
	Function->Parent = StackEnd;
	StackEnd->Child = Function;

	for (int32 Index = 0 ; Index < MaterialExpressions.Num() ; ++Index)
	{
		UMaterialExpression* Expression = MaterialExpressions[Index];
		UTexture* Texture = Cast<UTexture>(Expression->GetReferencedTexture());
		
		Function->MaterialName = Expression->GetFName().ToString();
	
		if (Texture)
		{
			if (GetPathNameSafe(Texture) == TextureData->TexturePath)
			{
				// Find!!
				TextureData->Referenced.Add(DeepCopyMaterialData(StackRoot));
			}
		}
		else
		{
			
			UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(Expression);
			if (FunctionCall)
			{
				// child function
				SearchTexture(TextureData, Cast<UMaterialFunction>(FunctionCall->MaterialFunction), StackRoot, StackEnd->Child);
			}
		}
	}
	
	// remove child
	StackEnd->Child.Reset();
}
/**
 * FCMTTextureFindResultMaterialData deep copy
 */

TSharedPtr<FCMTTextureFindResultMaterialData> SCMTTextureFind::DeepCopyMaterialData(TSharedPtr<FCMTTextureFindResultMaterialData> Base)
{
	TSharedPtr<FCMTTextureFindResultMaterialData> Ret(new FCMTTextureFindResultMaterialData());
	Ret->Material = Base->Material;
	Ret->Function = Base->Function;
	Ret->MaterialName = Base->MaterialName;
	if (Base->Child.IsValid())
	{
		Ret->Child = DeepCopyMaterialData(Base->Child);
		Ret->Child->Parent = Ret;
	}
	return Ret;
}


/**
 * get texture in MaterialFunction
 *
 * @Param[out] OutputTextures output texture list
 * @Param[in] TargetMaterial Search Texture in this material
 */
void SCMTTextureFind::GetTextureForMaterialFunction(TArray<UTexture*>* OutputTextures, UMaterialFunction* TargetMaterial)
{
	if (TargetMaterial == nullptr)
	{
		return;
	}
	const TArray<UMaterialExpression*>& MaterialExpressions = TargetMaterial->FunctionExpressions;
	
	for (int32 Index = 0 ; Index < MaterialExpressions.Num() ; ++Index)
	{
		UMaterialExpression* Expression = MaterialExpressions[Index];
		UTexture* Texture = Cast<UTexture>(Expression->GetReferencedTexture());
		if (Texture)
		{
			OutputTextures->Add(Texture);
		}
		else
		{
			UMaterialExpressionMaterialFunctionCall* FunctionCall = Cast<UMaterialExpressionMaterialFunctionCall>(Expression);
			if (FunctionCall)
			{
				UMaterialFunction* MaterialFunction = Cast<UMaterialFunction>(FunctionCall->MaterialFunction);
				GetTextureForMaterialFunction(OutputTextures, MaterialFunction);
			}
		}
	}
}


/** Export Text */
void SCMTTextureFind::AddClipboardText(const FCMTTextureFindResultData& TextureData)
{
	ClipboardText += FString::Printf(TEXT("[Texture] %s\n"), *FCUTUtility::NormalizePathText(TextureData.TexturePath));
	
	for (auto It = TextureData.Referenced.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromMaterialData(*It, 2, &ClipboardText);
	}
	ClipboardText += FString("\n");
}


/**
 * Add clip board string from MaterialData.
 */
void SCMTTextureFind::AddClipboardTextFromMaterialData(const TSharedPtr<FCMTTextureFindResultMaterialData>& MaterialData, int32 Indent, FString* ExportText)
{
	int32 ChildIndent = Indent;
	if (MaterialData->Material || MaterialData->Function)
	{
		FString MaterialName = MaterialData->Material ? GetPathNameSafe(MaterialData->Material) : GetPathNameSafe(MaterialData->Function);

		*ExportText += FString::Printf(TEXT("%s- %s : %s\n"), *FString::ChrN(Indent, TEXT(' ')), *FCUTUtility::NormalizePathText(MaterialName), *MaterialData->MaterialName);
		
		ChildIndent += 2;
	}

	if (MaterialData->Child.IsValid())
	{
		AddClipboardTextFromMaterialData(MaterialData->Child, ChildIndent, ExportText);
	}
} 



#undef LOCTEXT_NAMESPACE

