// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "AStarWithWeightCost.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarWithWeightCost : public UFindPathBase
{
	GENERATED_BODY()
public:
  UAStarWithWeightCost();

  virtual void SetMethodName() override {
    methodName = "AStarWithCost";
  }

  virtual float Cost(const TSharedPtr<FPoint>& p) override;

  float alpha = 0.2;

};
