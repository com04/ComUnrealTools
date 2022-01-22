// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Widgets/Layout/SBorder.h"

class UMaterial;
class UMaterialFunction;
class UMaterialInstance;
class UMaterialInterface;


/**
 * TextureFind result row
 */
class SCMTTextureFindResultWidgetAssetPathRange : public SBorder
{
public:

	SLATE_BEGIN_ARGS(SCMTTextureFindResultWidgetAssetPathRange)
	: _TopMaterial(nullptr)
	, _TopMaterialFunction(nullptr)
	, _EndMaterial(nullptr)
	, _EndMaterialFunction(nullptr)
	{}
		SLATE_ATTRIBUTE(UMaterialInterface*, TopMaterial)
		SLATE_ATTRIBUTE(UMaterialFunction*,  TopMaterialFunction)
		SLATE_ATTRIBUTE(UMaterialInterface*, EndMaterial)
		SLATE_ATTRIBUTE(UMaterialFunction*,  EndMaterialFunction)
	SLATE_END_ARGS()

public:

	void Construct(const FArguments& InArgs);
	
	
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply OnTopMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply OnTopMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& PointerEvent);
	virtual FReply OnEndMouseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply OnEndMouseButtonDoubleClick();

private:
	
	UMaterial*          TopMaterial;
	UMaterialInstance*  TopMaterialInstance;
	UMaterialFunction*  TopMaterialFunction;
	UMaterial*          EndMaterial;
	UMaterialInstance*  EndMaterialInstance;
	UMaterialFunction*  EndMaterialFunction;
};
