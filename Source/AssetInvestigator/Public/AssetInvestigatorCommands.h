// © 2024 DrElliot. All Rights Reserved.


#pragma once

#define LOCTEXT_NAMESPACE "Asset Investigator"

class FAssetInvestigatorCommands : public TCommands<FAssetInvestigatorCommands>
{
public:
	FAssetInvestigatorCommands()
		: TCommands(TEXT("AssetInvestigator"), NSLOCTEXT("Contexts", "AssetInvestigator", "Asset Investigator Plugin"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(OpenCommand, "Asset Investigator", "Open the Asset Investigator.", EUserInterfaceActionType::Button, FInputChord());
	}

	TSharedPtr<FUICommandInfo> OpenCommand;
};

#undef LOCTEXT_NAMESPACE
