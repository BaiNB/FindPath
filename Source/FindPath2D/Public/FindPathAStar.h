// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "FindPathAStar.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UFindPathAStar : public UFindPathBase
{
	GENERATED_BODY()
	
public:
	UFindPathAStar();

	virtual void SetMethodName() override {
		methodName = "AStar";

	}

	virtual float Cost(const TSharedPtr<FPoint>& p) {
		return GnCost(p) + HnCost(p);
	}

};
