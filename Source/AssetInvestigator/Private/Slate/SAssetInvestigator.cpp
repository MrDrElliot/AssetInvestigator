// © 2024 DrElliot. All Rights Reserved.


#include "Slate/SAssetInvestigator.h"

#include "AssetInvestigatorDevSettings.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"
#include "Slate/SAssetInvestigatorDetails.h"
#include "Slate/SAssetItem.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SAssetInvestigator::Construct(const FArguments& InArgs)
{
	SortOptions.Add(MakeShared<FString>(TEXT("Sort by Dependencies")));
    SortOptions.Add(MakeShared<FString>(TEXT("Sort by References")));

	// Set default sort option
	switch(UAssetInvestigatorDevSettings::Get()->SortingOption)
	{
	case EAssetInvestigatorSortingOption::Dependencies: CurrentSortOption = SortOptions[0];
		break;
	case EAssetInvestigatorSortingOption::References:	CurrentSortOption = SortOptions[1];
		break;
	}
	

	// Get a reference to the Asset Registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.PackagePaths.Add("/Game");
	
	
	GenerateAssetList(Filter);

	ChildSlot
	[
	    SNew(SVerticalBox)
	
	    + SVerticalBox::Slot()
	    .AutoHeight()
	    [
	        SNew(SBorder)
	        .BorderBackgroundColor(FLinearColor(0.1f, 0.1f, 0.1f, 1.0f)) // Dark grey border
	        .Padding(5)
	        [
	            SNew(SHorizontalBox)
	            + SHorizontalBox::Slot()
	            .AutoWidth()
	            .Padding(0, 0, 5, 0)
	            [
	                SNew(STextBlock)
	                .Text(FText::FromString(TEXT("Sort Assets By:")))
	                .Justification(ETextJustify::Right)
	            ]
	            + SHorizontalBox::Slot()
	            .AutoWidth()
	            .Padding(0, 0, 5, 0)
	            [
	                SNew(SComboBox<TSharedPtr<FString>>)
	                .OptionsSource(&SortOptions)
	                .OnSelectionChanged(this, &SAssetInvestigator::OnSortOptionChanged)
	                .OnGenerateWidget(this, &SAssetInvestigator::GenerateSortOptionWidget)
	                .ContentPadding(3)
	                [
	                    SNew(STextBlock)
	                    .Text(this, &SAssetInvestigator::GetCurrentSortOption)
	                ]
	            ]
	        ]
	    ]
	    + SVerticalBox::Slot()
	    .AutoHeight()
	    [
	        SNew(SEditableTextBox)
	        .OnTextChanged(this, &SAssetInvestigator::OnSearchTextChanged)
	        .HintText(FText::FromString(TEXT("Search assets...")))
	    ]
	    + SVerticalBox::Slot()
	    .FillHeight(1.0f) // Fill the remaining height
	    [
	        SNew(SSplitter)
	        .Orientation(Orient_Horizontal)
	        + SSplitter::Slot()
	        .Value(0.5f) // Proportion for the left panel
	        [
	            SNew(SScrollBox)
	            + SScrollBox::Slot()
	            [
	                SNew(SVerticalBox)
	                + SVerticalBox::Slot()
	                .AutoHeight()
	                [
	                    AssetList.ToSharedRef()
	                ]
	            ]
	        ]
	        + SSplitter::Slot()
	        .Value(0.5f)
	        [
	            SNew(SBorder)
	            .Padding(10.0f) // Reduced padding for better space utilization
	            [
	                SAssignNew(DetailsPanel, SAssetInvestigatorDetails)
	                .Visibility(EVisibility::Visible) // Ensure visibility adapts
	            ]
	        ]
	    ]
	];

	OnSortOptionChanged(CurrentSortOption, ESelectInfo::Type::Direct);
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


TSharedRef<SWidget> SAssetInvestigator::GenerateAssetList(FARFilter Filter)
{
	if (!AssetList.IsValid())
	{
		SAssignNew(AssetList, SListView<TSharedPtr<SAssetItem>>)
			.ItemHeight(24)
			.ListItemsSource(&AssetItems)
			.OnGenerateRow(this, &SAssetInvestigator::OnGenerateRowForList)
			.SelectionMode(ESelectionMode::Single);
	}

	AssetItems.Empty(); // Clear existing items

	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> TempAssetList;
	AssetRegistryModule.Get().GetAssets(Filter, TempAssetList);

	const float TotalWorkUnits = TempAssetList.Num();
	FScopedSlowTask SlowTask(TotalWorkUnits, FText::FromString("Generating Asset Lists..."));
	SlowTask.MakeDialog();

	for (const FAssetData& Asset : TempAssetList)
	{
		if (!Asset.IsUAsset()) continue; //Ignore redirectors and things.
        
		TSharedPtr<SAssetItem> NewItem = SNew(SAssetItem).AssetData(Asset);
		MasterAssetItems.Add(NewItem);
		SlowTask.EnterProgressFrame(1);
	}
	AssetItems = MasterAssetItems;
	AssetList->RequestListRefresh();

	return AssetList.ToSharedRef();
}

FReply SAssetInvestigator::OnAssetSelected(FAssetData Asset)
{
	SelectedAsset = Asset;
	DetailsPanel->SetAssetData(SelectedAsset);
	return FReply::Handled();
}

FText SAssetInvestigator::GetCurrentSortOption() const
{
	if (CurrentSortOption.IsValid())
	{
		return FText::FromString(*CurrentSortOption);
	}
	return FText::FromString("Select Sort Option");
}

TSharedRef<SWidget> SAssetInvestigator::GenerateSortOptionWidget(TSharedPtr<FString> InOption)
{
	return SNew(STextBlock).Text(FText::FromString(*InOption));
}

void SAssetInvestigator::OnSortOptionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo)
{
	CurrentSortOption = NewValue;
	
	if (!CurrentSortOption.IsValid())
		return;

	if (*CurrentSortOption == "Sort by Dependencies")
	{
		UAssetInvestigatorDevSettings::Get()->SortingOption = EAssetInvestigatorSortingOption::Dependencies;
		AssetItems.Sort([](const TSharedPtr<SAssetItem>& A, const TSharedPtr<SAssetItem>& B)
		{
			return A->GetNumberOfDependencies() > B->GetNumberOfDependencies(); // Reverse the sort order
		});
	}
	else if (*CurrentSortOption == "Sort by References")
	{
		UAssetInvestigatorDevSettings::Get()->SortingOption = EAssetInvestigatorSortingOption::References;
		AssetItems.Sort([](const TSharedPtr<SAssetItem>& A, const TSharedPtr<SAssetItem>& B)
		{
			return A->GetNumberOfReferences() > B->GetNumberOfReferences(); // Reverse the sort order
		});
	}

	UAssetInvestigatorDevSettings::Save();

	// Refresh the list display to reflect the new sort order
	if (AssetList.IsValid())
	{
		AssetList->RequestListRefresh();
	}
}

