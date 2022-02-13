// Copyright com04 All Rights Reserved.

#include "CUTDeveloperSettings.h"
#include "ViewTools/CVTVolumeRenderer/CVTVolumeRenderer.h"
#include "ViewTools/CVTMPCViewerWatch/CVTMPCViewerWatch.h"


#define LOCTEXT_NAMESPACE "CUTDeveloperSettings"


UCUTDeveloperSettings::UCUTDeveloperSettings()
: Super(),
  UseSavedCache(true),
  bUseDisplayNameInPropertySearch(true),
  CVTVolumeRendererSolidAlpha(0.5f)
{
	InitialzieCachedParameter();
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

/** パラメーターを初期化する */
void UCUTDeveloperSettings::InitialzieCachedParameter()
{
	CMTNodeSearcherSearchPath = FString(TEXT("/Game/"));
	CMTNodeSearcherSearchName = FString();
	CMTNodeSearcherCheckMaterialInstance = true;
	CMTParameterDumpDispOverrideOnly = false;
	CMTParameterSearcherSearchPath = FString(TEXT("/Game/"));
	CMTParameterSearcherSearchName = FString();
	CMTParameterSearcherScalarValue = 0.0f;
	CMTParameterSearcherVectorValue = FLinearColor(0.0f, 0.0f, 0.0f, 0.0f);
	CMTParameterSearcherTextureValue = FString();
	CMTParameterSearcherVectorFunction = 0;
	CMTParameterSearcherFunction = 0;
	CMTParameterSearcherSelectType = 0;
	CMTParameterSearcherSelectUseVectorR = true;
	CMTParameterSearcherSelectUseVectorG = true;
	CMTParameterSearcherSelectUseVectorB = true;
	CMTParameterSearcherSelectUseVectorA = true;
	CMTParameterSearcherSelectStaticSwitch = true;
	CMTParameterSearcherOverrideOnly = false;
	CMTStatListSearchPath = FString(TEXT("/Game/"));
	CMTStatListCheckMaterialInstance = true;
	CMTTextureFindSearchPath = FString(TEXT("/Game/"));
	CMTTextureFindSearchName = FString();
	CMTTextureFindOneAsset = true;
	CMTTextureFindDirectoryMaterial = true;
	CMTTextureFindDirectoryMaterialFunction = false;
	CSTTrackSearcherSearchPath = FString(TEXT("/Game/"));
	CSTTrackSearcherSearchName = FString();
	CSTTrackSearcherUsePropertySearch = true;
	CVTMPCViewerItems.Empty();
	CVTRenderTargetViewerPreviewWithoutAlpha = true;
	CVTRenderTargetViewerPreviewSize = 128;
	CVTVolumeRendererLineThickness = 50.0f;
	CVTVolumeRendererOneShotDuration = 5.0f;
	CVTVolumeRendererRenderDistance = 0.0f;
	CVTVolumeRendererItems.Empty();
	COTPropertySearcherSearchPath = FString(TEXT("/Game/"));
	COTPropertySearcherSearchName = FString();
}
const UCUTDeveloperSettings* UCUTDeveloperSettings::Get()
{
	return GetDefault<UCUTDeveloperSettings>();
}
UCUTDeveloperSettings* UCUTDeveloperSettings::GetWritable()
{
	return GetMutableDefault<UCUTDeveloperSettings>();
}

/** 値変更時のコールバック */
void UCUTDeveloperSettings::OnPropertySettingChanged(UObject* Object, struct FPropertyChangedEvent& Property)
{
	SCVTMPCViewerWatch::OnChangedEditorSettings(this, Property);
	SCVTVolumeRenderer::OnChangedEditorSettings(this, Property);

	SaveConfig();
}

#undef LOCTEXT_NAMESPACE
