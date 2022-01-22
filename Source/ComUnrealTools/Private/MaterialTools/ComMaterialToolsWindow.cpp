// Copyright com04 All Rights Reserved.

#include "ComMaterialToolsWindow.h"
#include "CMTNodeSearcher/CMTNodeSearcher.h"
#include "CMTStatList/CMTStatList.h"
#include "CMTTextureFind/CMTTextureFind.h"
#include "ComUnrealTools.h"

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

static const FName SearcherTabId("CMTNodeSearcher");
static const FName StatTabId("CMTStatList");
static const FName TextureFindTabId("CMTTextureFind");


/* SComMaterialToolsWindow interface
 *****************************************************************************/

void SComMaterialToolsWindow::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	// create & initialize tab manager
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	
	// node searcher window
	TabManager->RegisterTabSpawner(SearcherTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, SearcherTabId))
		.SetDisplayName(LOCTEXT("CMTNodeSearcherTabTitle", "Node Search"));
		
	TabManager->RegisterTabSpawner(StatTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, StatTabId))
		.SetDisplayName(LOCTEXT("CMTStatListTabTitle", "Stat List"));
	
	TabManager->RegisterTabSpawner(TextureFindTabId, FOnSpawnTab::CreateRaw(this, &SComMaterialToolsWindow::HandleTabManagerSpawnTab, TextureFindTabId))
		.SetDisplayName(LOCTEXT("CMTTextureFindTabTitle", "Texture Find"));
	
	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("ComMaterialTools")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
						->AddTab(SearcherTabId, ETabState::OpenedTab)
						->AddTab(StatTabId, ETabState::OpenedTab)
						->AddTab(TextureFindTabId, ETabState::OpenedTab)
						->SetForegroundTab(SearcherTabId)
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
		.TabRole(ETabRole::PanelTab);

	// create window
	if (TabIdentifier == SearcherTabId)
	{
		TabWidget = SNew(SCMTNodeSearcher);
	}
	else if (TabIdentifier == StatTabId)
	{
		TabWidget = SNew(SCMTStatList);
	}
	else if (TabIdentifier == TextureFindTabId)
	{
		TabWidget = SNew(SCMTTextureFind);
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}



#undef LOCTEXT_NAMESPACE
