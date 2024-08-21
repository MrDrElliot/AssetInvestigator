// © 2024 DrElliot. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "AssetInvestigatorSubsystem.generated.h"


UCLASS()
class ASSETINVESTIGATOR_API UAssetInvestigatorSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	static UAssetInvestigatorSubsystem* Get() { return GEngine->GetEngineSubsystem<UAssetInvestigatorSubsystem>(); }
		
	static bool IsBlueprintClass(const FAssetIdentifier& AssetIdentifier);
	
};
