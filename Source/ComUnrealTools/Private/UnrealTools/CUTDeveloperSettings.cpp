// Copyright com04 All Rights Reserved.

#include "CUTDeveloperSettings.h"
#include "ViewTools/CVTVolumeRenderer/CVTVolumeRenderer.h"


#define LOCTEXT_NAMESPACE "CUTDeveloperSettings"


UCUTDeveloperSettings::UCUTDeveloperSettings()
: Super()
{
	OnSettingChanged().AddUObject(this, &UCUTDeveloperSettings::OnPropertySettingChanged);
}
/** return "Project" or "Editor" */
FName UCUTDeveloperSettings::GetContainerName() const 
{
	return "Editor";
}

/** 設定画面の大項目のカテゴライズ */
FName UCUTDeveloperSettings::GetCategoryName() const
{
	return "Plugins";
}


/** 値変更時のコールバック */
void UCUTDeveloperSettings::OnPropertySettingChanged(UObject* Object, struct FPropertyChangedEvent& Property)
{
	SCVTVolumeRenderer::OnChangedEditorSettings(this, Property);

	SaveConfig();
}

#undef LOCTEXT_NAMESPACE
