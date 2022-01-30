// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


/**
 * SCVTMPCViewer list result
 */
struct FCVTMPCViewerResult
{
	FName ParameterName;

	bool bIsScalar;

	float DefaultScalarValue;
	float ScalarValue;
	
	FLinearColor DefaultVectorValue;
	FLinearColor VectorValue;

	FCVTMPCViewerResult()
	: ParameterName(NAME_None),
	  bIsScalar(true),
	  DefaultScalarValue(0.0f),
	  ScalarValue(0.0f),
	  DefaultVectorValue(FLinearColor::Black),
	  VectorValue(FLinearColor::Black)
	{}
};
