// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


class UBodySetup;
class ULevelSequence;
class UMaterial;
class UMaterialFunction;
class UMaterialInstance;
class UMovieSceneFolder;
class UMovieSceneTrack;
class UTexture;
class UWorld;
struct FMovieSceneBindingProxy;


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

	/**
	 * プロパティを名前で検索する
	 * "<変数名>=<値>" か "<DisplayName>=<値>" でマッチング。
	 * Objectや構造体は.区切りでマッチング "<変数名>.<変数名>=<値>"
	 * @param InSearchStrings 検索する文字列
	 * @param InSearchObjectProperty 保持しているUObjectの中身も辿って検索するか
	 * @param InSearchDisplayName DisplayNameでも文字列マッチングを行うか
	 * @param InFunction マッチングした際に呼び出される関数
	 */
	static void SearchProperty(const UObject* InObject, const FProperty* InProperty, const TArray<FString>& InSearchStrings, bool InSearchObjectProperty, bool InSearchDisplayName, TFunction<void(const FProperty& InProperty, const FString& ValueString)> InFunction);
protected:
	struct FPropertyObject
	{
		const void* Object;
		bool bIsUObject;
		static FPropertyObject CreateVoidObject(const void* InObject) { return FPropertyObject(InObject, false); }
		static FPropertyObject CreateUObject(const UObject* InObject) { return FPropertyObject(InObject, true); }
	private:
		FPropertyObject(const void* InObject, bool bInIsUObject) : Object(InObject), bIsUObject(bInIsUObject) {}
	};
	static void SearchPropertyInternal(const FPropertyObject& InObject, const FProperty* InProperty, const TArray<FString>& InSearchStrings, bool InSearchObjectProperty, bool InSearchDisplayName, TFunction<void(const FProperty& InProperty, const FString& ValueString)> InFunction, const TArray<FString>& InParentString, bool bInStringParentOnly, const TArray<const void*>& InSearchedObjects);



public:
	/** wakeup editor */
	static void WakeupMaterialEditor(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction);
	
	/** wakeup editor */
	static void WakeupTextureEditor(UTexture* Texture);
	
	/** wakeup editor */
	static void WakeupSequencerEditor(ULevelSequence* LevelSequence);
	static void SelectSequencerEditorTracks(const TArray<UMovieSceneTrack*>& InTracks);
	static void SelectSequencerEditorTracks(const TArray<const UMovieSceneTrack*>& InTracks);
	static void SelectSequencerEditorFolders(const TArray<UMovieSceneFolder*>& InFolders);
	static void SelectSequencerEditorFolders(const TArray<const UMovieSceneFolder*>& InFolders);
	static void SelectSequencerEditorObjects(const TArray<FMovieSceneBindingProxy>& InObjectBindings);



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
