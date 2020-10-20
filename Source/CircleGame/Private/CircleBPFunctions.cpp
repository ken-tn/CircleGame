// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleBPFunctions.h"
#include "Kismet/GameplayStatics.h"

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

void UCircleBPFunctions::CloneOrbiter(const AActor* mainOrbiter, UStaticMeshComponent*& staticMeshComponent, AActor*& actor)
{
	FTransform spawnTransform = mainOrbiter->GetRootComponent()->GetComponentTransform();
	actor = mainOrbiter->GetWorld()->SpawnActor<AActor>(spawnTransform.GetLocation(), spawnTransform.GetRotation().Rotator());
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Circles/orbit.orbit'"));
	UStaticMesh* orbiterMesh = MeshAsset.Object;
	staticMeshComponent = actor->CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	staticMeshComponent->SetStaticMesh(orbiterMesh);

	static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("MaterialInstanceConstant'/Game/Material/Orbit_Material.Orbit_Material'"));
	UMaterialInstance* orbiterMaterial = MaterialAsset.Object;
	staticMeshComponent->SetMaterial(0, orbiterMaterial);
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

void UCircleBPFunctions::FindNearestRing(const TMap<AActor*, float> nearRings, AActor*& nearestRing)
{
	float minDist = 100000.f;
	for (auto It = nearRings.CreateConstIterator(); It; ++It)
	{
		const float dist = It.Value();
		if (dist < minDist) {
			minDist = dist;
			nearestRing = It.Key();
		}
	}
}

UCircleBPFunctions::UCircleBPFunctions(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Circles/orbit.orbit'"));
		static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("MaterialInstanceConstant'/Game/Material/Orbit_Material.Orbit_Material'"));

		orbiterMesh = MeshAsset.Object;
		orbiterMaterial = MaterialAsset.Object;
	}