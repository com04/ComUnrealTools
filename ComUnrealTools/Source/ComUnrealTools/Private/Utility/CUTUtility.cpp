// Copyright com04 All Rights Reserved.


#include "CUTUtility.h"
#include "ComUnrealTools.h"
#include "ComUnrealToolsStyle.h"
#include "UnrealTools/CUTDeveloperSettings.h"

#include "DesktopPlatformModule.h"
#include "DrawDebugHelpers.h"
#include "Editor/EditorEngine.h"
#include "Engine/SCS_Node.h"
#include "HAL/FileManagerGeneric.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDesktopPlatform.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/ITextureEditorModule.h"
#include "LevelSequence.h"
#include "LevelSequenceEditorBlueprintLibrary.h"
#include "MaterialEditorModule.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"
#include "Modules/ModuleManager.h"
#include "MovieSceneBindingProxy.h"
#include "MovieSceneFolder.h"
#include "MovieSceneTrack.h"
#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/AggregateGeom.h"
#include "Subsystems/AssetEditorSubsystem.h"

/**
 * Get master material from Material instance
 */
UMaterial* FCUTUtility::GetMasterMaterial(UMaterialInstance* MaterialInstance)
{
	UMaterial* MasterMaterial = nullptr;
	// search master material
	if (MaterialInstance)
	{
		UMaterialInstance* ParentMaterialInstance = MaterialInstance;
		while (ParentMaterialInstance->Parent != nullptr)
		{
			UMaterialInstance* ParentInstance = Cast<UMaterialInstance>(ParentMaterialInstance->Parent);
			if (ParentInstance)
			{
				ParentMaterialInstance = ParentInstance;
			}
			else
			{
				MasterMaterial = Cast<UMaterial>(ParentMaterialInstance->Parent);
				break;
			}
		}
	}
	
	return MasterMaterial;
}

/**
 * Text box string split tokens
 */
void FCUTUtility::SplitStringTokens(const FString& String, TArray<FString>* OutTokens)
{
	// search text parse
	if (String.Contains("\"") && String.ParseIntoArray(*OutTokens, TEXT("\""), true) > 0)
	{
		for (auto &TokenIt : *OutTokens)
		{
			TokenIt = TokenIt.TrimQuotes();
			TokenIt = TokenIt.Replace(TEXT(" "), TEXT(""));
		}

		struct FRemoveMatchingStrings
		{
			bool operator()(const FString& RemovalCandidate) const
			{
				return RemovalCandidate.IsEmpty();
			}
		};
		OutTokens->RemoveAll(FRemoveMatchingStrings());
	}
	else
	{
		String.ParseIntoArray(*OutTokens, TEXT(" "), true);
	}
}

/** match one string */
bool FCUTUtility::StringMatchesSearchTokens(const TArray<FString>& Tokens, const FString& ComparisonString)
{
	bool bFoundAllTokens = true;
	for (auto TokIT(Tokens.CreateConstIterator()); TokIT; ++TokIT)
	{
		const FString& Token = *TokIT;
		if (!ComparisonString.Contains(Token))
		{
			bFoundAllTokens = false;
			break;
		}
	}
	return bFoundAllTokens;
}

