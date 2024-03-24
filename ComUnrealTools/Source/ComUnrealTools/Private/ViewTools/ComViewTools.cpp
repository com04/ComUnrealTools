// Copyright com04 All Rights Reserved.

#include "ViewTools/ComViewTools.h"
#include "Utility/CUTUtility.h"

#include "Components/BrushComponent.h"
#include "Components/ShapeComponent.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "PhysicsEngine/BodySetup.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "EditorViewportClient.h"
#endif

static TAutoConsoleVariable<float> CVarCVTVolumeRenderLineThickness(
		TEXT("CUT.VolumeRender.LineThickness"),
		10.0f,
		TEXT("Set the line thickness of VolumeRender. 'CUT.VolumeRender.LineThickness <Thickness>'"),
		FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* InVariable)
		{
			FComViewTools::Get().SetVolumeRendererLineThickness(InVariable->GetFloat());
		}),
		ECVF_Scalability);
static TAutoConsoleVariable<float> CVarCVTVolumeRenderOneShotDuration(
		TEXT("CUT.VolumeRender.OneShotDuration"),
		5.0f,
		TEXT("Set the oneshot duration of VolumeRender. 'CUT.VolumeRender.OneShotDuration <Duration>'"),
		FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* InVariable)
		{
			FComViewTools::Get().SetVolumeRendererOneShotDuration(InVariable->GetFloat());
		}),
		ECVF_Scalability);
static TAutoConsoleVariable<float> CVarCVTVolumeRenderRenderDistance(
		TEXT("CUT.VolumeRender.RenderDistance"),
		0.0f,
		TEXT("Set the render distance of VolumeRender. 'CUT.VolumeRender.RenderDistance <Distance>'"),
		FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* InVariable)
		{
			FComViewTools::Get().SetVolumeRendererRenderDistance(InVariable->GetFloat());
		}),
		ECVF_Scalability);
static TAutoConsoleVariable<float> CVarCVTVolumeRenderSolidAlpha(
		TEXT("CUT.VolumeRender.SolidAlpha"),
		0.5f,
		TEXT("Set the solid alpha of VolumeRender. 'CUT.VolumeRender.SolidAlpha <Alpha>'"),
		FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* InVariable)
		{
			FComViewTools::Get().SetVolumeRendererSolidAlpha(InVariable->GetFloat());
		}),
		ECVF_Scalability);

static FAutoConsoleCommand CVarCVTVolumeRenderAddItem(
		TEXT("CUT.VolumeRender.Add"),
		TEXT("Add VolumeRender drawing. 'CUT.VolumeRender.Add <Always or OneShot> <Class=PartialMatchName> <Color=>'"),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			bool bAlways = true;
			FLinearColor Color(0.8f, 0.2f, 0.2f);
			
			for (const FString& Arg : Args)
			{
				if (Arg.Compare(TEXT("Always"), ESearchCase::IgnoreCase) == 0)
				{
					bAlways = true;
				}
				else if (Arg.Compare(TEXT("OneShot"), ESearchCase::IgnoreCase) == 0)
				{
					bAlways = false;
				}
				else if (Arg.StartsWith(TEXT("Color=")))
				{
					FString ColorValueString = Arg.Mid(6);
					FLinearColor TmpColor;
					if (TmpColor.InitFromString(ColorValueString))
					{
						// match "(R=0,G=1,B=2)"
						Color = TmpColor;
					}
					else
					{
						// match "(0,1,2)"
						ColorValueString.ReplaceInline(TEXT("("), TEXT(" "));
						ColorValueString.ReplaceInline(TEXT(")"), TEXT(" "));
						TArray<FString> ValueArray;
						if (ColorValueString.ParseIntoArray(ValueArray, TEXT(",")) >= 3)
						{
							bool bSuccess = true;
							for (int32 Index = 0 ; Index < 3 ; ++Index)
							{
								float Value = 0.0f;
								if (!LexTryParseString(Value, *ValueArray[Index]))
								{
									bSuccess = false;
									break;
								}
								TmpColor.Component(Index) = Value;
							}
							if (bSuccess)
							{
								Color = TmpColor;
							}
						}
					}
				}
			}
			
			for (const FString& Arg : Args)
			{
				if (Arg.StartsWith(TEXT("Class=")))
				{
					FString ClassString = Arg.Mid(6);
					FComViewTools::Get().AddVolumeRendererItemFromClassName(ClassString, bAlways, Color);
				}
			}
		}));

static FAutoConsoleCommand CVarCVTVolumeRenderRemoveItem(
		TEXT("CUT.VolumeRender.Remove"),
		TEXT("Remove VolumeRender drawing. 'CUT.VolumeRender.Remove <Class=PartialMatchName>'"),
		FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
		{
			for (const FString& Arg : Args)
			{
				if (Arg.StartsWith(TEXT("Class=")))
				{
					FString ClassString = Arg.Mid(6);
					FComViewTools::Get().RemoveVolumeRendererItemFromClassName(ClassString);
				}
			}
		}));

