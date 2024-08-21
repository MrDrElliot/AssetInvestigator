// © 2024 DrElliot. All Rights Reserved.

#include "AssetInvestigator.h"

#include "ToolMenus.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Slate/SAssetInvestigator.h"

#define LOCTEXT_NAMESPACE "FAssetInvestigatorModule"

void FAssetInvestigatorModule::StartupModule()
{
	if(FSlateApplication::IsInitialized())
	{
		UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAssetInvestigatorModule::RegisterGameEditorMenus));
	}
}

void FAssetInvestigatorModule::ShutdownModule()
{
	if (FSlateApplication::IsInitialized())
	{
		FGlobalTabmanager::Get()->UnregisterNomadTabSpawner("AssetInvestigatorTab");
	}

	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

}

void FAssetInvestigatorModule::RegisterGameEditorMenus()
{
	UE_LOG(LogTemp, Error, TEXT("Creating Asset Investigator Tab"));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner( FName(TEXT("AssetInvestigatorTab")), FOnSpawnTab::CreateRaw(this, &FAssetInvestigatorModule::OpenAssetInvestigator))
		.SetDisplayName(NSLOCTEXT("FAssetInvestigatorModule", "AssetInvestigatorTitle", "Asset Investigator"))
		.SetTooltipText(NSLOCTEXT("FAssetInvestigatorModule", "AssetInvestigatorTooltipText", "Open Asset Investigator"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
		.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "Symbols.SearchGlass"));

}

TSharedRef<SDockTab> FAssetInvestigatorModule::OpenAssetInvestigator(const FSpawnTabArgs& Args)
{
	return SAssignNew(AssetInvestigatorTab, SDockTab)
	.TabRole(NomadTab)
	[
		SummonAssetInvestigator().ToSharedRef()
	];
}

TSharedPtr<SWidget> FAssetInvestigatorModule::SummonAssetInvestigator()
{
	TSharedPtr<SWidget> ReturnWidget;
	if(IsInGameThread())
	{
		return SNew(SAssetInvestigator);
	}
	return ReturnWidget;
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAssetInvestigatorModule, AssetInvestigator)