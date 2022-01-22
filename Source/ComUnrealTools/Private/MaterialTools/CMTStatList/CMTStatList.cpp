// Copyright com04 All Rights Reserved.

#include "CMTStatList.h"
#include "CMTStatListResult.h"
#include "CMTStatListResultRow.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetData.h"
#include "AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionComment.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialStatsCommon.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Notifications/SProgressBar.h"



#define LOCTEXT_NAMESPACE "CMTStatList"



/**
 * Add clip board string from Result.
 */
void AddClipboardTextFromStatListResult(const TSharedPtr<FCMTStatListResult>& Result, FString* ExportText)
{
	*ExportText += FString::Printf(TEXT("%s,%s,%s,%s,%u,%d,%u,%s,%s,%s\n"),
			*Result->Name,
			*Result->Domain,
			*Result->ShadingModel,
			*Result->BlendMode.ToString(),
			Result->Instruction,
			Result->TextureNum,
			Result->TextureSize,
			Result->UsePositionOffset > 0 ? TEXT("True") : TEXT("False"),
			Result->UseDepthOffset > 0 ? TEXT("True") : TEXT("False"),
			Result->UseRefraction > 0 ? TEXT("True") : TEXT("False"));
}


////////////////////////////////////
// SCMTStatList

FString SCMTStatList::SearchValue = FString("/Game/");
ECheckBoxState SCMTStatList::CheckMaterialInstance = ECheckBoxState::Checked;

const FText& SCMTStatList::GetHeaderNameText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderName", "Name"));
	return HeaderNameText;
}
FName SCMTStatList::GetHeaderNameTextName()
{
	return FName(*GetHeaderNameText().ToString());
}
const FText& SCMTStatList::GetHeaderShadingModelText()
{
	static const FText HeaderShadingModelText(LOCTEXT("HeaderShadingModel", "ShadingModel"));
	return HeaderShadingModelText;
}
FName SCMTStatList::GetHeaderShadingModelTextName()
{
	return FName(*GetHeaderShadingModelText().ToString());
}
const FText& SCMTStatList::GetHeaderBlendModeText()
{
	static const FText HeaderBlendModeText(LOCTEXT("HeaderBlendMOde", "BlendMode"));
	return HeaderBlendModeText;
}
FName SCMTStatList::GetHeaderBlendModeTextName()
{
	return FName(*GetHeaderBlendModeText().ToString());
}
const FText& SCMTStatList::GetHeaderInstructionsText()
{
	static const FText HeaderInstructionsText(LOCTEXT("HeaderInstructions", "Instructions"));
	return HeaderInstructionsText;
}
FName SCMTStatList::GetHeaderInstructionsTextName()
{
	return FName(*GetHeaderInstructionsText().ToString());
}
const FText& SCMTStatList::GetHeaderTextureNumText()
{
	static const FText HeaderTextureNumText(LOCTEXT("HeaderTextureNum", "TextureNum"));
	return HeaderTextureNumText;
}
FName SCMTStatList::GetHeaderTextureNumTextName()
{
	return FName(*GetHeaderTextureNumText().ToString());
}
const FText& SCMTStatList::GetHeaderTextureSizeText()
{
	static const FText HeaderTextureSizeText(LOCTEXT("HeaderTextureSize", "TextureSize (width*height)"));
	return HeaderTextureSizeText;
}
FName SCMTStatList::GetHeaderTextureSizeTextName()
{
	return FName(*GetHeaderTextureSizeText().ToString());
}
const FText& SCMTStatList::GetHeaderPositionOffsetText()
{
	static const FText HeaderPositionOffsetText(LOCTEXT("HeaderPosOffset", "PosOffset"));
	return HeaderPositionOffsetText;
}
FName SCMTStatList::GetHeaderPositionOffsetTextName()
{
	return FName(*GetHeaderPositionOffsetText().ToString());
}
const FText& SCMTStatList::GetHeaderDepthOffsetText()
{
	static const FText HeaderDepthOffsetText(LOCTEXT("HeaderDepthOffset", "DepthOffset"));
	return HeaderDepthOffsetText;
}
FName SCMTStatList::GetHeaderDepthOffsetTextName()
{
	return FName(*GetHeaderDepthOffsetText().ToString());
}
const FText& SCMTStatList::GetHeaderRefractionText()
{
	static const FText HeaderRefractionText(LOCTEXT("HeaderRefraction", "Refraction"));
	return HeaderRefractionText;
}
FName SCMTStatList::GetHeaderRefractionTextName()
{
	return FName(*GetHeaderRefractionText().ToString());
}



