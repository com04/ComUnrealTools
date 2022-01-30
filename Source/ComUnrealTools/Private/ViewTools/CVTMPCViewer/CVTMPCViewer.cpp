// Copyright com04 All Rights Reserved.

#include "CVTMPCViewer.h"
#include "CVTMPCViewerResult.h"
#include "CVTMPCViewerResultRow.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetData.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "CVTMPCViewer"




////////////////////////////////////
// SCVTMPCViewer
TWeakObjectPtr<UMaterialParameterCollection> SCVTMPCViewer::SelectedMPC;


SCVTMPCViewer::~SCVTMPCViewer()
{
}

void SCVTMPCViewer::Construct(const FArguments& InArgs)
{
	bPIEFinishReset = false;

	// asset search filter
	TArray<const UClass*> ClassFilters;
	ClassFilters.Add(UMaterialParameterCollection::StaticClass());
	
	// 結果Listのヘッダー構成
	TSharedPtr<SHeaderRow> Header =
		SNew(SHeaderRow)
		.ResizeMode(ESplitterResizeMode::FixedSize)
		
		+ CreateHeaderColumn(GetHeaderParameterNameText(), 100)
		+ CreateHeaderColumn(GetHeaderParameterDefaultValueText(), 100)
		+ CreateHeaderColumn(GetHeaderParameterValueText(), 100)
		+ CreateHeaderColumn(GetHeaderWatchText(), 20)
		;
	
	
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor::Gray) // Darken the outer border
		.Padding(FMargin(5.0f, 5.0f))
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
					.Text(LOCTEXT("CheckMPCLabel", "MPC Asset: "))
				]
				
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SObjectPropertyEntryBox)
					.OnShouldFilterAsset(this, &SCVTMPCViewer::ShouldFilterAsset)
					.NewAssetFactories(PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses(ClassFilters))
					.ObjectPath(this, &SCVTMPCViewer::GetObjectPath)
					.OnObjectChanged(this, &SCVTMPCViewer::OnObjectChanged)
				]
				+SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNullWidget::NullWidget
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 10.0f, 0.0f, 0.0f)
			[
				// アセット名表示
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.f, 4.f, 12.f, 0.f)
				[
					SNew(STextBlock)
					.Text(this, &SCVTMPCViewer::GetAssetPathText)
				]
				
				// ウォッチ
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 4.f, 12.f, 0.f)
				[
					SNew(SButton)
					.Text(LOCTEXT("MPCWatch", "ウォッチに入れる"))
					.OnClicked(this, &SCVTMPCViewer::ButtonMPCWatchClicked)
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 10.0f, 0.0f, 0.0f)
			[
				// ランタイムで変更されてない表示
				SAssignNew(MPCNoChangedBox, SHorizontalBox)

				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(0.f, 4.f, 12.f, 0.f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("MPCNoChanged", "ランタイムでの変更はありません"))
					.ColorAndOpacity(FLinearColor(0.8f, 0.2f, 0.2f, 1.0f))
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
					SAssignNew(ResultView, SListView<TSharedPtr<FCVTMPCViewerResult>>)
					.ListItemsSource(&ResultList)
					.HeaderRow(Header)
					.SelectionMode(ESelectionMode::None)
					.OnGenerateRow(this, &SCVTMPCViewer::OnGenerateRowResultList)
				]
			]
		]
	];
	
	SetupMPCResult();
}

const FText& SCVTMPCViewer::GetHeaderParameterNameText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderName", "Parameter Name"));
	return HeaderNameText;
}
const FText& SCVTMPCViewer::GetHeaderParameterDefaultValueText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderDefaultValue", "Default Value"));
	return HeaderNameText;
}
const FText& SCVTMPCViewer::GetHeaderParameterValueText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderValue", "Value"));
	return HeaderNameText;
}
const FText& SCVTMPCViewer::GetHeaderWatchText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderWatch", "ウォッチへ"));
	return HeaderNameText;
}

