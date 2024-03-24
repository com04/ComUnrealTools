// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"

#include "ComUnrealToolsCommon.h"
#include "ComViewTools.generated.h"


USTRUCT()
struct COMUNREALTOOLS_API FCVTVolumeRendererItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Class;
	UPROPERTY(EditAnywhere)
	FLinearColor DisplayColor;

	FCVTVolumeRendererItemInfo()
	: Class(),
	  DisplayColor(FLinearColor::White)
	{}
	
	bool operator==(const FCVTVolumeRendererItemInfo& Value) const
	{
		return (Class == Value.Class) && (DisplayColor == Value.DisplayColor);
	}
};


/**
 */
class COMUNREALTOOLS_API FComViewTools : public FTickableGameObject
{
public:
	static FComViewTools& Get();
	static bool IsInitialized();

	FComViewTools();
	~FComViewTools();
	

	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickable() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	
	void AddVolumeRendererItemAlways(const FCVTVolumeRendererItemInfo& InInfo);
	void RemoveVolumeRendererItemAlways(const FCVTVolumeRendererItemInfo& InInfo);
	void AddVolumeRendererItemOneshot(const FCVTVolumeRendererItemInfo& InInfo);
	void RemoveVolumeRendererItemAll();

	void AddVolumeRendererItemFromClassName(const FString& InClassName, bool bInAlways, const FLinearColor& InColor);
	void RemoveVolumeRendererItemFromClassName(const FString& InClassName);
	
	void SetVolumeRendererLineThickness(float InValue);
	float GetVolumeRendererLineThickness() const;
	
	void SetVolumeRendererOneShotDuration(float InValue);
	float GetVolumeRendererOneShotDuration() const;
	
	void SetVolumeRendererRenderDistance(float InValue);
	float GetVolumeRendererRenderDistance() const;
	
	void SetVolumeRendererSolidAlpha(float InValue);
	float GetVolumeRendererSolidAlpha() const;
	
	FDelegateHandle AddOnChangedParametersDelegate(FSimpleMulticastDelegate::FDelegate InDelegate);
	void RemoveOnChangedParametersDelegate(FDelegateHandle InDelegateHandle);
	
private:
	static FComViewTools* GetInstance(bool bInCreate);
	void RenderItem(class UWorld* InWorld, const FCVTVolumeRendererItemInfo& InInfo, float InDuration) const;

private:
	TArray<FCVTVolumeRendererItemInfo> VolumeRendererItemsAlways;
	TArray<FCVTVolumeRendererItemInfo> VolumeRendererItemsOneshot;

	/** VolumeRenderer: 線の太さ */
	float VolumeRendererLineThickness;
	/** VolumeRenderer: OneShotの際の描画時間 */
	float VolumeRendererOneShotDuration;
	/** VolumeRenderer: 表示距離 */
	float VolumeRendererRenderDistance;
	/** VolumeRenderer: 面部分のアルファ値 */
	float VolumeRendererSolidAlpha;

	/** 値が変更された際のDelegate */
	FSimpleMulticastDelegate OnChangedParameters;
};
