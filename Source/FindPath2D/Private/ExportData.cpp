// Fill out your copyright notice in the Description page of Project Settings.


#include "ExportData.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include <time.h>
#include <cstdlib>
#include <cstdio>
#include <fstream>

ExportData::ExportData()
{
  time_t timep;
  struct tm p;
  time(&timep);
  localtime_s(&p, &timep);
  char name[128]; // = std::to_string(1900 + p->tm_year);
  sprintf_s(name, "%sSaved/MyData/", TCHAR_TO_UTF8(*FPaths::ProjectDir()));
  this->pathHead += name;
  sprintf_s(name, "%d-%d %d ", 1 + p.tm_mon, p.tm_mday, p.tm_hour); // , (int)mapSize.X, (int)mapSize.Y, barrierDensity
  this->pathHead += name;

}

void ExportData::Init()
{
  isFindPath = false;
  allNodeNum = 0;
  iterNum = 0;
  pathNodeNum = 0;
  enemyNodeNum = -1;
  minBarrierDist = INT_MAX;
  insecureNodeNum = -1;
  findPathTime = 0.f;
}

void ExportData::Export()
{
  dataPath = pathHead;
  char name[128];
  sprintf_s(name, "%dx%d %.2f %.2f %s.csv", (int)mapSize.X, (int)mapSize.Y, barrierDensity, enemyDensity, methodName.c_str());
  this->dataPath += name;

  // 该文件是否存在
  bool isExist = false;
  std::ifstream ifs(dataPath);
  isExist = ifs.good();
  ifs.close();

  std::ofstream ofs(this->dataPath, std::ios::app);
  if (!ofs.is_open()) {
    ofs.close();
    return;
  }
  if (!isExist) {
    ofs << "methodName,isFindPath,iterNodeNum,iterNum,pathNodeNum,enemyNodeNum,minBarrierDist,insecureNodeNum,findPathTime"
      << std::endl;
  }
  ofs << methodName << "," << isFindPath << "," << this->allNodeNum << "," << this->iterNum << "," << this->pathNodeNum << ","
    << this->enemyNodeNum << "," << this->minBarrierDist << "," << this->insecureNodeNum << "," << this->findPathTime << std::endl;
  ofs.close();

}