SCMTStatList::~SCMTStatList()
{
}

void SCMTStatList::Construct(const FArguments& InArgs)
{
	SCMTStatList* Self = this;
	MaterialSearcher.OnSearchEnd.BindLambda([Self]{Self->FinishSearch();});
	
	ListSortName.Add(GetHeaderNameTextName());  // primary
	ListSortName.Add(GetHeaderTextureNumTextName());  // secondary
	ListSortMode.Add(EColumnSortMode::None);
	ListSortMode.Add(EColumnSortMode::None);
	
	
	
	TSharedPtr<SHeaderRow> Header =
		SNew(SHeaderRow)
		.ResizeMode(ESplitterResizeMode::FixedSize)
		
		+ CreateHeaderColumn(GetHeaderNameText(), 120)
		+ CreateHeaderColumn(GetHeaderShadingModelText(), 30)
		+ CreateHeaderColumn(GetHeaderBlendModeText(), 30)
		+ CreateHeaderColumn(GetHeaderInstructionsText(), 30)
		+ CreateHeaderColumn(GetHeaderTextureNumText(), 40)
		+ CreateHeaderColumn(GetHeaderTextureSizeText(), 50)
		+ CreateHeaderColumn(GetHeaderPositionOffsetText(), 30)
		+ CreateHeaderColumn(GetHeaderDepthOffsetText(), 30)
		+ CreateHeaderColumn(GetHeaderRefractionText(), 30)
		;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
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
				.Text(LOCTEXT("SearchPath", "Path"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("StatListFind", "Enter material path to find references..."))
				.InitialText(FText::FromString(SearchValue))
				.OnTextChanged(this, &SCMTStatList::OnSearchTextChanged)
				.OnTextCommitted(this, &SCMTStatList::OnSearchTextCommitted)
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
				.OnCheckStateChanged(this, &SCMTStatList::OnCheckMaterialInstanceChanged)
			]
			
			+ SHorizontalBox::Slot()
			.MaxWidth(50.f)
				
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTStatList::ButtonCopyClipBoardClicked)
			]
			
			+ SHorizontalBox::Slot()
			.MaxWidth(10.f)
			
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SAssignNew(ExportCSVButton, SButton)
				.Text(LOCTEXT("ExportCSV", "Export CSV"))
				.OnClicked(this, &SCMTStatList::ButtonExportCSVClicked)
			]
		]
		
		// Result list
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(0.f, 4.f, 0.f, 0.f)
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
			[
				SAssignNew(ResultView, SListView<TSharedPtr<FCMTStatListResult>>)
				.ListItemsSource(&ResultList)
				.HeaderRow(Header)
				.SelectionMode(ESelectionMode::None)
				.OnGenerateRow(this, &SCMTStatList::OnGenerateRow)
			]
		]
		
		// Progress bar
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f, 4.f, 0.f, 0.f)
		[
			SNew(SProgressBar)
			.Visibility(this, &SCMTStatList::GetProgressBarVisibility)
			.Percent(this, &SCMTStatList::GetProgressBarPercent)
		]
	];
	
}

FReply SCMTStatList::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}




// Search box --- Begin

/** text change event */
void SCMTStatList::OnSearchTextChanged(const FText& Text)
{
	SearchValue = Text.ToString();
}

/** text commit event */
void SCMTStatList::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType != ETextCommit::OnEnter) return;
	if (MaterialSearcher.IsAsyncLoading())  return;
	
	// search text parse
	FCUTUtility::SplitStringTokens(SearchValue, &SearchTokens);

	// last result clear
	ResultList.Empty();
	
	// search
	if (SearchTokens.Num() > 0)
	{
		MatchTokens();
	}
}


void SCMTStatList::MatchTokens()
{
	MaterialSearcher.SearchStart(SearchTokens, TArray<FString>(),
			true,
			true, (CheckMaterialInstance == ECheckBoxState::Checked),
			false);
}

