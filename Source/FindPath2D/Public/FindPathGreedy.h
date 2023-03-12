// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "FindPathGreedy.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UFindPathGreedy : public UFindPathBase
{
	GENERATED_BODY()
public:
	UFindPathGreedy();

	virtual void SetMethodName() override {
		methodName = "Greedy";
	}

	virtual float Cost(const TSharedPtr<FPoint>& p) override {
		return HnCost(p);
	}

};
