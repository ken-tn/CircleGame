// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbiterActor.h"

void AOrbiterActor::SetColor(FLinearColor Color)
{
	DynamicMaterialComponent->SetVectorParameterValue(FName("Color"), Color);
}

UStaticMeshComponent* AOrbiterActor::GetMesh()
{
	return StaticMeshComponent;
}

// Sets default values
AOrbiterActor::AOrbiterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Circles/orbit.orbit'"));
	ConstructorHelpers::FObjectFinder<UMaterial>MaterialAsset(TEXT("Material'/Game/Material/Neon.Neon'"));
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	if (MeshAsset.Object != nullptr) {
		StaticMesh = MeshAsset.Object;
	}

	if (MaterialAsset.Object != nullptr) {
		BaseMaterial = MaterialAsset.Object;
	}
}

// Called when the game starts or when spawned
void AOrbiterActor::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterialComponent = UMaterialInstanceDynamic::Create(BaseMaterial, this);
	DynamicMaterialComponent->SetScalarParameterValue("Strength", 10.f);

	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->SetMaterial(0, DynamicMaterialComponent);
}

// Called every frame
void AOrbiterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

