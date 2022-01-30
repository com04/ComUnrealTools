﻿// Copyright com04 All Rights Reserved.

#include "ComViewToolsWindow.h"
#include "CVTMPCViewer/CVTMPCViewer.h"
#include "CVTMPCViewerWatch/CVTMPCViewerWatch.h"
#include "ComUnrealTools.h"

#include "EditorStyleSet.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Docking/WorkspaceItem.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"


#define LOCTEXT_NAMESPACE "SComViewToolsWindow"


/* Local constants
 *****************************************************************************/

const FName SComViewToolsWindow::CVTMPCViewerTabId = FName("CVTMPCViewer");
const FName SComViewToolsWindow::CVTMPCViewerWatchTabId = FName("CVTMPCViewerWatch");


/* SComViewToolsWindow interface
 *****************************************************************************/

void SComViewToolsWindow::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	// create & initialize tab manager
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	
	TabManager->RegisterTabSpawner(CVTMPCViewerTabId, FOnSpawnTab::CreateRaw(this, &SComViewToolsWindow::HandleTabManagerSpawnTab, CVTMPCViewerTabId))
		.SetDisplayName(LOCTEXT("CVTMPCViewerTabTitle", "MPC Viewer"));
	TabManager->RegisterTabSpawner(CVTMPCViewerWatchTabId, FOnSpawnTab::CreateRaw(this, &SComViewToolsWindow::HandleTabManagerSpawnTab, CVTMPCViewerWatchTabId))
		.SetDisplayName(LOCTEXT("CVTMPCViewerWatchTabTitle", "MPC Viewer Watch"));
	
	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("ComViewTools")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
						->AddTab(CVTMPCViewerTabId, ETabState::OpenedTab)
						->AddTab(CVTMPCViewerWatchTabId, ETabState::OpenedTab)
						->SetForegroundTab(CVTMPCViewerTabId)
				)
		);
	
	// menubar
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SComViewToolsWindow::FillWindowMenu, TabManager),
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


/* SComViewToolsWindow implementation
 *****************************************************************************/

void SComViewToolsWindow::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager)
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




TSharedRef<SDockTab> SComViewToolsWindow::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier) const
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::PanelTab);

	// create window
	if (TabIdentifier == CVTMPCViewerTabId)
	{
		TabWidget = SNew(SCVTMPCViewer);
	}
	else if (TabIdentifier == CVTMPCViewerWatchTabId)
	{
		TabWidget = SNew(SCVTMPCViewerWatch);
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}



#undef LOCTEXT_NAMESPACE