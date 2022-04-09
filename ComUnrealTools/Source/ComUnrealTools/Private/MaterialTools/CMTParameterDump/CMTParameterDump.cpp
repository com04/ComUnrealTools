// Copyright com04 All Rights Reserved.

#include "CMTParameterDump.h"
#include "CMTParameterDumpResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetData.h"
#include "Engine/Font.h"
#include "Engine/Texture.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInterface.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "VT/RuntimeVirtualTexture.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "CMTParameterDump"




////////////////////////////////////
// SCMTParameterDump


SCMTParameterDump::~SCMTParameterDump()
{
}

void SCMTParameterDump::Construct(const FArguments& InArgs)
{
	SelectedMaterial = nullptr;
	
	// asset search filter
	TArray<const UClass*> ClassFilters;
	ClassFilters.Add(UMaterialInterface::StaticClass());
	
	
	ChildSlot
	[
		SNew(SVerticalBox)
	
		// target asset
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("CheckMaterialLabel", "Material Asset: "))
			]
			
			+ SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				SNew(SObjectPropertyEntryBox)
				.OnShouldFilterAsset(this, &SCMTParameterDump::ShouldFilterAsset)
				.NewAssetFactories(PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses(ClassFilters))
				.ObjectPath(this, &SCMTParameterDump::GetObjectPath)
				.OnObjectChanged(this, &SCMTParameterDump::OnObjectChanged)
			]
			+SHorizontalBox::Slot()
			.FillWidth(0.5f)
			[
				SNullWidget::NullWidget
			]
		]
		

		// options
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 4.0f, 0.0f, 0.0f)
		[
			SNew(SVerticalBox)
			// check box "disp override only"
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.f, 2.f)
			[
				SNew(SHorizontalBox)
			
				// check box
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SCheckBox)
					.IsChecked(GetDispOverrideOnlyState())
					.OnCheckStateChanged(this, &SCMTParameterDump::OnDispOverrideOnlyStateChanged)
				]
				
				// Asset property
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("DispOverrideOnly", "disp override only"))
				]
			]
		]
		
		// Button
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 10.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
		
			// Asset Check
			+ SHorizontalBox::Slot()
			.MaxWidth(300.f)
			[
				SAssignNew(AssetCheckButton, SButton)
				.Text(LOCTEXT("AssetCheckButton", "Asset Check"))
				.OnClicked(this, &SCMTParameterDump::ButtonAssetCheckClicked)
				.IsEnabled(false)
			]
			// copy clipboard
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(50.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTParameterDump::ButtonCopyClipBoardClicked)
				.IsEnabled(false)
			]
			// export Text
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCMTParameterDump::ButtonExportTextClicked)
				.IsEnabled(false)
			]
			// export csv
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportCsvButton, SButton)
				.Text(LOCTEXT("ExportCsv", "Export CSV"))
				.OnClicked(this, &SCMTParameterDump::ButtonExportCsvClicked)
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
				SAssignNew(TreeView, SCMTParameterDumpViewType)
				.ItemHeight(24)
				.TreeItemsSource(&ItemsFound)
				.OnGenerateRow(this, &SCMTParameterDump::OnGenerateRow)
				.OnGetChildren(this, &SCMTParameterDump::OnGetChildren)
				.OnMouseButtonDoubleClick(this,&SCMTParameterDump::OnTreeSelectionDoubleClicked)
			]
		]
	];
	
}

/** EntryBox asset filtering */
bool SCMTParameterDump::ShouldFilterAsset(const FAssetData& AssetData)
{
	UObject* AssetObject = AssetData.GetAsset();
	if (AssetObject->IsA(UMaterialInterface::StaticClass()))
	{
		return false;
	}
	return true;
}

/* EntryBox get display path */
FString SCMTParameterDump::GetObjectPath() const
{
	if (SelectedMaterial)
	{
		return SelectedMaterial->GetPathName();
	}
    return FString("");
}

/* EntryBox change object event */
void SCMTParameterDump::OnObjectChanged(const FAssetData& AssetData)
{
	SelectedMaterial = Cast<UMaterialInterface>(AssetData.GetAsset());
	AssetCheckButton->SetEnabled(SelectedMaterial != nullptr);
}

