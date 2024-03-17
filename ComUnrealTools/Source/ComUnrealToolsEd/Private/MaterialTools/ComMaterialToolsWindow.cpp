// Copyright com04 All Rights Reserved.

#include "ComMaterialToolsWindow.h"
#include "CMTNodeSearcher/CMTNodeSearcher.h"
#include "CMTStatList/CMTStatList.h"
#include "CMTTextureFind/CMTTextureFind.h"
#include "CMTParameterSearcher/CMTParameterSearcher.h"
#include "CMTParameterDump/CMTParameterDump.h"

#include "EditorStyleSet.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Docking/WorkspaceItem.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"


#define LOCTEXT_NAMESPACE "SComMaterialToolsWindow"


/* Local constants
 *****************************************************************************/

static const FName CMTNodeSearcherTabId("CMTNodeSearcher");
static const FName CMTParameterSearcherTabId("CMTParameterSearcher");
static const FName CMTParameterDumpTabId("CMTParameterDump");
static const FName CMTTextureFindTabId("CMTTextureFind");
static const FName CMTStatListTabId("CMTStatList");


/* SComMaterialToolsWindow interface
 *****************************************************************************/

void SComMaterialToolsWindow::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	// create & initialize tab manager
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	
	// node searcher window
	TabManager->RegisterTabSpawner(CMTNodeSearcherTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, CMTNodeSearcherTabId))
		.SetDisplayName(LOCTEXT("CMTNodeSearcherTabTitle", "Node Search"));
	
	TabManager->RegisterTabSpawner(CMTParameterSearcherTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, CMTParameterSearcherTabId))
		.SetDisplayName(LOCTEXT("CMTParameterSearcherTabTitle", "Parameter Search"));
	
	TabManager->RegisterTabSpawner(CMTParameterDumpTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, CMTParameterDumpTabId))
		.SetDisplayName(LOCTEXT("CMTParameterDumpTabTitle", "Parameter Dump"));
	
	TabManager->RegisterTabSpawner(CMTTextureFindTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, CMTTextureFindTabId))
		.SetDisplayName(LOCTEXT("CMTTextureFindTabTitle", "Texture Find"));
	
	TabManager->RegisterTabSpawner(CMTStatListTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, CMTStatListTabId))
		.SetDisplayName(LOCTEXT("CMTStatListTabTitle", "Stat List"));
	
	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("ComMaterialTools")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
						->AddTab(CMTNodeSearcherTabId, ETabState::OpenedTab)
						->AddTab(CMTParameterSearcherTabId, ETabState::OpenedTab)
						->AddTab(CMTParameterDumpTabId, ETabState::OpenedTab)
						->AddTab(CMTTextureFindTabId, ETabState::OpenedTab)
						->AddTab(CMTStatListTabId, ETabState::OpenedTab)
						->SetForegroundTab(CMTNodeSearcherTabId)
				)
		);
	
	// menubar
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SComMaterialToolsWindow::FillWindowMenu, TabManager),
		"Window"
		);
	
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			MenuBarBuilder.MakeWidget()
		]
		
		+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				TabManager->RestoreFrom(Layout, ConstructUnderWindow).ToSharedRef()
			]
	];
}


/* SComMaterialToolsWindow implementation
 *****************************************************************************/

void SComMaterialToolsWindow::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager)
{
	if (!TabManager.IsValid())
	{
		return;
	}

#if !WITH_EDITOR
	FGlobalTabmanager::Get()->PopulateTabSpawnerMenu(MenuBuilder, WorkspaceMenu::GetMenuStructure().GetStructureRoot());
#endif //!WITH_EDITOR

	TabManager->PopulateLocalTabSpawnerMenu(MenuBuilder);
}




TSharedRef<SDockTab> SComMaterialToolsWindow::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier) const
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	// create window
	if (TabIdentifier == CMTNodeSearcherTabId)
	{
		TabWidget = SNew(SCMTNodeSearcher);
	}
	else if (TabIdentifier == CMTParameterSearcherTabId)
	{
		TabWidget = SNew(SCMTParameterSearcher);
	}
	else if (TabIdentifier == CMTParameterDumpTabId)
	{
		TabWidget = SNew(SCMTParameterDump);
	}
	else if (TabIdentifier == CMTTextureFindTabId)
	{
		TabWidget = SNew(SCMTTextureFind);
	}
	else if (TabIdentifier == CMTStatListTabId)
	{
		TabWidget = SNew(SCMTStatList);
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}



#undef LOCTEXT_NAMESPACE
