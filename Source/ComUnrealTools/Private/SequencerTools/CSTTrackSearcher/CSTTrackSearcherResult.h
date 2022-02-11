// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Containers/Array.h"
#include "SlateFwd.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"


class ULevelSequence;
class UMovieSceneFolder;
class UMovieSceneTrack;

/** find result type */
enum class ECSTTrackSearcherResultType
{
	LevelSequence,
	Folder,
	Blueprint,
	Property,
	Track,
	Other
};

/* search result */
class FCSTTrackSearcherResult : public TSharedFromThis<FCSTTrackSearcherResult>
{
public: 
	/* root */
	FCSTTrackSearcherResult(const FText& InDisplayText);
	FCSTTrackSearcherResult(const FText& InDisplayText, ULevelSequence* InLevelSequence);
	FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, ECSTTrackSearcherResultType InType, const FText& InOptionText=FText());
	FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, const UMovieSceneFolder* InFolder, const FText& InOptionText=FText());
	FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, const FGuid& InObjectGuid, const FText& InOptionText=FText());
	FCSTTrackSearcherResult(const FText& InDisplayText, TSharedPtr<FCSTTrackSearcherResult> InParent, const UMovieSceneTrack* InTrack, const FText& InOptionText=FText());
	

	virtual ~FCSTTrackSearcherResult() {}
	
	/** tree view clicked event */
	virtual FReply OnClick();
	bool WakeupLevelSequenceEditor();
	void SelectLevelSequenceTrackTarget();
	
	/** tree view icon */
	TSharedRef<SWidget>	CreateIcon() const;
	
	/** tree view background brush */
	FName	GetBackgroundBrushName() const;

	/** result display text */
	FText GetDisplayText() const;
	
	/** result option text */
	FText GetOptionText() const;
	
	FString GetTypeString() const;
	ECSTTrackSearcherResultType GetType() const { return Type; }
	
	/** Add tree child */
	void AddChild(TSharedPtr<FCSTTrackSearcherResult> Child);
	const TArray<TSharedPtr<FCSTTrackSearcherResult>>& GetChildren() const { return Children; }

private:
	/** result tree child */
	TArray<TSharedPtr<FCSTTrackSearcherResult>> Children;

	/** tree parent*/
	TWeakPtr<FCSTTrackSearcherResult> Parent;
	
	/** match Level Sequence*/
	ULevelSequence* LevelSequence;
	const UMovieSceneFolder* LevelSequenceFolder;
	FGuid LevelSequenceObjectGuid;
	const UMovieSceneTrack* LevelSequenceTrack;

	/** result display */
	FText DisplayText;

	/** result option */
	FText OptionText;

	/** result type */
	ECSTTrackSearcherResultType Type;
};
