// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHUD.h"

void UBossHUD::UpdateBar(float CurrentHealth, float MaxHealth)
{
	float Percent = (MaxHealth > 0.f) ? (CurrentHealth / MaxHealth) : 0.f;
	OnHealthUpdated(Percent);
}

void UBossHUD::SetBossName(const FText& Name)
{
	OnNameSet(Name);
}