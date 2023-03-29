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

	if (mapInfoArr.Num() == 0) {
		LoadMapInfo();
		SetBarrierDensity();
	}
}

// Called every frame
void AMapCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMapCreator::SpawnMineBetween(const FVector& start, const FVector& end)
{
	// ��(start,end)�������mineNum������
	check((start.X != end.X) && (start.Y != end.Y));

	int startXIdx = start.X / 100;
	int startYIdx = start.Y / 100;
	int endXIdx = end.X / 100;
	int endYIdx = end.Y / 100;
	FVector2D startIdx(FMath::Min(startXIdx, endXIdx) + 1, FMath::Min(startYIdx, endYIdx) + 1);
	FVector2D endIdx(FMath::Max(startXIdx, endXIdx) - 1, FMath::Max(startYIdx, endYIdx) - 1);

	int num = 0;
	while (num < mineNum) {
		// �������(x,y)
		int32 randomX = UKismetMathLibrary::RandomInteger(endIdx.X - startIdx.X + 1) + startIdx.X;
		int32 randomY = UKismetMathLibrary::RandomInteger(endIdx.Y - startIdx.Y + 1) + startIdx.Y;
		auto flag = mapInfoArr[randomX * 75 + randomY];
		if (flag.Equals("10")) {
			continue;
		}
		if ((randomX == startIdx.X && randomY == startIdx.Y) || (randomX == endIdx.X && randomY == endIdx.Y)) {
			// �޳�start��end
			continue;
		}
		// ��(x,y)���ɵ���
		FVector location = FVector(randomX * 100, randomY * 100, 0);
		FTransform transform(FRotator::ZeroRotator, location, FVector::OneVector);
		UStaticMeshComponent* smComp = NewObject<UStaticMeshComponent>(this);
		smComp->SetupAttachment(rootComp);
		smComp->RegisterComponent();
		smComp->SetRelativeTransform(transform);
		smComp->SetStaticMesh(mineMesh);
		smComp->SetMaterial(0, mineMaterial);

		// �Ե��׽��б�ʶ
		mapInfoArr[randomX * 75 + randomY] = "30";
		++num;
	}
	isExistMine = true;

}

void AMapCreator::AddEnemy(const FVector& center, int radius)
{
	if (enemyInfo.Contains(center) && enemyInfo[center] == radius) {
		return;
	}
	enemyInfo.Emplace(center, radius);
}

bool AMapCreator::GetDistToEnemyEdge(const FVector& pos, float& dist, int& radius)
{
	bool isInEnemy = false;
	float minDist = MAX_FLT;
	int minRadius = 0;
	for (const auto& centerRadius : enemyInfo) {
		float currDist = Distance(pos, centerRadius.Key);
		if (currDist > centerRadius.Value) {
			continue;
		}
		if (!isInEnemy || (isInEnemy && currDist < minDist)) {
			isInEnemy = true;
			minDist = currDist;
			minRadius = centerRadius.Value;
		}
	}
	if (isInEnemy) {
		dist = minDist;
		radius = minRadius;
	}
	return isInEnemy;
}

void AMapCreator::SetEnemyDensity()
{
	float area = FMath::Abs(startPos.X - targetPos.X) * FMath::Abs(startPos.Y - targetPos.Y);
	float r = FMath::Sqrt(area * enemyDensity / PI);
	FVector center = FVector((startPos.X + targetPos.X) / 2, (startPos.Y + targetPos.Y) / 2, 0);
	this->AddEnemy(center, r);
}

float AMapCreator::GetEnemyDensity()
{
	return this->enemyDensity;
}

