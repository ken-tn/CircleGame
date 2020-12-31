// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleBPFunctions.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/ObjectLibrary.h"



UCircleBPFunctions::UCircleBPFunctions(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Circles/orbit.orbit'"));

		if (MeshAsset.Object != nullptr)
		{
			
		}

		static ConstructorHelpers::FObjectFinder<UMaterialInstance>MaterialAsset(TEXT("MaterialInstanceConstant'/Game/Material/Orbit_Material.Orbit_Material'"));

		if (MaterialAsset.Object != nullptr)
		{
			
		}
	}