/** M_Default.M_Default -> M_Default */
FString FCUTUtility::NormalizePathText(const FString& Path)
{
	FString Text = Path;
	Path.Split(TEXT("."), &Text, nullptr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	return Text;
}

/** icon brush name from material */
FName FCUTUtility::GetIconBrushName(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction)
{
	FName IconBrushName = NAME_None;
	// Material setup
	if (Material)
	{
		IconBrushName = FComUnrealToolsStyle::MaterialIconBrushName;
	}
	else if (MaterialInstance)
	{
		IconBrushName = FComUnrealToolsStyle::MaterialInstanceIconBrushName;
	}
	else if (MaterialFunction)
	{
		IconBrushName = FComUnrealToolsStyle::MaterialFunctionIconBrushName;
	}
	return IconBrushName;
}

/**
* プロパティを名前で検索する
* "<変数名>=<値>" か "<DisplayName>=<値>" でマッチング。
* Objectや構造体は.区切りでマッチング "<変数名>.<変数名>=<値>"
* @param InSearchStrings 検索する文字列
* @param InSearchObjectProperty 保持しているUObjectの中身も辿って検索するか
* @param InSearchDisplayName DisplayNameでも文字列マッチングを行うか
* @param InFunction マッチングした際に呼び出される関数
*/
void FCUTUtility::SearchProperty(const UObject* InObject, const FProperty* InProperty, const TArray<FString>& InSearchStrings, bool InSearchObjectProperty, bool InSearchDisplayName, TFunction<void(const FProperty& InProperty, const FString& ValueString)> InFunction)
{
	SearchPropertyInternal(FPropertyObject::CreateUObject(InObject), InProperty, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, TArray<FString>{FString(TEXT(""))}, false, {});
}
/**
 * @param InParentString 親から引き継いだ名前（構造体や配列の中身等）
 * @param bInStringParentOnly 親の文字列のみで表示するか（配列で[0]で止める場合等）
 * @param InSearchedObjects 見つかったオブジェクトリスト。入れ子を防ぐ
 */
void FCUTUtility::SearchPropertyInternal(const FPropertyObject& InObject, const FProperty* InProperty, const TArray<FString>& InSearchStrings, bool InSearchObjectProperty, bool InSearchDisplayName, TFunction<void(const FProperty& InProperty, const FString& ValueString)> InFunction, const TArray<FString>& InParentString, bool bInStringParentOnly, const TArray<const void*>& InSearchedObjects)
{
	if (InSearchedObjects.Find(InObject.Object) != INDEX_NONE)
	{
		return;
	}
	TArray<const void*> SearchedObjects = InSearchedObjects;
	SearchedObjects.Add(InObject.Object);

	FName ID = InProperty->GetID();
	FString NamePrivate = InProperty->NamePrivate.ToString();
	FString DisplayNameText = InProperty->GetDisplayNameText().ToString();
	if (UCUTDeveloperSettings::Get()->bUseDebugOutputNameInPropertySearch)
	{
		UE_LOG(LogTemp, Log, TEXT("SearchProperty ID=%s Name=%s DisplayName=%s"), *ID.ToString(), *NamePrivate, *DisplayNameText);
	}
	
	// パラメーター文字列が名前とマッチングするか
	const TArray<FString>& LocalSearchStrings = InSearchStrings;
	auto NameCheckFunc = [InProperty, LocalSearchStrings, InSearchDisplayName, InFunction, InParentString, bInStringParentOnly, NamePrivate, DisplayNameText](const FString& ValueString, const FString& DebugName) -> bool
	{
		FString DisplayNameBase = FString::Printf(TEXT("%s=%s"), (bInStringParentOnly ? TEXT("") : *DisplayNameText), *ValueString);
		FString PrivateNameBase = FString::Printf(TEXT("%s=%s"), (bInStringParentOnly ? TEXT("") : *NamePrivate), *ValueString);
		for (const FString& ParentString : InParentString)
		{
			FString DisplayNameString = FString::Printf(TEXT("%s%s"), *ParentString, *DisplayNameBase);
			FString PrivateNameString = FString::Printf(TEXT("%s%s"), *ParentString, *PrivateNameBase);
			// UE_LOG(LogTemp, Log, TEXT("    Check %s : %s / %s"), *DebugName, *DisplayNameString, *PrivateNameString);
			if (InSearchDisplayName && FCUTUtility::StringMatchesSearchTokens(LocalSearchStrings, DisplayNameString))
			{
				InFunction(*InProperty, DisplayNameString);
				return true;
			}
			else if (FCUTUtility::StringMatchesSearchTokens(LocalSearchStrings, PrivateNameString))
			{
				InFunction(*InProperty, PrivateNameString);
				return true;
			}
		}
		return false;
	};

	// Childへ引き継ぐ文字列。変数名とDisplayNameとで２つ用意する
	auto CreateParentStrings = [InSearchDisplayName, InParentString, NamePrivate, DisplayNameText](int32 InArrayIndex=-1, const FString& InAppendString=FString()) -> TArray<FString>
	{
		TArray<FString> RetStrings;
		
		FString ArrayString;
		if (InArrayIndex >= 0)
		{
			ArrayString = FString::Printf(TEXT("[%d]"), InArrayIndex);
		}
		RetStrings.Reserve(InParentString.Num() * 2);
		for (const FString& ParentString : InParentString)
		{
			if (InSearchDisplayName)
			{
				RetStrings.Add(FString::Printf(TEXT("%s%s%s%s"), *ParentString, *DisplayNameText, *ArrayString, *InAppendString));
			}
			RetStrings.Add(FString::Printf(TEXT("%s%s%s%s"), *ParentString, *NamePrivate, *ArrayString, *InAppendString));
		}
		return RetStrings;
	};
	
	// propertyが配列型か
	auto UseStoreType = [](const FProperty* InProperty) -> bool
	{
		if (InProperty->IsA(FStructProperty::StaticClass()) ||
				InProperty->IsA(FArrayProperty::StaticClass()) ||
				InProperty->IsA(FSetProperty::StaticClass()) ||
				InProperty->IsA(FMapProperty::StaticClass()))
		{
			return true;
		}
		return false;
	};
	
	// プロパティから値を引っ張る
	for (int32 ArrIndex = 0; ArrIndex < InProperty->ArrayDim; ++ArrIndex)
	{
		// from PythonizeValueImpl
		if (const FByteProperty* ByteProp = CastField<const FByteProperty>(InProperty))
		{
			const uint8* Value = InProperty->ContainerPtrToValuePtr<uint8>(InObject.Object);
			if (Value)
			{
				const UEnum* EnumType = ByteProp->Enum;
				// Editor上での列挙体もこちら
				if (EnumType)
				{
					const int32 EnumIndex = EnumType->GetIndexByValue(*Value);
					FString EnumIndexString = FString::Printf(TEXT("(%d)"), EnumIndex);
					FString EnumValStr = EnumType->GetDisplayNameTextByIndex(EnumIndex).ToString() + EnumIndexString;
					NameCheckFunc(EnumValStr, TEXT("ByteEnum"));
				}
				else
				{
					NameCheckFunc(FString::FromInt(*Value), TEXT("Byte"));
				}
			}
		}
		else if (const FEnumProperty* EnumProp = CastField<const FEnumProperty>(InProperty))
		{
			// C++宣言の列挙体
			const uint8* Value = InProperty->ContainerPtrToValuePtr<uint8>(InObject.Object);
			const UEnum* EnumType = EnumProp->GetEnum();
			if (Value && EnumType)
			{
				FString EnumIndexString = FString::Printf(TEXT("(%d)"), *Value);
				FString EnumValDisplayStr = EnumType->GetDisplayNameTextByIndex(*Value).ToString() + EnumIndexString;
				FString EnumValStr = EnumType->GetNameStringByIndex(*Value) + EnumIndexString;
				
				if (!InSearchDisplayName || !NameCheckFunc(EnumValDisplayStr, TEXT("Enum(DisplayName)")))
				{
					NameCheckFunc(EnumValStr, TEXT("Enum"));
				}
			}
		}
		else if (const FBoolProperty* BoolProp = CastField<const FBoolProperty>(InProperty))
		{
			const void* ValueAddress = InProperty->ContainerPtrToValuePtr<void>(InObject.Object);
			if (ValueAddress)
			{
				// bit fieldが有るのでこの関数でbit maskして取得する
				bool Value = BoolProp->GetPropertyValue(ValueAddress);
				NameCheckFunc((Value ? TEXT("True") : TEXT("False")), TEXT("Bool"));
			}
		}
		else if (const FIntProperty* IntProp = CastField<const FIntProperty>(InProperty))
		{
			const int32* Value = InProperty->ContainerPtrToValuePtr<int32>(InObject.Object);
			if (Value)
			{
				NameCheckFunc(FString::FromInt(*Value), TEXT("Int"));
			}
		}
		else if (const FInt64Property* Int64Prop = CastField<const FInt64Property>(InProperty))
		{
			const int64* Value = InProperty->ContainerPtrToValuePtr<int64>(InObject.Object);
			if (Value)
			{
				NameCheckFunc(FString::FromInt(*Value), TEXT("Int"));
			}
		}
		else if (const FFloatProperty* FloatProp = CastField<const FFloatProperty>(InProperty))
		{
			const float* Value = InProperty->ContainerPtrToValuePtr<float>(InObject.Object);
			if (Value)
			{
				NameCheckFunc(FString::SanitizeFloat(*Value), TEXT("Float"));
			}
		}
		else if (const FStrProperty* StrProp = CastField<const FStrProperty>(InProperty))
		{
			const FString* Value = InProperty->ContainerPtrToValuePtr<FString>(InObject.Object);
			if (Value)
			{
				NameCheckFunc(*Value, TEXT("String"));
			}
		}
		else if (const FNameProperty* NameProp = CastField<const FNameProperty>(InProperty))
		{
			const FName* Value = InProperty->ContainerPtrToValuePtr<FName>(InObject.Object);
			if (Value)
			{
				NameCheckFunc(*Value->ToString(), TEXT("Name"));
			}
		}
		else if (const FTextProperty* TextProp = CastField<const FTextProperty>(InProperty))
		{
			const FText* Value = InProperty->ContainerPtrToValuePtr<FText>(InObject.Object);
			if (Value)
			{
				NameCheckFunc(*Value->ToString(), TEXT("Text"));
			}
		}
		else if (const FObjectPropertyBase* ObjProp = CastField<const FObjectPropertyBase>(InProperty))
		{
			NameCheckFunc(TEXT(""), TEXT("Object"));
			
			if (InSearchObjectProperty)
			{
				if (ObjProp->PropertyClass)
				{
					const UObject* ObjectPtr = nullptr;
					// 保持しているコンポーネント
					if (const void* ValuePtr = ObjProp->ContainerPtrToValuePtr<const void>(InObject.Object))
					{
						ObjectPtr = ObjProp->GetObjectPropertyValue(ValuePtr);
					}
					// BPアセットの場合はコンポーネントをそのまま持っていないので、Classの中から引っ張る
					if (!ObjectPtr && InObject.bIsUObject)
					{
						// UObjectじゃない可能性は？
						UObject* InUObject = (UObject*)(InObject.Object);
						if (InUObject)
						{
							UBlueprintGeneratedClass* BPGC = Cast<UBlueprintGeneratedClass>(InUObject->GetClass());
							if (BPGC)
							{
								if (BPGC->SimpleConstructionScript)
								{
									const TArray<USCS_Node*>& ActorBlueprintNodes = BPGC->SimpleConstructionScript->GetAllNodes();
									for (USCS_Node* Node : ActorBlueprintNodes)
									{
										if (Node->ComponentClass == ObjProp->PropertyClass)
										{
											UObject* CompTemplate = Node->GetActualComponentTemplate(BPGC);
											if (CompTemplate)
											{
												// ここで生成されたComponent名には_GEN_VARIABLEがsuffixに付く
												FString TemplateName = CompTemplate->GetName().Replace(TEXT("_GEN_VARIABLE"), TEXT(""));
												if (NamePrivate.Compare(TemplateName) == 0)
												{
													ObjectPtr = CompTemplate;
													break;
												}
											}
										}
									}
								}
							}
						}
					}
					if (ObjectPtr)
					{
						for (TFieldIterator<FProperty> PropertyIterator(ObjProp->PropertyClass); PropertyIterator; ++PropertyIterator)
						{
							SearchPropertyInternal(FPropertyObject::CreateUObject(ObjectPtr), *PropertyIterator, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, CreateParentStrings(-1, TEXT(".")), false, SearchedObjects);
						}
					}
				}
			}
		}
		else if (const FStructProperty* StructProp = CastField<const FStructProperty>(InProperty))
		{
			const void* Value = InProperty->ContainerPtrToValuePtr<const void>(InObject.Object);
			if (Value && StructProp->Struct)
			{
				for (TFieldIterator<FProperty> PropertyIterator(StructProp->Struct); PropertyIterator; ++PropertyIterator)
				{
					SearchPropertyInternal(FPropertyObject::CreateVoidObject(Value), *PropertyIterator, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, CreateParentStrings(-1, TEXT(".")), false, SearchedObjects);
				}
			}
		}
		else if (const FArrayProperty* ArrayProperty = CastField<const FArrayProperty>(InProperty))
		{
			const FString Separator(UseStoreType(ArrayProperty->Inner) ? TEXT(".") : TEXT(""));
			
			FScriptArrayHelper ScriptHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(InObject.Object));
			for (int32 Index = 0 ; Index < ScriptHelper.Num() ; ++Index)
			{
				const void* Value = ScriptHelper.GetRawPtr(Index);
				SearchPropertyInternal(FPropertyObject::CreateVoidObject(Value), ArrayProperty->Inner, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, CreateParentStrings(Index, *Separator), true, SearchedObjects);
				
			}
		}
		else if (const FSetProperty* SetProperty = CastField<const FSetProperty>(InProperty))
		{
			const FString Separator(UseStoreType(SetProperty->ElementProp) ? TEXT(".") : TEXT(""));
			
			FScriptSetHelper ScriptHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(InObject.Object));
			for (int32 Index = 0 ; Index < ScriptHelper.Num() ; ++Index)
			{
				const void* IndexValuePtr = ScriptHelper.GetElementPtr(Index);
				SearchPropertyInternal(FPropertyObject::CreateVoidObject(IndexValuePtr), SetProperty->ElementProp, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, CreateParentStrings(Index, *Separator), true, SearchedObjects);
			}
		}
		else if (const FMapProperty* MapProperty = CastField<const FMapProperty>(InProperty))
		{
			FScriptMapHelper ScriptHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(InObject.Object));
			for (int32 Index = 0 ; Index < ScriptHelper.Num() ; ++Index)
			{
				if (ScriptHelper.IsValidIndex(Index))
				{
					const uint8* PairPtr = ScriptHelper.GetPairPtr(Index);
					SearchPropertyInternal(FPropertyObject::CreateVoidObject(PairPtr), MapProperty->KeyProp, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, CreateParentStrings(Index, TEXT(".Key")), true, SearchedObjects);
					SearchPropertyInternal(FPropertyObject::CreateVoidObject(PairPtr), MapProperty->ValueProp, InSearchStrings, InSearchObjectProperty, InSearchDisplayName, InFunction, CreateParentStrings(Index, TEXT(".Value")), true, SearchedObjects);
				}
			}
		}
		// else if (const FInterfaceProperty* InterfaceProp = CastField<const FInterfaceProperty>(InProperty))
		// else if (const FDelegateProperty* DelegateProp = CastField<const FDelegateProperty>(InProperty))
		// else if (const FMulticastDelegateProperty* MulticastDelegateProp = CastField<const FMulticastDelegateProperty>(InProperty))
	}
}

