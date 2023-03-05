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
  //UPROPERTY(VisibleAnywhere)
  //	USceneComponent* rootComp;

  TArray<FString> mapInfoArr;

  UPROPERTY(VisibleAnywhere)
    USceneComponent* rootComp;
  UPROPERTY(EditAnywhere)
  int mineNum = 5;
  UPROPERTY(EditAnywhere)
    UStaticMesh* mineMesh;
  UPROPERTY(EditAnywhere)
    UMaterialInterface* mineMaterial;


  void LoadMapInfo();	// ��ȡ��ͼ��Ϣ

  UFUNCTION(BlueprintCallable, CallInEditor)
  void AddingBattlefieldInfo(); // ����ս������Լ��

public:
  UFUNCTION(BlueprintCallable, CallInEditor)
    void CreateMap();	// ������ͼ

  UFUNCTION(BlueprintCallable, CallInEditor)
    void ExportNavMesh();	// ����NavMesh

};
