// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <iostream>
#include <vector>
#include <string>
/**
 * 
 */
class ExportData
{
public:
  ExportData();
  void Init();
  void Export();

  bool isFindPath = false;
  int allNodeNum = 0;
  int iterNum = 0;
  int pathNodeNum = 0;
  int enemyNodeNum = -1;
  int minBarrierDist = INT_MAX;
  int insecureNodeNum = -1; // 当障碍物距离小于carSize的节点数量
  float findPathTime = 0.f;
  FVector2D mapSize;
  float barrierDensity = 0.f;
  float enemyDensity = 0.f;
  
  //int exeNum = 0;
  std::string methodName;
  std::string dataPath;
  std::string pathHead;
};