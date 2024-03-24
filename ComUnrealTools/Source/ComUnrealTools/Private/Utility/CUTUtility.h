// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"


class UBodySetup;
class UWorld;


/**
 * CMT Plugin utility
 */
class FCUTUtility
{
public:

	/** Draw a debug sphere */
	static void DrawDebugSloidSphereAdvanced(const UWorld* InWorld, FVector const& XAxis, FVector const& YAxis, FVector const& ZAxis, FVector const& Center, float Radius, int32 Segments, float AngleRateY, FColor const& Color, bool bPersistentLines = false, float LifeTime=-1.f, uint8 DepthPriority = 0);

	/** Draw a debug sphere */
	static void DrawDebugSloidSphere(const UWorld* InWorld, FVector const& Center, float Radius, int32 Segments, FColor const& Color, bool bPersistentLines = false, float LifeTime=-1.f, uint8 DepthPriority = 0);

	/** Draw a debug capsule */
	static void DrawDebugSloidCapsule(const UWorld* InWorld, FVector const& Center, float HalfHeight, float Radius, const FQuat& Rotation, FColor const& Color, bool bPersistentLines = false, float LifeTime = -1.f, uint8 DepthPriority = 0);

	/** Draw BodySetup Geometry */
	static void DrawBodySetup(UWorld* World, const UBodySetup* BodySetup, const FTransform& Transform, FColor LineColor, FColor SolidColor, float Duration, float Thickness);
};
