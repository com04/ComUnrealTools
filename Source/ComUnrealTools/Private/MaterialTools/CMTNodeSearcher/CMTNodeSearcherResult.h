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
enum class ECMTNodeSearcherResultType
{
	Material,
	MaterialFunction,
	MaterialInstance,
	Expression,
	Texture,
	Other
};

/* search result */
class FCMTNodeSearcherResult : public TSharedFromThis<FCMTNodeSearcherResult>
{
public: 
	/* root */
	FCMTNodeSearcherResult(const FText& InDisplayText);
	FCMTNodeSearcherResult(const FText& InDisplayText, UMaterial* InMaterial);
	FCMTNodeSearcherResult(const FText& InDisplayText, UMaterialFunction* InFunction);
	FCMTNodeSearcherResult(const FText& InDisplayText, UMaterialInstance* InInstance);
	
	/* result expression */
	FCMTNodeSearcherResult(const FText& InDisplayText, TSharedPtr<FCMTNodeSearcherResult> InParent, UMaterialExpression* InExpression);
	FCMTNodeSearcherResult(const FText& InDisplayText, TSharedPtr<FCMTNodeSearcherResult> InParent, UTexture* InTexture);

	virtual ~FCMTNodeSearcherResult() {}
	
	/** tree view clicked event */
	virtual FReply OnClick();
	
	/** tree view icon */
	TSharedRef<SWidget>	CreateIcon() const;
	
	/** tree view background brush */
	FName	GetBackgroundBrushName() const;

	/** expression comment */
	FString GetCommentText() const;

	/** result display text */
	FText GetDisplayString() const;
	
	/** Add tree child */
	void AddChild(TSharedPtr<FCMTNodeSearcherResult> Child);
	const TArray<TSharedPtr<FCMTNodeSearcherResult>>& GetChildren() const { return Children; }

private:
	/** result tree child */
	TArray<TSharedPtr<FCMTNodeSearcherResult>> Children;

	/** tree parent*/
	TWeakPtr<FCMTNodeSearcherResult> Parent;
	
	/** match expression node*/
	UMaterialExpression* Expression;
	
	/** match material */
	UMaterial* Material;
	
	/** match material function */
	UMaterialFunction* Function;

	/** match material function */
	UMaterialInstance* Instance;

	/** result display */
	FText DisplayText;

	/** expression comment */
	FString CommentText;
	
	/** result type */
	ECMTNodeSearcherResultType Type;
};
