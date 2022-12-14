// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop.h"

#include "UObject/ConstructorHelpers.h"

// Sets default values
AProp::AProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	propMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMesh"));
	SetRootComponent(propMesh);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/FindPath/Materials/SM_Cube.SM_Cube'"));
	if (meshAsset.Succeeded()) propMesh->SetStaticMesh(meshAsset.Object);

}

// Called when the game starts or when spawned
void AProp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

