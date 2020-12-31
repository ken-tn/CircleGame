// Fill out your copyright notice in the Description page of Project Settings.


#include "OrbiterActor.h"

void AOrbiterActor::SetColor(FLinearColor Color)
{
	this->DynamicMaterialComponent->SetVectorParameterValue(FName("Color"), Color);
}

UStaticMeshComponent* AOrbiterActor::GetMesh()
{
	return StaticMeshComponent;
}

// Sets default values
AOrbiterActor::AOrbiterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Game/Circles/orbit.orbit'"));
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	if (MeshAsset.Object != nullptr) {
		StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	}

	DynamicMaterialComponent = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("DynamicMaterial"));
}

// Called when the game starts or when spawned
void AOrbiterActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOrbiterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