/** wakeup editor */
void FCUTUtility::WakeupMaterialEditor(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction)
{
	// wakeup MaterialEditor
	IMaterialEditorModule* MaterialEditorModule = &FModuleManager::LoadModuleChecked<IMaterialEditorModule>("MaterialEditor");
	if (MaterialEditorModule)
	{
		EToolkitMode::Type Mode = EToolkitMode::Standalone;
		
		if (Material)
		{
			if(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Material, true) == nullptr)
			{
				MaterialEditorModule->CreateMaterialEditor(Mode, TSharedPtr<IToolkitHost>(), Material);
			}
		}
		else if (MaterialInstance)
		{
			if(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(MaterialInstance, true) == nullptr)
			{
				MaterialEditorModule->CreateMaterialInstanceEditor(Mode, TSharedPtr<IToolkitHost>(), MaterialInstance);
			}
		}
		else if (MaterialFunction)
		{
			if(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(MaterialFunction, true) == nullptr)
			{
				MaterialEditorModule->CreateMaterialEditor(Mode, TSharedPtr<IToolkitHost>(), MaterialFunction);
			}
		}
	}
}


/** wakeup editor */
void FCUTUtility::WakeupTextureEditor(UTexture* Texture)
{
	ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
	if (TextureEditorModule)
	{
		EToolkitMode::Type Mode = EToolkitMode::Standalone;
		
		TextureEditorModule->CreateTextureEditor(Mode, TSharedPtr<IToolkitHost>(), Texture);
	}
}

