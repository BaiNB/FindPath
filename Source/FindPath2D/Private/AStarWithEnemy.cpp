// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarWithEnemy.h"

UAStarWithEnemy::UAStarWithEnemy() {
  ComponentTags.Add("AStarWithEnemy");
}

bool UAStarWithEnemy::IsPassable(int idx)
{
  return mapInfoArr[idx].Equals("20");
}
