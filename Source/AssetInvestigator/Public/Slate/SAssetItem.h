// © 2024 DrElliot. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SAssetItem final : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SAssetItem)
	{
		_AssetData = nullptr;
	}
	SLATE_ARGUMENT(FAssetData, AssetData)
	SLATE_EVENT(FOnClicked, OnButtonClicked)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void AnalyzeAssetReferences(const FAssetIdentifier& Asset);
	bool HasCircularDependency(const FAssetIdentifier& Asset);
	bool CheckForCycle(const FAssetIdentifier& Asset, TArray<FAssetIdentifier>& Visited, TArray<FAssetIdentifier>& RecursionStack);

	int32 GetNumberOfDependencies() const;
	int32 GetNumberOfReferences() const;
	
	TArray<FAssetIdentifier> GetDependencies() const { return Dependencies; }
	TArray<FAssetIdentifier> GetReferences() const { return References; }
	FAssetData GetAssetData() const { return AssetData; }

	

private:
	
	FAssetData AssetData;
	TArray<FAssetIdentifier> Dependencies;
	TArray<FAssetIdentifier> References;

	
	FOnClicked OnButtonClicked;
};