/** wakeup editor */
void FCUTUtility::WakeupSequencerEditor(ULevelSequence* LevelSequence)
{
	ULevelSequenceEditorBlueprintLibrary::OpenLevelSequence(LevelSequence);
}
void FCUTUtility::SelectSequencerEditorTracks(const TArray<UMovieSceneTrack*>& InTracks)
{
	ULevelSequenceEditorBlueprintLibrary::SelectTracks(InTracks);
}
void FCUTUtility::SelectSequencerEditorTracks(const TArray<const UMovieSceneTrack*>& InTracks)
{
	TArray<UMovieSceneTrack*> NewTracks;
	NewTracks.SetNumUninitialized(InTracks.Num());
	for (TArray<const UMovieSceneTrack*>::TConstIterator TrackIt(InTracks) ; TrackIt ; ++TrackIt)
	{
		NewTracks[TrackIt.GetIndex()] = const_cast<UMovieSceneTrack*>(*TrackIt);
	}
	SelectSequencerEditorTracks(NewTracks);
}
void FCUTUtility::SelectSequencerEditorFolders(const TArray<UMovieSceneFolder*>& InFolders)
{
	ULevelSequenceEditorBlueprintLibrary::SelectFolders(InFolders);
}
void FCUTUtility::SelectSequencerEditorFolders(const TArray<const UMovieSceneFolder*>& InFolders)
{
	TArray<UMovieSceneFolder*> NewFolders;
	NewFolders.SetNumUninitialized(InFolders.Num());
	for (TArray<const UMovieSceneFolder*>::TConstIterator TrackIt(InFolders) ; TrackIt ; ++TrackIt)
	{
		NewFolders[TrackIt.GetIndex()] = const_cast<UMovieSceneFolder*>(*TrackIt);
	}
	SelectSequencerEditorFolders(NewFolders);
}
void FCUTUtility::SelectSequencerEditorObjects(const TArray<FMovieSceneBindingProxy>& InObjectBindings)
{
	ULevelSequenceEditorBlueprintLibrary::SelectBindings(InObjectBindings);
}

