// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ViewTools/CVTVolumeRenderer/CVTVolumeRendererItem.h"
#include "CUTDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(config = EditorPerProjectUserSettings, meta = (DisplayName = "Com Unreal Tools Plugin"))
class UCUTDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UCUTDeveloperSettings();

	/* Activate or deactivate the whole plugin. Default: true */
	UPROPERTY(config, EditAnywhere, Category = "Volume Renderer", meta = (DisplayName = "Default Items"))
	TArray<FCVTVolumeRendererItemInfo> CVTVolumeRendererItems;

	/** return "Project" or "Editor" */
	virtual FName GetContainerName() const override;

	/** 設定画面の大項目のカテゴライズ */
	virtual FName GetCategoryName() const override;

protected:
	/** 値変更時のコールバック */
	void OnPropertySettingChanged(UObject* Object, struct FPropertyChangedEvent& Property);

};