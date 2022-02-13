// Copyright com04 All Rights Reserved.

#include "ComUnrealTools.h"

#include "ComUnrealToolsStyle.h"
#include "ComUnrealToolsCommands.h"
#include "ObjectTools/ComObjectToolsWindow.h"
#include "MaterialTools/ComMaterialToolsWindow.h"
#include "SequencerTools/ComSequencerToolsWindow.h"
#include "ViewTools/ComViewToolsWindow.h"
#include "UnrealTools/CUTDeveloperSettings.h"

#include "Interfaces/IPluginManager.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

const FName FComUnrealToolsModule::MaterialToolsTabName = FName("ComMaterialTools");
const FName FComUnrealToolsModule::ViewToolsTabName = FName("ComViewTools");
const FName FComUnrealToolsModule::SequencerToolsTabName = FName("ComSequencerTools");
const FName FComUnrealToolsModule::ObjectToolsTabName = FName("ComObjectTools");

#define LOCTEXT_NAMESPACE "FComUnrealToolsModule"

void FComUnrealToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FComUnrealToolsStyle::Initialize();
	FComUnrealToolsStyle::ReloadTextures();

	FComUnrealToolsCommands::Register();
	
	// config
	{
		FString PluginDirectory = IPluginManager::Get().FindPlugin(TEXT("ComUnrealTools"))->GetBaseDir();
		FString ConfigFilePath = PluginDirectory + TEXT("/Config/Settings.ini");

		DeveloperSettings = GetMutableDefault<UCUTDeveloperSettings>();
		// Pluginのデフォルト設定
		DeveloperSettings->LoadConfig(nullptr, *ConfigFilePath);
		// savedのユーザーが追加した設定
		DeveloperSettings->LoadConfig();
		if (!DeveloperSettings->bUseSavedCache)
		{
			DeveloperSettings->InitialzieCachedParameter();
			DeveloperSettings->LoadConfig(nullptr, *ConfigFilePath);
		}
	}

	// Add "Menubar - [Window] -> [Developer Tools]"
	const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();

	// グループ
	TSharedRef<FWorkspaceItem> ComUnrealToolsGroup = MenuStructure.GetDeveloperToolsMiscCategory()->AddGroup(
		LOCTEXT("WorkspaceMenu_ComUnrealToolsCategory", "com Unreal Tools"),
		LOCTEXT("ComUnrealToolsMenuTooltip", "Open a com Unreal Tools tab."),
		FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::UnrealToolsTabIconBrushName),
		true);
	
	// マテリアルツール
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(MaterialToolsTabName, FOnSpawnTab::CreateRaw(this, &FComUnrealToolsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ComMaterialToolsTitle", "com Material Tools"))
		.SetTooltipText(LOCTEXT("ComMaterialToolsTooltip", "com Material Tools"))
		.SetIcon(FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::MaterialToolsTabIconBrushName))
		.SetGroup(ComUnrealToolsGroup);
	
	// Viewツール
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ViewToolsTabName, FOnSpawnTab::CreateRaw(this, &FComUnrealToolsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ComViewToolsTitle", "com View Tools"))
		.SetTooltipText(LOCTEXT("ComViewToolsTooltip", "com View Tools"))
		.SetIcon(FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::ViewToolsTabIconBrushName))
		.SetGroup(ComUnrealToolsGroup);

	// Sequencerツール
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SequencerToolsTabName, FOnSpawnTab::CreateRaw(this, &FComUnrealToolsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ComSequencerToolsTitle", "com Sequencer Tools"))
		.SetTooltipText(LOCTEXT("ComSequencerToolsTooltip", "com Sequencer Tools"))
		.SetIcon(FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::SequencerToolsTabIconBrushName))
		.SetGroup(ComUnrealToolsGroup);

	// Objectツール
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ObjectToolsTabName, FOnSpawnTab::CreateRaw(this, &FComUnrealToolsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ComObjectToolsTitle", "com Object Tools"))
		.SetTooltipText(LOCTEXT("ComObjectToolsTooltip", "com Object Tools"))
		.SetIcon(FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::ObjectToolsTabIconBrushName))
		.SetGroup(ComUnrealToolsGroup);
}

void FComUnrealToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FComUnrealToolsStyle::Shutdown();

	FComUnrealToolsCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ObjectToolsTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SequencerToolsTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ViewToolsTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(MaterialToolsTabName);
}

TSharedRef<SDockTab> FComUnrealToolsModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::MajorTab);

	if (SpawnTabArgs.GetTabId() == FTabId(MaterialToolsTabName))
	{
		TSharedRef<SComMaterialToolsWindow> MaterialWindow = SNew(SComMaterialToolsWindow, DockTab, SpawnTabArgs.GetOwnerWindow());
		DockTab->SetContent(MaterialWindow);
	}
	else if (SpawnTabArgs.GetTabId() == FTabId(ViewToolsTabName))
	{
		TSharedRef<SComViewToolsWindow> ViewWindow = SNew(SComViewToolsWindow, DockTab, SpawnTabArgs.GetOwnerWindow());
		DockTab->SetContent(ViewWindow);
	}
	else if (SpawnTabArgs.GetTabId() == FTabId(SequencerToolsTabName))
	{
		TSharedRef<SComSequencerToolsWindow> ViewWindow = SNew(SComSequencerToolsWindow, DockTab, SpawnTabArgs.GetOwnerWindow());
		DockTab->SetContent(ViewWindow);
	}
	else if (SpawnTabArgs.GetTabId() == FTabId(ObjectToolsTabName))
	{
		TSharedRef<SComObjectToolsWindow> ViewWindow = SNew(SComObjectToolsWindow, DockTab, SpawnTabArgs.GetOwnerWindow());
		DockTab->SetContent(ViewWindow);
	}


	return DockTab;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComUnrealToolsModule, ComUnrealTools)