/** copy clipboard */
bool FCUTUtility::ExportClipboard(const FString& TextData)
{
	FPlatformApplicationMisc::ClipboardCopy(*TextData);
	return true;
}


/** Export csv */
bool FCUTUtility::ExportTxt(const FString& TitleName, const FString& DefaultFileName, FString TextData, FString FileType)
{
	TArray<FString> OpenFilenames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	bool bOpened = false;
	
	if (DesktopPlatform)
	{
		void* ParentWindowWindowHandle = NULL;

		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
		if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid())
		{
			ParentWindowWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}
		const FString Title = FString::Printf(TEXT("com Unreal Tools: %s"), *TitleName);
		
		bOpened = DesktopPlatform->SaveFileDialog(
				ParentWindowWindowHandle,
				*Title,
				FPaths::ProjectDir(),
				DefaultFileName,
				*FileType,
				EFileDialogFlags::None,
				OpenFilenames
				);
	}
	
	
	bool Export = false;
	if (bOpened && OpenFilenames.Num() > 0)
	{
		FArchive* SaveFile = FFileManagerGeneric::Get().CreateFileWriter(*OpenFilenames[0]);
		if (SaveFile)
		{
			UTF8CHAR UTF8BOM[] = {(UTF8CHAR)0xEF, (UTF8CHAR)0xBB, (UTF8CHAR)0xBF};
			SaveFile->Serialize(&UTF8BOM, UE_ARRAY_COUNT(UTF8BOM) * sizeof(UTF8CHAR));
			FTCHARToUTF8 UTF8Text(*TextData);
			SaveFile->Serialize((UTF8CHAR*)UTF8Text.Get(), UTF8Text.Length() * sizeof(UTF8CHAR));
			SaveFile->Close();
			delete SaveFile;
			SaveFile = NULL;
			Export = true;
		}
	}
	
	return Export;
}