FComViewTools& FComViewTools::Get()
{
	return *GetInstance(true);
}
bool FComViewTools::IsInitialized()
{
	return GetInstance(false) != nullptr;
}
FComViewTools* FComViewTools::GetInstance(bool bInCreate)
{
	static FComViewTools* Instance = nullptr;
	if (bInCreate && !Instance)
	{
		Instance = new FComViewTools;
	}
	return Instance;
}


FComViewTools::FComViewTools()
: VolumeRendererLineThickness(50.0f),
  VolumeRendererOneShotDuration(5.0f),
  VolumeRendererRenderDistance(0.0f),
  VolumeRendererSolidAlpha(0.5f)
{
}

FComViewTools::~FComViewTools()
{
}

bool FComViewTools::IsTickableWhenPaused() const
{
	return true;
}
bool FComViewTools::IsTickableInEditor() const
{
	return true;
}
bool FComViewTools::IsTickable() const
{
	return true;
}
void FComViewTools::Tick(float DeltaTime)
{
	UWorld* World = nullptr;
	// from FConsoleCommandExecutor::ExecInternal
#if WITH_EDITOR
	// PIEでの再生中World
	if (GEditor && GEditor->PlayWorld)
	{
		World = GEditor->PlayWorld;
	}
#endif
	if (World == nullptr)
	{
		ULocalPlayer* Player = GEngine->GetDebugLocalPlayer();
		if (Player)
		{
			World = Player->GetWorld();
		}
	}

#if WITH_EDITOR
	if (World == nullptr)
	{
		World = GEditor->GetEditorWorldContext().World();
	}
#endif
	
	// oneshot
	for (const FCVTVolumeRendererItemInfo& Item : VolumeRendererItemsOneshot)
	{
		RenderItem(World, Item, VolumeRendererOneShotDuration);
	}
	VolumeRendererItemsOneshot.Empty();
	
	// always
	for (const FCVTVolumeRendererItemInfo& Item : VolumeRendererItemsAlways)
	{
		RenderItem(World, Item, 0.0f);
	}
}
TStatId FComViewTools::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(FComViewTools, STATGROUP_Tickables);
}


void FComViewTools::AddVolumeRendererItemAlways(const FCVTVolumeRendererItemInfo& InInfo)
{
	VolumeRendererItemsAlways.AddUnique(InInfo);
}
void FComViewTools::RemoveVolumeRendererItemAlways(const FCVTVolumeRendererItemInfo& InInfo)
{
	VolumeRendererItemsAlways.RemoveSingle(InInfo);
}
void FComViewTools::AddVolumeRendererItemOneshot(const FCVTVolumeRendererItemInfo& InInfo)
{
	VolumeRendererItemsOneshot.AddUnique(InInfo);
}
void FComViewTools::RemoveVolumeRendererItemAll()
{
	VolumeRendererItemsAlways.Empty();
	VolumeRendererItemsOneshot.Empty();
}
void FComViewTools::AddVolumeRendererItemFromClassName(const FString& InClassName, bool bInAlways, const FLinearColor& InColor)
{
	for (TObjectIterator<UClass> ClassIterator ; ClassIterator ; ++ClassIterator)
	{
		if (!ClassIterator->GetName().Contains(InClassName))
		{
			continue;
		}
		FCVTVolumeRendererItemInfo NewInfo;
		NewInfo.Class = *ClassIterator;
		NewInfo.DisplayColor = InColor;
		if (bInAlways)
		{
			AddVolumeRendererItemAlways(NewInfo);
		}
		else
		{
			AddVolumeRendererItemOneshot(NewInfo);
		}
		UE_LOG(LogComUnrealTools, Log, TEXT("VolumeRenderer added class %s"), *ClassIterator->GetName());
	}
}
void FComViewTools::RemoveVolumeRendererItemFromClassName(const FString& InClassName)
{
	for (auto ItemIterator = VolumeRendererItemsAlways.CreateIterator() ; ItemIterator ; ++ItemIterator)
	{
		UClass* Class = ItemIterator->Class.Get();
		if (!Class)
		{
			continue;
		}
		if (!Class->GetName().Contains(InClassName))
		{
			continue;
		}
		UE_LOG(LogComUnrealTools, Log, TEXT("VolumeRenderer remove class %s"), *Class->GetName());
		ItemIterator.RemoveCurrent();
	}
}