/** AssetCheckButton clicked event */
FReply SCMTParameterDump::ButtonAssetCheckClicked()
{
	CheckMaterialName = FString();
	if (!IsValid(SelectedMaterial))
	{
		return FReply::Handled();
	}
	
	ItemsFound.Empty();
	
	const bool bOverridenOnly = (GetDispOverrideOnlyState() == ECheckBoxState::Checked);
	TArray<FMaterialParameterInfo> ScalarParameters;
	TArray<FMaterialParameterInfo> VectorParameters;
	TArray<FMaterialParameterInfo> TextureParameters;
	TArray<FMaterialParameterInfo> StaticSwitchParameters;
	TArray<FMaterialParameterInfo> StaticComponentMaskParameters;
	TArray<FMaterialParameterInfo> FontParameters;
	TArray<FMaterialParameterInfo> VirtualTextureParameters;
	
	if (GetDispOverrideOnlyState() == ECheckBoxState::Checked)
	{
		// オーバーライドしているパラメーターのみ
		UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(SelectedMaterial);
		if (IsValid(MaterialInstance))
		{
			// プロパティ欄のパラメーター（グラフではなく詳細欄にある）はGUIDが空っぽなのでそれで弾く
			ScalarParameters.Reserve(MaterialInstance->ScalarParameterValues.Num());
			for (const FScalarParameterValue& ParameterValue : MaterialInstance->ScalarParameterValues)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					ScalarParameters.Add(ParameterValue.ParameterInfo);
				}
			}
			VectorParameters.Reserve(MaterialInstance->VectorParameterValues.Num());
			for (const FVectorParameterValue& ParameterValue : MaterialInstance->VectorParameterValues)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					VectorParameters.Add(ParameterValue.ParameterInfo);
				}
			}
			TextureParameters.Reserve(MaterialInstance->TextureParameterValues.Num());
			for (const FTextureParameterValue& ParameterValue : MaterialInstance->TextureParameterValues)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					TextureParameters.Add(ParameterValue.ParameterInfo);
				}
			}
			const FStaticParameterSet& MaterialStaticParameter = MaterialInstance->GetStaticParameters();
			StaticSwitchParameters.Reserve(MaterialStaticParameter.StaticSwitchParameters.Num());
			for (const FStaticSwitchParameter& ParameterValue : MaterialStaticParameter.StaticSwitchParameters)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					StaticSwitchParameters.Add(ParameterValue.ParameterInfo);
				}
			}
			StaticComponentMaskParameters.Reserve(MaterialStaticParameter.StaticComponentMaskParameters.Num());
			for (const FStaticComponentMaskParameter& ParameterValue : MaterialStaticParameter.StaticComponentMaskParameters)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					StaticComponentMaskParameters.Add(ParameterValue.ParameterInfo);
				}
			}
			FontParameters.Reserve(MaterialInstance->FontParameterValues.Num());
			for (const FFontParameterValue& ParameterValue : MaterialInstance->FontParameterValues)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					FontParameters.Add(ParameterValue.ParameterInfo);
				}
			}
			VirtualTextureParameters.Reserve(MaterialInstance->RuntimeVirtualTextureParameterValues.Num());
			for (const FRuntimeVirtualTextureParameterValue& ParameterValue : MaterialInstance->RuntimeVirtualTextureParameterValues)
			{
				if (ParameterValue.ExpressionGUID.IsValid())
				{
					VirtualTextureParameters.Add(ParameterValue.ParameterInfo);
				}
			}
		}
	}
	else
	{
		// 全パラメーター
		TArray<FGuid> Guids;
		SelectedMaterial->GetAllScalarParameterInfo(ScalarParameters, Guids);
		SelectedMaterial->GetAllVectorParameterInfo(VectorParameters, Guids);
		SelectedMaterial->GetAllTextureParameterInfo(TextureParameters, Guids);
		SelectedMaterial->GetAllStaticSwitchParameterInfo(StaticSwitchParameters, Guids);
		SelectedMaterial->GetAllStaticComponentMaskParameterInfo(StaticComponentMaskParameters, Guids);
		SelectedMaterial->GetAllFontParameterInfo(FontParameters, Guids);
		SelectedMaterial->GetAllRuntimeVirtualTextureParameterInfo(VirtualTextureParameters, Guids);
	}
	
	for (const auto& Parameter : ScalarParameters)
	{
		float Value = 0.0f; 
		SelectedMaterial->GetScalarParameterValue(Parameter, Value);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(), FString::SanitizeFloat(Value), ECMTParameterDumpResultType::Scalar)));
	}
	for (const auto& Parameter : VectorParameters)
	{
		FLinearColor Value;
		SelectedMaterial->GetVectorParameterValue(Parameter, Value);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(),
				FString::Printf(TEXT("R=%f G=%f B=%f A=%f"), Value.R, Value.G, Value.B, Value.A),
				ECMTParameterDumpResultType::Vector)));
	}
	for (const auto& Parameter : TextureParameters)
	{
		UTexture* Value = nullptr;
		SelectedMaterial->GetTextureParameterValue(Parameter, Value);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(), Value ? Value->GetPathName() : TEXT("empty"), ECMTParameterDumpResultType::Texture, Value)));
	}
	for (const auto& Parameter : StaticSwitchParameters)
	{
		bool Value = false;
		FGuid Guid;
		SelectedMaterial->GetStaticSwitchParameterValue(Parameter, Value, Guid);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(), Value ? TEXT("ON") : TEXT("OFF"), ECMTParameterDumpResultType::Switch)));
	}
	for (const auto& Parameter : StaticComponentMaskParameters)
	{
		bool ValueR = false;
		bool ValueG = false;
		bool ValueB = false;
		bool ValueA = false;
		FGuid Guid;
		SelectedMaterial->GetStaticComponentMaskParameterValue(Parameter, ValueR, ValueG, ValueB, ValueA, Guid);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(),
				FString::Printf(TEXT("R=%s G=%s B=%s A=%s"), ValueR ? TEXT("ON") : TEXT("OFF"), ValueG ? TEXT("ON") : TEXT("OFF"), ValueB ? TEXT("ON") : TEXT("OFF"), ValueA ? TEXT("ON") : TEXT("OFF")), 
				ECMTParameterDumpResultType::ComponentMask)));
	}
	for (const auto& Parameter : FontParameters)
	{
		UFont* Value = nullptr; 
		int32 FontPage = 0;
		SelectedMaterial->GetFontParameterValue(Parameter, Value, FontPage);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(), Value ? Value->GetPathName() : TEXT("empty"), ECMTParameterDumpResultType::Font)));
	}
	for (const auto& Parameter : VirtualTextureParameters)
	{
		URuntimeVirtualTexture* Value = nullptr;
		SelectedMaterial->GetRuntimeVirtualTextureParameterValue(Parameter, Value);
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(Parameter.Name.ToString(), Value ? Value->GetPathName() : TEXT("empty"), ECMTParameterDumpResultType::VirtualTexture)));
	}
	
	// not found
	if (ItemsFound.Num() == 0)
	{
		ItemsFound.Add(FCMTParameterDumpResultShare(new FCMTParameterDumpResult(TEXT("No Results found"), TEXT(""), ECMTParameterDumpResultType::Other)));
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
	
	CheckMaterialName = SelectedMaterial->GetName();

	return FReply::Handled();
}

