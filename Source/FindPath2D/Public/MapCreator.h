// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapCreator.generated.h"

UCLASS()
class FINDPATH2D_API AMapCreator : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AMapCreator();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;


public:
  TArray<FString> mapInfoArr;

  UPROPERTY(VisibleAnywhere)
    USceneComponent* rootComp;

  UPROPERTY(EditAnywhere)
  FVector startPos;
  UPROPERTY(EditAnywhere)
  FVector targetPos;


  // 地雷相关参数
  UPROPERTY(EditAnywhere)
  int mineNum = 10;
  UPROPERTY(EditAnywhere)
    UStaticMesh* mineMesh;
  UPROPERTY(EditAnywhere)
    UMaterialInterface* mineMaterial;
  void SpawnMineBetween(const FVector& start, const FVector& end);
  bool isExistMine = false;

  // 敌方区域相关
  TMap<FVector, int> enemyInfo; // <center,radius>
  // 以center为圆心，radius为半径添加一个enemy区域
  void AddEnemy(const FVector& center, int radius);
  // 若pos位于某一enemy中，返回pos到该enemy边缘的距离dist
  bool GetDistToEnemyEdge(const FVector& pos, float& dist, int& radius);
  // 设置startPoint到targetPoint之间的Enemy密度
  void SetEnemyDensity();
  UPROPERTY(EditAnywhere)
  float enemyDensity = 0.2f;
  float GetEnemyDensity();

  //// 障碍物相关变量和函数
  TArray<TArray<int>> minBarrierDist;
  UFUNCTION(BlueprintCallable, CallInEditor)
    void CreateMap();	// 创建地图
  void CreateMinBarrierDist();
  UPROPERTY(EditAnywhere)
    float barrierDensity = 0.f;
  // 设置startPoint到targetPoint之间的障碍物密度
    void SetBarrierDensity();

public:

  void LoadMapInfo();	// 读取地图信息

  // Save mapInfoArr to text.txt
  UFUNCTION(BlueprintCallable, CallInEditor)
    void SaveMapInfo();

  UFUNCTION(BlueprintCallable, CallInEditor)
    void SetStartTarget();

  float GetBarrierDensity(const FVector& start, const FVector& end);

private:
  float Distance(const FVector& pos1, const FVector& pos2);
};
