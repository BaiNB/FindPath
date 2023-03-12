// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "FindPathBFS.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UFindPathBFS : public UFindPathBase
{
	GENERATED_BODY()
public:
	UFindPathBFS();


	virtual void FindStep() override;
	virtual void Start() override;

	TQueue<TSharedPtr<FPoint>> openQue;
	
};
