// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * SCVTRenderTargetViewer list result
 */
struct FCVTRenderTargetViewerResult
{
	FString Name;
	FString Format;
	// RenderTargetサイズ
	uint32 SizeX;
	uint32 SizeY;
	// 破棄されたか
	bool bDestroy;
	
	TWeakObjectPtr<UTextureRenderTarget2D> RenderTarget;

	FCVTRenderTargetViewerResult()
	: Name(),
	  Format(),
	  SizeX(1),
	  SizeY(1),
	  bDestroy(false),
	  RenderTarget()
	{}
	
};