void FComViewTools::SetVolumeRendererLineThickness(float InValue)
{
	VolumeRendererLineThickness = InValue;
	OnChangedParameters.Broadcast();
}
float FComViewTools::GetVolumeRendererLineThickness() const
{
	return VolumeRendererLineThickness;
}
void FComViewTools::SetVolumeRendererOneShotDuration(float InValue)
{
	VolumeRendererOneShotDuration = InValue;
	OnChangedParameters.Broadcast();
}
float FComViewTools::GetVolumeRendererOneShotDuration() const
{
	return VolumeRendererOneShotDuration;
}
void FComViewTools::SetVolumeRendererRenderDistance(float InValue)
{
	VolumeRendererRenderDistance = InValue;
	OnChangedParameters.Broadcast();
}
float FComViewTools::GetVolumeRendererRenderDistance() const
{
	return VolumeRendererRenderDistance;
}
void FComViewTools::SetVolumeRendererSolidAlpha(float InValue)
{
	VolumeRendererSolidAlpha = InValue;
	OnChangedParameters.Broadcast();
}
float FComViewTools::GetVolumeRendererSolidAlpha() const
{
	return VolumeRendererSolidAlpha;
}
FDelegateHandle FComViewTools::AddOnChangedParametersDelegate(FSimpleMulticastDelegate::FDelegate InDelegate)
{
	return OnChangedParameters.Add(InDelegate);
}
void FComViewTools::RemoveOnChangedParametersDelegate(FDelegateHandle InDelegateHandle)
{
	OnChangedParameters.Remove(InDelegateHandle);
}

void FComViewTools::RenderItem(UWorld* InWorld, const FCVTVolumeRendererItemInfo& InInfo, float InDuration) const
{
	if (!InInfo.Class)
	{
		return;
	}
	
	const FColor LineColor = InInfo.DisplayColor.ToFColor(true);
	const FColor SolidColor = FColor(LineColor.R, LineColor.G, LineColor.B, 255 * VolumeRendererSolidAlpha);
	for(TActorIterator<AActor> ActorIt(InWorld, InInfo.Class) ; ActorIt ; ++ActorIt)
	{
		AActor* TargetActor = *ActorIt;
		struct FGeomtryInfo
		{
			const UBodySetup* BodySetup = nullptr;
			FTransform Transform = FTransform::Identity;
			
			FGeomtryInfo(const UBodySetup* InBodySetup, const FTransform& InTransform)
			: BodySetup(InBodySetup),
			  Transform(InTransform)
			{}
		};
		TArray<FGeomtryInfo, TInlineAllocator<32>> GeometryInfos;
		
		// 距離カリング
		if (VolumeRendererRenderDistance > 0.01f)
		{
			const FVector ActorLocation = TargetActor->GetActorLocation();
			bool bNeedDraw = false;
#if WITH_EDITOR
			UEditorEngine* EditorEngine = (UEditorEngine*)GEngine;
			if (EditorEngine)
			{
				const TArray<class FEditorViewportClient*>& ViewportClients = EditorEngine->GetAllViewportClients();
				for (const FEditorViewportClient* ViewportClient : ViewportClients)
				{
					if (ViewportClient == nullptr)
					{
						continue;
					}
					if (FVector::Distance(ActorLocation, ViewportClient->GetViewLocation()) <= VolumeRendererRenderDistance)
					{
						bNeedDraw = true;
						break;
					}
				}
			}
#endif
			if (!bNeedDraw)
			{
				for (FLocalPlayerIterator Iterator(GEngine, InWorld); Iterator; ++Iterator)
				{
					ULocalPlayer* LocalPlayer = *Iterator;
					if (LocalPlayer == nullptr)
					{
						continue;
					}
					APlayerController* PlayerController = LocalPlayer->GetPlayerController(InWorld);
					if (PlayerController == nullptr)
					{
						continue;
					}
					FVector ViewLocation = FVector::ZeroVector;
					FRotator ViewRotation = FRotator::ZeroRotator;
					PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
					if (FVector::Distance(ActorLocation, ViewLocation) <= VolumeRendererRenderDistance)
					{
						bNeedDraw = true;
						break;
					}
				}
			}
			if (!bNeedDraw)
			{
				continue;
			}
		}
		
		// BodySetup を取得
		{
			// Volume等のBrush系
			TArray<UBrushComponent*, TInlineAllocator<16>> BurshComps;
			TargetActor->GetComponents(BurshComps);
			for (UBrushComponent* BrushComp : BurshComps)
			{
				if (BrushComp->BrushBodySetup)
				{
					GeometryInfos.Add(FGeomtryInfo(BrushComp->BrushBodySetup, BrushComp->GetComponentTransform()));
				}
			}
			// Box/Sphere/CapsuleCollision系
			TArray<UShapeComponent*, TInlineAllocator<16>> ShapeComps;
			TargetActor->GetComponents(ShapeComps);
			for (UShapeComponent* ShapeComp : ShapeComps)
			{
				if (ShapeComp->ShapeBodySetup)
				{
					GeometryInfos.Add(FGeomtryInfo(ShapeComp->ShapeBodySetup, ShapeComp->GetComponentTransform()));
				}
			}
		}
		for (const FGeomtryInfo& GeometryInfo : GeometryInfos)
		{
			FCUTUtility::DrawBodySetup(InWorld, GeometryInfo.BodySetup, GeometryInfo.Transform, LineColor, SolidColor, InDuration, VolumeRendererLineThickness);
		}
	}
}
