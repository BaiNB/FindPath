// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "AStarOptimizeSpeed.h"
#include "AStarOptimizeList.h"
#include "MultiObjectAStar.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (FindPath), meta = (BlueprintSpawnableComponent))
class FINDPATH2D_API UMultiObjectAStar : public UAStarOptimizeList
{
	GENERATED_BODY()
public:
	UMultiObjectAStar();

	virtual void SetMethodName() override {
		methodName = "MultiObjectAStar";
		methodName += FString::Printf(TEXT(" %.2f"), omega1);
		methodName += FString::Printf(TEXT(" %.2f"), omega2);
		methodName += FString::Printf(TEXT(" %.2f"), omega3);

	}

	virtual bool IsPassable(int idx) override;

	virtual float Cost(const TSharedPtr<FPoint>& p) override;

	UPROPERTY(EditAnywhere)
		float omega1 = 1.f;
	UPROPERTY(EditAnywhere)
		float omega2 = 0.f;
	UPROPERTY(EditAnywhere)
		float omega3 = 0.f;
	UPROPERTY(EditAnywhere)
		float gnFactor = 3.f;

	
};
