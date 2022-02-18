// Copyright com04 All Rights Reserved.

#include "CMTMaterialSearcher.h"
#include "Utility/CUTUtility.h"

#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"




FCMTMaterialSearcher::FCMTMaterialSearcher()
: FCUTAssetSearcher(),
  MaterialAssetIndex(-1),
  MaterialFunctionAssetIndex(-1),
  MaterialInstanceAssetIndex(-1)
{
}

void FCMTMaterialSearcher::SearchStart(
		const TArray<FString>& InSearchPathTokens,
		const TArray<FString>& InSearchNameTokens,
		bool InSearchEngineContent,
		bool InSearchMaterial, bool InSearchMaterialInstance,
		bool InSearchMaterialFunction)
{
	TArray<UClass*> SearchClasses;
	SearchClasses.Reserve(3);

	int32 AssetIndex = 0;
	if (InSearchMaterial)
	{
		MaterialAssetIndex = AssetIndex++;
		SearchClasses.Add(UMaterial::StaticClass());
	}
	if (InSearchMaterialFunction)
	{
		MaterialFunctionAssetIndex = AssetIndex++;
		SearchClasses.Add(UMaterialFunction::StaticClass());
	}
	if (InSearchMaterialInstance)
	{
		MaterialInstanceAssetIndex = AssetIndex++;
		SearchClasses.Add(UMaterialInstanceConstant::StaticClass());
	}
	
	FCUTAssetSearcher::SearchStart(InSearchPathTokens, InSearchNameTokens, InSearchEngineContent, SearchClasses);
}