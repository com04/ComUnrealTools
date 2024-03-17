// Copyright com04 All Rights Reserved.

#include "CVTMPCViewerWatch.h"
#include "CVTMPCViewerWatchResultRow.h"
#include "ViewTools/CVTMPCViewer/CVTMPCViewerResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "UnrealTools/CUTDeveloperSettings.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistry/AssetData.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "PropertyCustomizationHelpers.h"
#include "PropertyHandle.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "CVTMPCViewerWatch"




////////////////////////////////////
// SCVTMPCViewerWatch
TArray<FCVTMPCViewerWatchResultShare> SCVTMPCViewerWatch::ResultList;
bool SCVTMPCViewerWatch::bRequestRefreshResult = false;
bool SCVTMPCViewerWatch::bDirtyEditorSettings = true;


SCVTMPCViewerWatch::~SCVTMPCViewerWatch()
{
}

void SCVTMPCViewerWatch::Construct(const FArguments& InArgs)
{
	// asset search filter
	TArray<const UClass*> ClassFilters;
	ClassFilters.Add(UMaterialParameterCollection::StaticClass());
	
	// 結果Listのヘッダー構成
	TSharedPtr<SHeaderRow> Header =
		SNew(SHeaderRow)
		.ResizeMode(ESplitterResizeMode::FixedSize)
		
		+ CreateHeaderColumn(GetHeaderMPCNameText(), 60)
		+ CreateHeaderColumn(GetHeaderParameterNameText(), 60)
		+ CreateHeaderColumn(GetHeaderParameterDefaultValueText(), 80)
		+ CreateHeaderColumn(GetHeaderParameterValueText(), 80)
		+ CreateHeaderColumn(GetHeaderWatchEraseText(), 20)
		;
	
	
	ChildSlot
	[
		SNew(SBorder)
		.BorderImage(FCoreStyle::Get().GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor::Gray)
		.Padding(FMargin(5.0f, 5.0f))
		[
			SNew(SVerticalBox)
		
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 10.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("ClearWatch", "全てウォッチを解除する"))
				.OnClicked(this, &SCVTMPCViewerWatch::ButtonClearWatchClicked)
			]
			
			// Result list
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0.f, 4.f, 0.f, 0.f)
			[
				SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				[
					SAssignNew(ResultView, SListView<FCVTMPCViewerWatchResultShare>)
					.ListItemsSource(&ResultList)
					.HeaderRow(Header)
					.SelectionMode(ESelectionMode::None)
					.OnGenerateRow(this, &SCVTMPCViewerWatch::OnGenerateRowResultList)
				]
			]
		]
	];


	// エディター設定から
	if (bDirtyEditorSettings)
	{
		if (const UCUTDeveloperSettings* Settings = GetDefault<UCUTDeveloperSettings>())
		{
			for (const FCVTMPCViewerConfig& ConfigItem : Settings->CVTMPCViewerItems)
			{
				if (IsValid(ConfigItem.Collection))
				{
					if (ConfigItem.Collection->GetScalarParameterByName(ConfigItem.ParameterName))
					{
						AddWatchList(ConfigItem.Collection, ConfigItem.ParameterName, true);
					}
					else if (ConfigItem.Collection->GetVectorParameterByName(ConfigItem.ParameterName))
					{
						AddWatchList(ConfigItem.Collection, ConfigItem.ParameterName, false);
					}
				}
			}
		}
		bDirtyEditorSettings = false;
	}

	ResultView->RebuildList();
}

/** エディタの環境設定での値変更時のコールバック */
void SCVTMPCViewerWatch::OnChangedEditorSettings(UCUTDeveloperSettings* Settings, FPropertyChangedEvent& Property)
{
	if (!IsValid(Settings))
	{
		return;
	}
	// const FName PropertyName = Property.GetPropertyName();
}
/** エディタ終了時の現在環境の保存 */
void SCVTMPCViewerWatch::OnFinalizeEditorSettings(UCUTDeveloperSettings* Settings)
{
	if (!IsValid(Settings))
	{
		return;
	}
	// エディター設定に保存しておく
	for (const FCVTMPCViewerWatchResultShare& Result : ResultList)
	{
		// 既に設定が存在しているか
		if (Settings->CVTMPCViewerItems.FindByPredicate([Result](const FCVTMPCViewerConfig& InConfig)
				{
					return (Result->Collection.Get() == InConfig.Collection) && (Result->ParameterName == InConfig.ParameterName);
				}))
		{
			continue;
		}
		FCVTMPCViewerConfig NewItem;
		NewItem.Collection = Result->Collection.Get();
		NewItem.ParameterName = Result->ParameterName;
		Settings->CVTMPCViewerItems.Add(NewItem);
	}
}

const FText& SCVTMPCViewerWatch::GetHeaderMPCNameText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderMPCName", "MPC"));
	return HeaderNameText;
}
const FText& SCVTMPCViewerWatch::GetHeaderParameterNameText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderName", "Parameter Name"));
	return HeaderNameText;
}
const FText& SCVTMPCViewerWatch::GetHeaderParameterDefaultValueText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderDefaultValue", "Default Value"));
	return HeaderNameText;
}
const FText& SCVTMPCViewerWatch::GetHeaderParameterValueText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderValue", "Value"));
	return HeaderNameText;
}
const FText& SCVTMPCViewerWatch::GetHeaderWatchEraseText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderWatchErase", "ウォッチ解除"));
	return HeaderNameText;
}

