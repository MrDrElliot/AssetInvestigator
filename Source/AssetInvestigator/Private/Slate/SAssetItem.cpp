// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/SAssetItem.h"

#include "AssetRegistry/AssetRegistryModule.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetItem::Construct(const FArguments& InArgs)
{
	AssetData = InArgs._AssetData;
	OnButtonClicked = InArgs._OnButtonClicked;


	// Analyze asset references
	AnalyzeAssetReferences(FAssetIdentifier(AssetData.GetSoftObjectPath().GetLongPackageFName()));

	const bool bHasCircularDependency = HasCircularDependency(FAssetIdentifier(AssetData.GetSoftObjectPath().GetLongPackageFName()));
	
	// Get counts of dependencies and references
	const int32 DependencyCount = GetNumberOfDependencies();
	const int32 ReferenceCount = GetNumberOfReferences();
	// Construct UI
	ChildSlot
	[
		SNew(SButton)
		.ButtonStyle(FCoreStyle::Get(), "Button")
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.ContentPadding(FMargin(5, 3))
		.OnClicked(OnButtonClicked)
		[
			SNew(SHorizontalBox)
        
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5, 0)
			[
				SNew(SImage)
				.Image(FCoreStyle::Get().GetBrush("BlueprintDebugger.TabIcon"))
			]
        
			+ SHorizontalBox::Slot()
			.FillWidth(1.f)
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.Padding(10, 0)
			[
				SNew(STextBlock)
				.Text(FText::Format(
					NSLOCTEXT("AssetNamespace", "AssetInfo", "{0} - Dependencies: {1}, Referencers: {2}"),
					FText::FromName(AssetData.AssetName),
					FText::AsNumber(DependencyCount),
					FText::AsNumber(ReferenceCount)
				))
			]
		]
	];
}

void SAssetItem::AnalyzeAssetReferences(const FAssetIdentifier& Asset)
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	UE::AssetRegistry::FDependencyQuery DependencyQuery;
	DependencyQuery.Required = UE::AssetRegistry::EDependencyProperty::Hard;

	AssetRegistry.GetReferencers(Asset, References, UE::AssetRegistry::EDependencyCategory::Package, DependencyQuery);
	AssetRegistry.GetDependencies(Asset, Dependencies, UE::AssetRegistry::EDependencyCategory::Package, DependencyQuery);
	
}

bool SAssetItem::HasCircularDependency(const FAssetIdentifier& Asset)
{
	TArray<FAssetIdentifier> Visited;
	TArray<FAssetIdentifier> RecursionStack;

	return CheckForCycle(Asset, Visited, RecursionStack);
}

bool SAssetItem::CheckForCycle(const FAssetIdentifier& Asset, TArray<FAssetIdentifier>& Visited, TArray<FAssetIdentifier>& RecursionStack)
{
	if (!Visited.Contains(Asset))
	{
		Visited.Add(Asset);
		RecursionStack.Add(Asset);
		
		for (const FAssetIdentifier& Dep : Dependencies)
		{
			if (!Visited.Contains(Dep) && CheckForCycle(Dep, Visited, RecursionStack))
			{
				return true;
			}
			
			if(RecursionStack.Contains(Dep))
			{
				return true;
			}
		}
	}

	RecursionStack.Remove(Asset);
	return false;
}


int32 SAssetItem::GetNumberOfDependencies() const
{
	return Dependencies.Num();
}

int32 SAssetItem::GetNumberOfReferences() const
{
	return References.Num();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

