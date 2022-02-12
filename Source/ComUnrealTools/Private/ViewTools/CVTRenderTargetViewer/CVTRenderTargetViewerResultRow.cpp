// Copyright com04 All Rights Reserved.

#include "CVTRenderTargetViewerResultRow.h"
#include "CVTRenderTargetViewerResult.h"
#include "CVTRenderTargetViewer.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Widgets/SWidget.h"
#include "Widgets/Images/SImage.h"

#define LOCTEXT_NAMESPACE "CVTRenderTargetViewerResultRow"


void SCVTRenderTargetViewerResultRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Info = InArgs._Info;
	OpaqueImage = 0;
	SetBorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName));
	SMultiColumnTableRow<TSharedPtr<FCVTRenderTargetViewerResult>>::Construct(FSuperRowType::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SCVTRenderTargetViewerResultRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == FName(*SCVTRenderTargetViewer::GetHeaderNameText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Info->Name))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTRenderTargetViewer::GetHeaderFormatText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Info->Format))
				];
		BorderColor2List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTRenderTargetViewer::GetHeaderSizeText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("%dx%d"), Info->SizeX, Info->SizeY)))
				];
		BorderColor1List.Add(Border);
		return Border;
	}
	else if (ColumnName == FName(*SCVTRenderTargetViewer::GetHeaderImageText().ToString()))
	{
		TSharedRef<SBorder> Border = SNew(SBorder)
				.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
				.HAlign(HAlign_Right)
				[
					SNew(SHorizontalBox)
				
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(PreviewImage, SImage)
						.Image(Info->bDestroy ? nullptr : CreateSlateImageBrush())
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(Info->bDestroy ? LOCTEXT("RenderTargetDestroyed", "RenderTargetが破棄されました") : FText())
						.ColorAndOpacity(FLinearColor(0.8f, 0.2f, 0.2f, 1.0f))
					]
				];
		BorderColor2List.Add(Border);
		return Border;
	}

	return SNullWidget::NullWidget;
}


void SCVTRenderTargetViewerResultRow::OnMouseEnter(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	// change active border
	const FSlateBrush* Brush = FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGActiveBrushName);
	SetBorderImage(Brush);
	for (auto ItBorder = BorderColor1List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush);
	}
	for (auto ItBorder = BorderColor2List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush);
	}
	return SMultiColumnTableRow::OnMouseEnter(InMyGeometry, InMouseEvent);
}
void SCVTRenderTargetViewerResultRow::OnMouseLeave(const FPointerEvent& InMouseEvent)
{
	// restore border
	const FSlateBrush* Brush = FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName);
	const FSlateBrush* Brush2 = FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName);
	SetBorderImage(Brush);
	for (auto ItBorder = BorderColor1List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush);
	}
	for (auto ItBorder = BorderColor2List.CreateConstIterator(); ItBorder; ++ItBorder)
	{
		(*ItBorder)->SetBorderImage(Brush2);
	}
	
	return SMultiColumnTableRow::OnMouseLeave(InMouseEvent);
}

FReply SCVTRenderTargetViewerResultRow::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (Info->RenderTarget.IsValid())
	{
		FCUTUtility::WakeupTextureEditor(Info->RenderTarget.Get());
	}
	return FReply::Handled();
}

// Previewの画像表示を現在の設定で更新する
void SCVTRenderTargetViewerResultRow::RefreshPreviewImage()
{
	if (!Info->bDestroy)
	{
		PreviewImage->SetImage(CreateSlateImageBrush());
	}
}
FSlateImageBrush* SCVTRenderTargetViewerResultRow::CreateSlateImageBrush()
{
	const bool bWithoutAlpha = SCVTRenderTargetViewer::IsPreviewWithoutAlpha();
	
	UObject* Resource = Info->RenderTarget.Get();
	if (bWithoutAlpha)
	{
		// 不透明表示する為にマテリアルで反映する
		if (!IsValid(OpaqueImage))
		{
			UMaterial* DefaultOpaqueMaterial = FComUnrealToolsStyle::GetImageOpaqueMaterial();
			OpaqueImage = UKismetMaterialLibrary::CreateDynamicMaterialInstance(DefaultOpaqueMaterial, DefaultOpaqueMaterial);
			if (IsValid(OpaqueImage))
			{
				OpaqueImage->SetTextureParameterValue(TEXT("Texture"), Info->RenderTarget.Get());
			}
		}
		Resource = OpaqueImage;
	}
	return new FSlateImageBrush(Resource, SCVTRenderTargetViewer::GetPreviewSizeV2());
}


#undef LOCTEXT_NAMESPACE