// ウォッチリストに追加
void SCVTMPCViewerWatch::AddWatchList(TWeakObjectPtr<UMaterialParameterCollection> InCollection, FName InParameterName, bool bInIsScaler)
{
	if (!InCollection.IsValid())
	{
		return;
	}

	for (const FCVTMPCViewerWatchResultShare& Result : ResultList)
	{
		if ((Result->Collection == InCollection) &&
				(Result->ParameterName == InParameterName) &&
				(Result->bIsScalar == bInIsScaler))
		{
			// 既に存在している
			return;
		}
	}
	FCVTMPCViewerWatchResultShare NewResult(new FCVTMPCViewerResult);
	NewResult->Collection = InCollection;
	NewResult->ParameterName = InParameterName;
	NewResult->bIsScalar = bInIsScaler;
	
	bool bAdd = false;

	// ソートして挿入
	// MPC名 > Scalar優先 > ParameterName順
	for (int32 Index = 0 ; Index < ResultList.Num() ; ++Index)
	{
		const FCVTMPCViewerWatchResultShare& Result = ResultList[Index];
		if (Result->Collection.IsValid())
		{
			const int32 CompareResult = Result->Collection->GetPathName().Compare(InCollection->GetPathName());
			if (CompareResult > 0)
			{
				bAdd = true;
			}
			else if (CompareResult == 0)
			{
				if (!Result->bIsScalar && bInIsScaler)
				{
					// Scalar優先
					bAdd = true;
				}
				else if (Result->ParameterName.Compare(InParameterName) > 0)
				{
					bAdd = true;
				}
			}
			if (bAdd)
			{
				ResultList.Insert(NewResult, Index);
				break;
			}
		}
	}
	
	// ソートで挿入されなかったので末尾に追加
	if (!bAdd)
	{
		ResultList.Add(NewResult);
	}
	
	bRequestRefreshResult = true;
}

// ウォッチリストから削除
void SCVTMPCViewerWatch::RemoveWatchList(TWeakObjectPtr<UMaterialParameterCollection> InCollection, FName InParameterName, bool bInIsScaler)
{
	for (int32 Index = 0 ; Index < ResultList.Num() ; ++Index)
	{
		const FCVTMPCViewerWatchResultShare& Result = ResultList[Index];
		if ((Result->Collection == InCollection) &&
				(Result->ParameterName == InParameterName) &&
				(Result->bIsScalar == bInIsScaler))
		{
			ResultList.RemoveAt(Index);
			bRequestRefreshResult = true;
			break;
		}
	}
}

void SCVTMPCViewerWatch::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	UWorld* World = nullptr;
	// PIEでの再生中World
	if (GEditor)
	{
		World = GEditor->PlayWorld;
	}
	// Todo: Standalone再生中のWorldが取れない...

	if (World)
	{
		for (int32 Index = 0 ; Index < ResultList.Num() ; ++Index)
		{
			FCVTMPCViewerWatchResultShare& Result = ResultList[Index];
			if (Result->Collection.IsValid())
			{
				Result->UpdateDefaultValue();
				
				UMaterialParameterCollectionInstance* MPCInstance = World->GetParameterCollectionInstance(Result->Collection.Get());
				if (IsValid(MPCInstance))
				{
					Result->UpdateParameterValue(MPCInstance);
				}
			}
			else
			{
				// アセットが削除されたのでリストから除去
				ResultList.RemoveAt(Index);
				bRequestRefreshResult = true;
				Index -= 1;
			}
		}
	}
	else
	{
		// 再生終わっているので初期化する
		for (int32 Index = 0 ; Index < ResultList.Num() ; ++Index)
		{
			FCVTMPCViewerWatchResultShare& Result = ResultList[Index];
			if (Result->Collection.IsValid())
			{
				// パラメーターがEditorで変更された時用
				Result->UpdateDefaultValue();
				Result->ScalarValue = Result->DefaultScalarValue;
				Result->VectorValue = Result->DefaultVectorValue;
			}
			else
			{
				// アセットが削除されたのでリストから除去
				ResultList.RemoveAt(Index);
				bRequestRefreshResult = true;
				Index -= 1;
			}
		}
	}
	
	if (bRequestRefreshResult)
	{
		ResultView->RebuildList();
		bRequestRefreshResult = false;
	}
}

FReply SCVTMPCViewerWatch::ButtonClearWatchClicked()
{
	ResultList.Empty();
	ResultView->RebuildList();
	return FReply::Handled();
}

// Result List --------
SHeaderRow::FColumn::FArguments SCVTMPCViewerWatch::CreateHeaderColumn(const FText& Name, int32 Width)
{
	const FName NameName(*Name.ToString());
	
	return SHeaderRow::Column(NameName)
		.FillWidth(Width)
		.DefaultLabel(Name);
}
TSharedRef<ITableRow> SCVTMPCViewerWatch::OnGenerateRowResultList(FCVTMPCViewerWatchResultShare Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCVTMPCViewerWatchResultRow, OwnerTable)
			.Info(Item);
}
// -------- Result List


#undef LOCTEXT_NAMESPACE

