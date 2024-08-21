// © 2024 DrElliot. All Rights Reserved.


// DrElliot

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AssetInvestigatorDevSettings.generated.h"

UENUM(BlueprintType)
enum class EAssetInvestigatorSortingOption : uint8
{
	Dependencies,
	References,
};

/**
 * 
 */
UCLASS(config=EditorPerProjectUserSettings, meta=(DisplayName="Asset Investigator Settings"))
class ASSETINVESTIGATOR_API UAssetInvestigatorDevSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	static UAssetInvestigatorDevSettings* Get() { return GetMutableDefault<UAssetInvestigatorDevSettings>(); }
	static void Save() { Get()->SaveConfig(); }
	UPROPERTY(Config)
	EAssetInvestigatorSortingOption SortingOption;

	
};
