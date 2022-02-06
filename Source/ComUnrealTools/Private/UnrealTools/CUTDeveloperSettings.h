// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ViewTools/CVTMPCViewer/CVTMPCViewerResult.h"
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

	/*
	 * savedには保存するけどEditorSettingsに要らない→configのみ
	 * EditorSettingsにも必要→(config, EditAnywhere, Category = "", meta = (DisplayName = ""))
	 */
	 
	/** MPCViewerのウォッチ欄保存 */
	UPROPERTY(config)
	TArray<FCVTMPCViewerConfig> CVTMPCViewerItems;

	/** VolumeRendererのリスト保存 */
	UPROPERTY(config)
	TArray<FCVTVolumeRendererItemInfo> CVTVolumeRendererItems;




	/** return "Project" or "Editor" */
	virtual FName GetContainerName() const override;

	/** 設定画面の大項目のカテゴライズ */
	virtual FName GetCategoryName() const override;

protected:
	/** 値変更時のコールバック */
	void OnPropertySettingChanged(UObject* Object, struct FPropertyChangedEvent& Property);

};