void AMapCreator::CreateMinBarrierDist()
{
	TArray<int> rowInit;
	rowInit.Init(INT32_MAX, 75);
	minBarrierDist.Init(rowInit, 75);

	// ����
	for (int i = 0; i < 75; ++i) {
		for (int j = 0; j < 75; ++j) {
			if (!mapInfoArr[i * 75 + j].Equals("20")) {
				continue;
			}

			int neiborDist = INT32_MAX;
			TArray<int> dirX = { -1, 0, 1, 0 };
			TArray<int> dirY = { 0, -1, 0, 1 };
			// dist(n) = min{dist(n-1)}+dist(n-1, n)
			for (int k = 0; k < dirX.Num(); ++k) {
				int neiborX = i + dirX[k], neiborY = j + dirY[k];
				if (neiborX < 0 || neiborX >= 75 || neiborY < 0 || neiborY >= 75) {
					continue;
				}
				if (!mapInfoArr[neiborX * 75 + neiborY].Equals("20")) {
					// neiborΪ�ǿ���������
					minBarrierDist[i][j] = 1;
					break;
				}
				if (minBarrierDist[neiborX][neiborY] < neiborDist) {
					neiborDist = minBarrierDist[neiborX][neiborY];
					minBarrierDist[i][j] = neiborDist + 1;
				}
			}

		}
	}
	// ����
	for (int i = 74; i >= 0; --i) {
		for (int j = 74; j >= 0; --j) {
			if (!mapInfoArr[i * 75 + j].Equals("20")) {
				continue;
			}

			int neiborDist = INT32_MAX;
			TArray<int> dirX = { -1, 0, 1, 0 };
			TArray<int> dirY = { 0, -1, 0, 1 };
			// dist(n) = min{dist(n-1)}+dist(n-1, n)
			for (int k = 0; k < dirX.Num(); ++k) {
				int neiborX = i + dirX[k], neiborY = j + dirY[k];
				if (neiborX < 0 || neiborX >= 75 || neiborY < 0 || neiborY >= 75) {
					continue;
				}
				if (!mapInfoArr[neiborX * 75 + neiborY].Equals("20")) {
					// neiborΪ�ǿ���������
					minBarrierDist[i][j] = 1;
					break;
				}
				if (minBarrierDist[neiborX][neiborY] < neiborDist) {
					neiborDist = minBarrierDist[neiborX][neiborY];
					minBarrierDist[i][j] = neiborDist + 1;
				}
			}

		}
	}
}

void AMapCreator::LoadMapInfo()
{
	FString path = FPaths::ProjectDir() + FString("5000x5000.txt");
	FFileHelper::LoadFileToStringArray(mapInfoArr, *path);

}

void AMapCreator::SetBarrierDensity()
{
	int startXIdx = startPos.X / 100;
	int startYIdx = startPos.Y / 100;
	int targetXIdx = targetPos.X / 100;
	int targetYIdx = targetPos.Y / 100;
	FVector2D startIdx(FMath::Min(startXIdx, targetXIdx), FMath::Min(startYIdx, targetYIdx));
	FVector2D endIdx(FMath::Max(startXIdx, targetXIdx), FMath::Max(startYIdx, targetYIdx));

	int sum = (endIdx.X - startIdx.X + 1) * (endIdx.Y - startIdx.Y + 1);

	float currDensity = GetBarrierDensity(startPos, targetPos);
  if (currDensity < barrierDensity) {
    // m = (�� - С) * ��
		// ��Ҫ���m��prop
    int m = (barrierDensity - currDensity) * sum;
    int n = 0;
    while (n < m) {
      // �������(x,y)
      int32 randomX = UKismetMathLibrary::RandomInteger(endIdx.X - startIdx.X + 1) + startIdx.X;
      int32 randomY = UKismetMathLibrary::RandomInteger(endIdx.Y - startIdx.Y + 1) + startIdx.Y;
      auto flag = mapInfoArr[randomX * 75 + randomY];
      if (!flag.Equals("20")) {
        continue;
      }
			if ((randomX == startIdx.X && randomY == startIdx.Y) || (randomX == endIdx.X && randomY == endIdx.Y)) {
				// �޳�start��end
				continue;
			}

      // ��(x,y)�����ϰ���
      FVector location = FVector(randomX * 100, randomY * 100, 0);
      FTransform transform(FRotator::ZeroRotator, location, FVector::OneVector);
      // ����props
			UStaticMeshComponent* smComp = NewObject<UStaticMeshComponent>(this);
      smComp->SetupAttachment(rootComp);
      smComp->RegisterComponent();
      smComp->SetRelativeTransform(transform);
      static auto* mesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/FindPath/Materials/SM_Cube.SM_Cube'"));
      smComp->SetStaticMesh(mesh);

      // ���ϰ�����б�ʶ
      mapInfoArr[randomX * 75 + randomY] = "10";
      ++n;
    }

  }


}

void AMapCreator::SaveMapInfo()
{

	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProp::StaticClass(), outActors);

	int mapSize = 75 * 75;
	// ����prop���Ϊ10�����඼���Ϊ20
	mapInfoArr.Init("20", mapSize);
	for (const auto& prop : outActors) {
		auto propPos = prop->GetActorLocation();
		int xIdx = propPos.X / 100;
		int yIdx = propPos.Y / 100;
		mapInfoArr[xIdx * 75 + yIdx] = "10";
	}

	FString path = FPaths::ProjectDir() + FString("test.txt");
	FFileHelper::SaveStringArrayToFile(mapInfoArr, *path);
	mapInfoArr.Empty();
}

