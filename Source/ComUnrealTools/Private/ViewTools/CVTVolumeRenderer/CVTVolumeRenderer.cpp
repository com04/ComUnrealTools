// Copyright com04 All Rights Reserved.

#include "CVTVolumeRenderer.h"
#include "CVTVolumeRendererItem.h"
#include "CVTVolumeRendererItemRow.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "UnrealTools/CUTDeveloperSettings.h"
#include "Utility/CUTUtility.h"

#include "EngineUtils.h"
#include "Components/BrushComponent.h"
#include "Components/ShapeComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/STableViewBase.h"

#define LOCTEXT_NAMESPACE "CVTVolumeRenderer"




////////////////////////////////////
// SCVTVolumeRenderer
float SCVTVolumeRenderer::LineThickness = 50.0f;
float SCVTVolumeRenderer::OneShotDuration = 5.0f;
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
float SCVTVolumeRenderer::RenderDistance = 0.0f;
bool SCVTVolumeRenderer::bDirtyEditorSettings = true;


SCVTVolumeRenderer::~SCVTVolumeRenderer()
{
}

void SCVTVolumeRenderer::Construct(const FArguments& InArgs)
{
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
					SNew(SSpinBox<float>)
					.Value(LineThickness)
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
					SNew(SSpinBox<float>)
					.Value(OneShotDuration)
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
					SNew(SSpinBox<float>)
					.Value(RenderDistance)
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
}
void SCVTVolumeRenderer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	
	UWorld* World = GWorld;
	// PIEでの再生中World
	if (GEditor && GEditor->PlayWorld)
	{
		World = GEditor->PlayWorld;
	}
	// Todo: Standalone再生中のWorldが取れない...

	if (World)
	{
		for (TSharedPtr<FCVTVolumeRendererItem>& Item : RequestOneShotItems)
		{
			RenderItem(World, Item, OneShotDuration);
		}
		RequestOneShotItems.Empty();
		for (TArray<TSharedPtr<FCVTVolumeRendererItem>>::TIterator ItemIt(RequestAlwaysItems) ; ItemIt ; ++ItemIt)
		{
			RenderItem(World, *ItemIt, 0.0f);
			if (!(*ItemIt)->IsAlways())
			{
				ItemIt.RemoveCurrent();
			}
		}
	}
}

void SCVTVolumeRenderer::OnChangedEditorSettings(UCUTDeveloperSettings* Settings, FPropertyChangedEvent& Property)
{
	if (!IsValid(Settings))
	{
		return;
	}
	// const FName PropertyName = Property.GetPropertyName();
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
	LineThickness = InValue;
}
void SCVTVolumeRenderer::OnSpinBoxOneShotDurationChanged(float InValue)
{
	OneShotDuration = InValue;
}
void SCVTVolumeRenderer::OnSpinBoxRenderDistanceChanged(float InValue)
{
	RenderDistance = InValue;
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
	RequestAlwaysItems.Empty();
	RequestOneShotItems.Empty();
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
	NewItem->OnAlwaysON = [this](TSharedPtr<FCVTVolumeRendererItem> InItem) {
		this->OnAlwaysON(InItem);
	};
	NewItem->OnOneShot = [this](TSharedPtr<FCVTVolumeRendererItem> InItem) {
		this->OnOneShot(InItem);
	};
	NewItem->OnRemove = [this](TSharedPtr<FCVTVolumeRendererItem> InItem) {
		this->OnRemove(InItem);
	};
	Items.Add(NewItem);
}
// アイテム一つの描画
void SCVTVolumeRenderer::RenderItem(UWorld* InWorld, TSharedPtr<FCVTVolumeRendererItem> InItem, float InDuration)
{
	const FCVTVolumeRendererItemInfo& Info = InItem->GetInfo();
	if (!Info.Class)
	{
		return;
	}
	
	const FColor LineColor = Info.DisplayColor.ToFColor(true);
	const FColor SolidColor = FColor(LineColor.R, LineColor.G, LineColor.B, LineColor.A * 0.5f);
	for(TActorIterator<AActor> ActorIt(InWorld, Info.Class) ; ActorIt ; ++ActorIt)
	{
		AActor* TargetActor = *ActorIt;
		struct FGeomtryInfo
		{
			const UBodySetup* BodySetup = nullptr;
			FTransform Transform = FTransform::Identity;
			
			FGeomtryInfo(const UBodySetup* InBodySetup, const FTransform& InTransform)
			: BodySetup(InBodySetup),
			  Transform(InTransform)
			{}
		};
		TArray<FGeomtryInfo, TInlineAllocator<32>> GeometryInfos;
		
		// 距離カリング
		if ((RenderDistance > 0.01f) && (InWorld->ViewLocationsRenderedLastFrame.Num() > 0))
		{
			const FVector ActorLocation = TargetActor->GetActorLocation();
			bool bCulling = true;
			for (const FVector& ViewLocation : InWorld->ViewLocationsRenderedLastFrame)
			{
				if (FVector::Distance(ActorLocation, ViewLocation) <= RenderDistance)
				{
					bCulling = false;
					break;
				}
			}
			if (bCulling)
			{
				continue;
			}
		}
		
		// BodySetup を取得
		{
			// Volume等のBrush系
			TArray<UBrushComponent*, TInlineAllocator<16>> BurshComps;
			TargetActor->GetComponents(BurshComps);
			for (UBrushComponent* BrushComp : BurshComps)
			{
				if (BrushComp->BrushBodySetup)
				{
					GeometryInfos.Add(FGeomtryInfo(BrushComp->BrushBodySetup, BrushComp->GetComponentTransform()));
				}
			}
			// Box/Sphere/CapsuleCollision系
			TArray<UShapeComponent*, TInlineAllocator<16>> ShapeComps;
			TargetActor->GetComponents(ShapeComps);
			for (UShapeComponent* ShapeComp : ShapeComps)
			{
				if (ShapeComp->ShapeBodySetup)
				{
					GeometryInfos.Add(FGeomtryInfo(ShapeComp->ShapeBodySetup, ShapeComp->GetComponentTransform()));
				}
			}
		}
		for (const FGeomtryInfo& GeometryInfo : GeometryInfos)
		{
			FCUTUtility::DrawBodySetup(InWorld, GeometryInfo.BodySetup, GeometryInfo.Transform, LineColor, SolidColor, InDuration, LineThickness);
		}
	}
}

// AlwaysがONに設定された時のコールバック
void SCVTVolumeRenderer::OnAlwaysON(TSharedPtr<FCVTVolumeRendererItem> Item)
{
	RequestAlwaysItems.AddUnique(Item);
}
// OneShotが押された時のコールバック
void SCVTVolumeRenderer::OnOneShot(TSharedPtr<FCVTVolumeRendererItem> Item)
{
	RequestOneShotItems.AddUnique(Item);
}
// Removeが押された時のコールバック
void SCVTVolumeRenderer::OnRemove(TSharedPtr<FCVTVolumeRendererItem> Item)
{
	ItemInfos.RemoveSingle(Item->GetInfo());
	RequestAlwaysItems.RemoveSingle(Item);
	RequestOneShotItems.RemoveSingle(Item);
	Items.RemoveSingle(Item);
	ItemTileView->RequestListRefresh();
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

