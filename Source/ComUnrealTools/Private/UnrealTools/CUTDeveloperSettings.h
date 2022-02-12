// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ViewTools/CVTMPCViewer/CVTMPCViewerResult.h"
#include "ViewTools/CVTVolumeRenderer/CVTVolumeRendererItem.h"
#include "CUTDeveloperSettings.generated.h"


#define CUT_DEVSETTINGS_INTERNAL(Name, ConfigName, Type, RefOutType, SetFunc, GetFunc) \
	static void Set##Name(Type InValue) \
	{ \
		UCUTDeveloperSettings* Settings = UCUTDeveloperSettings::GetWritable(); \
		Settings->ConfigName = SetFunc; \
	} \
	static Type Get##Name()\
	{ \
		const UCUTDeveloperSettings* Settings = UCUTDeveloperSettings::Get(); \
		return GetFunc; \
	} \
	static RefOutType& Get##Name##Ref() \
	{ \
		return UCUTDeveloperSettings::GetWritable()->ConfigName; \
	}

#define CUT_DEVSETTINGS_ATOMIC(Name, ConfigName, Type) CUT_DEVSETTINGS_INTERNAL(Name, ConfigName, Type,        Type, InValue, Settings->ConfigName)
#define CUT_DEVSETTINGS_STRUCT(Name, ConfigName, Type) CUT_DEVSETTINGS_INTERNAL(Name, ConfigName, const Type&, Type, InValue, Settings->ConfigName)

