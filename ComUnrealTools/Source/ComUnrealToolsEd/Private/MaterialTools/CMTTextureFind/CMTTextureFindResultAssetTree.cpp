// Copyright com04 All Rights Reserved.

#include "CMTTextureFindResultAssetTree.h"
#include "CMTTextureFindResultAssetPath.h"
#include "CMTTextureFindResultData.h"

#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"
#include "Templates/SharedPointer.h"

#define LOCTEXT_NAMESPACE "CMTTextureFindResultWidgetAssetTree"


void SCMTTextureFindResultWidgetAssetTree::Construct(const FArguments& InArgs)
{
	TSharedPtr<FCMTTextureFindResultMaterialData> Item = InArgs._Item.Get();
	
	if (Item.IsValid())
	{
		ItemsFound.Add(Item);
		ChildSlot
		[
			SNew(SCMTTextureFindTreeViewType)
			.ItemHeight(24)
			.TreeItemsSource(&ItemsFound)
			.OnGenerateRow(this, &SCMTTextureFindResultWidgetAssetTree::OnGenerateRow)
			.OnGetChildren(this, &SCMTTextureFindResultWidgetAssetTree::OnGetChildren)
			.OnMouseButtonDoubleClick(this, &SCMTTextureFindResultWidgetAssetTree::OnTreeSelectionDoubleClicked)
		];
	}
}


/** row generate */
TSharedRef<ITableRow> SCMTTextureFindResultWidgetAssetTree::OnGenerateRow(FCMTTextureFindResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable) 
{
	return SNew(STableRow< TSharedPtr<FCMTTextureFindResultShare> >, OwnerTable)
		[
			SNew(SBorder)
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SNew(SCMTTextureFindResultWidgetAssetPath)
				.Material(InItem->Material)
				.MaterialFunction(InItem->Function)
				.AssetPath(InItem->Material ? GetPathNameSafe(InItem->Material) : GetPathNameSafe(InItem->Function))
				.ExpressionName(InItem->MaterialName)
			]
		];
}
/** tree child */
void SCMTTextureFindResultWidgetAssetTree::OnGetChildren(FCMTTextureFindResultShare InItem, TArray<FCMTTextureFindResultShare>& OutChildren) 
{
	if (InItem->Child.IsValid())
	{
		OutChildren.Add(InItem->Child);
	}
}

/** item double click event */
void SCMTTextureFindResultWidgetAssetTree::OnTreeSelectionDoubleClicked(FCMTTextureFindResultShare Item)
{
}

#undef LOCTEXT_NAMESPACE
