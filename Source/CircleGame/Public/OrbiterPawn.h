// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OrbiterPawn.generated.h"

USTRUCT(BlueprintType)
struct FOrbiterObjStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaticMeshComponent;
};

USTRUCT(BlueprintType)
struct FOrbiterStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Target;
};

USTRUCT(BlueprintType)
struct FRingStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Color;
};

UCLASS()
class CIRCLEGAME_API AOrbiterPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
		TArray<FLinearColor> Colours = {
		FLinearColor(1.f, 0.f, 0.f, 1.f),
		FLinearColor(0.f, 1.f, 0.f, 1.f),
		FLinearColor(0.f, 0.f, 1.f, 1.f),
		FLinearColor(1.f, 0.f, 1.f, 1.f),
		FLinearColor(1.f, 1.f, 0.f, 1.f)
	};

	UPROPERTY(EditAnywhere)
		uint8 NumberOfOrbiters = 4;

	UPROPERTY()
		TArray<AActor*> Ignore;

	UPROPERTY()
		TArray<FRingStruct> Rings;

	UPROPERTY()
		TArray<FOrbiterStruct> Orbiters;

	UPROPERTY(EditAnywhere)
		UStaticMesh* OrbiterMesh;

	UPROPERTY(EditAnywhere)
		UMaterialInstance* OrbiterMaterialInstance;

	UPROPERTY()
		UStaticMeshComponent* StaticMeshComponent;

	UFUNCTION(BlueprintCallable, Category = "CircleFunctions")
		static const FRotator NormalToRotator(FVector currentRotation, FVector normal);

	UFUNCTION(BlueprintCallable, Category = "CircleFunctions")
		static void RaycastFromCamera(const TArray<AActor*> Ignored, FHitResult& OutHit, bool& IsHit);

	UFUNCTION(BlueprintCallable, Category = "CircleFunctions")
		static void OrbiterToMouse(const TArray<AActor*> Ignored, AActor* AMainOrbiter);

	UFUNCTION(Category = "CircleOrbiter")
		static void OrbitLoop(const AActor* center, AActor* object, const float radius, const float eclipse, const float orbitTime, const float currentRotation, bool reverse, float& newRotation, FVector& newPosition);

	UFUNCTION(BlueprintCallable, Category = "CircleRings")
		FOrbiterObjStruct SpawnOrbiter(FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "CircleRings")
		static void RingInRange(const AActor* orbiter, const AActor* currentTarget, const float lockRadius, float& distance, bool& inRange);

	UFUNCTION(Category = "CircleRings")
		static void FindNearestRing(TMap<AActor*, float> nearRings, AActor*& ANearestRing);

	UFUNCTION(Category = "CircleRings")
		void OrbiterRotate(const AActor* AMainOrbiter);

	/*
	UFUNCTION(BlueprintCallable, Category = "CircleRings")
		static TArray<FRingStruct> KillTarget(const int Input, const TArray<FOrbiterStruct> Orbiters, TArray<FRingStruct> Rings);
	*/

public:
	// Sets default values for this pawn's properties
	AOrbiterPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
