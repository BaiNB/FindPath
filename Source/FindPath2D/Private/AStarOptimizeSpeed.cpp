// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarOptimizeSpeed.h"

#include "MapCreator.h"
#include "Kismet/GameplayStatics.h"
#include "Algo/Impl/BinaryHeap.h"


UAStarOptimizeSpeed::UAStarOptimizeSpeed() {
  ComponentTags.Add("SpeedAStar");
}

float UAStarOptimizeSpeed::Cost(const TSharedPtr<FPoint>& p) {
  float t = GnCost(p) / HnCost(p);
  float k = pow(theta, t);
  return k * GnCost(p) + (1 - k) * HnCost(p);

	//return GnCost(p) + HnCost(p);
}

void UAStarOptimizeSpeed::Start()
{
	UFindPathBase::Start();
	pointFlag.Empty();
	pointFlag.Emplace(startPoint->pos, 1);
}

void UAStarOptimizeSpeed::FindStep()
{
	if (openSet.Num() == 0) {
		// find path failed
		UE_LOG(LogTemp, Error, TEXT("No path found, algorithm failed!"));
		bFinding = false;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		return;
	}

	auto cmp = [&](const TSharedPtr<FPoint>& a, const TSharedPtr<FPoint>& b) {
		return Cost(a) < Cost(b);
	};

	openSet.HeapPop(currPoint, cmp);
	closeSet.Push(currPoint);
	pointFlag.Emplace(currPoint->pos, 0);

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
		TSharedPtr<FPoint> neibor = MakeShareable(new FPoint(currPos + FVector(dirX[i] * 100, dirY[i] * 100, 0)));
		neibor->parent = currPoint;
		neibor->gnCost = GnCost(currPoint) + Distance(currPoint, neibor);
		neibor->hnCost = Distance(neibor, targetPoint);

		if (pointFlag.Contains(neibor->pos) && (pointFlag[neibor->pos] % 2 == 0)) { // pointFlag.Contains(neibor->pos) && (pointFlag[neibor->pos] % 2 == 0)
			// 已加入closeSet closeSet.Contains(neibor)
			continue;
		}

		//int idx = openSet.Find(neibor);
		if (pointFlag.Contains(neibor->pos) && pointFlag[neibor->pos] == 1) { // pointFlag.Contains(neibor->pos) && pointFlag[neibor->pos] == 1
			//idx != INDEX_NONE
			// 若由currPoint到达neibor的GnCost更小则更新parent
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
			pointFlag.Emplace(neibor->pos, 1);
		}
		UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), currPoint->pos, 50, FLinearColor::Red, 5);

	}

}

