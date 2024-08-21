// © 2024 DrElliot. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAssetInvestigatorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	void RegisterGameEditorMenus();
	TSharedRef<SDockTab> OpenAssetInvestigator(const FSpawnTabArgs& Args);
	TSharedPtr<SWidget> SummonAssetInvestigator();

private:
	TWeakPtr<SDockTab> AssetInvestigatorTab;
		
};
