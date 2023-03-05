// Fill out your copyright notice in the Description page of Project Settings.


#include "MapCreator.h"

#include "Editor.h"
#include "EditorModeManager.h"
#include "LevelEditorViewport.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Prop.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "NavMesh/RecastNavMesh.h"
#include "AI/NavDataGenerator.h"

#include "Algo/Impl/BinaryHeap.h"


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

	//for (int i = -5; i < 6; ++i) {
	//	testSet.HeapPush(i, cmp);
	//}

	//testSet.HeapPush(100, cmp);
	//testSet.HeapPush(98, cmp);
	//testSet.HeapPush(120, cmp);

	//FString showSet{};
	//for (auto num : testSet) {
	//	showSet += FString::FromInt(num);
	//	showSet += " ";
	//}
	//UE_LOG(LogTemp, Warning, TEXT("showSet: %s"), *showSet);

	//auto idx = testSet.Find(5);

	//testSet.RemoveAtSwap(idx);
	//auto PredicateWrapper(cmp);
	//AlgoImpl::HeapSiftDown(testSet.GetData(), idx, testSet.Num(), FIdentityFunctor(), PredicateWrapper);

	////testSet.Heapify(cmp);
	//testSet.HeapPush(-3, cmp);


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
	FString path = FPaths::ProjectDir() + FString("MapInfoEnemy.txt");
	FFileHelper::LoadFileToStringArray(mapInfoArr, *path);

	// 随机生成mineNum个地雷
	int num = 0;
	while (num < mineNum) {
		// 随机生成(x,y)
		int32 randomX = UKismetMathLibrary::RandomInteger(26) + 24;
		int32 randomY = UKismetMathLibrary::RandomInteger(26) + 24;
		auto flag = mapInfoArr[randomX * 75 + randomY].Left(2);
		if (flag.Equals("10")) {
			continue;
		}
		// 在(x,y)生成地雷
		FVector location = FVector(randomX * 100, randomY * 100, 0);
		FTransform transform(FRotator::ZeroRotator, location, FVector::OneVector);
    UStaticMeshComponent* smComp = NewObject<UStaticMeshComponent>(this);
    smComp->SetupAttachment(rootComp);
    smComp->RegisterComponent();
    smComp->SetRelativeTransform(transform);
    smComp->SetStaticMesh(mineMesh);
		smComp->SetMaterial(0, mineMaterial);

		// 对地雷进行标识
		mapInfoArr[randomX * 75 + randomY] += "0";
		++num;
	}

}

void AMapCreator::AddingBattlefieldInfo()
{
	LoadMapInfo();
	// (2500,4000,0)->(5000,5000,0)设置为enemy区域
	FVector start(25, 40, 0);
	FVector end(50, 50, 0);
	for (int i = start.X; i <= end.X; ++i) {
		for (int j = start.Y; j <= end.Y; ++j) {
			mapInfoArr[i * 75 + j] += "0";
			//UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), FVector(i * 100, j * 100, 200) , 50, FLinearColor::Red, 5);
		}
	}
	FString path = FPaths::ProjectDir() + FString("MapInfoEnemy.txt");
	FFileHelper::SaveStringArrayToFile(mapInfoArr, *path);
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
	if (mapInfoArr.Num() == 0 || GEditor == nullptr) {
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

void AMapCreator::ExportNavMesh()
{
	UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	if (NavSys) {
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsWithTag(this, FName("start"), OutActors);
		UE_LOG(LogTemp, Warning, TEXT("start: %d"), OutActors.Num());
		check(OutActors.Num() == 1);
		AActor* start = OutActors[0];

		UGameplayStatics::GetAllActorsWithTag(this, FName("target"), OutActors);
		UE_LOG(LogTemp, Warning, TEXT("target: %d"), OutActors.Num());
		check(OutActors.Num() == 1);
		AActor* end = OutActors[0];

		//UNavigationSystemV1::FindPathSync();
		UNavigationPath* path = NavSys->FindPathToLocationSynchronously(GetWorld(), start->GetActorLocation() + FVector(0, 0, -50), end->GetActorLocation() + FVector(0, 0, -50));
		//if (path != NULL) {
		//	UE_LOG(LogTemp, Warning, TEXT("PathPoints.num: %d"), path->PathPoints.Num());
		//}

		ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::ECreateIfEmpty::DontCreate);
		if (NavData) {
			ARecastNavMesh* NavMesh = Cast<ARecastNavMesh>(NavData);
			NavMesh->GetRecastMesh(); // dtNavMesh
			NavMesh->GetGenerator()->ExportNavigationData(TEXT("F:\\UE_projects\\FindPath2D\\Saved\\"));
			UE_LOG(LogTemp, Warning, TEXT("Export NavMesh Succeed."));
		}
	}

}

