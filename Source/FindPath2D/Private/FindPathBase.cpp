// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPathBase.h"

#include "MapCreator.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/Impl/BinaryHeap.h"


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

	// ��ʼ��startPoint��targetPoint
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


	// ��������openSet
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

	// �ҵ�openSet��fn��С�ĵ㣬��ֵ��currPoint
	PopSmallestPoint(currPoint, openSet);
	//openSet.HeapPop(currPoint, cmp);
	closeSet.Push(currPoint);

	if (currPoint->pos.Equals(targetPoint->pos, 1.0f)) {
		// find targetPoint
		bFinding = false;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		//UE_LOG(LogTemp, Warning, TEXT("Find target."));
		FinishFind();
		return;
	}

	// ����currPoint��Χ�Ľڵ��Ƿ���Ҫ����openSet�����Ѿ���openSet������Ҫ���Ǹ��½϶̵�cost��parent
  TArray<int> dirX = { -1, 0, 1, 0 };
  TArray<int> dirY = { 0, -1, 0, 1 };
	auto currPos = currPoint->pos;
	for (int i = 0; i < dirX.Num(); ++i) {
		if (!IsValid(currPos.X / 100 + dirX[i], currPos.Y / 100 + dirY[i])) {
			// ������ͼ�߽�����ϰ���ڵ�����
			continue;
		}
		TSharedPtr<FPoint> neibor = MakeShareable(new FPoint(currPos + FVector(dirX[i] * 100, dirY[i] *100, 0)));
		neibor->parent = currPoint;
		neibor->gnCost = GnCost(currPoint) + Distance(currPoint, neibor);
		neibor->hnCost = Distance(neibor, targetPoint);

		if (closeSet.Contains(neibor)) {
			// �Ѽ���closeSet
			continue;
		}
		int idx = openSet.Find(neibor);
		if (idx != INDEX_NONE) {
			// ����currPoint����neibor��GnCost��С�����parent
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
		}
		UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), currPoint->pos, 50, FLinearColor::Red, 5);

	}
}

void UFindPathBase::FinishFind()
{
	// ������д��res.txt����������·��
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
	if (!IsPassable(x * 75 + y)) {
		// (x, y) is a barrier
		return false;
	}

	return true;
}

bool UFindPathBase::IsPassable(int idx)
{
	auto flag = mapInfoArr[idx].Left(2);
	return flag.Equals("20");
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