/** Draw a debug sphere */
void FCUTUtility::DrawDebugSloidSphereAdvanced(const UWorld* InWorld, FVector const& XAxis, FVector const& YAxis, FVector const& ZAxis, FVector const& Center, float Radius, int32 Segments, float AngleRateY, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority)
{
	// sample from /Engine/Source/Runtime/Engine/Private/DrawDebugHelpers.cpp - DrawDebugSphere
	
	// Need at least 4 segments
	Segments = FMath::Max(Segments, 4);
	int32 NumSegmentsY = static_cast<int32>(static_cast<float>(Segments) * AngleRateY);

	FVector Vertex1, Vertex2, Vertex3, Vertex4;
	const float AngleInc = 2.f * PI / float(Segments);
	const float AngleYInc = 2.f * PI / float(Segments) * AngleRateY;
	float Latitude = AngleYInc;
	int32 NumSegmentsX;
	float Longitude;
	float SinY1 = 0.0f, CosY1 = 1.0f, SinY2, CosY2;
	float SinX, CosX;
	int32 Index = 0;

	TArray<FVector> VertexData;
	TArray<int32> IndexData;

	VertexData.Reserve(NumSegmentsY * Segments * 2 + NumSegmentsY * 2);
	IndexData.Reserve(NumSegmentsY * Segments * 6);
	while (NumSegmentsY--)
	{
		SinY2 = FMath::Sin(Latitude);
		CosY2 = FMath::Cos(Latitude);

		Vertex1 = (XAxis * SinY1 + ZAxis * CosY1) * Radius + Center;
		Vertex3 = (XAxis * SinY2 + ZAxis * CosY2) * Radius + Center;
		Longitude = AngleInc;

		VertexData.Add(Vertex1);
		VertexData.Add(Vertex3);

		NumSegmentsX = Segments;
		while (NumSegmentsX--)
		{
			SinX = FMath::Sin(Longitude);
			CosX = FMath::Cos(Longitude);

			Vertex2 = (XAxis * (CosX * SinY1) + YAxis * (SinX * SinY1) + ZAxis * CosY1) * Radius + Center;
			Vertex4 = (XAxis * (CosX * SinY2) + YAxis * (SinX * SinY2) + ZAxis * CosY2) * Radius + Center;

			VertexData.Add(Vertex2);
			VertexData.Add(Vertex4);
			IndexData.Add(Index+0);
			IndexData.Add(Index+1);
			IndexData.Add(Index+2);
			IndexData.Add(Index+2);
			IndexData.Add(Index+3);
			IndexData.Add(Index+1);

			Vertex1 = Vertex2;
			Vertex3 = Vertex4;
			Longitude += AngleInc;
			Index += 2;
		}
		SinY1 = SinY2;
		CosY1 = CosY2;
		Latitude += AngleYInc;
		Index += 2;
	}
	
	::DrawDebugMesh(InWorld, VertexData, IndexData, Color, bPersistentLines, LifeTime, DepthPriority);
}
/** Draw a debug sphere */
void FCUTUtility::DrawDebugSloidSphere(const UWorld* InWorld, FVector const& Center, float Radius, int32 Segments, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority)
{
	DrawDebugSloidSphereAdvanced(InWorld, FVector(1.0f, 0.0f, 0.0f), FVector(0.0f, 1.0f, 0.0f), FVector(0.0f, 0.0f, 1.0f), Center, Radius, Segments, 1.0f, Color, bPersistentLines, LifeTime, DepthPriority);
}

