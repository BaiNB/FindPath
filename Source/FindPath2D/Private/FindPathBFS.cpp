// Fill out your copyright notice in the Description page of Project Settings.


#include "FindPathBFS.h"

UFindPathBFS::UFindPathBFS() {
	ComponentTags.Add("BFS");
}

void UFindPathBFS::FindStep()
{
	UE_LOG(LogTemp, Warning, TEXT("This is FindPathBFS::FindStep"));
	this->bFinding = false;
}
