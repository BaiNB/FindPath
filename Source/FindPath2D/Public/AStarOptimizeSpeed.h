// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarOptimizeList.h"

#include "AStarOptimizeSpeed.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarOptimizeSpeed : public UAStarOptimizeList
{
	GENERATED_BODY()

public:
	UAStarOptimizeSpeed();

	virtual void SetMethodName() override {
		methodName = "SpeedAStar";
	}

  virtual float Cost(const TSharedPtr<FPoint>& p) override;

  float alpha = 0.1;
};