/** CopyClipboard clicked event */
FReply SCMTParameterDump::ButtonCopyClipBoardClicked()
{
	FString ClipBoard = GetClipboardText();

	FCUTUtility::ExportClipboard(ClipBoard);
	
	return FReply::Handled();
}

/** ExportText clicked event */
FReply SCMTParameterDump::ButtonExportTextClicked()
{
	FString ClipBoard = GetClipboardText();

	FString ExportFileName = CheckMaterialName.IsEmpty() ? TEXT("CMTParameterDump") : CheckMaterialName;
	ExportFileName += FString(TEXT(".txt"));
	FCUTUtility::ExportTxt(TEXT("Parameter Dump"), ExportFileName, ClipBoard, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

/** ExportText clicked event */
FReply SCMTParameterDump::ButtonExportCsvClicked()
{
	FString ClipBoard = GetClipboardCsv();

	FString ExportFileName = CheckMaterialName.IsEmpty() ? TEXT("CMTParameterDump") : CheckMaterialName;
	ExportFileName += FString(TEXT(".csv"));
	FCUTUtility::ExportTxt(TEXT("Parameter Dump"), ExportFileName, ClipBoard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}



/** row generate */
TSharedRef<ITableRow> SCMTParameterDump::OnGenerateRow(FCMTParameterDumpResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	FString ParameterValueString;
	FString ItemParameterValueString = InItem->GetParameterValueString();
	if (!ItemParameterValueString.IsEmpty())
	{
		ParameterValueString = FString::Printf(TEXT("     -> %s"), *ItemParameterValueString);
	}

	return SNew(STableRow< TSharedPtr<FCMTParameterDumpResultShare> >, OwnerTable)
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
					.Text(FText::FromString(InItem->GetParamNameString()))
					.ColorAndOpacity(FLinearColor::White)
				]
				
				// parameter value
				+SHorizontalBox::Slot()
				.FillWidth(1)
				.VAlign(VAlign_Center)
				.Padding(2,0)
				[
					SNew(STextBlock)
					.Text(FText::FromString(ParameterValueString))
					.ColorAndOpacity(FLinearColor::Yellow)
				]
			]
		];
}
/** tree child */
void SCMTParameterDump::OnGetChildren(FCMTParameterDumpResultShare InItem, TArray<FCMTParameterDumpResultShare>& OutChildren)
{
}
/** item double click event */
void SCMTParameterDump::OnTreeSelectionDoubleClicked(FCMTParameterDumpResultShare Item)
{
	if(Item.IsValid())
	{
		Item->OnClick();
	}
}





FString SCMTParameterDump::GetClipboardText()
{
	FString RetString;
	if (IsValid(SelectedMaterial))
	{
		RetString = FString::Printf(TEXT("Material: %s\n"), *SelectedMaterial->GetPathName());
	}
	else
	{
		RetString = FString::Printf(TEXT("Material: \n"));
	}
	
	
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		RetString += FString::Printf(TEXT("- %s: %s     -> %s\n"), *(*It)->GetTypeString(), *(*It)->GetParamNameString(), *(*It)->GetParameterValueString());
	}
	return RetString;
}

FString SCMTParameterDump::GetClipboardCsv()
{
	FString RetString;
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		RetString += FString::Printf(TEXT("%s,%s,\"%s\"\n"), *(*It)->GetTypeString(), *(*It)->GetParamNameString(), *(*It)->GetParameterValueString());
	}
	return RetString;
}


#undef LOCTEXT_NAMESPACE