#define CUT_DEVSETTINGS_INT(Name, ConfigName) CUT_DEVSETTINGS_ATOMIC(Name, ConfigName, int32)
#define CUT_DEVSETTINGS_FLOAT(Name, ConfigName) CUT_DEVSETTINGS_ATOMIC(Name, ConfigName, float)
#define CUT_DEVSETTINGS_COLOR(Name, ConfigName) CUT_DEVSETTINGS_ATOMIC(Name, ConfigName, FLinearColor)
#define CUT_DEVSETTINGS_STRING(Name, ConfigName) CUT_DEVSETTINGS_STRUCT(Name, ConfigName, FString)
#define CUT_DEVSETTINGS_CHECKBOX(Name, ConfigName) CUT_DEVSETTINGS_INTERNAL(Name, ConfigName, ECheckBoxState, bool, (InValue == ECheckBoxState::Checked), Settings->ConfigName ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
#define CUT_DEVSETTINGS_ENUM(Name, ConfigName, EnumType) CUT_DEVSETTINGS_INTERNAL(Name, ConfigName, EnumType, int32, static_cast<int32>(InValue), static_cast<EnumType>(Settings->ConfigName))






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
	
	/** 使用中のパラメーターをSavedに保存して、次回起動時に復元するか */
	UPROPERTY(config, EditAnywhere, Category = "Option", meta = (DisplayName = "設定をキャッシュに保存するか"))
	bool UseSavedCache;

	
	 
	/** NodeSearcher: 検索パス */
	UPROPERTY(config)
	FString CMTNodeSearcherSearchPath;
	/** NodeSearcher: 検索名 */
	UPROPERTY(config)
	FString CMTNodeSearcherSearchName;
	/** NodeSearcher: マテリアルインスタンスも検索するか */
	UPROPERTY(config)
	bool CMTNodeSearcherCheckMaterialInstance;

	/** ParameterDump: Overrideしている値のみ表示するか */
	UPROPERTY(config)
	bool CMTParameterDumpDispOverrideOnly;

	/** ParameterSearcher: 検索パス */
	UPROPERTY(config)
	FString CMTParameterSearcherSearchPath;
	/** ParameterSearcher: 検索名 */
	UPROPERTY(config)
	FString CMTParameterSearcherSearchName;
	/** ParameterSearcher: 検索Scalar */
	UPROPERTY(config)
	float CMTParameterSearcherScalarValue;
	/** ParameterSearcher: 検索Vector */
	UPROPERTY(config)
	FLinearColor CMTParameterSearcherVectorValue;
	/** ParameterSearcher: 検索Texture */
	UPROPERTY(config)
	FString CMTParameterSearcherTextureValue;
	/** ParameterSearcher: 検索Vector関数 */
	UPROPERTY(config)
	int32 CMTParameterSearcherVectorFunction;
	/** ParameterSearcher: 検索関数 */
	UPROPERTY(config)
	int32 CMTParameterSearcherFunction;
	/** ParameterSearcher: 検索種類 */
	UPROPERTY(config)
	int32 CMTParameterSearcherSelectType;
	/** ParameterSearcher: 検索Vector.R使うか */
	UPROPERTY(config)
	bool CMTParameterSearcherSelectUseVectorR;
	/** ParameterSearcher: 検索Vector.G使うか */
	UPROPERTY(config)
	bool CMTParameterSearcherSelectUseVectorG;
	/** ParameterSearcher: 検索Vector.B使うか */
	UPROPERTY(config)
	bool CMTParameterSearcherSelectUseVectorB;
	/** ParameterSearcher: 検索Vector.A使うか */
	UPROPERTY(config)
	bool CMTParameterSearcherSelectUseVectorA;
	/** ParameterSearcher: 検索StaticSwitch */
	UPROPERTY(config)
	bool CMTParameterSearcherSelectStaticSwitch;
	/** ParameterSearcher: Overrideしている値のみ表示するか */
	UPROPERTY(config)
	bool CMTParameterSearcherOverrideOnly;

	/** StatList: 検索パス */
	UPROPERTY(config)
	FString CMTStatListSearchPath;
	/** StatList: マテリアルインスタンスも検索するか */
	UPROPERTY(config)
	bool CMTStatListCheckMaterialInstance;

	/** TextureFind: 検索パス */
	UPROPERTY(config)
	FString CMTTextureFindSearchPath;
	/** TextureFind: 検索名 */
	UPROPERTY(config)
	FString CMTTextureFindSearchName;
	/** TextureFind: 一つ指定か */
	UPROPERTY(config)
	bool CMTTextureFindOneAsset;
	/** TextureFind: ディレクトリ検索でマテリアルを検索するか */
	UPROPERTY(config)
	bool CMTTextureFindDirectoryMaterial;
	/** TextureFind: ディレクトリ検索でマテリアル関数を検索するか */
	UPROPERTY(config)
	bool CMTTextureFindDirectoryMaterialFunction;

	/** TrackSearcher: 検索パス */
	UPROPERTY(config)
	FString CSTTrackSearcherSearchPath;
	/** TrackSearcher: 検索名 */
	UPROPERTY(config)
	FString CSTTrackSearcherSearchName;
	/** TrackSearcher: プロパティも検索するか */
	UPROPERTY(config)
	bool CSTTrackSearcherUsePropertySearch;

	/** MPCViewer: ウォッチ欄保存 */
	UPROPERTY(config)
	TArray<FCVTMPCViewerConfig> CVTMPCViewerItems;

	/** RenderTargetViewer: アルファ無しでプレビュー */
	UPROPERTY(config)
	bool CVTRenderTargetViewerPreviewWithoutAlpha;
	/** RenderTargetViewer: プレビューサイズ */
	UPROPERTY(config)
	int32 CVTRenderTargetViewerPreviewSize;

	/** VolumeRenderer: 線の太さ */
	UPROPERTY(config)
	float CVTVolumeRendererLineThickness;
	/** VolumeRenderer: OneShotの際の描画時間 */
	UPROPERTY(config)
	float CVTVolumeRendererOneShotDuration;
	/** VolumeRenderer: 表示距離 */
	UPROPERTY(config)
	float CVTVolumeRendererRenderDistance;
	/** VolumeRenderer: リスト保存 */
	UPROPERTY(config)
	TArray<FCVTVolumeRendererItemInfo> CVTVolumeRendererItems;
	/** VolumeRenderer: 面部分のアルファ値 */
	UPROPERTY(config, EditAnywhere, Category = "ViewTools|VolumeRenderer", meta = (DisplayName = "面部分のアルファ値", ClampMin=0.0f, ClampMax=1.0f))
	float CVTVolumeRendererSolidAlpha;




	/** return "Project" or "Editor" */
	virtual FName GetContainerName() const override;

	/** 設定画面の大項目のカテゴライズ */
	virtual FName GetCategoryName() const override;
	
	/** パラメーターを初期化する */
	void InitialzieCachedParameter();

public:
	static const UCUTDeveloperSettings* Get();
	static UCUTDeveloperSettings* GetWritable();
	

protected:
	/** 値変更時のコールバック */
	void OnPropertySettingChanged(UObject* Object, struct FPropertyChangedEvent& Property);

};