void SCMTStatList::FinishSearch()
{
	const TArray<FAssetData>& MaterialAsset = MaterialSearcher.GetMaterialAssets();
	const TArray<FAssetData>& MaterialInstanceAsset = MaterialSearcher.GetMaterialInstanceAssets();
	
	for (auto It = MaterialAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterial* Material = FindObject<UMaterial>(NULL, *AssetPathString);
		if (Material == nullptr) continue;
		
		TSharedPtr<FCMTStatListResult> Result(new FCMTStatListResult);
		Result->IsInstance = false;
		Result->Name = FCUTUtility::NormalizePathText(AssetPathString);
		SetupMaterialResult(Result, Material, Material);
		ResultList.Add(Result);
	}
	
	for (auto It = MaterialInstanceAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->ObjectPath.ToString();
		
		UMaterialInstance* MaterialInstance = FindObject<UMaterialInstance>(NULL, *AssetPathString);
		if (MaterialInstance == nullptr) return;
		
		// search master material
		UMaterial* MasterMaterial = FCUTUtility::GetMasterMaterial(MaterialInstance);
		if (MasterMaterial == nullptr)
		{
			MasterMaterial = UMaterial::GetDefaultMaterial(MD_Surface);
		}
		
		TSharedPtr<FCMTStatListResult> Result(new FCMTStatListResult);
		Result->IsInstance = true;
		Result->Name = FCUTUtility::NormalizePathText(AssetPathString);
		SetupMaterialResult(Result, MaterialInstance, MasterMaterial);
		ResultList.Add(Result);
	}
	
	// search finish. clear variable.
	ResultView->RebuildList();
	SearchTokens.Empty();
	SortList();
}


