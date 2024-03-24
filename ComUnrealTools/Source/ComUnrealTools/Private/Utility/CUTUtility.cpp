// Copyright com04 All Rights Reserved.


#include "CUTUtility.h"

#include "PhysicsEngine/BodySetup.h"
#include "PhysicsEngine/AggregateGeom.h"


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
