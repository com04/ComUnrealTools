// Copyright com04 All Rights Reserved.

#include "CVTRenderTargetViewer.h"
#include "CVTRenderTargetViewerResult.h"
#include "CVTRenderTargetViewerResultRow.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetData.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Images/SImage.h"

#define LOCTEXT_NAMESPACE "CVTRenderTargetViewer"




////////////////////////////////////
// SCVTRenderTargetViewer


SCVTRenderTargetViewer::~SCVTRenderTargetViewer()
{
}

void SCVTRenderTargetViewer::Construct(const FArguments& InArgs)
{
	// 結果Listのヘッダー構成
	TSharedPtr<SHeaderRow> Header =
		SNew(SHeaderRow)
		.ResizeMode(ESplitterResizeMode::FixedSize)
		
		+ CreateHeaderColumn(GetHeaderNameText(), 40)
		+ CreateHeaderColumn(GetHeaderFormatText(), 30)
		+ CreateHeaderColumn(GetHeaderSizeText(), 20)
		+ CreateHeaderColumn(GetHeaderImageText(), 100)
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
			.Padding(10.0f, 4.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
			
				// check box
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(GetCheckPreviewWithoutAlpha())
					.OnCheckStateChanged(this, &SCVTRenderTargetViewer::OnCheckPreviewWithoutAlphaChanged)
				]
				
				// Asset property
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PreviewWithoutAlpha", "Alpha無しでプレビュー"))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(10.0f, 4.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("PreviewSize", "プレビューサイズ"))
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(10.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(SSpinBox<int32>)
					.Value(GetPreviewSize())
					.MinValue(32)
					.MaxValue(1024)
					.OnValueChanged(this, &SCVTRenderTargetViewer::OnSpinBoxPreviewSizeChanged)
				]
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 10.0f, 0.0f, 0.0f)
			[
				SNew(SButton)
				.Text(LOCTEXT("Refresh", "更新"))
				.OnClicked(this, &SCVTRenderTargetViewer::ButtonRefreshClicked)
			]
			// Result list
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(0.f, 4.f, 0.f, 0.f)
			[
				SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				[
					SAssignNew(ResultView, SListView<TSharedPtr<FCVTRenderTargetViewerResult>>)
					.ListItemsSource(&ResultList)
					.HeaderRow(Header)
					.SelectionMode(ESelectionMode::None)
					.OnGenerateRow(this, &SCVTRenderTargetViewer::OnGenerateRowResultList)
				]
			]
		]
	];
	
	SearchRenderTargets();
}

void SCVTRenderTargetViewer::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	// RenderTargetが破棄されたかチェック
	bool bRebuild = false;
	for (int32 Index = 0 ; Index < ResultList.Num() ; ++Index)
	{
		TSharedPtr<FCVTRenderTargetViewerResult> Result = ResultList[Index];
		if (!Result->bDestroy && !Result->RenderTarget.IsValid())
		{
			Result->bDestroy = true;
			bRebuild = true;
		}
	}
	if (bRebuild)
	{
		ResultView->RebuildList();
	}

	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

FText SCVTRenderTargetViewer::GetHeaderNameText()
{
	static const FText HeaderNameText(LOCTEXT("HeaderName", "Name"));
	return HeaderNameText;
}
FText SCVTRenderTargetViewer::GetHeaderFormatText()
{
	static const FText HeaderFormatText(LOCTEXT("HeaderFormat", "Format"));
	return HeaderFormatText;
}
FText SCVTRenderTargetViewer::GetHeaderSizeText()
{
	static const FText HeaderSizeText(LOCTEXT("HeaderSize", "Size"));
	return HeaderSizeText;
}
FText SCVTRenderTargetViewer::GetHeaderImageText()
{
	static const FText HeaderImageText(LOCTEXT("HeaderImage", "Texture"));
	return HeaderImageText;
}
FVector2D SCVTRenderTargetViewer::GetPreviewSizeV2()
{
	int32 LocalPreviewSize = GetPreviewSize();
	return FVector2D(LocalPreviewSize, LocalPreviewSize);
}

