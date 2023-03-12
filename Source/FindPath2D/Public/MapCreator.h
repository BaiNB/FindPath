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


  // ������ز���
  UPROPERTY(EditAnywhere)
  int mineNum = 10;
  UPROPERTY(EditAnywhere)
    UStaticMesh* mineMesh;
  UPROPERTY(EditAnywhere)
    UMaterialInterface* mineMaterial;
  void SpawnMineBetween(const FVector& start, const FVector& end);
  bool isExistMine = false;

  // �з��������
  TMap<FVector, int> enemyInfo; // <center,radius>
  // ��centerΪԲ�ģ�radiusΪ�뾶���һ��enemy����
  void AddEnemy(const FVector& center, int radius);
  // ��posλ��ĳһenemy�У�����pos����enemy��Ե�ľ���dist
  bool GetDistToEnemyEdge(const FVector& pos, float& dist, int& radius);
  // ����startPoint��targetPoint֮���Enemy�ܶ�
  void SetEnemyDensity();
  UPROPERTY(EditAnywhere)
  float enemyDensity = 0.2f;
  float GetEnemyDensity();

  //// �ϰ�����ر����ͺ���
  TArray<TArray<int>> minBarrierDist;
  UFUNCTION(BlueprintCallable, CallInEditor)
    void CreateMap();	// ������ͼ
  void CreateMinBarrierDist();
  UPROPERTY(EditAnywhere)
    float barrierDensity = 0.f;
  // ����startPoint��targetPoint֮����ϰ����ܶ�
    void SetBarrierDensity();

public:

  void LoadMapInfo();	// ��ȡ��ͼ��Ϣ

  // Save mapInfoArr to text.txt
  UFUNCTION(BlueprintCallable, CallInEditor)
    void SaveMapInfo();

  UFUNCTION(BlueprintCallable, CallInEditor)
    void SetStartTarget();

  float GetBarrierDensity(const FVector& start, const FVector& end);

private:
  float Distance(const FVector& pos1, const FVector& pos2);
};
