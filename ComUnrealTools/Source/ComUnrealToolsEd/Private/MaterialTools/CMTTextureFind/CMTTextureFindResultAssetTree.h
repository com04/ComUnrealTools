// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"

#include "MaterialTools/CMTTextureFind/CMTTextureFindResultData.h"

typedef TSharedPtr<FCMTTextureFindResultMaterialData> FCMTTextureFindResultShare;
typedef STreeView<FCMTTextureFindResultShare>  SCMTTextureFindTreeViewType;


/**
 * TextureFind result Tree
 */
class SCMTTextureFindResultWidgetAssetTree : public SBorder
{
public:

	SLATE_BEGIN_ARGS(SCMTTextureFindResultWidgetAssetTree)
	: _Item()
	{}
		SLATE_ATTRIBUTE(FCMTTextureFindResultShare, Item)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);
	
	
	/** row generate */
	TSharedRef<ITableRow> OnGenerateRow(FCMTTextureFindResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable);
	/** tree child */
	void OnGetChildren(FCMTTextureFindResultShare InItem, TArray<FCMTTextureFindResultShare>& OutChildren);
	/** item double click event */
	void OnTreeSelectionDoubleClicked(FCMTTextureFindResultShare Item);

	
private:
	/** Find result list */
	TArray<FCMTTextureFindResultShare> ItemsFound;
	
	
	FCMTTextureFindResultData* ResultData;
};
