// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "SAssetItem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Misc/ScopedSlowTask.h"


class SAssetInvestigatorDetails;
class SAssetItem;

class SAssetInvestigator final : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAssetInvestigator) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	TSharedRef<SWidget> GenerateAssetList(FARFilter Filter);

	FReply OnAssetSelected(FAssetData Asset);
	FText GetCurrentSortOption() const;
	TSharedRef<SWidget> GenerateSortOptionWidget(TSharedPtr<FString> InOption);
	void OnSortOptionChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo);
	void OnSearchTextChanged(const FText& Text);

	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<SAssetItem> Item, const TSharedRef<STableViewBase>& OwnerTable);

	TArray<TSharedPtr<SAssetItem>>& GetMasterAssetItems() { return MasterAssetItems; }
private:
	
	TSharedPtr<SListView<TSharedPtr<SAssetItem>>> AssetList;
	TArray<TSharedPtr<SAssetItem>> AssetItems;
	TArray<TSharedPtr<SAssetItem>> MasterAssetItems;
	
	FAssetData SelectedAsset;
	TSharedPtr<SAssetInvestigatorDetails> DetailsPanel;
	TSharedPtr<FString> CurrentSortOption;
	TArray<TSharedPtr<FString>> SortOptions;

};
