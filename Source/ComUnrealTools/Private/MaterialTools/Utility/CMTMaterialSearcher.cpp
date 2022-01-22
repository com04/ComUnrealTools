// Copyright com04 All Rights Reserved.

#include "CMTMaterialSearcher.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistryModule.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"




FCMTMaterialSearcher::FCMTMaterialSearcher()
: SearchPathTokens(),
  SearchNameTokens(),
  SearchEngineContent(true),
  SearchMaterial(true),
  SearchMaterialInstance(true),
  SearchMaterialFunction(true),
  LoadCounter(0),
  AssetDataMaterial(),
  AssetDataMaterialFunction(),
  AssetDataMaterialInstance()
{
}

void FCMTMaterialSearcher::SearchStart(
		const TArray<FString>& InSearchPathTokens,
		const TArray<FString>& InSearchNameTokens,
		bool InSearchEngineContent,
		bool InSearchMaterial, bool InSearchMaterialInstance,
		bool InSearchMaterialFunction)
{
	FCMTMaterialSearcher* Self = this;
	SearchPathTokens = InSearchPathTokens;
	SearchNameTokens = InSearchNameTokens;
	SearchEngineContent = InSearchEngineContent;
	SearchMaterial = InSearchMaterial;
	SearchMaterialInstance = InSearchMaterialInstance;
	SearchMaterialFunction = InSearchMaterialFunction;

	// search finish. clear variable.
	StreamableHandleInMaterial.Reset();
	StreamableHandleInMaterialFunction.Reset();
	StreamableHandleInMaterialInstance.Reset();
	AssetDataMaterial.Reset();
	AssetDataMaterialFunction.Reset();
	AssetDataMaterialInstance.Reset();
	
	// 2 type async load
	LoadCounter = 0;
	
	
	// get UMaterial assets
	if (SearchMaterial)
	{
		StreamableHandleInMaterial = RequestASyncLoadAssets(AssetDataMaterial, UMaterial::StaticClass(), [Self]()
			{
				Self->FinishAsyncLoad(Self->AssetDataMaterial);
			});
	}
	
	// get UMaterialFunction
	if (SearchMaterialFunction)
	{
		StreamableHandleInMaterial = RequestASyncLoadAssets(AssetDataMaterialFunction, UMaterialFunction::StaticClass(), [Self]()
			{
				Self->FinishAsyncLoad(Self->AssetDataMaterialFunction);
			});
	}
	
	// get UMaterialInstance
	if (SearchMaterialInstance)
	{
		StreamableHandleInMaterial = RequestASyncLoadAssets(AssetDataMaterialInstance, UMaterialInstanceConstant::StaticClass(), [Self]()
			{
				Self->FinishAsyncLoad(Self->AssetDataMaterialInstance);
			});
	}
	
	// no find
	if (LoadCounter == 0)
	{
		OnSearchEnd.Execute();
	}
}

float FCMTMaterialSearcher::GetProgress() const
{
	float Percent = 0.0f;
	float PercentMax = 0.0f;
	if (SearchMaterial)
	{
		PercentMax += 1.0f;
		if (StreamableHandleInMaterial.IsValid())
		{
			Percent += StreamableHandleInMaterial->GetProgress();
		}
		else
		{
			Percent += 1.0f;
		}
	}
	if (SearchMaterialFunction)
	{
		PercentMax += 1.0f;
		if (StreamableHandleInMaterialFunction.IsValid())
		{
			Percent += StreamableHandleInMaterialFunction->GetProgress();
		}
		else
		{
			Percent += 1.0f;
		}
	}
	if (SearchMaterialInstance)
	{
		PercentMax += 1.0f;
		if (StreamableHandleInMaterialInstance.IsValid())
		{
			Percent += StreamableHandleInMaterialInstance->GetProgress();
		}
		else
		{
			Percent += 1.0f;
		}
	}
	return FMath::Max(Percent, 0.01f) / FMath::Max(PercentMax, 0.01f);
}

bool FCMTMaterialSearcher::IsAsyncLoading() const
{
	return LoadCounter > 0;
}

/** async load start */
TSharedPtr<FStreamableHandle> FCMTMaterialSearcher::RequestASyncLoadAssets(TArray<FAssetData>& InOutAssetData, UClass* SearchAssetClass, TFunction<void()>&& Callback)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	
	AssetRegistryModule.Get().GetAssetsByClass(SearchAssetClass->GetFName(), InOutAssetData);
	TArray<FSoftObjectPath> AssetStream;
	
	CheckSearchPathContent(&InOutAssetData);
	CheckSearchNameContent(&InOutAssetData);
	
	for (auto ItAssetData = InOutAssetData.CreateConstIterator(); ItAssetData; ++ItAssetData)
	{
		FString AssetPathString = ItAssetData->ObjectPath.ToString();
		
		AssetStream.AddUnique(AssetPathString);
	}
	TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(AssetStream, FStreamableDelegate::CreateLambda(MoveTemp(Callback)));
	if (Handle.IsValid())
	{
		++LoadCounter;
	}
	return Handle;
}


/** check SearchPathTokens */
void FCMTMaterialSearcher::CheckSearchPathContent(TArray<FAssetData>* InList)
{
	for (int32 ListIndex = 0 ; ListIndex < InList->Num() ; ++ListIndex)
	{
		FString AssetPathString = (*InList)[ListIndex].ObjectPath.ToString();
		if (!FCUTUtility::StringMatchesSearchTokens(SearchPathTokens, AssetPathString))
		{
			InList->RemoveAt(ListIndex);
			--ListIndex;
		}
		else if ((SearchEngineContent == false) && (AssetPathString.Find("/Engine/") >= 0))
		{
			InList->RemoveAt(ListIndex);
			--ListIndex;
		}
	}
}

/** check SearchNameTokens */
void FCMTMaterialSearcher::CheckSearchNameContent(TArray<FAssetData>* InList)
{
	for (int32 ListIndex = 0 ; ListIndex < InList->Num() ; ++ListIndex)
	{
		FString AssetPathString = (*InList)[ListIndex].ObjectPath.ToString();
		if (!FCUTUtility::StringMatchesSearchTokens(SearchNameTokens, AssetPathString))
		{
			InList->RemoveAt(ListIndex);
			--ListIndex;
		}
	}
}



void FCMTMaterialSearcher::FinishAsyncLoad(const TArray<FAssetData>& InAssetData)
{
	// this function call from main thread.
	
	if (--LoadCounter <= 0)
	{
		// all load finished
		
		OnSearchEnd.Execute();
	}
}