void AMapCreator::CreateMap()
{
	//TArray<AActor*> outActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProp::StaticClass(), outActors);
	//int barrierSize = 75 * 75;
	//if (outActors.Num() == barrierSize) {
	//	UE_LOG(LogTemp, Warning, TEXT("Map has been created."));
	//	return;
	//}

	LoadMapInfo();


	// ʹ��GEditor��Ҫ��Build.cs�����UnrealEdģ��
	if (mapInfoArr.Num() == 0 || GEditor == nullptr) {
		return;
	}

	UWorld* world = GEditor->GetAllViewportClients()[0]->GetWorld();
	//UWorld* world = GetWorld();
	ULevel* level = world->GetLevel(0);

	int startXIdx = startPos.X / 100;
	int startYIdx = startPos.Y / 100;
	int targetXIdx = targetPos.X / 100;
	int targetYIdx = targetPos.Y / 100;
	FVector2D startIdx(FMath::Min(startXIdx, targetXIdx), FMath::Min(startYIdx, targetYIdx));
	FVector2D targetIdx(FMath::Max(startXIdx, targetXIdx), FMath::Max(startYIdx, targetYIdx));

	// ��startPos��targetPos����֮���ڱ༭���������ϰ������֮����SetBarrierDensity����������
	for (int i = 0; i < mapInfoArr.Num(); ++i) { // mapInfoArr.Num() / 2
		if (mapInfoArr[i] == "20") {
			continue;
		}
		int row = i / 75;
		int col = i % 75;
		if ((row == startIdx.X && col == startIdx.Y) || (row == targetIdx.X && col == targetIdx.Y)) {
			continue;
		}
		if (row > startIdx.X && row < targetIdx.X && col > startIdx.Y && col < targetIdx.Y) {
			// �����ڵ��ϰ�������ʱ�������
			continue;
		}

		// Calc prop's transform
		FVector location = FVector(row * 100, col * 100, 0);
		FTransform transform(FRotator::ZeroRotator, location, FVector::OneVector);

		// ��Editor������props
		check(AProp::StaticClass() != nullptr);
		AActor* prop = GEditor->AddActor(level, AProp::StaticClass(), transform);
		//GEditor->removeactor
		prop->SetActorLabel(FString::Printf(TEXT("%s-%d-%d"), *mapInfoArr[i], row, col));
		prop->Tags.Add(*mapInfoArr[i]);

		prop->RegisterAllComponents();
		prop->RerunConstructionScripts();

		//// ����ʱ����props
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

void AMapCreator::SetStartTarget()
{
	TArray<AActor*> outActors;
	UGameplayStatics::GetAllActorsWithTag(this, "start", outActors);
	check(outActors.Num() == 1);
	startPos = outActors[0]->GetActorLocation();
	outActors.Empty();
	UGameplayStatics::GetAllActorsWithTag(this, "target", outActors);
	check(outActors.Num() == 1);
	targetPos = outActors[0]->GetActorLocation();

}

float AMapCreator::GetBarrierDensity(const FVector& start, const FVector& end)
{
	check((start.X != end.X) && (start.Y != end.Y));

	int startXIdx = start.X / 100;
	int startYIdx = start.Y / 100;
	int endXIdx = end.X / 100;
	int endYIdx = end.Y / 100;
	FVector2D startIdx(FMath::Min(startXIdx, endXIdx), FMath::Min(startYIdx, endYIdx));
	FVector2D endIdx(FMath::Max(startXIdx, endXIdx), FMath::Max(startYIdx, endYIdx));

	int sum = (endIdx.X - startIdx.X + 1) * (endIdx.Y - startIdx.Y + 1);
	int bNum = 0;
	for (int i = startIdx.X; i <= endIdx.X; ++i) {
		for (int j = startIdx.Y; j <= endIdx.Y; ++j) {
			if (mapInfoArr[i * 75 + j].Equals("20")) {
				continue;
			}
			++bNum;
		}
	}
	return (float)bNum / sum;
}

float AMapCreator::Distance(const FVector& pos1, const FVector& pos2)
{
	return FMath::Abs(pos1.X - pos2.X) + FMath::Abs(pos1.Y - pos2.Y);
}

