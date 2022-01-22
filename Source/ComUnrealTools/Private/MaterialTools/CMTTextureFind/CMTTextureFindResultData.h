// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "Templates/SharedPointer.h"

class UMaterialFunction;
class UMaterialInterface;

/**
 * TextureFind result data - function
 */

class FCMTTextureFindResultMaterialData : public TSharedFromThis<FCMTTextureFindResultMaterialData>
{
public:
	
	UMaterialInterface*	Material;
	UMaterialFunction*	Function;
	FString				MaterialName;
	
	TSharedPtr<FCMTTextureFindResultMaterialData>	Parent;
	TSharedPtr<FCMTTextureFindResultMaterialData>	Child;
	
	FCMTTextureFindResultMaterialData()
	: Material(nullptr),
	  Function(nullptr),
	  MaterialName(),
	  Parent(),
	  Child()
	{}
};


/**
 * TextureFind result data
 */
class FCMTTextureFindResultData
{
public:
	
	UTexture* Texture;
	FString TexturePath;
	
	UPROPERTY()
	TArray<TSharedPtr<FCMTTextureFindResultMaterialData>>	Referenced;
	
	FCMTTextureFindResultData()
	: Texture(nullptr),
	  TexturePath(),
	  Referenced()
	{}
};
