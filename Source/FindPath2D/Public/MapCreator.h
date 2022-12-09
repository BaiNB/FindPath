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
	void LoadMapInfo();	// 读取地图信息

public:
	UFUNCTION(BlueprintCallable, CallInEditor)
	void CreateMap();	// 创建地图
};
