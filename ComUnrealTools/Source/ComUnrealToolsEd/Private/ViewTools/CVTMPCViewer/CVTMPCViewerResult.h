﻿// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CVTMPCViewerResult.generated.h"

class UMaterialParameterCollection;
class UMaterialParameterCollectionInstance;


/**
 * Config保存用
 */
USTRUCT()
struct FCVTMPCViewerConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	UMaterialParameterCollection* Collection;
	UPROPERTY(EditAnywhere)
	FName ParameterName;

	FCVTMPCViewerConfig()
	: Collection(nullptr),
	  ParameterName()
	{}
};

/**
 * SCVTMPCViewer list result
 */
struct FCVTMPCViewerResult
{
	TWeakObjectPtr<UMaterialParameterCollection> Collection;
	FName ParameterName;

	bool bIsScalar;

	float DefaultScalarValue;
	float ScalarValue;
	
	FLinearColor DefaultVectorValue;
	FLinearColor VectorValue;

	FCVTMPCViewerResult()
	: Collection(nullptr),
	  ParameterName(NAME_None),
	  bIsScalar(true),
	  DefaultScalarValue(0.0f),
	  ScalarValue(0.0f),
	  DefaultVectorValue(FLinearColor::Black),
	  VectorValue(FLinearColor::Black)
	{}
	
	// デフォルト値を更新
	void UpdateDefaultValue();
	/**
	 * InCollectionInstanceから現在パラメーターを更新
	 * @retval 変更が入ったか
	 */
	bool UpdateParameterValue(UMaterialParameterCollectionInstance* InCollectionInstance);
	
	FText GetScalarValueText(float ScalarParameter) const;
	FText GetVectorValueText(FLinearColor VectorParameter) const;
	FText GetParameterDefaultValueText() const;
	FText GetParameterValueText() const;
};
