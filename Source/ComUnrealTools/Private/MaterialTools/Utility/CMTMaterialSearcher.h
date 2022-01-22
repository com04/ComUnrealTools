// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"

struct FAssetData;
struct FStreamableHandle;

DECLARE_DELEGATE(FCMTMaterialSearcherDispatcher);


class FCMTMaterialSearcher
{
public:
	FCMTMaterialSearcher();

	void SearchStart(
			const TArray<FString>& InSearchPathTokens,
			const TArray<FString>& InSearchNameTokens,
			bool InSearchEngineContent,
			bool InSearchMaterial, bool InSearchMaterialInstance,
			bool InSearchMaterialFunction);
	
	const TArray<FAssetData>& GetMaterialAssets() const { return AssetDataMaterial; }
	const TArray<FAssetData>& GetMaterialFunctionAssets() const { return AssetDataMaterialFunction; }
	const TArray<FAssetData>& GetMaterialInstanceAssets() const { return AssetDataMaterialInstance; }
	float GetProgress() const;
	bool IsAsyncLoading() const;
	
public:
	FCMTMaterialSearcherDispatcher OnSearchEnd;
	
private:
	/** async load start */
	TSharedPtr<FStreamableHandle> RequestASyncLoadAssets(TArray<FAssetData>& InOutAssetData, UClass* SearchAssetClass, TFunction<void()>&& Callback);

	/** check SearchPathTokens */
	void CheckSearchPathContent(TArray<FAssetData>* InList);
	/** check SearchNameTokens */
	void CheckSearchNameContent(TArray<FAssetData>* InList);
	
	/** finish StreamableManager async load callback. this function call in main thread.*/
	void FinishAsyncLoad(const TArray<FAssetData>& InAssetData);
	
	
	
private:
	/** search path list */
	TArray<FString> SearchPathTokens;
	
	/** search name list*/
	TArray<FString> SearchNameTokens;
	
	bool SearchEngineContent;
	bool SearchMaterial;
	bool SearchMaterialInstance;
	bool SearchMaterialFunction;
	
	
	
	
	/** async load counter */
	int32           LoadCounter;
	
	/** search asset list UMaterial */
	TArray<FAssetData> AssetDataMaterial;
	
	/** search asset list UMaterialFunction */
	TArray<FAssetData> AssetDataMaterialFunction;
	
	/** search asset list UMaterialInstance */
	TArray<FAssetData> AssetDataMaterialInstance;
	
	
	/** async load manager */
	FStreamableManager StreamableManager;
	
	/** StreamableManager async handle. */
	TSharedPtr<FStreamableHandle> StreamableHandleInMaterial;
	/** StreamableManager async handle. */
	TSharedPtr<FStreamableHandle> StreamableHandleInMaterialFunction;
	/** StreamableManager async handle. */
	TSharedPtr<FStreamableHandle> StreamableHandleInMaterialInstance;
	
};
