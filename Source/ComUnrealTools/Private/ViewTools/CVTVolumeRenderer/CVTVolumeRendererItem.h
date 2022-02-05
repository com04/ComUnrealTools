// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * SCVTVolumeRenderer Item
 */
struct FCVTVolumeRendererItemInfo
{
	TSubclassOf<AActor> Class;
	bool bAlways;
	FLinearColor DisplayColor;

	FCVTVolumeRendererItemInfo()
	: Class(),
	  bAlways(false),
	  DisplayColor(FLinearColor::White)
	{}
	
};

/* added item */
class FCVTVolumeRendererItem : public TSharedFromThis<FCVTVolumeRendererItem>
{
public: 
	FCVTVolumeRendererItem(const FCVTVolumeRendererItemInfo& InResultInfo)
	: ResultInfo(InResultInfo)
	{}

	virtual ~FCVTVolumeRendererItem() {}
	

	void SetInfoDisplayColor(FLinearColor InColor) { ResultInfo.DisplayColor = InColor; }
	void SetInfoAlways(bool bInAlways) { ResultInfo.bAlways = bInAlways; }
	const FCVTVolumeRendererItemInfo& GetInfo() const { return ResultInfo; }

	TFunction<void(TSharedPtr<FCVTVolumeRendererItem>)> OnAlwaysON;
	TFunction<void(TSharedPtr<FCVTVolumeRendererItem>)> OnOneShot;
private:
	FCVTVolumeRendererItemInfo ResultInfo;
};

