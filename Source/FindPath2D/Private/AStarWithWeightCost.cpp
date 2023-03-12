// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarWithWeightCost.h"

UAStarWithWeightCost::UAStarWithWeightCost() {
  ComponentTags.Add("AStarWithCost");

}

float UAStarWithWeightCost::Cost(const TSharedPtr<FPoint>& p)
{
  float t = GnCost(p) / HnCost(p);
  float k = pow(alpha, t);
  return k * GnCost(p) + (1 - k) * HnCost(p);
}
