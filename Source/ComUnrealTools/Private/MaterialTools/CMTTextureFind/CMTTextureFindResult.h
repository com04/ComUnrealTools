// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CMTTextureFindResultData.h"

#include "Widgets/Layout/SBorder.h"


/**
 * TextureFind result row
 */
// class FCMTTextureFindResult : public TSharedFromThis<FCMTTextureFindResult>
class FCMTTextureFindResult : public SBorder
{
public:
	SLATE_BEGIN_ARGS(FCMTTextureFindResult) { }
		SLATE_ARGUMENT(FCMTTextureFindResultData, TextureResultData)
	SLATE_END_ARGS()

	/**
	 * Constructs the application.
	 *
	 * @param InArgs The Slate argument list.
	 * @param ConstructUnderMajorTab The major tab which will contain the session front-end.
	 * @param ConstructUnderWindow The window in which this widget is being constructed.
	 */
	void Construct(const FArguments& InArgs);

	~FCMTTextureFindResult();
	
	
protected:
	/** Material Thumbnail double click */
	FReply OnThumbnailMouseDoubleClick(const FGeometry& MyGeometry,const FPointerEvent& MouseEvent);
	
private:

	TSharedPtr<class FAssetThumbnailPool> AssetThumbnailPool;
	TSharedPtr<class FAssetThumbnail> AssetThumbnail;
	TSharedPtr<class SVerticalBox> ReferenceVerticalBox;
	
	FCMTTextureFindResultData TextureResultData;
};
