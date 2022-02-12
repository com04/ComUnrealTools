// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UMaterialInterface;


/**
 * Stat list result
 */
struct FCMTStatListResult
{
	/** is UMaterialInstance? */
	bool	IsInstance;
	UMaterialInterface* Material;
	
	FText	Tooltip;
	
	// material information
	FString	Name;
	FText	BlendMode;
	uint32	Instruction;
	int32	TextureNum;
	uint32	TextureSize;
	int8	UseRenderAfterDOF;
	int8	UsePositionOffset;
	int8	UseDepthOffset;
	int8	UseRefraction;
	FString	Domain;
	FString	ShadingModel;
};
