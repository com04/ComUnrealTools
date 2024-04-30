// Copyright com04 All Rights Reserved.

#include "CVTVolumeRenderer.h"
#include "CVTVolumeRendererItem.h"
#include "CVTVolumeRendererItemRow.h"
#include "ComUnrealToolsStyle.h"
#include "UnrealTools/CUTDeveloperSettings.h"
#include "Utility/CUTUtility.h"

#include "EngineUtils.h"
#include "Components/BrushComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STileView.h"

#define LOCTEXT_NAMESPACE "CVTVolumeRenderer"




////////////////////////////////////
// SCVTVolumeRenderer
FString SCVTVolumeRenderer::MatchActorName;
TArray<FCVTVolumeRendererItemInfo> SCVTVolumeRenderer::ItemInfos;
const TArray<FLinearColor> SCVTVolumeRenderer::DefaultColorList =
{
	FLinearColor(0.8f, 0.2f, 0.2f),
	FLinearColor(0.2f, 0.8f, 0.2f),
	FLinearColor(0.2f, 0.2f, 0.8f),
	FLinearColor(0.8f, 0.8f, 0.2f),
	FLinearColor(0.8f, 0.2f, 0.8f),
	FLinearColor(0.2f, 0.8f, 0.8f),
	FLinearColor(0.8f, 0.8f, 0.8f),
};
bool SCVTVolumeRenderer::bDirtyEditorSettings = true;


SCVTVolumeRenderer::~SCVTVolumeRenderer()
{
	if (OnChangedEditorSettingsHandle.IsValid())
	{
		UCUTDeveloperSettings* Settings = UCUTDeveloperSettings::GetWritable();
		if (Settings)
		{
			Settings->RemoveOnChangedDelegate(OnChangedEditorSettingsHandle);
			OnChangedEditorSettingsHandle.Reset();
		}
	}
}

