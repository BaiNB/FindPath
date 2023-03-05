// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathAStar.h"
#include "AStarWithEnemy.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarWithEnemy : public UFindPathAStar
{
	GENERATED_BODY()

public:
	UAStarWithEnemy();

	virtual bool IsPassable(int idx) override;
	
};
