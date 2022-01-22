// Copyright com04 All Rights Reserved.

#include "ComUnrealTools.h"

#include "ComUnrealToolsStyle.h"
#include "ComUnrealToolsCommands.h"
#include "MaterialTools/ComMaterialToolsWindow.h"

#include "SlateBasics.h"
#include "SlateExtras.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName ComUnrealToolsTabName("ComUnrealTools");

#define LOCTEXT_NAMESPACE "FComUnrealToolsModule"

void FComUnrealToolsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FComUnrealToolsStyle::Initialize();
	FComUnrealToolsStyle::ReloadTextures();

	FComUnrealToolsCommands::Register();
	
	// Add "Menubar - [Window] -> [Developer Tools]"
	const IWorkspaceMenuStructure& MenuStructure = WorkspaceMenu::GetMenuStructure();

	// グループ
	TSharedRef<FWorkspaceItem> ComUnrealToolsGroup = MenuStructure.GetDeveloperToolsMiscCategory()->AddGroup(
		LOCTEXT("WorkspaceMenu_ComUnrealToolsCategory", "com Unreal Tools"),
		LOCTEXT("ComUnrealToolsMenuTooltip", "Open a com Unreal Tools tab."),
		FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::UnrealToolsTabIconBrushName),
		true);
	
	// マテリアルエディター
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ComUnrealToolsTabName, FOnSpawnTab::CreateRaw(this, &FComUnrealToolsModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ComMaterialToolsTitle", "com Material Tools"))
		.SetTooltipText(LOCTEXT("ComMaterialToolsTooltip", "com Material Tools"))
		.SetIcon(FSlateIcon(FComUnrealToolsStyle::Get().GetStyleSetName(), FComUnrealToolsStyle::MaterialToolsTabIconBrushName))
		.SetGroup(ComUnrealToolsGroup);
}

void FComUnrealToolsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FComUnrealToolsStyle::Shutdown();

	FComUnrealToolsCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ComUnrealToolsTabName);
}

TSharedRef<SDockTab> FComUnrealToolsModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	const TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::MajorTab);

	// main window
	TSharedRef<SComMaterialToolsWindow> Window = SNew(SComMaterialToolsWindow, DockTab, SpawnTabArgs.GetOwnerWindow());

	DockTab->SetContent(Window);

	return DockTab;
}

void FComUnrealToolsModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ComUnrealToolsTabName);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComUnrealToolsModule, ComUnrealTools)
