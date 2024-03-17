// Copyright com04 All Rights Reserved.

#include "CMTStatList.h"
#include "CMTStatListResult.h"
#include "CMTStatListResultRow.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
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




////////////////////////////////////
// SCMTStatList

const FText& SCMTStatList::GetHeaderNameText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderName", "Name"));
	return HeaderNameText;
}
FName SCMTStatList::GetHeaderNameTextName()
{
	return FName(*GetHeaderNameText().ToString());
}
const FText& SCMTStatList::GetHeaderDomainText()
{
	static const FText HeaderDomainText(LOCTEXT("HeaderDomain", "Domain"));
	return HeaderDomainText;
}
FName SCMTStatList::GetHeaderDomainTextName()
{
	return FName(*GetHeaderDomainText().ToString());
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
const FText& SCMTStatList::GetHeaderRenderAfterDOFText()
{
	static const FText HeaderRenderAfterDOFText(LOCTEXT("HeaderRenderAfterDOF", "RenderAfterDOF"));
	return HeaderRenderAfterDOFText;
}
FName SCMTStatList::GetHeaderRenderAfterDOFTextName()
{
	return FName(*GetHeaderRenderAfterDOFText().ToString());
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
		+ CreateHeaderColumn(GetHeaderDomainText(), 30)
		+ CreateHeaderColumn(GetHeaderShadingModelText(), 30)
		+ CreateHeaderColumn(GetHeaderBlendModeText(), 30)
		+ CreateHeaderColumn(GetHeaderInstructionsText(), 30)
		+ CreateHeaderColumn(GetHeaderTextureNumText(), 30)
		+ CreateHeaderColumn(GetHeaderTextureSizeText(), 30)
		+ CreateHeaderColumn(GetHeaderRenderAfterDOFText(), 30)
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
		.Padding(0.0f, 4.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 4.0f, 12.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SearchPath", "Search Path"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("StatListFind", "Enter material path to find references..."))
				.InitialText(FText::FromString(GetSearchValue()))
				.OnTextCommitted(this, &SCMTStatList::OnSearchTextCommitted)
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
				.OnCheckStateChanged(this, &SCMTStatList::OnCheckMaterialInstanceChanged)
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
				.OnClicked(this, &SCMTStatList::ButtonSearchStartClicked)
				.IsEnabled(false)
			]
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(50.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTStatList::ButtonCopyClipBoardClicked)
				.IsEnabled(false)
			]
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCMTStatList::ButtonExportTextClicked)
				.IsEnabled(false)
			]
			// export csv
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportCsvButton, SButton)
				.Text(LOCTEXT("ExportCsv", "Export CSV"))
				.OnClicked(this, &SCMTStatList::ButtonExportCsvClicked)
				.IsEnabled(false)
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
	
	OnSearchTextCommitted(FText::FromString(GetSearchValue()), ETextCommit::OnEnter);
}

FReply SCMTStatList::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}




// Search box --- Begin

/** text commit event */
void SCMTStatList::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchValue(Text.ToString());
	
	SearchStartButton->SetEnabled(!GetSearchValue().IsEmpty());
}

void SCMTStatList::FinishSearch()
{
	const TArray<FAssetData>& MaterialAsset = MaterialSearcher.GetMaterialAssets();
	const TArray<FAssetData>& MaterialInstanceAsset = MaterialSearcher.GetMaterialInstanceAssets();
	
	for (auto It = MaterialAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->GetObjectPathString();
		
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
		FString AssetPathString = It->GetObjectPathString();
		
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
	
	if (ResultList.Num() <= 0)
	{
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
	
	// search finish. clear variable.
	ResultView->RebuildList();
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
	Result->UseRenderAfterDOF = 0;
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
#if ENGINE_MAJOR_VERSION == 5
			Result->UseRenderAfterDOF = (MasterMaterial->TranslucencyPass == MTP_AfterDOF) ? 1 : 0;
#else
			Result->UseRenderAfterDOF = MasterMaterial->bEnableSeparateTranslucency ? 1 : 0;
#endif
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
	else if (SortName == GetHeaderDomainTextName())
	{
		return InsertResult->Domain.Compare(CheckResult->Domain, ESearchCase::CaseSensitive);
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
	else if (SortName == GetHeaderRenderAfterDOFTextName())
	{
		if (InsertResult->UseRenderAfterDOF < CheckResult->UseRenderAfterDOF)
		{
			return -1;
		}
		else if (InsertResult->UseRenderAfterDOF > CheckResult->UseRenderAfterDOF)
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

/** SearchStart clicked event */
FReply SCMTStatList::ButtonSearchStartClicked()
{
	// search text parse
	FCUTUtility::SplitStringTokens(GetSearchValue(), &SearchTokens);

	// last result clear
	ResultList.Empty();
	
	// search
	if (SearchTokens.Num() > 0)
	{
		MaterialSearcher.SearchStart(SearchTokens, TArray<FString>(),
				true,
				true, (GetCheckMaterialInstance() == ECheckBoxState::Checked),
				false);
	}
	return FReply::Handled();
}

/* CopyClipboard clicked event */
FReply SCMTStatList::ButtonCopyClipBoardClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportClipboard(Clipboard);
	
	return FReply::Handled();
}

/** ExportText clicked event */
FReply SCMTStatList::ButtonExportTextClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportTxt("StatList", "CMTStatList.txt", Clipboard, TEXT("Text |*.txt"));
	return FReply::Handled();
}

/* ExportCSV clicked event */
FReply SCMTStatList::ButtonExportCsvClicked()
{
	FString Clipboard = GetClipboardText();
	FCUTUtility::ExportTxt("StatList", "CMTStatList.csv", Clipboard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}

FString SCMTStatList::GetClipboardText()
{
	FString RetString;
	RetString = FString::Printf(TEXT("Search Path: %s\n"), *GetSearchValue());
	RetString += FString::Printf(TEXT("Name,Domain,ShadingModel,BlendMode,Instruction,TextureNum,TextureSize,UseRenderAfterDOF,UsePositionOffset,UseDepthOffset,UseRefraction\n"));
	
	for (auto It = ResultList.CreateConstIterator() ; It ; ++It)
	{
		AddClipboardTextFromStatListResult(*It, &RetString);
	}
	return RetString;
}

/**
 * Add clip board string from Result.
 */
void SCMTStatList::AddClipboardTextFromStatListResult(const TSharedPtr<FCMTStatListResult>& Result, FString* ExportText)
{
	*ExportText += FString::Printf(TEXT("%s,%s,%s,%s,%u,%d,%u,%s,%s,%s,%s\n"),
			*Result->Name,
			*Result->Domain,
			*Result->ShadingModel,
			*Result->BlendMode.ToString(),
			Result->Instruction,
			Result->TextureNum,
			Result->TextureSize,
			Result->UseRenderAfterDOF > 0 ? TEXT("True") : TEXT("False"),
			Result->UsePositionOffset > 0 ? TEXT("True") : TEXT("False"),
			Result->UseDepthOffset > 0 ? TEXT("True") : TEXT("False"),
			Result->UseRefraction > 0 ? TEXT("True") : TEXT("False"));
}



#undef LOCTEXT_NAMESPACE

