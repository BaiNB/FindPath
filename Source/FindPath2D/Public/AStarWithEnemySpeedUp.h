// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AStarOptimizeSpeed.h"
#include "AStarWithEnemySpeedUp.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarWithEnemySpeedUp : public UAStarOptimizeSpeed
{
	GENERATED_BODY()
public:
	UAStarWithEnemySpeedUp();

	virtual bool IsPassable(int idx) override;
	
};
