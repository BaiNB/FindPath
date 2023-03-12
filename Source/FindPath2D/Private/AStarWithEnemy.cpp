// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarWithEnemy.h"


UAStarWithEnemy::UAStarWithEnemy() {
  ComponentTags.Add("AStarWithEnemy");

}

bool UAStarWithEnemy::IsPassable(int idx)
{
  return mapInfoArr[idx].Equals("20");
}

float UAStarWithEnemy::Cost(const TSharedPtr<FPoint>& p)
{
  //return GnCost(p) + HnCost(p);
  check(this->mapCreator->minBarrierDist.Num() != 0);
  float safety = 0.f, invisibility = 0.f;
  check(gnFactor != 1.f);
  float efficiency = omega3 * (GnCost(p) - this->Distance(p, startPoint)) / ((gnFactor - 1) * this->Distance(p, startPoint));
  //float efficiency = omega3 * GnCost(p)/ this->Distance(p, startPoint);
  int x = p->pos.X / 100, y = p->pos.Y / 100;
  float minDist = this->mapCreator->minBarrierDist[x][y] * 200 - 100;
  if (minDist <= carSize) {
    safety = omega1 * (1 - minDist / carSize);
  }
  int radius = 0;
  bool isInEnemy = this->mapCreator->GetDistToEnemyEdge(p->pos, minDist, radius);
  if (isInEnemy) {
    check(radius != 0);
    invisibility = omega2 * minDist / radius;
  }

  float hn = HnCost(p) / this->Distance(startPoint, targetPoint);
  //float hn = HnCost(p) / this->Distance(startPoint, targetPoint);
  return safety + invisibility + efficiency + hn;
}

void UAStarWithEnemy::FindPath()
{
  UFindPathBase::FindPath();
}