void SCVTMPCViewer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	if (SelectedMPC.IsValid())
	{
		UWorld* World = nullptr;
		// PIEでの再生中World
		if (GEditor)
		{
			World = GEditor->PlayWorld;
		}
		// Todo: Standalone再生中のWorldが取れない...

		bool bChangedParameter = false;
		if (World)
		{
			bPIEFinishReset = false;
			UMaterialParameterCollectionInstance* MPCInstance = World->GetParameterCollectionInstance(SelectedMPC.Get());
			for (TSharedPtr<FCVTMPCViewerResult>& Result : ResultList)
			{
				if (Result->bIsScalar)
				{
					float ParameterValue = 0.0f;
					if (MPCInstance->GetScalarParameterValue(Result->ParameterName, ParameterValue))
					{
						Result->ScalarValue = ParameterValue;
						bChangedParameter = bChangedParameter || !FMath::IsNearlyEqual(Result->ScalarValue, Result->DefaultScalarValue);
					}
				}
				else
				{
					FLinearColor ParameterValue = FLinearColor::Black;
					if (MPCInstance->GetVectorParameterValue(Result->ParameterName, ParameterValue))
					{
						Result->VectorValue = ParameterValue;
						bChangedParameter = bChangedParameter || !Result->VectorValue.Equals(Result->DefaultVectorValue);
					}
				}
			}
		}
		else if (!bPIEFinishReset)
		{
			// 再生終わっているので初期化する
			for (TSharedPtr<FCVTMPCViewerResult>& Result : ResultList)
			{
				Result->ScalarValue = Result->DefaultScalarValue;
				Result->VectorValue = Result->DefaultVectorValue;
			}
			bPIEFinishReset = true;
		}
		if (MPCNoChangedBox.IsValid())
		{
			MPCNoChangedBox->SetVisibility(bChangedParameter ? EVisibility::Hidden : EVisibility::SelfHitTestInvisible);
		}
	}
}

// Asset Select --------
/** アセット選択のクラスフィルター */
bool SCVTMPCViewer::ShouldFilterAsset(const FAssetData& AssetData)
{
	UObject* AssetObject = AssetData.GetAsset();
	if (AssetObject->IsA(UMaterialParameterCollection::StaticClass()))
	{
		return false;
	}
	return true;
}

/* 選択したアセット表示パス */
FString SCVTMPCViewer::GetObjectPath() const
{
	if (SelectedMPC.IsValid())
	{
		return SelectedMPC->GetPathName();
	}
    return FString("");
}

/* アセット選択変更イベント */
void SCVTMPCViewer::OnObjectChanged(const FAssetData& AssetData)
{
	SelectedMPC = Cast<UMaterialParameterCollection>(AssetData.GetAsset());
	SetupMPCResult();
}

void SCVTMPCViewer::SetupMPCResult()
{
	ResultList.Empty();
	if (SelectedMPC.IsValid())
	{
		AssetPathText = FText::FromString(FCUTUtility::NormalizePathText(SelectedMPC->GetPathName()));
		for (const FCollectionScalarParameter& Parameter : SelectedMPC->ScalarParameters)
		{
			TSharedPtr<FCVTMPCViewerResult> Result(new FCVTMPCViewerResult);
			Result->ParameterName = Parameter.ParameterName;
			Result->bIsScalar = true;
			Result->DefaultScalarValue = Result->ScalarValue = Parameter.DefaultValue;
			ResultList.Add(Result);
		}
		for (const FCollectionVectorParameter& Parameter : SelectedMPC->VectorParameters)
		{
			TSharedPtr<FCVTMPCViewerResult> Result(new FCVTMPCViewerResult);
			Result->ParameterName = Parameter.ParameterName;
			Result->bIsScalar = false;
			Result->DefaultVectorValue = Result->VectorValue = Parameter.DefaultValue;
			ResultList.Add(Result);
		}
	}
	else
	{
		AssetPathText = FText();
	}
	ResultView->RebuildList();
}

// -------- Asset Select

// Text --------
FText SCVTMPCViewer::GetAssetPathText() const
{
	return AssetPathText;
}

// -------- Text

// Buttons --------
/* ウォッチに入れるボタン */
FReply SCVTMPCViewer::ButtonMPCWatchClicked()
{
	// todo
	return FReply::Handled();
}
// -------- Buttons


// Result List --------
SHeaderRow::FColumn::FArguments SCVTMPCViewer::CreateHeaderColumn(const FText& Name, int32 Width)
{
	const FName NameName(*Name.ToString());
	
	return SHeaderRow::Column(NameName)
		.FillWidth(Width)
		.DefaultLabel(Name);
}
TSharedRef<ITableRow> SCVTMPCViewer::OnGenerateRowResultList(TSharedPtr<FCVTMPCViewerResult> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCVTMPCViewerResultRow, OwnerTable)
			.Info(Item);
}
// -------- Result List


#undef LOCTEXT_NAMESPACE

