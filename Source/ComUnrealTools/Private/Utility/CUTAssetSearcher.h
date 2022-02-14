// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"

struct FAssetData;
struct FStreamableHandle;

DECLARE_DELEGATE(FCUTAssetSearcherDispatcher);


class FCUTAssetSearcher
{
public:
	FCUTAssetSearcher();

	void SearchStart(
			const TArray<FString>& InSearchPathTokens,
			const TArray<FString>& InSearchNameTokens,
			bool InSearchEngineContent,
			const TArray<UClass*>& SearchAssetClasses);
	
	const TArray<FAssetData>& GetAssets(int32 InIndex) const;
	int32 GetMaxAssetsIndex() const;
	float GetProgress() const;
	bool IsAsyncLoading() const;
	void Reset();
	
public:
	FCUTAssetSearcherDispatcher OnSearchEnd;
	
private:
	/** check SearchPathTokens */
	void CheckSearchPathContent(TArray<FAssetData>* InList);
	/** check SearchNameTokens */
	void CheckSearchNameContent(TArray<FAssetData>* InList);
	
	/** finish StreamableManager async load callback. this function call in main thread.*/
	void FinishAsyncLoad(int32 SearchDataIndex);
	
	
	
private:
	/** search path list */
	TArray<FString> SearchPathTokens;
	
	/** search name list*/
	TArray<FString> SearchNameTokens;
	
	bool SearchEngineContent;
	
	
	/** async load counter */
	int32           LoadCounter;
	
	struct FSearchData
	{
		TArray<FAssetData> AssetData;
		TSharedPtr<FStreamableHandle> StreamableHandle;
	};
	
	TArray<FSearchData> SearchData;
	
	/** async load manager */
	FStreamableManager StreamableManager;
};
