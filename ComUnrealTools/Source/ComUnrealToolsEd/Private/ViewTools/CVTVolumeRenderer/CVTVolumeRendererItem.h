// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ViewTools/ComViewTools.h"


/* added item */
class FCVTVolumeRendererItem : public TSharedFromThis<FCVTVolumeRendererItem>
{
public: 
	FCVTVolumeRendererItem(const FCVTVolumeRendererItemInfo& InResultInfo)
	: ResultInfo(InResultInfo),
	  bAlways(false)
	{}

	virtual ~FCVTVolumeRendererItem() {}
	

	void SetInfoDisplayColor(FLinearColor InColor) { ResultInfo.DisplayColor = InColor; }
	const FCVTVolumeRendererItemInfo& GetInfo() const { return ResultInfo; }

	void SetAlways(bool bInAlways) { bAlways = bInAlways; }
	bool IsAlways() const { return bAlways; }
	TFunction<void(TSharedPtr<FCVTVolumeRendererItem>)> OnChangedAlways;
	TFunction<void(TSharedPtr<FCVTVolumeRendererItem>)> OnOneShot;
	TFunction<void(TSharedPtr<FCVTVolumeRendererItem>)> OnRemove;
private:
	FCVTVolumeRendererItemInfo ResultInfo;
	bool bAlways;
};

