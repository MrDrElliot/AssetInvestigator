// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AssetInvestigatorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ASSETINVESTIGATOR_API UAssetInvestigatorSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	static UAssetInvestigatorSubsystem* Get() { return GEngine->GetEngineSubsystem<UAssetInvestigatorSubsystem>(); }
		
	static bool IsBlueprintClass(const FAssetIdentifier& AssetIdentifier);
	
};
