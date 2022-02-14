// Copyright com04 All Rights Reserved.

#include "CUTAssetSearcher.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistryModule.h"




FCUTAssetSearcher::FCUTAssetSearcher()
: SearchPathTokens(),
  SearchNameTokens(),
  SearchEngineContent(true),
  LoadCounter(0),
  SearchData(),
  StreamableManager()
{
}

void FCUTAssetSearcher::SearchStart(
		const TArray<FString>& InSearchPathTokens,
		const TArray<FString>& InSearchNameTokens,
		bool InSearchEngineContent,
		const TArray<UClass*>& SearchAssetClasses)
{
	FCUTAssetSearcher* Self = this;
	SearchPathTokens = InSearchPathTokens;
	SearchNameTokens = InSearchNameTokens;
	SearchEngineContent = InSearchEngineContent;

	// search finish. clear variable.
	SearchData.Empty();
	
	// 2 type async load
	LoadCounter = 0;
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(AssetRegistryConstants::ModuleName);
	
	int32 SearchIndex = 0;
	for (UClass* SearchAssetClass : SearchAssetClasses)
	{
		FSearchData Data;
		
		if (SearchAssetClass)
		{
			AssetRegistryModule.Get().GetAssetsByClass(SearchAssetClass->GetFName(), Data.AssetData);
			TArray<FSoftObjectPath> AssetStream;
			
			CheckSearchPathContent(&Data.AssetData);
			CheckSearchNameContent(&Data.AssetData);
			
			for (auto ItAssetData = Data.AssetData.CreateConstIterator(); ItAssetData; ++ItAssetData)
			{
				FString AssetPathString = ItAssetData->ObjectPath.ToString();
				
				AssetStream.AddUnique(AssetPathString);
			}
			auto LoadEndFunc = [Self, SearchIndex]()
			{
				Self->FinishAsyncLoad(SearchIndex);
			};
			Data.StreamableHandle = StreamableManager.RequestAsyncLoad(AssetStream, FStreamableDelegate::CreateLambda(MoveTemp(LoadEndFunc)));
			if (Data.StreamableHandle.IsValid())
			{
				++LoadCounter;
			}
		}
		SearchData.Add(Data);
		++SearchIndex;
	}
	// no find
	if (LoadCounter == 0)
	{
		OnSearchEnd.Execute();
	}
}

const TArray<FAssetData>& FCUTAssetSearcher::GetAssets(int32 InIndex) const
{
	if (!SearchData.IsValidIndex(InIndex))
	{
		static const TArray<FAssetData> Dummy;
		return Dummy;
	}
	return SearchData[InIndex].AssetData;
}
int32 FCUTAssetSearcher::GetMaxAssetsIndex() const
{
	return SearchData.Num();
}

float FCUTAssetSearcher::GetProgress() const
{
	float Percent = 0.0f;
	float PercentMax = 0.0f;
	
	for (const FSearchData& Data : SearchData)
	{
		PercentMax += 1.0f;
		if (Data.StreamableHandle.IsValid())
		{
			Percent += Data.StreamableHandle->GetProgress();
		}
		else
		{
			Percent += 1.0f;
		}
	}
	return FMath::Max(Percent, 0.01f) / FMath::Max(PercentMax, 0.01f);
}

bool FCUTAssetSearcher::IsAsyncLoading() const
{
	return LoadCounter > 0;
}

void FCUTAssetSearcher::Reset()
{
	SearchPathTokens.Empty();
	SearchNameTokens.Empty();
	SearchData.Empty();
}

/** check SearchPathTokens */
void FCUTAssetSearcher::CheckSearchPathContent(TArray<FAssetData>* InList)
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
void FCUTAssetSearcher::CheckSearchNameContent(TArray<FAssetData>* InList)
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



void FCUTAssetSearcher::FinishAsyncLoad(int32 SearchDataIndex)
{
	// this function call from main thread.
	
	if (--LoadCounter <= 0)
	{
		// all load finished
		
		OnSearchEnd.Execute();
	}
}


