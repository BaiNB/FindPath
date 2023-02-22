// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPathBase.h"

#include "MapCreator.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UFindPathBase::UFindPathBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFindPathBase::BeginPlay()
{
	Super::BeginPlay();

	AActor* mapCreator = UGameplayStatics::GetActorOfClass(this, AMapCreator::StaticClass());
	if (mapCreator) {
		mapInfoArr = Cast<AMapCreator>(mapCreator)->mapInfoArr;
	}
}


// Called every frame
void UFindPathBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFindPathBase::FindPath()
{
	Start();

	while (bFinding) {
		FindStep();
	}
}

void UFindPathBase::FindPathOnce(float deltaTime)
{
	Start();

	GetWorld()->GetTimerManager().ClearTimer(timerHandle);
	GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &UFindPathBase::FindStep, deltaTime, true);
}

void UFindPathBase::Start()
{
	openSet.Empty();
	closeSet.Empty();
	currPoint = nullptr;

	// 初始化startPoint和targetPoint
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsWithTag(this, "start", outActors);
	check(outActors.Num() == 1);
	startPoint = MakeShareable(new FPoint(outActors[0]->GetActorLocation() + FVector(0, 0, -50)));
	UE_LOG(LogTemp, Warning, TEXT("start: %s"), *startPoint->pos.ToString());
	outActors.Empty();
	UGameplayStatics::GetAllActorsWithTag(this, "target", outActors);
	check(outActors.Num() == 1);
	targetPoint = MakeShareable(new FPoint(outActors[0]->GetActorLocation() + FVector(0, 0, -50)));
	UE_LOG(LogTemp, Warning, TEXT("target: %s"), *targetPoint->pos.ToString());


	// 将起点加入openSet
	TSharedPtr<FPoint> start = MakeShareable(new FPoint(startPoint->pos));
	start->hnCost = Distance(startPoint, targetPoint);
	openSet.Push(start);

	bFinding = true;
}

void UFindPathBase::FindStep()
{
	if (openSet.Num() == 0) {
		// find path failed
		UE_LOG(LogTemp, Error, TEXT("No path found, algorithm failed!"));
		bFinding = false;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		return;
	}

	//auto prev = currPoint;
	auto cmp = [&](const TSharedPtr<FPoint>& a, const TSharedPtr<FPoint>& b) {
		return Cost(a) < Cost(b);
	};

	//TSharedPtr<FPoint> testPtr = nullptr;
	//if (openSet.Num() > 20) {
	//	while (openSet.Num() != 0) {
	//		openSet.HeapPop(testPtr, cmp);
	//		float gnCost = testPtr->gnCost;
	//		float hnCost = testPtr->hnCost;
	//		float cost = gnCost + hnCost;
	//		UE_LOG(LogTemp, Warning, TEXT("%f +%f = %f"), gnCost, hnCost, cost);
	//	}
	//	bFinding = false;
	//	return;
	//}

	openSet.HeapPop(currPoint, cmp);
	closeSet.Push(currPoint);

	if (currPoint->pos.Equals(targetPoint->pos, 1.0f)) {
		// find targetPoint
		bFinding = false;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Find target."));
		FinishFind();
		return;
	}

	// 考虑currPoint周围的节点是否需要加入openSet，若已经在openSet中则需要考虑更新较短的cost和parent
  TArray<int> dirX = { -1, 0, 1, 0 };
  TArray<int> dirY = { 0, -1, 0, 1 };
	auto currPos = currPoint->pos;
	for (int i = 0; i < dirX.Num(); ++i) {
		if (!IsValid(currPos.X / 100 + dirX[i], currPos.Y / 100 + dirY[i])) {
			// 超出地图边界或是障碍物节点跳过
			continue;
		}
		TSharedPtr<FPoint> neibor = MakeShareable(new FPoint(currPos + FVector(dirX[i] * 100, dirY[i] *100, 0)));
		neibor->parent = currPoint;
		neibor->gnCost = GnCost(currPoint) + Distance(currPoint, neibor);
		neibor->hnCost = Distance(neibor, targetPoint);

		if (closeSet.Contains(neibor)) {
			// 已加入closeSet
			continue;
		}
		int idx = openSet.Find(neibor);
		if (idx != INDEX_NONE) {
			// 若由currPoint到达neibor的GnCost更小则更新parent
			// GnCost(currPoint) + Distance(currPoint, neibor) < openSet[idx]->gnCost
			if (CmpSamePoint(neibor, openSet[idx])) {
				//UE_LOG(LogTemp, Warning, TEXT("Find repeat point."));
				openSet[idx]->parent = currPoint;
			}
		}
		else {
			openSet.HeapPush(neibor, cmp); // &UFindPathBase::CmpCost
		}
		UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), currPoint->pos, 50, FLinearColor::Red, 5);

	}
}

void UFindPathBase::FinishFind()
{
	// 将数据写入res.txt并构建最终路径
	BuildPath();
}


void UFindPathBase::BuildPath()
{
	while (currPoint.IsValid()) {
		UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -30), FVector::UpVector), currPoint->pos, 50, FLinearColor::Blue, 5);
		currPoint = currPoint->parent;
	}
}

bool UFindPathBase::IsValid(int x, int y)
{
	if (x < 0 || x >= 75 || y < 0 || y >= 75) {
		// map overflow
		return false;
	}
	if (!mapInfoArr[x * 75 + y].Equals("20")) {
		// (x, y) is barrier
		return false;
	}

	return true;
}


