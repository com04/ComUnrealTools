// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "SlateFwd.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"


class UMaterial;
class UMaterialExpression;
class UMaterialFunction;
class UMaterialInstance;
class UTexture;

/** find result type */
enum class ECMTParameterSearcherResultType
{
	Material,
	MaterialInstance,
	Other
};

/* search result */
class FCMTParameterSearcherResult : public TSharedFromThis<FCMTParameterSearcherResult>
{
public: 
	/* root */
	FCMTParameterSearcherResult(const FText& InDisplayText, bool bError=false);
	FCMTParameterSearcherResult(const FText& InDisplayText, const FText& InValueText, UMaterial* InMaterial);
	FCMTParameterSearcherResult(const FText& InDisplayText, const FText& InValueText, UMaterialInstance* InInstance);

	virtual ~FCMTParameterSearcherResult() {}
	
	/** tree view clicked event */
	virtual FReply OnClick();
	
	/** tree view icon */
	TSharedRef<SWidget>	CreateIcon() const;
	
	/** tree view background brush */
	FName	GetBackgroundBrushName() const;

	/** result display text */
	FText GetDisplayText() const;

	/** 値文字 */
	FText GetValueText() const;

	/** エラー文字か */
	bool IsError() const { return bError; }

private:
	/** match material */
	UMaterial* Material;
	
	/** match material function */
	UMaterialInstance* Instance;

	/** result display */
	FText DisplayText;

	/** expression comment */
	FText ValueText;
	
	/** エラー文字か */
	bool bError;
	
	/** result type */
	ECMTParameterSearcherResultType Type;
};