bool SCVTRenderTargetViewer::IsPreviewWithoutAlpha()
{
	return GetCheckPreviewWithoutAlpha() == ECheckBoxState::Checked;
}

// 使用中のRenderTargetを検索して結果に反映する
void SCVTRenderTargetViewer::SearchRenderTargets()
{
	ResultList.Empty();
	
	// PIEでの再生中World
	if (GEditor->PlayWorld)
	{
		const UEnum* FormatEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("ETextureRenderTargetFormat"));
		for (TObjectIterator<UTextureRenderTarget2D> RenderTargetIt ; RenderTargetIt ; ++RenderTargetIt)
		{
			UTextureRenderTarget2D* RenderTaregt = *RenderTargetIt;
			FString FormatString(TEXT("unknown format"));
			if (FormatEnum)
			{
				FormatString = FormatEnum->GetNameStringByIndex(static_cast<int32>(RenderTaregt->RenderTargetFormat));
			}
			
			TSharedPtr<FCVTRenderTargetViewerResult> Result(new FCVTRenderTargetViewerResult);
			Result->Name = FString::Printf(TEXT("%s/\n%s"),
					RenderTaregt->GetOuter() ? *RenderTaregt->GetOuter()->GetName() : TEXT(""),
					*RenderTaregt->GetName());
			Result->Format = FormatString;
			Result->SizeX = RenderTaregt->SizeX;
			Result->SizeY = RenderTaregt->SizeY;
			Result->bDestroy = false;
			Result->RenderTarget = RenderTaregt;
			ResultList.Add(Result);
		}
	}
	ResultView->RebuildList();
}
// リザルト欄のPreview表示を現在の設定で更新する
void SCVTRenderTargetViewer::RefreshResultPreview()
{
	if (ResultView.IsValid())
	{
		for (TSharedPtr<FCVTRenderTargetViewerResult>& Result : ResultList)
		{
			if (Result.IsValid())
			{
				TSharedPtr<ITableRow> TableRow = ResultView->WidgetFromItem(Result);
				if (TableRow)
				{
					TSharedPtr<SCVTRenderTargetViewerResultRow> ResultRow = StaticCastSharedPtr<SCVTRenderTargetViewerResultRow>(TableRow);
					if (ResultRow.IsValid())
					{
						ResultRow->RefreshPreviewImage();
					}
				}
			}
		}
		ResultView->RequestListRefresh();
	}
}

// Check Box  --------
void SCVTRenderTargetViewer::OnCheckPreviewWithoutAlphaChanged(ECheckBoxState InValue)
{
	SetCheckPreviewWithoutAlpha(InValue);
	RefreshResultPreview();
}
// -------- Check Box

// Spin Box  --------
void SCVTRenderTargetViewer::OnSpinBoxPreviewSizeChanged(int32 InValue)
{
	SetPreviewSize(InValue);
	RefreshResultPreview();
}
// -------- Spin Box

// Button  --------
FReply SCVTRenderTargetViewer::ButtonRefreshClicked()
{
	SearchRenderTargets();
	
	return FReply::Handled();
}
// -------- Button

// Result List --------
SHeaderRow::FColumn::FArguments SCVTRenderTargetViewer::CreateHeaderColumn(const FText& Name, int32 Width)
{
	const FName NameName(*Name.ToString());
	
	return SHeaderRow::Column(NameName)
		.FillWidth(Width)
		.DefaultLabel(Name);
}
TSharedRef<ITableRow> SCVTRenderTargetViewer::OnGenerateRowResultList(TSharedPtr<FCVTRenderTargetViewerResult> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SCVTRenderTargetViewerResultRow, OwnerTable)
			.Info(Item);
}
// -------- Result List

#undef LOCTEXT_NAMESPACE

