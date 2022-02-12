// Copyright com04 All Rights Reserved.

#include "CMTNodeSearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Widgets/Images/SImage.h"



//////////////////////////////////////////////////////////////////////////
// FCMTNodeSearcherResult

FCMTNodeSearcherResult::FCMTNodeSearcherResult(const FText& InDisplayText) 
	: Parent(nullptr),
	  Expression(nullptr),
	  Material(nullptr),
	  Function(nullptr),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  Type(ECMTNodeSearcherResultType::Other)
{
}

FCMTNodeSearcherResult::FCMTNodeSearcherResult(const FText& InDisplayText, UMaterial* InMaterial) 
	: Parent(nullptr),
	  Expression(nullptr),
	  Material(InMaterial),
	  Function(nullptr),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  Type(ECMTNodeSearcherResultType::Material)
{
}
FCMTNodeSearcherResult::FCMTNodeSearcherResult(const FText& InDisplayText, UMaterialFunction* InFunction) 
	: Parent(nullptr),
	  Expression(nullptr),
	  Material(nullptr),
	  Function(InFunction),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  Type(ECMTNodeSearcherResultType::MaterialFunction)
{
}
FCMTNodeSearcherResult::FCMTNodeSearcherResult(const FText& InDisplayText, UMaterialInstance* InInstance) 
	: Parent(nullptr),
	  Expression(nullptr),
	  Material(nullptr),
	  Function(nullptr),
	  Instance(InInstance),
	  DisplayText(InDisplayText),
	  Type(ECMTNodeSearcherResultType::MaterialInstance)
{
}

FCMTNodeSearcherResult::FCMTNodeSearcherResult(const FText& InDisplayText, TSharedPtr<FCMTNodeSearcherResult> InParent, UMaterialExpression* InExpression) 
	: Parent(InParent),
	  Expression(InExpression),
	  Material(nullptr),
	  Function(nullptr),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  Type(ECMTNodeSearcherResultType::Expression)
{
	if (Expression)
	{
		CommentText = FText::FromString(Expression->Desc);
	}
}
FCMTNodeSearcherResult::FCMTNodeSearcherResult(const FText& InDisplayText, TSharedPtr<FCMTNodeSearcherResult> InParent, UTexture* InTexture)
	: Parent(InParent),
	  Expression(nullptr),
	  Material(nullptr),
	  Function(nullptr),
	  Instance(nullptr),
	  DisplayText(InDisplayText),
	  Type(ECMTNodeSearcherResultType::Texture)
{
}

FReply FCMTNodeSearcherResult::OnClick()
{
	if(Parent.IsValid())
	{
		FReply Reply = Parent.Pin()->OnClick();
		
#if 0
		//
		// マテリアルエディターのノードにフォーカスしたいけど、アクセスが公開されてない
		//
		if (Type == ECMTNodeSearcherResultType::Expression)
		{
			IAssetEditorInstance* AssetEditor = nullptr;
			if (Expression->Material)
			{
				AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Expression->Material, true);
			}
			else
			{
				AssetEditor = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Expression->Function, true);
			}
			if (AssetEditor)
			{
				IMaterialEditor* MaterialEditor = reinterpret_cast<IMaterialEditor*>(AssetEditor);
				// FMaterialEditor* MaterialEditor = reinterpret_cast<FMaterialEditor*>(AssetEditor);
				if (MaterialEditor)
				{
					UMaterialInterface* Interface = MaterialEditor->GetMaterialInterface();
					if (Interface)
					{
						UMaterial* EditorMaterial = Interface->GetMaterial();
						if (EditorMaterial)
						{
							for (auto ItExpression = EditorMaterial->Expressions.CreateConstIterator(); ItExpression; ++ItExpression)
							{
								if (Expression->MaterialExpressionGuid == (*ItExpression)->MaterialExpressionGuid)
								{
									// MaterialEditor->AddToSelection(*ItExpression);
									// MaterialEditor->JumpToNode((*ItExpression)->GraphNode);  // bad, can't access...
									break;
								}
							}
						}
					}
				}
			}
		}
#endif
		return Reply;
	}
	else
	{
		// wakeup MaterialEditor
		switch (Type)
		{
		case ECMTNodeSearcherResultType::Material:
			FCUTUtility::WakeupMaterialEditor(Material, nullptr, nullptr);
			break;
		case ECMTNodeSearcherResultType::MaterialFunction:
			FCUTUtility::WakeupMaterialEditor(nullptr, nullptr, Function);
			break;
		case ECMTNodeSearcherResultType::MaterialInstance:
			FCUTUtility::WakeupMaterialEditor(nullptr, Instance, nullptr);
			break;
		}
	}
	
	return FReply::Handled();
}

TSharedRef<SWidget>	FCMTNodeSearcherResult::CreateIcon() const
{
	FName BrushName;
	switch (Type)
	{
	case ECMTNodeSearcherResultType::Material:
		BrushName = FComUnrealToolsStyle::MaterialIconBrushName;
		break;
	case ECMTNodeSearcherResultType::MaterialFunction:
		BrushName = FComUnrealToolsStyle::MaterialFunctionIconBrushName;
		break;
	case ECMTNodeSearcherResultType::MaterialInstance:
		BrushName = FComUnrealToolsStyle::MaterialInstanceIconBrushName;
		break;
	default:
		BrushName = GetBackgroundBrushName();
		break;
	}
	
	
	return 	SNew(SImage)
		.Image(FComUnrealToolsStyle::Get().GetBrush(BrushName))
		.ColorAndOpacity(FLinearColor::White);
}

FName	FCMTNodeSearcherResult::GetBackgroundBrushName() const
{
	FName BrushName(FComUnrealToolsStyle::MenuBGBrushName);
	switch (Type)
	{
	case ECMTNodeSearcherResultType::Material:
	case ECMTNodeSearcherResultType::MaterialFunction:
	case ECMTNodeSearcherResultType::MaterialInstance:
		BrushName = FName(FComUnrealToolsStyle::MenuBG2BrushName);
		break;
	}
	return BrushName;
}

FText FCMTNodeSearcherResult::GetCommentText() const
{
	return CommentText;
}


FText FCMTNodeSearcherResult::GetDisplayText() const
{
	return DisplayText;
}

void FCMTNodeSearcherResult::AddChild(TSharedPtr<FCMTNodeSearcherResult> Child)
{
	Children.Add(Child);
}
