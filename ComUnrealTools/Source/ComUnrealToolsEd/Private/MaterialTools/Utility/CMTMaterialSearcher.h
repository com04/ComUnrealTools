// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Utility/CUTAssetSearcher.h"

struct FAssetData;

DECLARE_DELEGATE(FCMTMaterialSearcherDispatcher);


class FCMTMaterialSearcher : public FCUTAssetSearcher
{
public:
	FCMTMaterialSearcher();

	void SearchStart(
			const TArray<FString>& InSearchPathTokens,
			const TArray<FString>& InSearchNameTokens,
			bool InSearchEngineContent,
			bool InSearchMaterial, bool InSearchMaterialInstance,
			bool InSearchMaterialFunction);
	
	const TArray<FAssetData>& GetMaterialAssets() const { return GetAssets(MaterialAssetIndex); }
	const TArray<FAssetData>& GetMaterialFunctionAssets() const { return GetAssets(MaterialFunctionAssetIndex); }
	const TArray<FAssetData>& GetMaterialInstanceAssets() const { return GetAssets(MaterialInstanceAssetIndex); }
	
public:
	int32 MaterialAssetIndex;
	int32 MaterialFunctionAssetIndex;
	int32 MaterialInstanceAssetIndex;
};
