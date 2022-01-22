// Copyright com04 All Rights Reserved.

#include "CMTTextureFindResult.h"
#include "CMTTextureFindResultAssetPath.h"
#include "CMTTextureFindResultAssetPathRange.h"
#include "CMTTextureFindResultAssetTree.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetThumbnail.h"
#include "EditorStyleSet.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Layout/SExpandableArea.h"


#define LOCTEXT_NAMESPACE "CMTTextureFind"



/** Search top use MaterialData */
TSharedPtr<FCMTTextureFindResultMaterialData> SearchTopMaterialData(TSharedPtr<FCMTTextureFindResultMaterialData> MaterialData)
{
	if (MaterialData->Material || MaterialData->Function)
	{
		return MaterialData;
	}
	if (MaterialData->Child.IsValid())
	{
		return SearchTopMaterialData(MaterialData->Child);
	}
	return MaterialData;
}
/** Search end use MaterialData */
TSharedPtr<FCMTTextureFindResultMaterialData> SearchEndMaterialData(TSharedPtr<FCMTTextureFindResultMaterialData> MaterialData)
{
	if (MaterialData->Child.IsValid())
	{
		return SearchEndMaterialData(MaterialData->Child);
	}
	return MaterialData;
}



void FCMTTextureFindResult::Construct(const FArguments& InArgs)
{
	// Texture Thumbnail
	const uint32 ResultTextureThumbnailSize = 64;

	TextureResultData = InArgs._TextureResultData;
	
	AssetThumbnailPool = MakeShareable(new FAssetThumbnailPool(1));
	AssetThumbnail = MakeShareable(new FAssetThumbnail(TextureResultData.Texture, ResultTextureThumbnailSize, ResultTextureThumbnailSize, AssetThumbnailPool));
	AssetThumbnail->GetViewportRenderTargetTexture(); // Access the texture once to trigger it to render
	
	ChildSlot
	[
		SNew(SVerticalBox)
			
		// Texture Path
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBG2BrushName))
			[
				SNew(SHorizontalBox)
				
				// title
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(LOCTEXT("TextureTitle", "TexturePath:"))
					.Margin(FMargin(4.0f, 2.0f))
				]
				
				// asset path
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SBorder)
					.BorderImage(nullptr)
					.Padding(FMargin(0))
					[
						SNew(SCMTTextureFindResultWidgetAssetPath)
						.AssetPath(TextureResultData.TexturePath)
						.Texture(TextureResultData.Texture)
					]
				]
			]
		]
		
		// Texture info
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
			[
				SNew(SHorizontalBox)
				
				// Texture Thumbnail
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SBorder)
						.Padding(4.0f)
						.BorderImage(FEditorStyle::GetBrush("PropertyEditor.AssetThumbnailShadow"))
						.OnMouseDoubleClick(this, &FCMTTextureFindResult::OnThumbnailMouseDoubleClick)
						[
							SNew(SBox)
							.WidthOverride(ResultTextureThumbnailSize)
							.HeightOverride(ResultTextureThumbnailSize)
							[
								SNew(SBorder)
								.Padding(1)
								.BorderImage(FEditorStyle::GetBrush("AssetThumbnail", ".Border"))
								.BorderBackgroundColor(FLinearColor::Black)
								[
									AssetThumbnail->MakeThumbnailWidget()
								]
							]
						]
					]
					
					// Spacer
					+ SVerticalBox::Slot()
					.FillHeight(1.0f)
					.VAlign(VAlign_Fill)
					[
						SNew(SSpacer)
					]
				]
				
				// Texture Reference
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SHorizontalBox)
					
					// title
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ReferencedTitle", "  Referenced from: "))
						.Margin(FMargin(2.0f))
					]
					
					// list
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SAssignNew(ReferenceVerticalBox, SVerticalBox)
					]
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
	];
	
	
	// Material
	for (int32 Index = 0 ; Index < TextureResultData.Referenced.Num() ; ++Index)
	{
		TSharedPtr<FCMTTextureFindResultMaterialData> TargetMaterial = TextureResultData.Referenced[Index];
		TSharedPtr<FCMTTextureFindResultMaterialData> TopMaterial = SearchTopMaterialData(TargetMaterial);
		TSharedPtr<FCMTTextureFindResultMaterialData> EndMaterial = SearchEndMaterialData(TopMaterial);
		
		ReferenceVerticalBox->AddSlot()
		.Padding(0.0f)
		.AutoHeight()
		[
			SNew(SExpandableArea)
			.InitiallyCollapsed(true)
			.Padding(FMargin(10.0f,0.0f,0.0f,0.0f))
			
			.HeaderContent()
			[
				SNew(SCMTTextureFindResultWidgetAssetPathRange)
				.TopMaterial(TopMaterial->Material)
				.TopMaterialFunction(TopMaterial->Function)
				.EndMaterial(EndMaterial->Material)
				.EndMaterialFunction(EndMaterial->Function)
			]
			
			.BodyContent()
			[
				SNew(SCMTTextureFindResultWidgetAssetTree)
				.Item(TopMaterial)
			]
		];
	}
	
	// Spacer
	ReferenceVerticalBox->AddSlot()
	.MaxHeight(500.f)
	[
		SNew(SSpacer)
	];
	
}

FCMTTextureFindResult::~FCMTTextureFindResult()
{
}

/** Material Thumbnail double click */
FReply FCMTTextureFindResult::OnThumbnailMouseDoubleClick(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent)
{
	if (TextureResultData.Texture)
	{
		FCUTUtility::WakeupTextureEditor(TextureResultData.Texture);
	}
	
	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

