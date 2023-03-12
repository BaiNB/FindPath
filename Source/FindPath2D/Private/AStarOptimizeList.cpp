// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarOptimizeList.h"

#include "MapCreator.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/Impl/BinaryHeap.h"


UAStarOptimizeList::UAStarOptimizeList() {
  ComponentTags.Add("AStarList");
}

float UAStarOptimizeList::Cost(const TSharedPtr<FPoint>& p)
{
  return GnCost(p) + HnCost(p);
}

void UAStarOptimizeList::Start()
{
	UFindPathBase::Start();

	pointFlag.Empty();
	pointFlag.Emplace(startPoint->pos, 1);
}

void UAStarOptimizeList::FindStep()
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

	auto cmp = [&](const TSharedPtr<FPoint>& a, const TSharedPtr<FPoint>& b) {
		return Cost(a) < Cost(b);
	};

	openSet.HeapPop(currPoint, cmp);
	//closeSet.Push(currPoint);
	pointFlag.Emplace(currPoint->pos, 0);

	if (currPoint->pos.Equals(targetPoint->pos, 1.0f)) {
		// find targetPoint
		bFinding = false;
		this->exportData->isFindPath = true;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		//UE_LOG(LogTemp, Warning, TEXT("Find target."));
		return;
	}
	++this->exportData->iterNum;

	// ����currPoint��Χ�Ľڵ��Ƿ���Ҫ����openSet�����Ѿ���openSet������Ҫ���Ǹ��½϶̵�cost��parent
	TArray<int> dirX = { -1, 0, 1, 0 };
	TArray<int> dirY = { 0, -1, 0, 1 };
	auto currPos = currPoint->pos;
	for (int i = 0; i < dirX.Num(); ++i) {
		if (!IsValid(currPos.X / 100 + dirX[i], currPos.Y / 100 + dirY[i])) {
			// ������ͼ�߽�����ϰ���ڵ�����
			continue;
		}
		TSharedPtr<FPoint> neibor = MakeShareable(new FPoint(currPos + FVector(dirX[i] * 100, dirY[i] * 100, 0)));
		neibor->parent = currPoint;
		neibor->gnCost = GnCost(currPoint) + Distance(currPoint, neibor);
		neibor->hnCost = Distance(neibor, targetPoint);

		if (pointFlag.Contains(neibor->pos) && (pointFlag[neibor->pos] % 2 == 0)) { // pointFlag.Contains(neibor->pos) && (pointFlag[neibor->pos] % 2 == 0)
			// �Ѽ���closeSet closeSet.Contains(neibor)
			continue;
		}

		//int idx = openSet.Find(neibor);
		if (pointFlag.Contains(neibor->pos) && pointFlag[neibor->pos] == 1) { // pointFlag.Contains(neibor->pos) && pointFlag[neibor->pos] == 1
			//idx != INDEX_NONE
			// ����currPoint����neibor��GnCost��С�����parent
			// GnCost(currPoint) + Distance(currPoint, neibor) < openSet[idx]->gnCost
			int idx = openSet.Find(neibor);
			if (CmpSamePoint(neibor, openSet[idx])) {
				//UE_LOG(LogTemp, Warning, TEXT("Find repeat point."));
				openSet[idx]->gnCost = neibor->gnCost;
				openSet[idx]->parent = currPoint;
				auto temp = openSet[idx];

				openSet.RemoveAtSwap(idx);
				auto PredicateWrapper(cmp);
				AlgoImpl::HeapSiftDown(openSet.GetData(), idx, openSet.Num(), FIdentityFunctor(), PredicateWrapper);
				openSet.HeapPush(temp, cmp);
			}
		}
		else {
			openSet.HeapPush(neibor, cmp);
			++this->exportData->allNodeNum;

			pointFlag.Emplace(neibor->pos, 1);
		}
		//UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), currPoint->pos, 50, FLinearColor::Red, 1);
	}

}
