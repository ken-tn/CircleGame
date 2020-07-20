// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CircleBPFunctions.generated.h"

/**
 * 
 */

UCLASS()
class CIRCLEGAME_API UCircleBPFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = "CircleFunctions")
			static const FRotator NormalToRotator(FVector currentRotation, FVector normal);

		UFUNCTION(BlueprintCallable, Category = "CircleFunctions")
			static void RaycastFromCamera(const AActor* Camera, const TArray<AActor*> ignore, FHitResult &outHit, bool &hit);

		UFUNCTION(BlueprintCallable, Category = "CircleOrbiter")
			static void OrbitLoop(const AActor* center, AActor* object, const float radius, const float eclipse, const float orbitTime, const float currentRotation, bool reverse, float &newRotation);
};