/** result material infomation setup */
void SCMTStatList::SetupMaterialResult(TSharedPtr<FCMTStatListResult>& Result, UMaterialInterface* SelfMaterial, UMaterial* MasterMaterial)
{
	{
		FText MasterMaterialText(LOCTEXT("MasterMaterialSelf", "Self"));
		FText DomainText;
		FString ShadingModelString;
		
		if (SelfMaterial != MasterMaterial)
		{
			MasterMaterialText = FText::FromString(FCUTUtility::NormalizePathText(MasterMaterial->GetPathName()));
		}
		
		switch (MasterMaterial->MaterialDomain)
		{
		case MD_Surface:		DomainText = LOCTEXT("DomainSurface",		"Surface");			break;
		case MD_DeferredDecal:	DomainText = LOCTEXT("DomainDeferredDecal",	"Deferred Decal");	break;
		case MD_LightFunction:	DomainText = LOCTEXT("DomainLightFunction",	"Light Function");	break;
		case MD_Volume:			DomainText = LOCTEXT("DomainVolume",		"Volume");			break;
		case MD_PostProcess:	DomainText = LOCTEXT("DomainPostProcess",	"Post Process");	break;
		case MD_UI:				DomainText = LOCTEXT("DomainUI",			"User Interface");	break;
		}
		FMaterialShadingModelField ShadingModels = SelfMaterial->GetShadingModels();
		if (ShadingModels.HasShadingModel(MSM_Unlit))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("Unlit");
		}
		if (ShadingModels.HasShadingModel(MSM_DefaultLit))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("DefaultLit");
		}
		if(ShadingModels.HasShadingModel(MSM_Subsurface))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("Subsurface");
		}
		if(ShadingModels.HasShadingModel(MSM_PreintegratedSkin))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("PreintegratedSkin");
		}
		if(ShadingModels.HasShadingModel(MSM_ClearCoat))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("ClearCoat");
		}
		if(ShadingModels.HasShadingModel(MSM_SubsurfaceProfile))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("SubsurfaceProfile");
		}
		if(ShadingModels.HasShadingModel(MSM_TwoSidedFoliage))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("TwoSidedFoliage");
		}
		if (ShadingModels.HasShadingModel(MSM_Hair))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("Hair");
		}
		if (ShadingModels.HasShadingModel(MSM_Cloth))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("Cloth");
		}
		if (ShadingModels.HasShadingModel(MSM_Eye))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("Eye");
		}
		if (ShadingModels.HasShadingModel(MSM_SingleLayerWater))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("SingleLayerWater");
		}
		if (ShadingModels.HasShadingModel(MSM_ThinTranslucent))
		{
			if (!ShadingModelString.IsEmpty()) ShadingModelString += TEXT(",");
			ShadingModelString += TEXT("ThinTranslucent");
		}
		Result->Tooltip = FText::Format(LOCTEXT("Tooltip", "Master: {0}\nDomain: {1}\nShading Model: {2}"),
				MasterMaterialText,
				DomainText,
				FText::FromString(ShadingModelString));
		Result->Domain = DomainText.ToString();
		Result->ShadingModel = ShadingModelString;
	}
	switch (SelfMaterial->GetBlendMode())
	{
	case BLEND_Opaque:			Result->BlendMode = LOCTEXT("BlendOpaque",			"Opaque");			break;
	case BLEND_Masked:			Result->BlendMode = LOCTEXT("BlendMasked",			"Masked");			break;
	case BLEND_Translucent:		Result->BlendMode = LOCTEXT("BlendTranslucent",		"Translucent");		break;
	case BLEND_Additive:		Result->BlendMode = LOCTEXT("BlendAdditive",		"Additive");		break;
	case BLEND_Modulate:		Result->BlendMode = LOCTEXT("BlendModulate",		"Modulate");		break;
	case BLEND_AlphaComposite:	Result->BlendMode = LOCTEXT("BlendAlphaComposite",	"AlphaComposite");	break;
	case BLEND_AlphaHoldout:	Result->BlendMode = LOCTEXT("BlendAlphaHoldout",	"AlphaHoldout");	break;
	}
	Result->Material = SelfMaterial;
	Result->Instruction = 0;
	Result->UsePositionOffset = 0;
	Result->UseDepthOffset = 0;
	Result->UseRefraction = 0;
	
	{
		TArray<UTexture*> Textures;
		uint32 TextureSize = 0;
		
		SelfMaterial->GetUsedTextures(Textures, EMaterialQualityLevel::High, false, ERHIFeatureLevel::SM5, false);
		Result->TextureNum = Textures.Num();
		
		// don't search SceneTextures
		for (auto ItTexture = Textures.CreateConstIterator(); ItTexture; ++ItTexture)
		{
			TextureSize += (*ItTexture)->Source.GetSizeX() * (*ItTexture)->Source.GetSizeY();
		}
		Result->TextureSize = TextureSize;
	}
	
	FMaterialResource* MaterialResource = SelfMaterial->GetMaterialResource(ERHIFeatureLevel::SM5);
	if (MaterialResource)
	{
		FMaterialShaderMap* MaterialShaderMap = MaterialResource->GetGameThreadShaderMap();
		if (MaterialShaderMap && MaterialShaderMap->IsCompilationFinalized())
		{
			// FMaterialStatsUtils::GetRepresentativeInstructionCounts
			TArray<FMaterialStatsUtils::FShaderInstructionsInfo> InstructionResult;
			TMap<FName, TArray<FMaterialStatsUtils::FRepresentativeShaderInfo>> ShaderTypeNamesAndDescriptions;
			FMaterialStatsUtils::GetRepresentativeShaderTypesAndDescriptions(ShaderTypeNamesAndDescriptions, MaterialResource);

			if (MaterialResource->IsUIMaterial())
			{
				bool Hit = false;
				for (auto DescriptionPair : ShaderTypeNamesAndDescriptions)
				{
					auto& DescriptionArray = DescriptionPair.Value;
					for (int32 i = 0; i < DescriptionArray.Num(); ++i)
					{
						const FMaterialStatsUtils::FRepresentativeShaderInfo& ShaderInfo = DescriptionArray[i];

						if ((ShaderInfo.ShaderDescription.Find("Base pass") >= 0) ||
								(ShaderInfo.ShaderDescription.Find("UI Pixel Shader") >= 0))
						{
							FShaderType* ShaderType = FindShaderTypeByName(ShaderInfo.ShaderName);
							const int32 NumInstructions = MaterialShaderMap->GetMaxNumInstructionsForShader(ShaderType);
							Result->Instruction = NumInstructions;
							Hit = true;
							break;
						}
					}
					if (Hit)
					{
						break;
					}
				}
			}
			else
			{
				bool Hit = false;
				for (auto DescriptionPair : ShaderTypeNamesAndDescriptions)
				{
					FVertexFactoryType* FactoryType = FindVertexFactoryType(DescriptionPair.Key);
					const FMeshMaterialShaderMap* MeshShaderMap = MaterialShaderMap->GetMeshShaderMap(FactoryType);
					if (MeshShaderMap)
					{
						TMap<FHashedName, TShaderRef<FShader>> ShaderMap;
						MeshShaderMap->GetShaderList(*MaterialShaderMap, ShaderMap);

						auto& DescriptionArray = DescriptionPair.Value;

						for (int32 i = 0; i < DescriptionArray.Num(); ++i)
						{
							const FMaterialStatsUtils::FRepresentativeShaderInfo& ShaderInfo = DescriptionArray[i];
							if ((ShaderInfo.ShaderDescription.Find("Base pass") >= 0) ||
									(ShaderInfo.ShaderDescription.Find("UI Pixel Shader") >= 0))
							{
								TShaderRef<FShader>* ShaderEntry = ShaderMap.Find(ShaderInfo.ShaderName);
								if (ShaderEntry != nullptr)
								{
									FShaderType* ShaderType = (*ShaderEntry).GetType();
									{
										const int32 NumInstructions = MeshShaderMap->GetMaxNumInstructionsForShader(*MaterialShaderMap, ShaderType);

										Result->Instruction = NumInstructions;
										Hit = true;
										break;
									}
								}
							}
						}
						if (Hit)
						{
							break;
						}
					}
				}
			}
			
			Result->UsePositionOffset = MaterialShaderMap->UsesWorldPositionOffset() ? 1 : 0;
			Result->UseDepthOffset = MaterialShaderMap->UsesPixelDepthOffset() ? 1 : 0;
			Result->UseRefraction = (MaterialResource->IsDistorted()) ? 1 : 0;
		}
	}
	else
	{
		// error
	}
}

