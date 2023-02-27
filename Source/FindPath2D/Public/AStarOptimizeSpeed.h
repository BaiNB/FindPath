// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"

#include "AStarOptimizeSpeed.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarOptimizeSpeed : public UFindPathBase
{
	GENERATED_BODY()

public:
	UAStarOptimizeSpeed();

	virtual float Cost(const TSharedPtr<FPoint>& p) override;
	virtual void Start() override;
	virtual void FindStep() override;

	// 0:openSet无closeSet有; -1:openSet中已删除closeSet无; 2:openSet有closeSet有 1:openSet有closeSet无; 
	TMap<FVector, int> pointFlag;
	float theta = 0.2;
};
