// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "FindPathDijkstra.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UFindPathDijkstra : public UFindPathBase
{
	GENERATED_BODY()
public:
	UFindPathDijkstra();

	virtual void SetMethodName() override {
		methodName = "Dijkstra";
	}

	virtual float Cost(const TSharedPtr<FPoint>& p) override {
		return GnCost(p);
	}

};
