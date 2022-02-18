// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "SlateFwd.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"



/** find result type */
enum class ECOTPropertySearcherResultType
{
	Blueprint,
	Material,
	Property,
	Other
};

/* search result */
class FCOTPropertySearcherResult : public TSharedFromThis<FCOTPropertySearcherResult>
{
public: 
	/* root */
	FCOTPropertySearcherResult(const FText& InDisplayText);
	FCOTPropertySearcherResult(const FText& InDisplayText, UBlueprint* InBlueprint);
	FCOTPropertySearcherResult(const FText& InDisplayText, UMaterial* InMaterial);
	FCOTPropertySearcherResult(const FText& InDisplayText, TSharedPtr<FCOTPropertySearcherResult> InParent);
	

	virtual ~FCOTPropertySearcherResult() {}
	
	/** tree view clicked event */
	virtual FReply OnClick();
	bool WakeupEditor();
	
	/** tree view icon */
	TSharedRef<SWidget>	CreateIcon() const;
	
	/** tree view background brush */
	FName	GetBackgroundBrushName() const;

	/** result display text */
	FText GetDisplayText() const;
	
	/** result option text */
	FText GetOptionText() const;
	
	FString GetTypeString() const;
	ECOTPropertySearcherResultType GetType() const { return Type; }
	
	/** Add tree child */
	void AddChild(TSharedPtr<FCOTPropertySearcherResult> Child);
	const TArray<TSharedPtr<FCOTPropertySearcherResult>>& GetChildren() const { return Children; }

private:
	/** result tree child */
	TArray<TSharedPtr<FCOTPropertySearcherResult>> Children;

	/** tree parent*/
	TWeakPtr<FCOTPropertySearcherResult> Parent;
	
	/** match Object*/
	UObject* ValueObject;

	/** result display */
	FText DisplayText;

	/** result option */
	FText OptionText;

	/** result type */
	ECOTPropertySearcherResultType Type;
};