// Search box --- End




// Result list --- Begin


SHeaderRow::FColumn::FArguments SCMTStatList::CreateHeaderColumn(const FText& Name, int32 Width)
{
	const FName NameName(*Name.ToString());
	
	return SHeaderRow::Column(NameName)
		.FillWidth(Width)
		.SortMode(TAttribute<EColumnSortMode::Type>::Create(TAttribute<EColumnSortMode::Type>::FGetter::CreateSP(this, &SCMTStatList::GetHeaderSortMode, NameName)))
		.SortPriority(TAttribute< EColumnSortPriority::Type >::Create(TAttribute< EColumnSortPriority::Type >::FGetter::CreateSP(this, &SCMTStatList::GetHeaderSortPriority, NameName)))
		.OnSort(FOnSortModeChanged::CreateSP(this, &SCMTStatList::OnSortHeader))
		.DefaultLabel(Name);
}

TSharedRef<ITableRow> SCMTStatList::OnGenerateRow(TSharedPtr<FCMTStatListResult> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCMTStatListResultRow, OwnerTable)
			.Info(Item);
}
EColumnSortMode::Type SCMTStatList::GetHeaderSortMode(const FName ColumnName) const
{
	for (int32 PriorityIdx = 0; PriorityIdx < ListSortName.Num(); PriorityIdx++)
	{
		if (ColumnName == ListSortName[PriorityIdx])
		{
			return ListSortMode[PriorityIdx];
		}
	}
	
	return EColumnSortMode::None;
}

EColumnSortPriority::Type SCMTStatList::GetHeaderSortPriority(const FName ColumnName) const
{
	for (int32 PriorityIdx = 0; PriorityIdx < ListSortName.Num(); PriorityIdx++)
	{
		if (ColumnName == ListSortName[PriorityIdx])
		{
			return static_cast<EColumnSortPriority::Type>(PriorityIdx);
		}
	}
	
	return EColumnSortPriority::Secondary;
}


void SCMTStatList::OnSortHeader(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type NewSortMode)
{
	int32 Index = INDEX_NONE;
	ListSortName.Find(ColumnName, Index);
	if (Index != INDEX_NONE)
	{
		ListSortName.RemoveAt(Index);
		ListSortMode.RemoveAt(Index);
		ListSortName.Insert(ColumnName, 0);
		ListSortMode.Insert(NewSortMode, 0);
	}
	else
	{
		ListSortName.Insert(ColumnName, 0);
		ListSortMode.Insert(NewSortMode, 0);
		ListSortName.RemoveAt(2);
		ListSortMode.RemoveAt(2);
	}
	
	// list sort
	SortList();
}

