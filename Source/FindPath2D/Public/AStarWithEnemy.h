// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FindPathBase.h"
#include "AStarWithEnemy.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UAStarWithEnemy : public UFindPathBase
{
	GENERATED_BODY()

public:
	UAStarWithEnemy();

	virtual void SetMethodName() override {
		methodName = "AStarWithEnemy";
		methodName += FString::Printf(TEXT(" %.2f"), omega1);
		methodName += FString::Printf(TEXT(" %.2f"), omega2);
		methodName += FString::Printf(TEXT(" %.2f"), omega3);

	}

	virtual bool IsPassable(int idx) override;

	virtual float Cost(const TSharedPtr<FPoint>& p) override;

	virtual void FindPath() override;

	UPROPERTY(EditAnywhere)
	float omega1 = 1.f;
	UPROPERTY(EditAnywhere)
	float omega2 = 0.f;
	UPROPERTY(EditAnywhere)
	float omega3 = 0.f;
	UPROPERTY(EditAnywhere)
	float gnFactor = 3.f;

};
