// Copyright com04 All Rights Reserved.


#include "CUTUtility.h"
#include "ComUnrealTools.h"

#include "DesktopPlatformModule.h"
#include "HAL/FileManagerGeneric.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IDesktopPlatform.h"
#include "Interfaces/IMainFrameModule.h"
#include "Interfaces/ITextureEditorModule.h"
#include "MaterialEditorModule.h"
#include "Materials/Material.h"
#include "Materials/MaterialFunction.h"
#include "Materials/MaterialInstance.h"
#include "Modules/ModuleManager.h"



/**
 * Get master material from Material instance
 */
UMaterial* FCUTUtility::GetMasterMaterial(UMaterialInstance* MaterialInstance)
{
	UMaterial* MasterMaterial = nullptr;
	// search master material
	if (MaterialInstance)
	{
		UMaterialInstance* ParentMaterialInstance = MaterialInstance;
		while (ParentMaterialInstance->Parent != nullptr)
		{
			UMaterialInstance* ParentInstance = Cast<UMaterialInstance>(ParentMaterialInstance->Parent);
			if (ParentInstance)
			{
				ParentMaterialInstance = ParentInstance;
			}
			else
			{
				MasterMaterial = Cast<UMaterial>(ParentMaterialInstance->Parent);
				break;
			}
		}
	}
	
	return MasterMaterial;
}

/**
 * Text box string split tokens
 */
void FCUTUtility::SplitStringTokens(const FString& String, TArray<FString>* OutTokens)
{
	// search text parse
	if (String.Contains("\"") && String.ParseIntoArray(*OutTokens, TEXT("\""), true) > 0)
	{
		for (auto &TokenIt : *OutTokens)
		{
			TokenIt = TokenIt.TrimQuotes();
			TokenIt = TokenIt.Replace(TEXT(" "), TEXT(""));
		}

		struct FRemoveMatchingStrings
		{
			bool operator()(const FString& RemovalCandidate) const
			{
				return RemovalCandidate.IsEmpty();
			}
		};
		OutTokens->RemoveAll(FRemoveMatchingStrings());
	}
	else
	{
		String.ParseIntoArray(*OutTokens, TEXT(" "), true);
	}
}

/** match one string */
bool FCUTUtility::StringMatchesSearchTokens(const TArray<FString>& Tokens, const FString& ComparisonString)
{
	bool bFoundAllTokens = true;
	for (auto TokIT(Tokens.CreateConstIterator()); TokIT; ++TokIT)
	{
		const FString& Token = *TokIT;
		if (!ComparisonString.Contains(Token))
		{
			bFoundAllTokens = false;
			break;
		}
	}
	return bFoundAllTokens;
}

/** M_Default.M_Default -> M_Default */
FString FCUTUtility::NormalizePathText(const FString& Path)
{
	FString Text = Path;
	Path.Split(TEXT("."), &Text, nullptr, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
	return Text;
}

/** icon brush name from material */
FName FCUTUtility::GetIconBrushName(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction)
{
	FName IconBrushName = NAME_None;
	// Material setup
	if (Material)
	{
		IconBrushName = FComUnrealToolsStyle::MaterialIconBrushName;
	}
	else if (MaterialInstance)
	{
		IconBrushName = FComUnrealToolsStyle::MaterialInstanceIconBrushName;
	}
	else if (MaterialFunction)
	{
		IconBrushName = FComUnrealToolsStyle::MaterialFunctionIconBrushName;
	}
	return IconBrushName;
}

/** wakeup editor */
void FCUTUtility::WakeupMaterialEditor(UMaterial* Material, UMaterialInstance* MaterialInstance, UMaterialFunction* MaterialFunction)
{
	// wakeup MaterialEditor
	IMaterialEditorModule* MaterialEditorModule = &FModuleManager::LoadModuleChecked<IMaterialEditorModule>("MaterialEditor");
	if (MaterialEditorModule)
	{
		EToolkitMode::Type Mode = EToolkitMode::Standalone;
		
		if (Material)
		{
			if(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(Material, true) == nullptr)
			{
				MaterialEditorModule->CreateMaterialEditor(Mode, TSharedPtr<IToolkitHost>(), Material);
			}
		}
		else if (MaterialInstance)
		{
			if(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(MaterialInstance, true) == nullptr)
			{
				MaterialEditorModule->CreateMaterialInstanceEditor(Mode, TSharedPtr<IToolkitHost>(), MaterialInstance);
			}
		}
		else if (MaterialFunction)
		{
			if(GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorForAsset(MaterialFunction, true) == nullptr)
			{
				MaterialEditorModule->CreateMaterialEditor(Mode, TSharedPtr<IToolkitHost>(), MaterialFunction);
			}
		}
	}
}


/** wakeup editor */
void FCUTUtility::WakeupTextureEditor(UTexture* Texture)
{
	ITextureEditorModule* TextureEditorModule = &FModuleManager::LoadModuleChecked<ITextureEditorModule>("TextureEditor");
	if (TextureEditorModule)
	{
		EToolkitMode::Type Mode = EToolkitMode::Standalone;
		
		TextureEditorModule->CreateTextureEditor(Mode, TSharedPtr<IToolkitHost>(), Texture);
	}
}


/** copy clipboard */
bool FCUTUtility::ExportClipboard(const FString& TextData)
{
	FPlatformApplicationMisc::ClipboardCopy(*TextData);
	return true;
}


/** Export csv */
bool FCUTUtility::ExportTxt(const FString& TitleName, const FString& DefaultFileName, FString TextData, FString FileType)
{
	TArray<FString> OpenFilenames;
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	bool bOpened = false;
	
	if (DesktopPlatform)
	{
		void* ParentWindowWindowHandle = NULL;

		IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		const TSharedPtr<SWindow>& MainFrameParentWindow = MainFrameModule.GetParentWindow();
		if (MainFrameParentWindow.IsValid() && MainFrameParentWindow->GetNativeWindow().IsValid())
		{
			ParentWindowWindowHandle = MainFrameParentWindow->GetNativeWindow()->GetOSWindowHandle();
		}
		const FString Title = FString::Printf(TEXT("com Unreal Tools: %s"), *TitleName);
		
		bOpened = DesktopPlatform->SaveFileDialog(
				ParentWindowWindowHandle,
				*Title,
				FPaths::ProjectDir(),
				DefaultFileName,
				*FileType,
				EFileDialogFlags::None,
				OpenFilenames
				);
	}
	
	
	bool Export = false;
	if (bOpened && OpenFilenames.Num() > 0)
	{
		FArchive* SaveFile = FFileManagerGeneric::Get().CreateFileWriter(*OpenFilenames[0]);
		if (SaveFile)
		{
			SaveFile->Serialize(TCHAR_TO_ANSI(*TextData), TextData.Len());
			SaveFile->Close();
			delete SaveFile;
			SaveFile = NULL;
			Export = true;
		}
	}
	
	return Export;
}