void SCVTVolumeRenderer::Construct(const FArguments& InArgs)
{
	bExternalOnChanged = false;
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
			.Padding(10.0f, 4.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TargetClass", "Target Class: "))
				]
				
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SClassPropertyEntryBox)
					.MetaClass(AActor::StaticClass())
					.ShowTreeView(true)
					.IsBlueprintBaseOnly(false)
					.SelectedClass(this, &SCVTVolumeRenderer::OnGetClass)
					.OnSetClass(this, &SCVTVolumeRenderer::OnSetClass)
				]
				+SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNullWidget::NullWidget
				]
			]

			// Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 4.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
			
				// Added class
				+ SHorizontalBox::Slot()
				.MaxWidth(300.f)
				[
					SNew(SButton)
					.Text(LOCTEXT("ClassAddButton", "クラスを追加"))
					.OnClicked(this, &SCVTVolumeRenderer::ButtonClassAddClicked)
				]
				
				// clear items
				+ SHorizontalBox::Slot()
				// .AutoWidth()
				.MaxWidth(200.f)
				.Padding(50.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(SButton)
					.Text(LOCTEXT("ClearItems", "全てクリア"))
					.OnClicked(this, &SCVTVolumeRenderer::ButtonClearItemsClicked)
				]
				
			]

			// option
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 10.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("ActorName", "Match Actor Name: "))
				]
				
				+ SHorizontalBox::Slot()
				.FillWidth(0.5f)
				[
					SNew(SSearchBox)
					.HintText(LOCTEXT("Find", "Enter actor name..."))
					.InitialText(FText::FromString(MatchActorName))
					.OnTextCommitted(this, &SCVTVolumeRenderer::OnMatchActorNameCommitted)
				]
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 10.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("LineThickness", "ラインの太さ"))
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(10.0f, 0.0f, 0.0f, 0.0f)
				[
					SAssignNew(SpinBoxLineThickness, SSpinBox<float>)
					.Value(GetLineThickness())
					.MinValue(0.0f)
					.MaxSliderValue(100)
					.Delta(1.0f)
					.MinDesiredWidth(50.0f)
					.OnValueChanged(this, &SCVTVolumeRenderer::OnSpinBoxLineThicknessChanged)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(40.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("OneShotDuration", "「一回表示」での表示時間"))
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(10.0f, 0.0f, 0.0f, 0.0f)
				[
					SAssignNew(SpinBoxOneShotDuration, SSpinBox<float>)
					.Value(GetOneShotDuration())
					.MinValue(0.0f)
					.MaxSliderValue(30.0f)
					.Delta(0.1f)
					.MinDesiredWidth(50.0f)
					.OnValueChanged(this, &SCVTVolumeRenderer::OnSpinBoxOneShotDurationChanged)
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(40.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("RenderDistance", "表示する最大距離"))
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(10.0f, 0.0f, 0.0f, 0.0f)
				[
					SAssignNew(SpinBoxRenderDistance, SSpinBox<float>)
					.Value(GetRenderDistance())
					.MinValue(0.0f)
					.MaxSliderValue(100000.0f)
					.Delta(10.0f)
					.MinDesiredWidth(70.0f)
					.OnValueChanged(this, &SCVTVolumeRenderer::OnSpinBoxRenderDistanceChanged)
				]
			]
			
			// Added Items
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0.f, 4.f, 0.f, 0.f)
			[
				SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				[
					SAssignNew(ItemTileView, STileView<TSharedPtr<FCVTVolumeRendererItem>>)
					.ListItemsSource(&Items)
					.SelectionMode(ESelectionMode::None)
					.AllowOverscroll(EAllowOverscroll::No)
					.Orientation(EOrientation::Orient_Horizontal)
					.IsFocusable(false)
					.OnGenerateTile(this, &SCVTVolumeRenderer::OnGenerateRow)
					.ItemHeight(36)
					.ItemWidth(470)
				]
			]
		]
	];
	
	// エディター設定から
	if (bDirtyEditorSettings)
	{
		if (const UCUTDeveloperSettings* Settings = GetDefault<UCUTDeveloperSettings>())
		{
			for (const FCVTVolumeRendererItemInfo& ItemInfo : Settings->CVTVolumeRendererItems)
			{
				if (ItemInfo.Class.Get() && !FindClassInItemInfos(ItemInfos, ItemInfo.Class))
				{
					ItemInfos.Add(ItemInfo);
				}
			}
		}
		bDirtyEditorSettings = false;
	}
	
	// キャッシュから復元
	for (TArray<FCVTVolumeRendererItemInfo>::TIterator ItemInfoIt(ItemInfos) ; ItemInfoIt ; ++ItemInfoIt)
	{
		if (!ItemInfoIt->Class.Get())
		{
			ItemInfoIt.RemoveCurrent();
			continue;
		}
		
		AddItem(*ItemInfoIt);
	}
	
	ItemTileView->RequestListRefresh();

	// 外部でのパラメーター変更似合わせてUIを更新する
	UCUTDeveloperSettings* Settings = UCUTDeveloperSettings::GetWritable();
	if (Settings)
	{
		OnChangedEditorSettingsHandle = Settings->AddOnChangedDelegate(FCUTOnChangedDeveloperSettings::FDelegate::CreateLambda([this]()
				{
					bExternalOnChanged = true;
					SpinBoxLineThickness->SetValue(GetLineThickness());
					SpinBoxOneShotDuration->SetValue(GetOneShotDuration());
					SpinBoxRenderDistance->SetValue(GetRenderDistance());
					bExternalOnChanged = false;
				}));
	}
}

/** エディタ終了時の現在環境の保存 */
void SCVTVolumeRenderer::OnFinalizeEditorSettings(UCUTDeveloperSettings* Settings)
{
	if (!IsValid(Settings))
	{
		return;
	}
	// エディター設定に保存しておく
	for (const FCVTVolumeRendererItemInfo& ItemInfo : ItemInfos)
	{
		if (!FindClassInItemInfos(Settings->CVTVolumeRendererItems, ItemInfo.Class))
		{
			Settings->CVTVolumeRendererItems.Add(ItemInfo);
		}
	}
}

// Asset Select --------
const UClass* SCVTVolumeRenderer::OnGetClass() const
{
	return SelectedClass;
}
void SCVTVolumeRenderer::OnSetClass(const UClass* InClass)
{
	SelectedClass = const_cast<UClass*>(InClass);
}
// -------- Asset Select

