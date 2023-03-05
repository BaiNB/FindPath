// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarWithEnemySpeedUp.h"

UAStarWithEnemySpeedUp::UAStarWithEnemySpeedUp() {
  ComponentTags.Add("AStarWithEnemySpeedUp");
}

bool UAStarWithEnemySpeedUp::IsPassable(int idx)
{
  return mapInfoArr[idx].Equals("20");
}
