// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbiterPawn.h"

#include "OrbiterActor.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"

const FRotator AOrbiterPawn::NormalToRotator(FVector currentRotation, FVector normal)
{
	FQuat rootQuat = currentRotation.ToOrientationQuat();
	FVector upVector = rootQuat.GetUpVector();
	float rotationAngle = acos(FVector::DotProduct(upVector, normal));
	FVector crossVector = FVector::CrossProduct(upVector, normal);
	crossVector.Normalize(0.0001);
	FQuat crossedQuat = FQuat(crossVector, rotationAngle);

	return FRotator(rootQuat * crossedQuat);
}

void AOrbiterPawn::RaycastFromCamera(const TArray<AActor*> Ignored, FHitResult& OutHit, bool& IsHit)
{
	// Get Player Controller
	if (GEngine != 0)
	{
		const UWorld* World = GEngine->GameViewport->GetWorld();
		const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);

		// Get Mouse location and direction
		FVector WorldLoc;
		FVector WorldDir;
		PlayerController->DeprojectMousePositionToWorld(WorldLoc, WorldDir);

		// Get Camera location
		FVector CameraWorldLocation = PlayerController->PlayerCameraManager->GetRootComponent()->GetComponentLocation();

		// A position 9000 units in camera's direction
		WorldDir *= 9000;
		WorldLoc += WorldDir;

		// Create ignore collisions
		FCollisionQueryParams CollisionParams;
		for (auto& Actor : Ignored)
		{
			CollisionParams.AddIgnoredActor(Actor);
		}

		bool bIsHit = World->LineTraceSingleByChannel(OutHit, CameraWorldLocation, WorldLoc, ECollisionChannel::ECC_Visibility, CollisionParams);
	}
}

void AOrbiterPawn::OrbiterToMouse(const TArray<AActor*> Ignored, AActor* AMainOrbiter)
{
	// Raycast
	bool bIsHit;
	FHitResult OutHit;
	RaycastFromCamera(Ignored, OutHit, bIsHit);

	// Move orbiter
	if (bIsHit)
	{
		FRotator NewRotation = NormalToRotator(AMainOrbiter->GetActorRotation().Vector(), OutHit.Normal);
		AMainOrbiter->SetActorLocation(OutHit.Location);
		AMainOrbiter->SetActorRotation(NewRotation);
	}
}

void AOrbiterPawn::OrbitLoop(const AActor* center, AActor* object, const float radius, const float eclipse, const float orbitTime, const float currentRotation, bool reverse, float& newRotation, FVector& newPosition)
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

FOrbiterObjStruct AOrbiterPawn::SpawnOrbiter(FLinearColor Color)
{
	FActorSpawnParameters SpawnInfo;
	AOrbiterActor* Orbiter = GetWorld()->SpawnActor<AOrbiterActor>(this->GetActorLocation(), this->GetActorRotation(), SpawnInfo);

	return FOrbiterObjStruct{ Orbiter, Orbiter->GetMesh() };
}

void AOrbiterPawn::RingInRange(const AActor* orbiter, const AActor* currentTarget, const float lockRadius, float& distance, bool& inRange)
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

void AOrbiterPawn::FindNearestRing(TMap<AActor*, float> nearRings, AActor*& ANearestRing)
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

void AOrbiterPawn::OrbiterRotate(const AActor* AMainOrbiter)
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
}

/*TArray<FRingStruct> AOrbiterPawn::KillTarget(const int Input, const TArray<FOrbiterStruct> Orbiters, TArray<FRingStruct> Rings)
{
	if (Input < NumberOfOrbiters)
	{
		FOrbiterStruct Orbiter = Orbiters[Input];
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
	}

	return Rings;
}*/

// Sets default values
AOrbiterPawn::AOrbiterPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	//StaticMeshComponent->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AOrbiterPawn::BeginPlay()
{
	// Create mesh
	if (OrbiterMesh != nullptr) {
		StaticMeshComponent->SetStaticMesh(OrbiterMesh);
	}
	
	// Create dynamic material
	if (OrbiterMaterialInstance != nullptr)
	{
		StaticMeshComponent->CreateDynamicMaterialInstance(0, OrbiterMaterialInstance, FName("MaterialInstanceDynamic"));
	}

	// Spawn the orbiters
	for (int i = 0; i < NumberOfOrbiters; i++)
	{
		FOrbiterObjStruct Orbiter = SpawnOrbiter(Colours[i]);
		
		FOrbiterStruct OrbiterStruct = {};
		OrbiterStruct.Actor = Orbiter.Actor;
		OrbiterStruct.Rotation = i * (360 / NumberOfOrbiters);
		OrbiterStruct.Color = i;

		Ignore.Add(Orbiter.Actor);
		Orbiters.Add(OrbiterStruct);
	}

	Ignore.Add(this);

	Super::BeginPlay();
	
}

// Called every frame
void AOrbiterPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OrbiterToMouse(Ignore, this);
	OrbiterRotate(this);
}

// Called to bind functionality to input
void AOrbiterPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
