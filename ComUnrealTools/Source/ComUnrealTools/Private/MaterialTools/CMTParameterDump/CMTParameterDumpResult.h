// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SlateFwd.h"
#include "Widgets/SWidget.h"


class UTexture;

/** find result type */
enum class ECMTParameterDumpResultType
{
	Scalar,
	Vector,
	Texture,
	Switch,
	ComponentMask,
	Font,
	VirtualTexture,
	Other,
};

/* search result */
class FCMTParameterDumpResult : public TSharedFromThis<FCMTParameterDumpResult>
{
public: 
	/* root */
	FCMTParameterDumpResult(const FString& InParameterNameString, const FString& InParameterValueString, ECMTParameterDumpResultType InType, UTexture* InTexture=nullptr);

	virtual ~FCMTParameterDumpResult() {}
	
	/** tree view clicked event */
	virtual FReply OnClick();
	
	/** tree view icon */
	TSharedRef<SWidget>	CreateIcon() const;
	
	/** tree view background brush */
	FName	GetBackgroundBrushName() const;

	/** parameter name */
	FString GetParamNameString() const;
	
	/** type */
	FString GetTypeString() const;

	/** parameter value */
	FString GetParameterValueString() const;

private:
	/** result display */
	FString ParameterNameString;

	/** parameter value text */
	FString ParameterValueString;

	/** result type */
	ECMTParameterDumpResultType Type;

	/** expression comment */
	UTexture* Texture;
	
};
