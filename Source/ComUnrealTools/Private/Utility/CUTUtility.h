// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


class UMaterial;
class UMaterialFunction;
class UMaterialInstance;
class UTexture;


/**
 * CMT Plugin utility
 */
class FCUTUtility
{
public:
	/**
	 * Get master material from Material instance
	 */
	static UMaterial* GetMasterMaterial(UMaterialInstance* MaterialInstance);

	/**
	 * Text box string split tokens
	 */
	static void SplitStringTokens(const FString& String, TArray<FString>* OutTokens);
	
	/** match one string */
	static bool StringMatchesSearchTokens(const TArray<FString>& Tokens, const FString& ComparisonString);
	
	/** M_Default.M_Default -> M_Default */
	static FString NormalizePathText(const FString& Path);

	
	/** icon brush name from material */
	static FName GetIconBrushName(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction);
	
	/** wakeup editor */
	static void WakeupMaterialEditor(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction);
	
	
	/** wakeup editor */
	static void WakeupTextureEditor(UTexture* Texture);
	
	
	
	/** copy clipboard */
	static bool ExportClipboard(const FString& TextData);

	/** Export text */
	static bool ExportTxt(const FString& TitleName, const FString& DefaultFileName, FString TextData, FString FileType=FString(TEXT("")));
	
};
