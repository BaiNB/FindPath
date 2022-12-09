// Fill out your copyright notice in the Description page of Project Settings.


#include "MapCreator.h"

#include "Editor.h"
#include "EditorModeManager.h"
#include "LevelEditorViewport.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Prop.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMapCreator::AMapCreator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMapCreator::BeginPlay()
{
	Super::BeginPlay();

	LoadMapInfo();

	//TArray<AActor*> outActors;
	//UGameplayStatics::GetAllActorsWithTag(this, "start", outActors);
	//check(outActors.Num() == 1);
	//UE_LOG(LogTemp, Warning, TEXT("start: %s"), *outActors[0]->GetActorLocation().ToString());
	//outActors.Empty();
	//UGameplayStatics::GetAllActorsWithTag(this, "target", outActors);
	//check(outActors.Num() == 1);
	//UE_LOG(LogTemp, Warning, TEXT("target: %s"), *outActors[0]->GetActorLocation().ToString());

	//auto cmp = [](int a, int b) {
	//	return a < b;
	//};
	//TArray<int> testSet;
	//testSet.Push(100);
	//testSet.Push(98);
	//testSet.Push(120);
	//for (int i = 0; i < 10; ++i) {
	//	testSet.HeapPush(i, cmp);
	//}
 // FString str = FString::FromInt(testSet.Num());
 // UE_LOG(LogTemp, Warning, TEXT("testSet.Num: %s"), *str);
 // int val = -1;
 // while (testSet.Num() != 0) {
 //   testSet.HeapPop(val, cmp);
 //   UE_LOG(LogTemp, Warning, TEXT("%i"), val);
 // }

	
}

// Called every frame
void AMapCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapCreator::LoadMapInfo()
{
	FString path = FPaths::ProjectDir() + FString("MapInfo.txt");
	FFileHelper::LoadFileToStringArray(mapInfoArr, *path);
}

void AMapCreator::CreateMap()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProp::StaticClass(), outActors);
	if (outActors.Num() != 0) {
		UE_LOG(LogTemp, Warning, TEXT("Map has been created."));
		return;
	}

	if (mapInfoArr.Num() == 0) {
		LoadMapInfo();
	}

	// 使用GEditor需要在Build.cs里添加UnrealEd模块
	if (mapInfoArr.Num() == 0 || GEditor == nullptr) { // || GEditor == nullptr
		return;
	}
	UWorld* world = GEditor->GetAllViewportClients()[0]->GetWorld();
	//UWorld* world = GetWorld();
	ULevel* level = world->GetLevel(0);

	for (int i = 0; i < mapInfoArr.Num(); ++i) {
		if (mapInfoArr[i] == "20") {
			continue;
		}

		// Calc prop's transform
		int row = i / 75; // floor(i / 75)
		int col = i % 75;
		FVector location = FVector(row * 100, col * 100, 0);
		FTransform transform(FRotator::ZeroRotator, location, FVector::OneVector);

		// 在Editor中生成props
		check(AProp::StaticClass() != nullptr);
		AActor* prop = GEditor->AddActor(level, AProp::StaticClass(), transform);
		prop->SetActorLabel(FString::Printf(TEXT("%s-%d-%d"), *mapInfoArr[i], row, col));
		prop->Tags.Add(*mapInfoArr[i]);

		prop->RegisterAllComponents();
		prop->RerunConstructionScripts();

		//// 运行时生成props
		//UStaticMeshComponent* smComp = NewObject<UStaticMeshComponent>(this);
		//smComp->SetupAttachment(rootComp);
		//smComp->RegisterComponent();
		//smComp->SetRelativeTransform(transform);
		//static auto* mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FindPath/Materials/SM_Cube.SM_Cube'"));
		//smComp->SetStaticMesh(mesh);


	}
	GEditor->EditorUpdateComponents();
	world->UpdateWorldComponents(true, false);
	GLevelEditorModeTools().MapChangeNotify();
}

