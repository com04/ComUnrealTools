// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Widgets/Layout/SBorder.h"


class UMaterial;
class UMaterialFunction;
class UMaterialInstance;
class UTexture;


/**
 * TextureFind result row
 */
class SCMTTextureFindResultWidgetAssetPath : public SBorder
{
public:

	SLATE_BEGIN_ARGS(SCMTTextureFindResultWidgetAssetPath)
	: _AssetPath()
	, _ExpressionName()
	, _Texture(nullptr)
	, _Material(nullptr)
	, _MaterialFunction(nullptr)
	{}
		SLATE_ATTRIBUTE(FString, AssetPath)
		SLATE_ATTRIBUTE(FString, ExpressionName)
		SLATE_ATTRIBUTE(UTexture*, Texture)
		SLATE_ATTRIBUTE(UMaterialInterface*, Material)
		SLATE_ATTRIBUTE(UMaterialFunction*,  MaterialFunction)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);
	
	
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

private:
	
	UTexture*           Texture;
	UMaterial*          Material;
	UMaterialInstance*  MaterialInstance;
	UMaterialFunction*  MaterialFunction;
};