void SCMTStatList::SortList()
{
	TArray<TSharedPtr<FCMTStatListResult>> TmpList(ResultList);
	
	ResultList.Empty();
	for (auto ItResultAdd = TmpList.CreateConstIterator(); ItResultAdd; ++ItResultAdd)
	{
		bool bInsert = false;
		for (int32 Index = 0 ; Index < ResultList.Num() ; ++Index)
		{
			TSharedPtr<FCMTStatListResult> Result = ResultList[Index];
			int32 SortResult = CheckSortResult(*ItResultAdd, Result, ListSortName[0]);
			if ((ListSortMode[0] == EColumnSortMode::Ascending) && (SortResult < 0))
			{
				ResultList.Insert(*ItResultAdd, Index);
				bInsert = true;
				break;
			}
			else if ((ListSortMode[0] == EColumnSortMode::Descending) && (SortResult > 0))
			{
				ResultList.Insert(*ItResultAdd, Index);
				bInsert = true;
				break;
			}
		}
		if (bInsert == false)
		{
			ResultList.Add(*ItResultAdd);
		}
	}
	
	// widget rebuild
	ResultView->RebuildList();
	
	AddClipboardText();
}

int32 SCMTStatList::CheckSortResult(const TSharedPtr<FCMTStatListResult>& InsertResult,
		const TSharedPtr<FCMTStatListResult>& CheckResult, const FName& SortName)
{
	if (SortName == GetHeaderNameTextName())
	{
		if (InsertResult->Name < CheckResult->Name)
		{
			return -1;
		}
		else if (InsertResult->Name > CheckResult->Name)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (SortName == GetHeaderShadingModelTextName())
	{
		return InsertResult->ShadingModel.Compare(CheckResult->ShadingModel, ESearchCase::CaseSensitive);
	}
	else if (SortName == GetHeaderBlendModeTextName())
	{
		return InsertResult->BlendMode.CompareTo(CheckResult->BlendMode);
	}
	else if (SortName == GetHeaderInstructionsTextName())
	{
		if (InsertResult->Instruction < CheckResult->Instruction)
		{
			return -1;
		}
		else if (InsertResult->Instruction > CheckResult->Instruction)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (SortName == GetHeaderTextureNumTextName())
	{
		if (InsertResult->TextureNum < CheckResult->TextureNum)
		{
			return -1;
		}
		else if (InsertResult->TextureNum > CheckResult->TextureNum)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (SortName == GetHeaderTextureSizeTextName())
	{
		if (InsertResult->TextureSize < CheckResult->TextureSize)
		{
			return -1;
		}
		else if (InsertResult->TextureSize > CheckResult->TextureSize)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (SortName == GetHeaderPositionOffsetTextName())
	{
		if (InsertResult->UsePositionOffset < CheckResult->UsePositionOffset)
		{
			return -1;
		}
		else if (InsertResult->UsePositionOffset > CheckResult->UsePositionOffset)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (SortName == GetHeaderDepthOffsetTextName())
	{
		if (InsertResult->UseDepthOffset < CheckResult->UseDepthOffset)
		{
			return -1;
		}
		else if (InsertResult->UseDepthOffset > CheckResult->UseDepthOffset)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (SortName == GetHeaderRefractionTextName())
	{
		if (InsertResult->UseRefraction < CheckResult->UseRefraction)
		{
			return -1;
		}
		else if (InsertResult->UseRefraction > CheckResult->UseRefraction)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 1;
}


// Result list --- End




// Progress Bar --- Begin

EVisibility SCMTStatList::GetProgressBarVisibility() const
{
	return MaterialSearcher.IsAsyncLoading() ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SCMTStatList::GetProgressBarPercent() const
{
	if (!MaterialSearcher.IsAsyncLoading())
	{
		return 1.0f;
	}
	
	return MaterialSearcher.GetProgress();
}

// Progress Bar --- End

/* CopyClipboard clicked event */
FReply SCMTStatList::ButtonCopyClipBoardClicked()
{
	FCUTUtility::ExportClipboard(TextClipboard);
	
	return FReply::Handled();
}

/* ExportCSV clicked event */
FReply SCMTStatList::ButtonExportCSVClicked()
{
	FCUTUtility::ExportTxt("StatList", "CMTStatList.csv", TextClipboard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}

void SCMTStatList::AddClipboardText()
{
	TextClipboard = FString::Printf(TEXT("Search Path: %s\n"), *SearchValue);
	TextClipboard += FString::Printf(TEXT("Name,Domain,ShadingModel,BlendMode,Instruction,TextureNum,TextureSize,UsePositionOffset,UseDepthOffset,UseRefraction\n"));
	
	for (auto It = ResultList.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromStatListResult(*It, &TextClipboard);
	}
}



#undef LOCTEXT_NAMESPACE

