// Copyright com04 All Rights Reserved.

#include "CUTDeveloperSettings.h"
#include "ViewTools/CVTMPCViewerWatch/CVTMPCViewerWatch.h"


#define LOCTEXT_NAMESPACE "CUTDeveloperSettings"


UCUTDeveloperSettings::UCUTDeveloperSettings()
: Super(),
  bUseSavedCache(true),
  bUseDisplayNameInPropertySearch(true),
  bUseDebugOutputNameInPropertySearch(false),
  CVTVolumeRendererSolidAlpha(0.5f),
  bPropertyChange(false)
{
	InitialzieCachedParameter();
	OnSettingChanged().AddUObject(this, &UCUTDeveloperSettings::OnPropertySettingChanged);

	// FComViewTools 内で変更されたパラメーターを受け取る
	ViewToolsOnChangedDelegateHandle = FComViewTools::Get().AddOnChangedParametersDelegate(
			FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &UCUTDeveloperSettings::OnExternalPropertySettingChanged));
}
UCUTDeveloperSettings::~UCUTDeveloperSettings()
{
	if (ViewToolsOnChangedDelegateHandle.IsValid())
	{
		FComViewTools::Get().RemoveOnChangedParametersDelegate(ViewToolsOnChangedDelegateHandle);
		ViewToolsOnChangedDelegateHandle.Reset();
	}
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
void UCUTDeveloperSettings::PostInitProperties()
{
	Super::PostInitProperties();
	OnPropertySettingChanged();
}
const UCUTDeveloperSettings* UCUTDeveloperSettings::Get()
{
	return GetDefault<UCUTDeveloperSettings>();
}
UCUTDeveloperSettings* UCUTDeveloperSettings::GetWritable()
{
	return GetMutableDefault<UCUTDeveloperSettings>();
}


FDelegateHandle UCUTDeveloperSettings::AddOnChangedDelegate(FCUTOnChangedDeveloperSettings::FDelegate InDelegate)
{
	return OnChangedDelegate.Add(InDelegate);
}

void UCUTDeveloperSettings::RemoveOnChangedDelegate(FDelegateHandle InDelegateHandle)
{
	OnChangedDelegate.Remove(InDelegateHandle);
}

/** 値変更時のコールバック */
void UCUTDeveloperSettings::OnPropertySettingChanged(UObject* Object, struct FPropertyChangedEvent& Property)
{
	OnPropertySettingChanged();
}
void UCUTDeveloperSettings::OnPropertySettingChanged()
{
	bPropertyChange = true;
	// runtime 側へ受け渡し
	{
		FComViewTools& ViewTools = FComViewTools::Get();
		ViewTools.SetVolumeRendererLineThickness(CVTVolumeRendererLineThickness);
		ViewTools.SetVolumeRendererOneShotDuration(CVTVolumeRendererOneShotDuration);
		ViewTools.SetVolumeRendererRenderDistance(CVTVolumeRendererRenderDistance);
		ViewTools.SetVolumeRendererSolidAlpha(CVTVolumeRendererSolidAlpha);
	}
	
	OnChangedDelegate.Broadcast();

	SaveConfig();
	bPropertyChange = false;
}

void UCUTDeveloperSettings::OnExternalPropertySettingChanged()
{
	if (bPropertyChange)
	{
		return;
	}
	FComViewTools& ViewTools = FComViewTools::Get();
	CVTVolumeRendererLineThickness = ViewTools.GetVolumeRendererLineThickness();
	CVTVolumeRendererOneShotDuration = ViewTools.GetVolumeRendererOneShotDuration();
	CVTVolumeRendererRenderDistance = ViewTools.GetVolumeRendererRenderDistance();
	CVTVolumeRendererSolidAlpha = ViewTools.GetVolumeRendererSolidAlpha();

	OnChangedDelegate.Broadcast();
	SaveConfig();
}

#undef LOCTEXT_NAMESPACE