void SAssetInvestigator::OnSearchTextChanged(const FText& Text)
{
	FString SearchString = Text.ToString().TrimStartAndEnd(); // Trim whitespace for better accuracy
	if (SearchString.IsEmpty())
	{
		AssetItems = MasterAssetItems; // Reset list if search text is empty
	}
	else
	{
		FName SearchName(*SearchString); // Convert string to FName for comparison

		TArray<TSharedPtr<SAssetItem>> FilteredItems;
		for (const TSharedPtr<SAssetItem>& Item : MasterAssetItems) // Ensure you iterate over MasterAssetItems
		{
			// Use Contains to check if AssetName contains the SearchName (case insensitive)
			if (Item->GetAssetData().AssetName.ToString().Contains(SearchString, ESearchCase::IgnoreCase))
			{
				FilteredItems.Add(Item);
			}
		}

		AssetItems = FilteredItems; // Replace the current list with filtered items
	}

	AssetList->RequestListRefresh(); // Refresh the list view
}

TSharedRef<ITableRow> SAssetInvestigator::OnGenerateRowForList(TSharedPtr<SAssetItem> Item, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow<TSharedPtr<SAssetItem>>, OwnerTable)
	[
		SAssignNew(Item, SAssetItem)
		.AssetData(Item->GetAssetData())
		.OnButtonClicked(this, &SAssetInvestigator::OnAssetSelected, Item->GetAssetData())
	];
}

