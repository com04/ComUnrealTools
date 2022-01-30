// Copyright com04 All Rights Reserved.

#include "CVTMPCViewerResult.h"

#include "Materials/MaterialParameterCollection.h"



void FCVTMPCViewerResult::UpdateDefaultValue()
{
	if (Collection.IsValid())
	{
		if (bIsScalar)
		{
			const FCollectionScalarParameter* CollectionParameter = Collection->GetScalarParameterByName(ParameterName);
			if (CollectionParameter)
			{
				DefaultScalarValue = CollectionParameter->DefaultValue;
			}
		}
		else
		{
			const FCollectionVectorParameter* CollectionParameter = Collection->GetVectorParameterByName(ParameterName);
			if (CollectionParameter)
			{
				DefaultVectorValue = CollectionParameter->DefaultValue;
			}
		}
	}
	else
	{
		// todo
	}
}
/**
 * InCollectionInstanceから現在パラメーターを更新
 * @retval 変更が入ったか
 */
bool FCVTMPCViewerResult::UpdateParameterValue(UMaterialParameterCollectionInstance* InCollectionInstance)
{
	if (!InCollectionInstance)
	{
		return false;
	}
	bool bRetChanged = false;
	if (bIsScalar)
	{
		float ParameterValue = 0.0f;
		if (InCollectionInstance->GetScalarParameterValue(ParameterName, ParameterValue))
		{
			ScalarValue = ParameterValue;
			bRetChanged = !FMath::IsNearlyEqual(ScalarValue, DefaultScalarValue);
		}
	}
	else
	{
		FLinearColor ParameterValue = FLinearColor::Black;
		if (InCollectionInstance->GetVectorParameterValue(ParameterName, ParameterValue))
		{
			VectorValue = ParameterValue;
			bRetChanged = !VectorValue.Equals(DefaultVectorValue);
		}
	}
	return bRetChanged;
}


FText FCVTMPCViewerResult::GetScalarValueText(float ScalarParameter) const
{
	return FText::FromString(FString::Printf(TEXT("%f"), ScalarParameter));
}
FText FCVTMPCViewerResult::GetVectorValueText(FLinearColor VectorParameter) const
{
	return FText::FromString(FString::Printf(TEXT("R=%f, G=%f, B=%f, A=%f"),
			VectorParameter.R, VectorParameter.G, VectorParameter.B, VectorParameter.A));
}
FText FCVTMPCViewerResult::GetParameterDefaultValueText() const
{
	return (bIsScalar) ? GetScalarValueText(DefaultScalarValue) : GetVectorValueText(DefaultVectorValue);
}
FText FCVTMPCViewerResult::GetParameterValueText() const
{
	return (bIsScalar) ? GetScalarValueText(ScalarValue) : GetVectorValueText(VectorValue);
}
