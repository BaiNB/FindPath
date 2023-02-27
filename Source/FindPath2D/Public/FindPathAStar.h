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

	//virtual bool CmpSamePoint(TSharedPtr<FPoint>& neibor, TSharedPtr<FPoint>& oldNeibor) {
	//	if (neibor->gnCost < oldNeibor->gnCost) {
	//		//oldNeibor->gnCost = neibor->gnCost;
	//		return true;
	//	}
	//	return false;
	//}

	virtual float Cost(const TSharedPtr<FPoint>& p) {
		return GnCost(p) + HnCost(p);
	}

};
