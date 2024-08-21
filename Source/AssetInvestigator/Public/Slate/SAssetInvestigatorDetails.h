// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


class UObjectPropertyBase;

class SAssetInvestigatorDetails final : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SAssetInvestigatorDetails) {}
	SLATE_ARGUMENT(FAssetData, AssetData);
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	void SetAssetData(const FAssetData& InData);

	TSharedRef<SWidget> CreateAssetListWidget(FString ListTitle, const TSharedPtr<SVerticalBox>& ListContent);

	TSharedRef<SVerticalBox> PopulateDependencyList();
	TSharedRef<SVerticalBox> PopulateReferenceList();

	FReply OpenAssetEditor(const FAssetIdentifier& Identifier);
	FReply OnOpenAssetClicked();

	FReply OnNodeReferenceClicked(UEdGraphNode* Node);
	FReply OnPropertyReferenceClicked(FObjectPropertyBase* Property, UBlueprint* Blueprint);
	
	TSharedRef<SWidget> GenerateComboBoxWidget(TSharedPtr<FString> InOption);

	
	void OnFilterChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type SelectInfo);
	FText GetCurrentFilterOption() const;


private:

	TArray<TSharedPtr<FString>> FilterOptions;
	FString CurrentFilter;
	
	TArray<FAssetIdentifier> Dependencies;
	TArray<FAssetIdentifier> References;

	TSharedPtr<SVerticalBox> ReferenceList;
	TSharedPtr<SVerticalBox> DependencyList;
	FAssetData AssetData;

	bool bFilterNativeClasses = false;

};
