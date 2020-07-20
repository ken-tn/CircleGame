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

void UCircleBPFunctions::OrbitLoop(const AActor* center, AActor* object, const float radius, const float eclipse, const float orbitTime, const float currentRotation, bool reverse, float& newRotation)
{
	float rotationSpeed = PI * 2 / orbitTime;
	float calculateEclipse = eclipse * radius;
	/*if (reverse) {
		newRotation = currentRotation + (1/60) * rotationSpeed;
	}
	else {
		newRotation = currentRotation - (1/60) * rotationSpeed;
	}*/
	
	newRotation = currentRotation + (1 / 60) * rotationSpeed;
	UE_LOG(LogTemp, Warning, TEXT("%f"), newRotation);

	FVector mainOrbiterPosition = center->GetActorLocation();
	FVector newPosition = FVector(sin(newRotation) * calculateEclipse, 0, cos(newRotation) * radius) + mainOrbiterPosition;

	object->SetActorLocation(newPosition);
}