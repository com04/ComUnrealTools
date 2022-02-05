// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


class UBodySetup;
class UMaterial;
class UMaterialFunction;
class UMaterialInstance;
class UTexture;
class UWorld;


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

	/** Draw a debug sphere */
	static void DrawDebugSloidSphereAdvanced(const UWorld* InWorld, FVector const& XAxis, FVector const& YAxis, FVector const& ZAxis, FVector const& Center, float Radius, int32 Segments, float AngleRateY, FColor const& Color, bool bPersistentLines = false, float LifeTime=-1.f, uint8 DepthPriority = 0);

	/** Draw a debug sphere */
	static void DrawDebugSloidSphere(const UWorld* InWorld, FVector const& Center, float Radius, int32 Segments, FColor const& Color, bool bPersistentLines = false, float LifeTime=-1.f, uint8 DepthPriority = 0);

	/** Draw a debug capsule */
	static void DrawDebugSloidCapsule(const UWorld* InWorld, FVector const& Center, float HalfHeight, float Radius, const FQuat& Rotation, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0);

	/** Draw BodySetup Geometry */
	static void DrawBodySetup(UWorld* World, const UBodySetup* BodySetup, const FTransform& Transform, FColor LineColor, FColor SolidColor, float Duration, float Thickness);
};
