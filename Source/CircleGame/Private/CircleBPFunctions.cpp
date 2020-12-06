// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleBPFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"

const FRotator UCircleBPFunctions::NormalToRotator(FVector currentRotation, FVector normal)
{
	FQuat rootQuat = currentRotation.ToOrientationQuat();
	FVector upVector = rootQuat.GetUpVector();
	float rotationAngle = acos(FVector::DotProduct(upVector, normal));
	FVector crossVector = FVector::CrossProduct(upVector, normal);
	crossVector.Normalize(0.0001);
	FQuat crossedQuat = FQuat(crossVector, rotationAngle);

	return FRotator(rootQuat * crossedQuat);
}

//unused function :(
void UCircleBPFunctions::RaycastFromCamera(const AActor* Camera, const TArray<AActor*> ignore, FHitResult &outHit, bool &hit)
{
	//USceneComponent* RootComponent = Camera->GetRootComponent();
	//FVector start = RootComponent->GetComponentLocation();

	//APlayerController PlayerController = UGameplayStatics::GetPlayerController(0);
	//FVector end = PlayerController.DeprojectMousePositionToWorld().WorldDirection;
	//hit = UWorld::LineTraceSingleByChannel();

	//ConvertMouseLocationToWorldSpace doesn't exist in C++, scrapped :)
}

void UCircleBPFunctions::OrbitLoop(const AActor* center, AActor* object, const float radius, const float eclipse, const float orbitTime, const float currentRotation, bool reverse, float& newRotation, FVector& newPosition)
{
	float rotationSpeed = PI * 2 / orbitTime;
	float calculateEclipse = eclipse * radius;
	if (reverse) {
		newRotation = currentRotation + (0.166666f) * rotationSpeed;
	}
	else {
		newRotation = currentRotation - (0.166666f) * rotationSpeed;
	}

	FVector mainOrbiterPosition = center->GetActorLocation();
	newPosition = FVector(sin(newRotation) * calculateEclipse, cos(newRotation) * radius, 0) + mainOrbiterPosition;
}

//unused function :(
void UCircleBPFunctions::CloneOrbiter(const AActor* AMainOrbiter, UStaticMeshComponent* &staticMeshComponent, AActor* &actor, bool& Success)
{
	FTransform SpawnTransform = AMainOrbiter->GetRootComponent()->GetComponentTransform();
	AActor* AOrbiter = AMainOrbiter->GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), SpawnTransform);
	
	//TEXT("StaticMesh'/Game/Circles/orbit.orbit'")
	UStaticMeshComponent* StaticMeshComponent = NewObject<UStaticMeshComponent>(UStaticMeshComponent::StaticClass(), TEXT("StaticMesh"));
	//StaticMeshComponent->AttachTo(AMainOrbiter->GetRootComponent());
	//UMaterialInstanceConstant* Material = AOrbiter->ConstructObject<UMaterialInstanceConstant>(TEXT("MaterialInstanceConstant'/Game/Material/Orbit_Material.Orbit_Material'"));
	//StaticMeshComponent->SetMaterial(0, Material);
}

void UCircleBPFunctions::RingInRange(const AActor* orbiter, const AActor* currentTarget, const float lockRadius, float &distance, bool& inRange)
{
	FVector orbiterLocation = orbiter->GetActorLocation();
	FVector ringLocation = currentTarget->GetActorLocation();
	distance = (orbiterLocation - ringLocation).Size();

	if (distance < lockRadius) {
		inRange = true;
	}
	else {
		inRange = false;
	}

	return;
}

void UCircleBPFunctions::FindNearestRing(TMap<AActor*, float> nearRings, AActor*& ANearestRing)
{
	float minDist = 100000.f;
	for (auto& Elem : nearRings)
	{
		const float dist = Elem.Value;
		if (dist < minDist) {
			minDist = dist;
			ANearestRing = Elem.Key;
		}
	}
}

TArray<FOrbiterStruct> UCircleBPFunctions::OrbiterRotate(const AActor* AMainOrbiter, TArray<FOrbiterStruct> Orbiters, TArray<FRingStruct> Rings)
{
	float LockOnRadius = 500.f;
	int index = 0;

	for (auto& Orbiter : Orbiters)
	{
		TMap<AActor*, float> NearRings;
		AActor* ANearestRing;
		
		// Scan for near rings
		for (auto& Ring : Rings)
		{
			AActor* ARing = Ring.Actor;
			float distance;
			bool inRange;
			RingInRange(AMainOrbiter, ARing, LockOnRadius, distance, inRange);

			if (inRange)
			{
				if (Orbiter.Color == Ring.Color)
				{
					ANearestRing = ARing;
					NearRings.Add(ARing, distance);
				}
			}
		}

		if (NearRings.Num() > 0)
		{
			FindNearestRing(NearRings, ANearestRing);
			Orbiter.Actor->SetActorTransform(ANearestRing->GetActorTransform());
			Orbiter.Target = ANearestRing;
		}
		else
		{
			float NewRotation;
			FVector NewLocation;
			OrbitLoop(AMainOrbiter, Orbiter.Actor, LockOnRadius, 1.f, 10.f, Orbiter.Rotation, index % 2 == 0, NewRotation, NewLocation);

			// c++ magic
			Orbiter.Rotation = NewRotation;
			Orbiter.Actor->SetActorLocation(NewLocation);
		}

		index++;
	}

	return Orbiters;
}

TArray<FRingStruct> UCircleBPFunctions::KillTarget(const int orbiterNumber, const TArray<FOrbiterStruct> Orbiters, TArray<FRingStruct> Rings)
{
	FOrbiterStruct Orbiter = Orbiters[orbiterNumber];
	AActor* ATarget = Orbiter.Target;
	if (ATarget)
	{
		ATarget->Destroy();
		for (int i = 0; i < Rings.Num(); i++)
		{
			AActor* ARing = Rings[i].Actor;
			if (ARing == ATarget)
			{
				Rings.RemoveAt(i);
			}
		}
	}

	return Rings;
}

UCircleBPFunctions::UCircleBPFunctions(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
	{
		//unused :(
		static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Circles/orbit.orbit'"));
		static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("MaterialInstanceConstant'/Game/Material/Orbit_Material.Orbit_Material'"));

		orbiterMesh = MeshAsset.Object;
		orbiterMaterial = MaterialAsset.Object;
	}