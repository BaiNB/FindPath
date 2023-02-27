// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ArrowComponent.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FindPathBase.generated.h"


USTRUCT()
struct FPoint {
	GENERATED_BODY()
public:
	FVector pos;
	float gnCost = 0.0f;
	float hnCost = 0.0f;
	TSharedPtr<FPoint> parent;

	FPoint() : pos(FVector::ZeroVector), parent(nullptr) {

	}

	FPoint(const FVector& loc) : pos(loc), parent(nullptr) {

	}

	bool operator==(const FPoint& rhs) { return pos.Equals(rhs.pos, 1.0f); }

};

static bool operator==(const TSharedPtr<FPoint>& a, const TSharedPtr<FPoint>& b) {
	return  a->pos.Equals(b->pos, 1.0f);
}

UCLASS( ClassGroup=(FindPath), meta=(BlueprintSpawnableComponent) )
class FINDPATH2D_API UFindPathBase : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFindPathBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	TArray<TSharedPtr<FPoint>> openSet;
	TArray<TSharedPtr<FPoint>> closeSet;
	TSharedPtr<FPoint> startPoint;
	TSharedPtr<FPoint> targetPoint;
	TSharedPtr<FPoint> currPoint;

	// 10---障碍物; 20---可通行; 100/200---enemy; 40---地雷; 
	TArray<FString> mapInfoArr;
	FTimerHandle timerHandle;
	bool bFinding;

public:
	UFUNCTION(BlueprintCallable)
		virtual void FindPath();
	UFUNCTION(BlueprintCallable)
		virtual void FindPathOnce(float deltaTime);

	virtual void Start();
	virtual void FindStep();
	virtual void FinishFind();

	void BuildPath();

	bool IsValid(int x, int y);

	virtual bool IsPassable(int idx);

	virtual bool CmpSamePoint(TSharedPtr<FPoint>& neibor, TSharedPtr<FPoint>& oldNeibor) {
		if (neibor->gnCost < oldNeibor->gnCost) {
			//oldNeibor->gnCost = neibor->gnCost;
			return true;
		}
		return false;

	}

	virtual float GnCost(const TSharedPtr<FPoint>& p) {
		return p->gnCost;
	}
	virtual float HnCost(const TSharedPtr<FPoint>& p) {
		return p->hnCost;
	}
	float Distance(const TSharedPtr<FPoint>& p1, const TSharedPtr<FPoint>& p2) {
		return FMath::Abs(p1->pos.X - p2->pos.X) + FMath::Abs(p1->pos.Y - p2->pos.Y);
	}

	virtual float Cost(const TSharedPtr<FPoint>& p) {
		return 0;
	}
private:
	void PopSmallestPoint(TSharedPtr<FPoint>& outPoint, TArray<TSharedPtr<FPoint>>& set);

};