// Spin Box  --------
void SCVTVolumeRenderer::OnSpinBoxLineThicknessChanged(float InValue)
{
	if (bExternalOnChanged)
	{
		return;
	}
	FComViewTools::Get().SetVolumeRendererLineThickness(InValue);
}
void SCVTVolumeRenderer::OnSpinBoxOneShotDurationChanged(float InValue)
{
	if (bExternalOnChanged)
	{
		return;
	}
	FComViewTools::Get().SetVolumeRendererOneShotDuration(InValue);
}
void SCVTVolumeRenderer::OnSpinBoxRenderDistanceChanged(float InValue)
{
	if (bExternalOnChanged)
	{
		return;
	}
	FComViewTools::Get().SetVolumeRendererRenderDistance(InValue);
}
// -------- Spin Box

// Buttons --------

FReply SCVTVolumeRenderer::ButtonClassAddClicked()
{
	if (SelectedClass)
	{
		// 重複登録チェック
		if (FindClassInItemInfos(ItemInfos, SelectedClass))
		{
			return FReply::Handled();
		}
	
		FCVTVolumeRendererItemInfo NewItemInfo;
		NewItemInfo.Class = SelectedClass;
		NewItemInfo.DisplayColor = DefaultColorList[ItemInfos.Num() % DefaultColorList.Num()];
		ItemInfos.Add(NewItemInfo);

		AddItem(NewItemInfo);

		ItemTileView->RequestListRefresh();
	}
	return FReply::Handled();
}

FReply SCVTVolumeRenderer::ButtonClearItemsClicked()
{
	ItemInfos.Empty();
	FComViewTools::Get().RemoveVolumeRendererItemAll();
	Items.Empty();
	ItemTileView->RequestListRefresh();
	return FReply::Handled();
}

// -------- Buttons

TSharedRef<ITableRow> SCVTVolumeRenderer::OnGenerateRow(TSharedPtr<FCVTVolumeRendererItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCVTVolumeRendererItemRow, InItem, OwnerTable);
}

void SCVTVolumeRenderer::AddItem(const FCVTVolumeRendererItemInfo& InInfo)
{
	TSharedPtr<FCVTVolumeRendererItem> NewItem = MakeShareable(new FCVTVolumeRendererItem(InInfo));
	// コールバック登録
	NewItem->OnChangedAlways = [this](TSharedPtr<FCVTVolumeRendererItem> InItem) {
		this->OnChangedAlways(InItem);
	};
	NewItem->OnOneShot = [this](TSharedPtr<FCVTVolumeRendererItem> InItem) {
		this->OnOneShot(InItem);
	};
	NewItem->OnRemove = [this](TSharedPtr<FCVTVolumeRendererItem> InItem) {
		this->OnRemove(InItem);
	};
	Items.Add(NewItem);
}
// AlwaysがONに設定された時のコールバック
void SCVTVolumeRenderer::OnChangedAlways(TSharedPtr<FCVTVolumeRendererItem> Item)
{
	if (!Item.IsValid())
	{
		return;
	}
	if (Item->IsAlways())
	{
		FComViewTools::Get().AddVolumeRendererItemAlways(Item->GetInfo(), MatchActorName);
	}
	else
	{
		FComViewTools::Get().RemoveVolumeRendererItemAlways(Item->GetInfo());
	}
}
// OneShotが押された時のコールバック
void SCVTVolumeRenderer::OnOneShot(TSharedPtr<FCVTVolumeRendererItem> Item)
{
	if (!Item.IsValid())
	{
		return;
	}
	FComViewTools::Get().AddVolumeRendererItemOneshot(Item->GetInfo(), MatchActorName);
}
// Removeが押された時のコールバック
void SCVTVolumeRenderer::OnRemove(TSharedPtr<FCVTVolumeRendererItem> Item)
{
	ItemInfos.RemoveSingle(Item->GetInfo());
	Items.RemoveSingle(Item);
	ItemTileView->RequestListRefresh();
	
	if (!Item.IsValid())
	{
		return;
	}
	FComViewTools::Get().RemoveVolumeRendererItemAlways(Item->GetInfo());
}
void SCVTVolumeRenderer::OnMatchActorNameCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	MatchActorName = Text.ToString();
}

// 既に登録しているクラスか
bool SCVTVolumeRenderer::FindClassInItemInfos(const TArray<FCVTVolumeRendererItemInfo>& InItemInfos, TSubclassOf<AActor> InClass)
{
	for (const FCVTVolumeRendererItemInfo& Info : InItemInfos)
	{
		if (Info.Class == InClass)
		{
			return true;
		}
	}
	return false;
}

#undef LOCTEXT_NAMESPACE

