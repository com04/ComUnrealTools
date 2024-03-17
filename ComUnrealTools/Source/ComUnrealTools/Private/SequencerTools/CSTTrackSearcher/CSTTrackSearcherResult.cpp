// Copyright com04 All Rights Reserved.

#include "CSTTrackSearcherResult.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialInstance.h"
#include "Widgets/Images/SImage.h"



//////////////////////////////////////////////////////////////////////////
// FCSTTrackSearcherResult

FCSTTrackSearcherResult::FCSTTrackSearcherResult(const FText& InDisplayText) 
	: Parent(nullptr),
	  LevelSequence(nullptr),
	  LevelSequenceFolder(nullptr),
	  LevelSequenceBindingProxy(),
	  LevelSequenceTrack(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(),
	  Type(ECSTTrackSearcherResultType::Other)
{
}

FCSTTrackSearcherResult::FCSTTrackSearcherResult(const FText& InDisplayText, ULevelSequence* InLevelSequence) 
	: Parent(nullptr),
	  LevelSequence(InLevelSequence),
	  LevelSequenceFolder(nullptr),
	  LevelSequenceBindingProxy(),
	  LevelSequenceTrack(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(),
	  Type(ECSTTrackSearcherResultType::LevelSequence)
{
}
FCSTTrackSearcherResult::FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, ECSTTrackSearcherResultType InType, const FText& InOptionText)
	: Parent(InParent),
	  LevelSequence(nullptr),
	  LevelSequenceFolder(nullptr),
	  LevelSequenceBindingProxy(),
	  LevelSequenceTrack(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(InOptionText),
	  Type(InType)
{
}
FCSTTrackSearcherResult::FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, const UMovieSceneFolder* InFolder, const FText& InOptionText)
	: Parent(InParent),
	  LevelSequence(nullptr),
	  LevelSequenceFolder(InFolder),
	  LevelSequenceBindingProxy(),
	  LevelSequenceTrack(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(InOptionText),
	  Type(ECSTTrackSearcherResultType::Folder)
{
}
FCSTTrackSearcherResult::FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, const FMovieSceneBindingProxy& InObjectBindingProxy, const FText& InOptionText)
	: Parent(InParent),
	  LevelSequence(nullptr),
	  LevelSequenceFolder(nullptr),
	  LevelSequenceBindingProxy(InObjectBindingProxy),
	  LevelSequenceTrack(nullptr),
	  DisplayText(InDisplayText),
	  OptionText(InOptionText),
	  Type(ECSTTrackSearcherResultType::Blueprint)
{
}
FCSTTrackSearcherResult::FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, const UMovieSceneTrack* InTrack, const FText& InOptionText)
	: Parent(InParent),
	  LevelSequence(nullptr),
	  LevelSequenceFolder(nullptr),
	  LevelSequenceBindingProxy(),
	  LevelSequenceTrack(InTrack),
	  DisplayText(InDisplayText),
	  OptionText(InOptionText),
	  Type(ECSTTrackSearcherResultType::Track)
{
}

FReply FCSTTrackSearcherResult::OnClick()
{
	if (WakeupLevelSequenceEditor())
	{
		// ここでやってもターゲットされない。待つ必要ある？
		SelectLevelSequenceTrackTarget();
	}
	
	return FReply::Handled();
}
bool FCSTTrackSearcherResult::WakeupLevelSequenceEditor()
{
	if (Type == ECSTTrackSearcherResultType::LevelSequence)
	{
		FCUTUtility::WakeupSequencerEditor(LevelSequence);
		return true;
	}
	else
	{
		if (Parent.IsValid())
		{
			return Parent.Pin()->WakeupLevelSequenceEditor();
		}
	}
	return false;
}
void FCSTTrackSearcherResult::SelectLevelSequenceTrackTarget()
{
	switch (Type)
	{
	case ECSTTrackSearcherResultType::Folder:
		if (LevelSequenceFolder)
		{
			FCUTUtility::SelectSequencerEditorFolders(TArray<const UMovieSceneFolder*>{LevelSequenceFolder});
		}
		break;
	case ECSTTrackSearcherResultType::Blueprint:
		if (LevelSequenceBindingProxy.BindingID.IsValid())
		{
			FCUTUtility::SelectSequencerEditorObjects(TArray<FMovieSceneBindingProxy>{LevelSequenceBindingProxy});
		}
		break;
	case ECSTTrackSearcherResultType::Property:
		if (Parent.IsValid())
		{
			Parent.Pin()->SelectLevelSequenceTrackTarget();
		}
		break;
	case ECSTTrackSearcherResultType::Track:
		if (LevelSequenceTrack)
		{
			FCUTUtility::SelectSequencerEditorTracks(TArray<const UMovieSceneTrack*>{LevelSequenceTrack});
		}
		break;
	}
}

TSharedRef<SWidget>	FCSTTrackSearcherResult::CreateIcon() const
{
	FName BrushName = FComUnrealToolsStyle::MenuBGBrushName;
	switch (Type)
	{
	case ECSTTrackSearcherResultType::LevelSequence:
		BrushName = FComUnrealToolsStyle::LevelSequenceIconBrushName;
		break;
	case ECSTTrackSearcherResultType::Folder:
		BrushName = FComUnrealToolsStyle::FolderIconBrushName;
		break;
	case ECSTTrackSearcherResultType::Blueprint:
		BrushName = FComUnrealToolsStyle::BlueprintIconBrushName;
		break;
	case ECSTTrackSearcherResultType::Track:
		BrushName = FComUnrealToolsStyle::TrackIconBrushName;
		break;
	}
	
	
	return 	SNew(SImage)
		.Image(FComUnrealToolsStyle::Get().GetBrush(BrushName))
		.ColorAndOpacity(FLinearColor::White);
}

FName	FCSTTrackSearcherResult::GetBackgroundBrushName() const
{
	FName BrushName(FComUnrealToolsStyle::MenuBGBrushName);
	switch (Type)
	{
	case ECSTTrackSearcherResultType::LevelSequence:
		BrushName = FName(FComUnrealToolsStyle::MenuBG2BrushName);
		break;
	}
	return BrushName;
}


FText FCSTTrackSearcherResult::GetDisplayText() const
{
	return DisplayText;
}
FText FCSTTrackSearcherResult::GetOptionText() const
{
	return OptionText;
}
FString FCSTTrackSearcherResult::GetTypeString() const
{
	FString RetString;
	switch (GetType())
	{
	case ECSTTrackSearcherResultType::LevelSequence:
		RetString = FString(TEXT("LevelSequence"));
		break;
	case ECSTTrackSearcherResultType::Folder:
		RetString = FString(TEXT("Folder"));
		break;
	case ECSTTrackSearcherResultType::Blueprint:
		RetString = FString(TEXT("BP"));
		break;
	case ECSTTrackSearcherResultType::Property:
		RetString = FString(TEXT("Property"));
		break;
	case ECSTTrackSearcherResultType::Track:
		RetString = FString(TEXT("Track"));
		break;
	}
	return RetString;
}


void FCSTTrackSearcherResult::AddChild(TSharedPtr<FCSTTrackSearcherResult> Child)
{
	Children.Add(Child);
}