/** Draw a debug capsule */
void FCUTUtility::DrawDebugSloidCapsule(const UWorld* InWorld, FVector const& Center, float HalfHeight, float Radius, const FQuat& Rotation, FColor const& Color, bool bPersistentLines, float LifeTime, uint8 DepthPriority)
{
	// sample from /Engine/Source/Runtime/Engine/Private/DrawDebugHelpers.cpp - DrawDebugCapsule

	const int32 DrawCollisionSides = 16;

	FVector Origin = Center;
	FMatrix Axes = FQuatRotationTranslationMatrix(Rotation, FVector::ZeroVector);
	FVector XAxis = Axes.GetScaledAxis( EAxis::X );
	FVector YAxis = Axes.GetScaledAxis( EAxis::Y );
	FVector ZAxis = Axes.GetScaledAxis( EAxis::Z ); 

	float HalfAxis = FMath::Max<float>(HalfHeight - Radius, 1.f);
	FVector TopEnd = Origin + HalfAxis*ZAxis;
	FVector BottomEnd = Origin - HalfAxis*ZAxis;

	// 半球部分
	const int32 HalfSphereSegment = 6;
	DrawDebugSloidSphereAdvanced(InWorld, XAxis, YAxis,  ZAxis, TopEnd,    Radius, HalfSphereSegment, 0.5f, Color, bPersistentLines, LifeTime, DepthPriority);
	DrawDebugSloidSphereAdvanced(InWorld, XAxis, YAxis, -ZAxis, BottomEnd, Radius, HalfSphereSegment, 0.5f, Color, bPersistentLines, LifeTime, DepthPriority);

	// 胴体部分
	const int32 Segments = 6;
	const float AngleInc = 2.f * PI / float(Segments);
	
	float Angle = 0.0f;
	int32 SegmentX = Segments;
	int32 Index = 0;
	
	TArray<FVector> VertexData;
	TArray<int32> IndexData;
	VertexData.Reserve(Segments * 2 + 2);
	IndexData.Reserve(Segments * 6);

	float SinX = FMath::Sin(Angle);
	float CosX = FMath::Cos(Angle);
	FVector VertexX = FVector(CosX * XAxis * Radius + SinX * YAxis * Radius);
	VertexData.Add(TopEnd + VertexX);
	VertexData.Add(BottomEnd + VertexX);
	Angle += AngleInc;
	while (SegmentX--)
	{
		SinX = FMath::Sin(Angle);
		CosX = FMath::Cos(Angle);
		VertexX = FVector(CosX * XAxis * Radius + SinX * YAxis * Radius);
		VertexData.Add(TopEnd + VertexX);
		VertexData.Add(BottomEnd + VertexX);
		
		IndexData.Add(Index+0);
		IndexData.Add(Index+1);
		IndexData.Add(Index+2);
		IndexData.Add(Index+2);
		IndexData.Add(Index+3);
		IndexData.Add(Index+1);
		
		Angle += AngleInc;
		Index += 2;
	}
	
	::DrawDebugMesh(InWorld, VertexData, IndexData, Color, bPersistentLines, LifeTime, DepthPriority);
}

