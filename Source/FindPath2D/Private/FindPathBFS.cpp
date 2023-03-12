// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPathBFS.h"
#include "Kismet/GameplayStatics.h"


UFindPathBFS::UFindPathBFS() {
	ComponentTags.Add("BFS");
}

void UFindPathBFS::FindStep()
{
	//UE_LOG(LogTemp, Warning, TEXT("This is BFS::FindStep"));
	if (openQue.IsEmpty()) {
		// find path failed
		UE_LOG(LogTemp, Error, TEXT("No path found, algorithm failed!"));
		bFinding = false;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		return;
	}

	openQue.Dequeue(currPoint);
	closeSet.Push(currPoint);

	if (currPoint->pos.Equals(targetPoint->pos, 1.0f)) {
		// find targetPoint
		bFinding = false;
		GetWorld()->GetTimerManager().ClearTimer(timerHandle);
		UE_LOG(LogTemp, Warning, TEXT("Find target."));
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
		TSharedPtr<FPoint> neibor = MakeShareable(new FPoint(currPos + FVector(dirX[i] * 100, dirY[i] * 100, 0)));
		neibor->parent = currPoint;

		if (closeSet.Contains(neibor)) {
			// �Ѽ���closeSet
			continue;
		}

		openQue.Enqueue(neibor);
		UKismetSystemLibrary::DrawDebugPlane(GetWorld(), FPlane(FVector(0, 0, -40), FVector::UpVector), currPoint->pos, 50, FLinearColor::Red, 1);
	}

}

void UFindPathBFS::Start()
{
	openQue.Empty();
	openSet.Empty();
	closeSet.Empty();
	currPoint = nullptr;

	// ��ʼ��startPoint��targetPoint
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsWithTag(this, "start", outActors);
	check(outActors.Num() == 1);
	startPoint = MakeShareable(new FPoint(outActors[0]->GetActorLocation() + FVector(0, 0, -50)));
	outActors.Empty();
	UGameplayStatics::GetAllActorsWithTag(this, "target", outActors);
	check(outActors.Num() == 1);
	targetPoint = MakeShareable(new FPoint(outActors[0]->GetActorLocation() + FVector(0, 0, -50)));


	// ��������openSet
	TSharedPtr<FPoint> start = MakeShareable(new FPoint(startPoint->pos));
	openQue.Enqueue(start);

	bFinding = true;
}
