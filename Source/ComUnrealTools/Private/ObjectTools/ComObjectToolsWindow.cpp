// Copyright com04 All Rights Reserved.

#include "ComObjectToolsWindow.h"
#include "COTPropertySearcher/COTPropertySearcher.h"
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


#define LOCTEXT_NAMESPACE "SComObjectToolsWindow"


/* Local constants
 *****************************************************************************/

const FName SComObjectToolsWindow::COTPropertySearcherTabId = FName("COTPropertySearcher");


/* SComObjectToolsWindow interface
 *****************************************************************************/

void SComObjectToolsWindow::Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow)
{
	// create & initialize tab manager
	TabManager = FGlobalTabmanager::Get()->NewTabManager(ConstructUnderMajorTab);
	
	TabManager->RegisterTabSpawner(COTPropertySearcherTabId, FOnSpawnTab::CreateRaw(this, &SComObjectToolsWindow::HandleTabManagerSpawnTab, COTPropertySearcherTabId))
		.SetDisplayName(LOCTEXT("COTPropertySearcherTabTitle", "Property Search"));
	
	// create tab layout
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("ComObjectTools")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
						->AddTab(COTPropertySearcherTabId, ETabState::OpenedTab)
						->SetForegroundTab(COTPropertySearcherTabId)
				)
		);
	
	// menubar
	FMenuBarBuilder MenuBarBuilder = FMenuBarBuilder(TSharedPtr<FUICommandList>());
	MenuBarBuilder.AddPullDownMenu(
		LOCTEXT("WindowMenuLabel", "Window"),
		FText::GetEmpty(),
		FNewMenuDelegate::CreateStatic(&SComObjectToolsWindow::FillWindowMenu, TabManager),
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
SComObjectToolsWindow::~SComObjectToolsWindow()
{
	// 終了時に現在の設定を保存しておく
	UCUTDeveloperSettings* DeveloperSettings = GetMutableDefault<UCUTDeveloperSettings>();
	// DeveloperSettings->SaveConfig();
}

/* SComObjectToolsWindow implementation
 *****************************************************************************/

void SComObjectToolsWindow::FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<FTabManager> TabManager)
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




TSharedRef<SDockTab> SComObjectToolsWindow::HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier) const
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	TSharedRef<SDockTab> DockTab = SNew(SDockTab)
		.TabRole(ETabRole::NomadTab);

	// create window
	if (TabIdentifier == COTPropertySearcherTabId)
	{
		TabWidget = SNew(SCOTPropertySearcher);
	}

	DockTab->SetContent(TabWidget.ToSharedRef());

	return DockTab;
}



#undef LOCTEXT_NAMESPACE
