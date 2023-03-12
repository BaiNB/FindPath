// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPathBase.h"

#include "Kismet/GameplayStatics.h"
#include "Algo/Impl/BinaryHeap.h"

//#include "time.h"
#include <windows.h>


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

	// get mapInfo
	AActor* myMap = UGameplayStatics::GetActorOfClass(this, AMapCreator::StaticClass());
	if (myMap) {
		mapCreator = Cast<AMapCreator>(myMap);
		mapInfoArr = mapCreator->mapInfoArr;
	}

	// 初始化startPoint和targetPoint
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsWithTag(this, "start", outActors);
	check(outActors.Num() == 1);
	startPoint = MakeShareable(new FPoint(outActors[0]->GetActorLocation() + FVector(0, 0, -50)));
	//UE_LOG(LogTemp, Warning, TEXT("start: %s"), *startPoint->pos.ToString());
	outActors.Empty();
	UGameplayStatics::GetAllActorsWithTag(this, "target", outActors);
	check(outActors.Num() == 1);
	targetPoint = MakeShareable(new FPoint(outActors[0]->GetActorLocation() + FVector(0, 0, -50)));
	//UE_LOG(LogTemp, Warning, TEXT("target: %s"), *targetPoint->pos.ToString());

	this->exportData = MakeShareable(new ExportData());
	// 设置exportData地图大小和障碍物密度
	this->exportData->mapSize.X = FMath::Abs(startPoint->pos.X - targetPoint->pos.X) + 100;
	this->exportData->mapSize.Y = FMath::Abs(startPoint->pos.Y - targetPoint->pos.Y) + 100;

	if (!mapCreator->isExistMine) {
		// 在start和target之间生成mine
		mapCreator->SpawnMineBetween(startPoint->pos, targetPoint->pos);
		mapInfoArr = mapCreator->mapInfoArr; // 更新mapinfoArr
	}
	mapCreator->CreateMinBarrierDist();	// 生成距离障碍物矩阵

	this->exportData->barrierDensity = mapCreator->GetBarrierDensity(startPoint->pos, targetPoint->pos);

	// 添加enemy区域
	mapCreator->SetEnemyDensity();
	this->exportData->enemyDensity = mapCreator->GetEnemyDensity();
}


// Called every frame
void UFindPathBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFindPathBase::FindPath()
{
	// 初始化exportData
  exportData->Init();
	SetMethodName();
	this->exportData->methodName = TCHAR_TO_UTF8(*methodName);

	LARGE_INTEGER t1, t2, tc;
	QueryPerformanceFrequency(&tc);
	QueryPerformanceCounter(&t1);

	Start();
	while (bFinding) {
		FindStep();
	}
	QueryPerformanceCounter(&t2);
	this->exportData->findPathTime = (float)(t2.QuadPart - t1.QuadPart) / (float)tc.QuadPart;

	FinishFind();

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

	// 将起点加入openSet
	TSharedPtr<FPoint> start = MakeShareable(new FPoint(startPoint->pos));
	start->hnCost = Distance(startPoint, targetPoint);
	openSet.Push(start);
	++this->exportData->allNodeNum;

	bFinding = true;
}

void UFindPathBase::FindStep()
{
	if (openSet.Num() == 0) {
		// find path failed
		UE_LOG(LogTemp, Error, TEXT("No path found, algorithm failed!"));
		bFinding = false;
		this->exportData->isFindPath = false;
		currPoint = nullptr;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		return;
	}

	//auto prev = currPoint;
	auto cmp = [&](const TSharedPtr<FPoint>& a, const TSharedPtr<FPoint>& b) {
		return Cost(a) < Cost(b);
	};

	// 找到openSet里fn最小的点，赋值给currPoint
	PopSmallestPoint(currPoint, openSet);
	//openSet.HeapPop(currPoint, cmp);
	closeSet.Push(currPoint);

	if (currPoint->pos.Equals(targetPoint->pos, 1.0f)) {
		// find targetPoint
		bFinding = false;
		this->exportData->isFindPath = true;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		return;
	}
	++this->exportData->iterNum;

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
			// 若由currPoint到达neibor的GnCost更小则更新parent和gnCost
			// GnCost(currPoint) + Distance(currPoint, neibor) < openSet[idx]->gnCost
			if (CmpSamePoint(neibor, openSet[idx])) {
				//UE_LOG(LogTemp, Warning, TEXT("Find repeat point."));
				openSet[idx]->gnCost = neibor->gnCost;
				openSet[idx]->parent = currPoint;
				auto temp = openSet[idx];

				openSet.RemoveAtSwap(idx);
				openSet.Push(temp);
			}
		}
		else {
			openSet.Push(neibor);
			++this->exportData->allNodeNum;
		}
		//UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), currPoint->pos, 50, FLinearColor::Red, 1);

	}
}

void UFindPathBase::FinishFind()
{
	// 将数据写入res.txt并构建最终路径
	BuildPath();

	this->exportData->Export();
}


void UFindPathBase::BuildPath()
{
	while (currPoint.IsValid()) {
		UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -30), FVector::UpVector), currPoint->pos, 50, FLinearColor::Blue, 1);
		++this->exportData->pathNodeNum; // 路径上的节点数量

		// 记录路径上enemy区域内节点数量
		float dist = 0.f;
		int radius = 0;
		if (this->mapCreator->GetDistToEnemyEdge(currPoint->pos, dist, radius)) {
			// currPoint在enemy区域内
			if (this->exportData->enemyNodeNum == -1) {
				this->exportData->enemyNodeNum = 0;
			}
			++this->exportData->enemyNodeNum;
		}

		// 记录路径上距离障碍物最近距离
		int x = currPoint->pos.X / 100, y = currPoint->pos.Y / 100;
		check(this->mapCreator->minBarrierDist.Num() != 0);
		if (this->mapCreator->minBarrierDist[x][y] != INT32_MAX) {
			int minDist = this->mapCreator->minBarrierDist[x][y] * 200 - 100;
			if (minDist < this->exportData->minBarrierDist) {
				this->exportData->minBarrierDist = minDist;
			}
			if (minDist < carSize) {
				if (this->exportData->insecureNodeNum == -1) {
					this->exportData->insecureNodeNum = 0;
				}
				++this->exportData->insecureNodeNum;
			}
		}
		currPoint = currPoint->parent;

	}
}

bool UFindPathBase::IsValid(int x, int y)
{
	if (x < 0 || x >= 75 || y < 0 || y >= 75) {
		// map overflow
		return false;
	}
	if (!IsPassable(x * 75 + y)) {
		// (x, y) is a barrier
		return false;
	}

	return true;
}

bool UFindPathBase::IsPassable(int idx)
{
	auto flag = mapInfoArr[idx].Left(2);
	return !flag.Equals("10");
}

void UFindPathBase::PopSmallestPoint(TSharedPtr<FPoint>& outPoint, TArray<TSharedPtr<FPoint>>& set)
{
	TSharedPtr<FPoint> minPoint = set[0];
	int minIdx = 0, size = set.Num();
	for (int i = 1; i < size; ++i) {
		if (Cost(set[i]) < Cost(minPoint)) {
			minPoint = set[i];
			minIdx = i;
		}
	}
	outPoint = minPoint;
	set.RemoveAtSwap(minIdx);
}


