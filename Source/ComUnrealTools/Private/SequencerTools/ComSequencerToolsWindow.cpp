// Copyright com04 All Rights Reserved.

#include "ComSequencerToolsWindow.h"
#include "CSTTrackSearcher/CSTTrackSearcher.h"
#include "ComUnrealTools.h"
#include "UnrealTools/CUTDeveloperSettings.h"

#include "EditorStyleSet.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/Docking/WorkspaceItem.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Modules/ModuleManager.h"
#include "Textures/SlateIcon.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/SBoxPanel.h"


#define LOCTEXT_NAMESPACE "SComSequencerToolsWindow"


/* Local constants
 *****************************************************************************/

const FName SComSequencerToolsWindow::CSTTrackSearcherTabId = FName("CSTTrackSearcher");


/* SComSequencerToolsWindow interface
 *****************************************************************************/

void SComSequencerToolsWindow::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	// create & initialize tab manager
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	
	TabManager->RegisterTabSpawner(CSTTrackSearcherTabId, FOnSpawnTab::CreateRaw(this, &SComSequencerToolsWindow::HandleTabManagerSpawnTab, CSTTrackSearcherTabId))
		.SetDisplayName(LOCTEXT("CSTTrackSearcherTabTitle", "Track Searcher"));
	
	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("ComSequencerTools")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
						->AddTab(CSTTrackSearcherTabId, ETabState::OpenedTab)
						->SetForegroundTab(CSTTrackSearcherTabId)
				)
		);
	
	// menubar
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SComSequencerToolsWindow::FillWindowMenu, TabManager),
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
SComSequencerToolsWindow::~SComSequencerToolsWindow()
{
	// 終了時に現在の設定を保存しておく
	UCUTDeveloperSettings* DeveloperSettings = GetMutableDefault<UCUTDeveloperSettings>();
	// DeveloperSettings->SaveConfig();
}

/* SComSequencerToolsWindow implementation
 *****************************************************************************/

void SComSequencerToolsWindow::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager)
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




TSharedRef<SDockTab> SComSequencerToolsWindow::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier) const
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	// create window
	if (TabIdentifier == CSTTrackSearcherTabId)
	{
		TabWidget = SNew(SCSTTrackSearcher);
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}



#undef LOCTEXT_NAMESPACE
