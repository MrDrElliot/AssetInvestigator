// © 2024 DrElliot. All Rights Reserved.


#include "AssetInvestigatorSubsystem.h"

#include "AssetRegistry/AssetRegistryModule.h"

bool UAssetInvestigatorSubsystem::IsBlueprintClass(const FAssetIdentifier& AssetIdentifier)
{
	// Ensure the AssetRegistryModule is loaded
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Use FSoftObjectPath to manage asset paths
	FSoftObjectPath AssetPath(AssetIdentifier.ObjectName);
    
	// Retrieve FAssetData using the FSoftObjectPath
	const FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(AssetPath);
	if (!AssetData.IsValid())
	{
		// Asset not found
		return false;
	}

	// Check if the asset is a Blueprint class
	if (AssetData.AssetClassPath.GetAssetName() == FName(TEXT("Blueprint")))
	{
		// Optionally, check if it's a specific type of Blueprint, such as an Actor
		UObject* Asset = AssetData.GetAsset();
		const UBlueprint* Blueprint = Cast<UBlueprint>(Asset);

		if (Blueprint && Blueprint->GeneratedClass)
		{
			return true;
		}
	}

	return false;
}