/** Draw BodySetup Geometry */
void FCUTUtility::DrawBodySetup(UWorld* World, const UBodySetup* BodySetup, const FTransform& Transform, FColor LineColor, FColor SolidColor, float Duration, float Thickness)
{
	if (!World || !IsValid(BodySetup))
	{
		return;
	}
	const FKAggregateGeom& AggGeom = BodySetup->AggGeom;
	const FQuat TransformQuat = Transform.GetRotation();
	const FVector TransformScale = Transform.GetScale3D();
	const bool bDrawLine = (LineColor.A > 0.0f) && (Thickness > 0.0f);
	const bool bDrawSolid = (SolidColor.A > 0.0f);
	
	/*
	UE_LOG(LogTemp, Log, TEXT("VolumeRenderer %s Sphere=%d Box=%d Sphyl=%d Convex=%d Tapered=%d"),
			*TargetActor->GetName(),
			AggGeom.SphereElems.Num(), AggGeom.BoxElems.Num(), AggGeom.SphylElems.Num(), AggGeom.ConvexElems.Num(), AggGeom.TaperedCapsuleElems.Num());
	*/
	for (const FKSphereElem& Elem :  AggGeom.SphereElems)
	{
		const FVector Center = Transform.TransformPosition(Elem.Center);
		const float Radius = Elem.Radius * TransformScale.GetMin();
		if (bDrawLine)
		{
			::DrawDebugSphere(World, Center, Radius, 10, LineColor, false, Duration, 0, Thickness);
		}
		if (bDrawSolid)
		{
			DrawDebugSloidSphere(World, Center, Radius, 10, SolidColor, false, Duration, 0);
		}
	}
	for (const FKBoxElem& Elem :  AggGeom.BoxElems)
	{
		const FVector Center = Transform.TransformPosition(Elem.Center);
		const FVector Extent = FVector(Elem.X, Elem.Y, Elem.Z) * 0.5f * TransformScale;
		const FQuat Rotation = TransformQuat * Elem.Rotation.Quaternion();
		if (bDrawLine)
		{
			::DrawDebugBox(World, Center, Extent, Rotation, LineColor, false, Duration, 0, Thickness);
		}
		if (bDrawSolid)
		{
			::DrawDebugSolidBox(World, Center, Extent, Rotation, SolidColor, false, Duration, 0);
		}
	}
	for (const FKSphylElem& Elem :  AggGeom.SphylElems)
	{
		const FVector Center = Transform.TransformPosition(Elem.Center);
		// from InvalidateOrUpdateCapsuleBodySetup 
		const float HalfLength = (Elem.Length * 0.5f + Elem.Radius) * TransformScale.Z;
		const float Radius = Elem.Radius * FMath::Max(TransformScale.X, TransformScale.Y);
		const FQuat Rotation = TransformQuat * Elem.Rotation.Quaternion();
		if (bDrawLine)
		{
			::DrawDebugCapsule(World, Center, HalfLength, Radius, Rotation, LineColor, false, Duration, 0, Thickness);
		}
		if (bDrawSolid)
		{
			DrawDebugSloidCapsule(World, Center, HalfLength, Radius, Rotation, SolidColor, false, Duration, 0);
		}
	}
	for (const FKConvexElem& Elem :  AggGeom.ConvexElems)
	{
		TArray<FVector> VertexData(Elem.VertexData);
		for (FVector& Vertex : VertexData)
		{
			Vertex = Transform.TransformPosition(Vertex);
		}
		if (bDrawSolid)
		{
			::DrawDebugMesh(World, VertexData, Elem.IndexData, SolidColor, false, Duration, 0);
		}
		if (bDrawLine)
		{
			TArray<TPair<int32, int32>> LineList;
			LineList.Reserve(Elem.IndexData.Num());
			for (int32 Index = 0 ; Index < Elem.IndexData.Num() / 3 ; ++Index)
			{
				const int32 VertexIndex0 = Elem.IndexData[Index*3+0];
				const int32 VertexIndex1 = Elem.IndexData[Index*3+1];
				const int32 VertexIndex2 = Elem.IndexData[Index*3+2];
				LineList.AddUnique(TPair<int32, int32>(FMath::Min(VertexIndex0, VertexIndex1), FMath::Max(VertexIndex0, VertexIndex1)));
				LineList.AddUnique(TPair<int32, int32>(FMath::Min(VertexIndex0, VertexIndex2), FMath::Max(VertexIndex0, VertexIndex2)));
				LineList.AddUnique(TPair<int32, int32>(FMath::Min(VertexIndex1, VertexIndex2), FMath::Max(VertexIndex1, VertexIndex2)));
			}
			for (const TPair<int32, int32>& Line : LineList)
			{
				::DrawDebugLine(World, VertexData[Line.Key], VertexData[Line.Value], LineColor, false, Duration, 0, Thickness);
			}
		}
	}
	// AggGeom.TaperedCapsuleElems
}
