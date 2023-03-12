// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "AStarOptimizeList.generated.h"

enum NodeFlags
{
	NODE_OPEN = 0x01,
	NODE_CLOSED = 0x02,
};

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarOptimizeList : public UFindPathBase
{
	GENERATED_BODY()
public:
	UAStarOptimizeList();

	virtual float Cost(const TSharedPtr<FPoint>& p) override;
	virtual void Start() override;
	virtual void FindStep() override;

	virtual void SetMethodName() override {
		methodName = "AStarList";
	}

	// 0:openSet无closeSet有; -1:openSet中已删除closeSet无; 2:openSet有closeSet有 1:openSet有closeSet无; 
	TMap<FVector, int> pointFlag;
	
};
