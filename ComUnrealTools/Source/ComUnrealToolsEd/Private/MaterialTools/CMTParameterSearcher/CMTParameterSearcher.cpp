// Copyright com04 All Rights Reserved.

#include "CMTParameterSearcher.h"
#include "CMTParameterSearcherResult.h"
#include "ComUnrealToolsStyle.h"
#include "Utility/CUTUtility.h"

#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Texture.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionComment.h"
#include "Materials/MaterialExpressionCustom.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SConstraintCanvas.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Notifications/SProgressBar.h"


#define LOCTEXT_NAMESPACE "CMTParameterSearcher"


////////////////////////////////////
// SCMTParameterSearcher


SCMTParameterSearcher::~SCMTParameterSearcher()
{
}

void SCMTParameterSearcher::Construct(const FArguments& InArgs)
{
	SCMTParameterSearcher* Self = this;
	bDirtySearchPath = true;
	bDirtySearchText = true;
	MaterialSearcher.OnSearchEnd.BindLambda([Self]{Self->FinishSearch();});
	
	
	if (VectorFunctionSource.Num() == 0)
	{
		VectorFunctionSource.Add(MakeShareable(new ECMTParameterSearcherVectorFunction(ECMTParameterSearcherVectorFunction::All)));
		VectorFunctionSource.Add(MakeShareable(new ECMTParameterSearcherVectorFunction(ECMTParameterSearcherVectorFunction::Any)));
	}
	if (FunctionSource.Num() == 0)
	{
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::Equals)));
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::NotEquals)));
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::GreaterThan)));
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::GreaterThanEquals)));
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::LessThan)));
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::LessThanEquals)));
		FunctionSource.Add(MakeShareable(new ECMTParameterSearcherFunction(ECMTParameterSearcherFunction::All)));
	}
	
	// 各パラメーターのマージン値
	const FMargin ParameterTopMargin(20.0f, 4.0f, 0.0f, 4.0f);
	const FMargin VectorComponentMargin(10.0f, 2.0f, 5.0f, 2.0f);
	// 各パラメーター名と値のスペース
	const float TextBlockToParamPadding = 10.0f;
	const float VectorTextBlockToParamPadding = 5.0f;
	// SpinBoxの最小サイズ
	const float SpinBoxMinDesiredWidth = 40.0f;
#define SPINBOX_OPTION	.MinDesiredWidth(SpinBoxMinDesiredWidth) \
		.Delta(0.01f) \
		.MinFractionalDigits(2)

	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Search path
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 4.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 4.0f, 12.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SearchPath", "Search Path"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("FindPath", "Enter material path to find references..."))
				.InitialText(FText::FromString(GetSearchPath()))
				.OnTextCommitted(this, &SCMTParameterSearcher::OnSearchPathCommitted)
			]
		]
		
		// Search box
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 4.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 4.0f, 12.0f, 0.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("SearchParameterName", "Parameter Name"))
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SSearchBox)
				.HintText(LOCTEXT("Find", "Enter material or node name, texture name to find references..."))
				.InitialText(FText::FromString(GetSearchValue()))
				.OnTextCommitted(this, &SCMTParameterSearcher::OnSearchTextCommitted)
			]
		]
		
		// Scalar
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(ParameterTopMargin)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(ScalarCheckBox, SCheckBox)
				.IsChecked(GetSelectType() == ESelectType::Scalar ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxScalarChanged)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ScalarParameter", "Scalar"))
			]
			
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(TextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ScalarGroupWidget, SBox)
				.Padding(FMargin(0.0f))
				[
					SNew(SSpinBox<float>)
					SPINBOX_OPTION
					.Value(GetScalarValue())
					.OnValueChanged(this, &SCMTParameterSearcher::OnSpinBoxScalarValueChanged)
				]
			]
		]

		// Vector
		+SVerticalBox::Slot()
		.AutoHeight()
		// .Padding(ParameterTopMargin)
		.Padding(20.0f, 2.0f, 0.0f, 2.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(VectorCheckBox, SCheckBox)
				.IsChecked(GetSelectType() == ESelectType::Vector ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxVectorChanged)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("VectorParameter", "Vector"))
			]
			
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(TextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(VectorGroupWidget, SBox)
				.Padding(FMargin(0.0f))
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(0.0f, 0.0f, 10.0f, 0.0f)
					[
						SNew(SBox)
						.VAlign(EVerticalAlignment::VAlign_Center)
						.MinDesiredWidth(110.0f)
						[
							SNew(SComboBox<TSharedPtr<ECMTParameterSearcherVectorFunction>>)
							.OptionsSource(&VectorFunctionSource)
							.OnSelectionChanged( this, &SCMTParameterSearcher::OnSelectionChangedVectorFunction )
							.OnGenerateWidget(this, &SCMTParameterSearcher::GenerateVectorFunctionItem)
							[
								SNew(STextBlock)
								.Text(this, &SCMTParameterSearcher::GetSelectionVectorFunctionText)
							]
						]
					]
					
					// Vector - R
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
							.IsChecked(GetCheckBoxStateVectorR())
							.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxVectorRChanged)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SAssignNew(VectorRGroupWidget, SBox)
							.Padding(FMargin(0.0f))
							[
								SNew(SHorizontalBox)
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("VectorParameterR", "R:"))
								]
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(VectorTextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
								[
									SNew(SSpinBox<float>)
									SPINBOX_OPTION
									.Value(GetVectorValue().R)
									.OnValueChanged(this, &SCMTParameterSearcher::OnSpinBoxVectorRValueChanged)
								]
							]
						]
					]
					// Vector - G
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(15.0f, 0.0f, 0.0f, 0.0f)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
							.IsChecked(GetCheckBoxStateVectorG())
							.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxVectorGChanged)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SAssignNew(VectorGGroupWidget, SBox)
							.Padding(FMargin(0.0f))
							[
								SNew(SHorizontalBox)
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("VectorParameterG", "G:"))
								]
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(VectorTextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
								[
									SNew(SSpinBox<float>)
									SPINBOX_OPTION
									.Value(GetVectorValue().G)
									.OnValueChanged(this, &SCMTParameterSearcher::OnSpinBoxVectorGValueChanged)
								]
							]
						]
					]
					// Vector - B
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(15.0f, 0.0f, 0.0f, 0.0f)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
							.IsChecked(GetCheckBoxStateVectorB())
							.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxVectorBChanged)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SAssignNew(VectorBGroupWidget, SBox)
							.Padding(FMargin(0.0f))
							[
								SNew(SHorizontalBox)
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("VectorParameterB", "B:"))
								]
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(VectorTextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
								[
									SNew(SSpinBox<float>)
									SPINBOX_OPTION
									.Value(GetVectorValue().B)
									.OnValueChanged(this, &SCMTParameterSearcher::OnSpinBoxVectorBValueChanged)
								]
							]
						]
					]
					// Vector - A
					+SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.Padding(15.0f, 0.0f, 0.0f, 0.0f)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SNew(SCheckBox)
							.IsChecked(GetCheckBoxStateVectorA())
							.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxVectorAChanged)
						]
						+SHorizontalBox::Slot()
						.AutoWidth()
						.VAlign(VAlign_Center)
						[
							SAssignNew(VectorAGroupWidget, SBox)
							.Padding(FMargin(0.0f))
							[
								SNew(SHorizontalBox)
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								[
									SNew(STextBlock)
									.Text(LOCTEXT("VectorParameterA", "A:"))
								]
								+SHorizontalBox::Slot()
								.AutoWidth()
								.VAlign(VAlign_Center)
								.Padding(VectorTextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
								[
									SNew(SSpinBox<float>)
									SPINBOX_OPTION
									.Value(GetVectorValue().A)
									.OnValueChanged(this, &SCMTParameterSearcher::OnSpinBoxVectorAValueChanged)
								]
							]
						]
					]
				]
			]
		]
		
		// Texture
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(ParameterTopMargin)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(TextureCheckBox, SCheckBox)
				.IsChecked(GetSelectType() == ESelectType::Texture ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxTextureChanged)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("TextureParameter", "Texture"))
			]
			+SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.FillWidth(1.0f)
			.Padding(TextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(TextureGroupWidget, SBox)
				.Padding(FMargin(0.0f))
				[
					SNew(SSearchBox)
					.HintText(LOCTEXT("TextureValue", "検索したいテクスチャ名を入れてください（部分一致）"))
					.InitialText(FText::FromString(GetTextureValue()))
					.OnTextCommitted(this, &SCMTParameterSearcher::OnTextureValueCommitted)
				]
			]
		]
		
		// StaticSwitch
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(ParameterTopMargin)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SAssignNew(StaticSwitchCheckBox, SCheckBox)
				.IsChecked(GetSelectType() == ESelectType::StaticSwitch ? ECheckBoxState::Checked : ECheckBoxState::Unchecked)
				.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxStaticSwitchChanged)
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("StaticSwitchParameter", "StaticSwitch"))
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(TextBlockToParamPadding, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(StaticSwitchGroupWidget, SBox)
				.Padding(FMargin(0.0f))
				[
					SNew(SCheckBox)
					.IsChecked(GetCheckBoxStateStaticSwitchValue())
					.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxStaticSwitchValueChanged)
				]
			]
		]

		// Option
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 4.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			// function
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SBox)
				.VAlign(EVerticalAlignment::VAlign_Center)
				.MinDesiredWidth(100.0f)
				[
					SNew(SComboBox<TSharedPtr<ECMTParameterSearcherFunction>>)
					.OptionsSource(&FunctionSource)
					.OnSelectionChanged(this, &SCMTParameterSearcher::OnSelectionChangedFunction)
					.OnGenerateWidget(this, &SCMTParameterSearcher::GenerateFunctionItem)
					[
						SNew(STextBlock)
						.Text(this, &SCMTParameterSearcher::GetSelectionFunctionText)
					]
				]
			]
			
			// check box
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(30.0f, 0.0f, 0.0f, 0.0f)
			[
				SNew(SCheckBox)
				.IsChecked(GetCheckBoxStateOverrideOnly())
				.OnCheckStateChanged(this, &SCMTParameterSearcher::OnCheckBoxOverrideOnlyChanged)
			]
			// override only
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("MIOverrideOnly", "material instance override only"))
			]
		]
		
		// Button
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 20.0f, 0.0f, 0.0f)
		[
			SNew(SHorizontalBox)
			
			// 実行ボタン
			+SHorizontalBox::Slot()
			.MaxWidth(300.f)
			[
				SAssignNew(SearchStartButton, SButton)
				.Text(LOCTEXT("SearchStart", "Search Start"))
				.OnClicked(this, &SCMTParameterSearcher::ButtonSearchStartClicked)
				.IsEnabled(false)
			]
			// copy clipboard
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(50.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CopyClipBoardButton, SButton)
				.Text(LOCTEXT("CopyClipboard", "Copy ClipBoard"))
				.OnClicked(this, &SCMTParameterSearcher::ButtonCopyClipBoardClicked)
				.IsEnabled(false)
			]
			// export Text
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportTextButton, SButton)
				.Text(LOCTEXT("ExportText", "Export Text"))
				.OnClicked(this, &SCMTParameterSearcher::ButtonExportTextClicked)
				.IsEnabled(false)
			]
			// export CSV
			+SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportCsvButton, SButton)
				.Text(LOCTEXT("ExportCSV", "Export CSV"))
				.OnClicked(this, &SCMTParameterSearcher::ButtonExportCSVClicked)
				.IsEnabled(false)
			]
		]
		
		
		// Result tree
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(0.0f, 4.0f, 0.0f, 0.0f)
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(FComUnrealToolsStyle::MenuBGBrushName))
			[
				SAssignNew(TreeView, SCMTParameterSearcherTreeViewType)
				.ItemHeight(24)
				.TreeItemsSource(&ItemsFound)
				.OnGenerateRow(this, &SCMTParameterSearcher::OnGenerateRow)
				.OnGetChildren(this, &SCMTParameterSearcher::OnGetChildren)
				.OnMouseButtonDoubleClick(this,&SCMTParameterSearcher::OnTreeSelectionDoubleClicked)
			]
		]
		
		
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.0f, 4.0f, 0.0f, 0.0f)
		[
			SNew(SProgressBar)
			.Visibility(this, &SCMTParameterSearcher::GetProgressBarVisibility)
			.Percent(this, &SCMTParameterSearcher::GetProgressBarPercent)
		]
	];
#undef SPINBOX_OPTION
	
	
	// 前回の状態を復帰。Enabledでの灰色化対応。
	OnSearchPathCommitted(FText::FromString(GetSearchPath()), ETextCommit::OnEnter);
	OnSearchTextCommitted(FText::FromString(GetSearchValue()), ETextCommit::OnEnter);
	SetupCheckBoxType();
	OnCheckBoxVectorRChanged(GetCheckBoxStateVectorR());
	OnCheckBoxVectorGChanged(GetCheckBoxStateVectorG());
	OnCheckBoxVectorBChanged(GetCheckBoxStateVectorB());
	OnCheckBoxVectorAChanged(GetCheckBoxStateVectorA());
}

FReply SCMTParameterSearcher::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) 
{
	return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
}


// Search path --- Begin

/** text change event */
void SCMTParameterSearcher::OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchPath(Text.ToString());
	bDirtySearchPath = true;
	
	SearchStartButton->SetEnabled((!GetSearchPath().IsEmpty() && !GetSearchValue().IsEmpty()));
}

/** text change event */
void SCMTParameterSearcher::OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetSearchValue(Text.ToString());
	bDirtySearchText = true;
	
	SearchStartButton->SetEnabled((!GetSearchPath().IsEmpty() && !GetSearchValue().IsEmpty()));
}

/** Search */
void SCMTParameterSearcher::SearchStart()
{
	// search path parse
	if (bDirtySearchPath)
	{
		FCUTUtility::SplitStringTokens(GetSearchPath(), &SearchPathTokens);
		bDirtySearchPath = false;
	}
	
	// search text parse
	if (bDirtySearchText)
	{
		FCUTUtility::SplitStringTokens(GetSearchValue(), &SearchTokens);
		bDirtySearchText = false;
	}
	
	// 前回の検索結果をクリアする
	for (auto It(ItemsFound.CreateIterator()); It; ++It)
	{
		TreeView->SetItemExpansion(*It, false);
	}
	ItemsFound.Empty();		

	// エラー対処
	
	// 検索文字列が無い
	if ((SearchTokens.Num() == 0) || (SearchPathTokens.Num() == 0))
	{
		FCMTParameterSearcherResultShare Result(new FCMTParameterSearcherResult(FText::FromString(TEXT("Error!! 検索パス、パラメーター名を設定してください！")), true));
		ItemsFound.Add(Result);
	}
	else
	{
		bool bNonNumericError = false;
		switch (GetMatchFunction())
		{
		case ECMTParameterSearcherFunction::GreaterThan:
		case ECMTParameterSearcherFunction::GreaterThanEquals:
		case ECMTParameterSearcherFunction::LessThan:
		case ECMTParameterSearcherFunction::LessThanEquals:
			bNonNumericError = true;
			break;
		}
		bool bError = false;
		FString ErrorTypeString;
		switch (GetSelectType())
		{
		case ESelectType::Texture:
			if (bNonNumericError)
			{
				bError = true;
				ErrorTypeString = FString::Printf(TEXT("Texture"));
			}
			break;
		case ESelectType::StaticSwitch:
			if (bNonNumericError)
			{
				ErrorTypeString = FString::Printf(TEXT("StaticSwitch"));
				bError = true;
			}
			break;
		}
		if (bError)
		{
			FString ErrorString = FString::Printf(TEXT("Error!! %s検索の場合は、マッチ条件を「%s」「%s」「%s」のどれかにしてください！"),
					*ErrorTypeString,
					*GetFunctionText(ECMTParameterSearcherFunction::Equals).ToString(),
					*GetFunctionText(ECMTParameterSearcherFunction::NotEquals).ToString(),
					*GetFunctionText(ECMTParameterSearcherFunction::All).ToString());
			FCMTParameterSearcherResultShare Result(new FCMTParameterSearcherResult(FText::FromString(ErrorString), true));
			ItemsFound.Add(Result);
		}
	}
	
	// エラーがあれば表示して検索スキップ
	if (ItemsFound.Num() > 0)
	{
		TreeView->RequestTreeRefresh();
		CopyClipBoardButton->SetEnabled(false);
		ExportTextButton->SetEnabled(false);
		ExportCsvButton->SetEnabled(false);
		return;
	}
	
	
	// 検索開始
	HighlightText = FText::FromString(GetSearchValue());
	
	MaterialSearcher.SearchStart(SearchPathTokens, TArray<FString>(),
			true,
			true, true, false);
}


/** search finish callback */
void SCMTParameterSearcher::FinishSearch()
{
	const TArray<FAssetData>& MaterialAsset = MaterialSearcher.GetMaterialAssets();
	const TArray<FAssetData>& MaterialInstanceAsset = MaterialSearcher.GetMaterialInstanceAssets();
	
	for (auto It = MaterialAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->GetObjectPathString();
		
		UMaterial* Material = FindObject<UMaterial>(NULL, *AssetPathString);
		if (Material == nullptr) continue;
		
		bool bFindMatch = false;
		FText ValueText;
		switch (GetSelectType())
		{
		case ESelectType::Scalar:
			bFindMatch = CheckMaterialParameterInfoScalar(Material, ValueText);
			break;
		case ESelectType::Vector:
			bFindMatch = CheckMaterialParameterInfoVector(Material, ValueText);
			break;
		case ESelectType::Texture:
			bFindMatch = CheckMaterialParameterInfoTexture(Material, ValueText);
			break;
		case ESelectType::StaticSwitch:
			bFindMatch = CheckMaterialParameterInfoStaticSwitch(Material, ValueText);
			break;
		}
		if (bFindMatch)
		{
			FCMTParameterSearcherResultShare Result(new FCMTParameterSearcherResult(FText::FromString(FCUTUtility::NormalizePathText(Material->GetPathName())),
							ValueText, Material));
			ItemsFound.Add(Result);
		}
	}
	
	for (auto It = MaterialInstanceAsset.CreateConstIterator() ; It ; ++It)
	{
		FString AssetPathString = It->GetObjectPathString();
		
		UMaterialInstance* MaterialInstance = FindObject<UMaterialInstance>(NULL, *AssetPathString);
		if (MaterialInstance == nullptr) continue;
		
		bool bFindMatch = false;
		FText ValueText;
		switch (GetSelectType())
		{
		case ESelectType::Scalar:
			if (GetCheckBoxStateOverrideOnly() == ECheckBoxState::Checked)
			{
				for (const FScalarParameterValue& ParameterValue : MaterialInstance->ScalarParameterValues)
				{
					if (CheckMaterialParameterValueScalar(ParameterValue.ParameterInfo, ParameterValue.ParameterValue, ValueText))
					{
						bFindMatch = true;
						break;
					}
				}
			}
			else
			{
				bFindMatch = CheckMaterialParameterInfoScalar(MaterialInstance, ValueText);
			}
			break;
		case ESelectType::Vector:
			if (GetCheckBoxStateOverrideOnly() == ECheckBoxState::Checked)
			{
				for (const FVectorParameterValue& ParameterValue : MaterialInstance->VectorParameterValues)
				{
					if (CheckMaterialParameterValueVector(ParameterValue.ParameterInfo, ParameterValue.ParameterValue, ValueText))
					{
						bFindMatch = true;
						break;
					}
				}
			}
			else
			{
				bFindMatch = CheckMaterialParameterInfoVector(MaterialInstance, ValueText);
			}
			break;
		case ESelectType::Texture:
			if (GetCheckBoxStateOverrideOnly() == ECheckBoxState::Checked)
			{
				for (const FTextureParameterValue& ParameterValue : MaterialInstance->TextureParameterValues)
				{
					if (CheckMaterialParameterValueTexture(ParameterValue.ParameterInfo, ParameterValue.ParameterValue, ValueText))
					{
						bFindMatch = true;
						break;
					}
				}
			}
			else
			{
				bFindMatch = CheckMaterialParameterInfoTexture(MaterialInstance, ValueText);
			}
			break;
		case ESelectType::StaticSwitch:
			if (GetCheckBoxStateOverrideOnly() == ECheckBoxState::Checked)
			{
				for (const FStaticSwitchParameter& ParameterValue : MaterialInstance->GetStaticParameters().GetRuntime().StaticSwitchParameters)
				{
					if (CheckMaterialParameterValueStaticSwitch(ParameterValue.ParameterInfo, ParameterValue.Value, ValueText))
					{
						bFindMatch = true;
						break;
					}
				}
			}
			else
			{
				bFindMatch = CheckMaterialParameterInfoStaticSwitch(MaterialInstance, ValueText);
			}
			break;
		}
		if (bFindMatch)
		{
			FCMTParameterSearcherResultShare Result(new FCMTParameterSearcherResult(FText::FromString(FCUTUtility::NormalizePathText(MaterialInstance->GetPathName())),
							ValueText, MaterialInstance));
			ItemsFound.Add(Result);
		}
	}
	
	
	// not found
	if (ItemsFound.Num() == 0)
	{
		ItemsFound.Add(FCMTParameterSearcherResultShare(new FCMTParameterSearcherResult(LOCTEXT("ResultNoResults", "No Results found"))));
		CopyClipBoardButton->SetEnabled(false);
		ExportTextButton->SetEnabled(false);
		ExportCsvButton->SetEnabled(false);
	}
	else
	{
		CopyClipBoardButton->SetEnabled(true);
		ExportTextButton->SetEnabled(true);
		ExportCsvButton->SetEnabled(true);
	}

	TreeView->RequestTreeRefresh();

	for (auto It(ItemsFound.CreateIterator()); It; ++It)
	{
		TreeView->SetItemExpansion(*It, true);
	}
}

// Search box --- End


// Check box --- Begin
void SCMTParameterSearcher::OnCheckBoxScalarChanged(ECheckBoxState InValue)
{
	if (InValue == ECheckBoxState::Checked)
	{
		SetSelectType(ESelectType::Scalar);
	}
	SetupCheckBoxType();
}
void SCMTParameterSearcher::OnCheckBoxVectorChanged(ECheckBoxState InValue)
{
	if (InValue == ECheckBoxState::Checked)
	{
		SetSelectType(ESelectType::Vector);
	}
	SetupCheckBoxType();
}
void SCMTParameterSearcher::OnCheckBoxTextureChanged(ECheckBoxState InValue)
{
	if (InValue == ECheckBoxState::Checked)
	{
		SetSelectType(ESelectType::Texture);
	}
	SetupCheckBoxType();
}
void SCMTParameterSearcher::OnCheckBoxStaticSwitchChanged(ECheckBoxState InValue)
{
	if (InValue == ECheckBoxState::Checked)
	{
		SetSelectType(ESelectType::StaticSwitch);
	}
	SetupCheckBoxType();
}
void SCMTParameterSearcher::SetupCheckBoxType()
{
#define SETUP(Type) { \
		const bool bEnabled = (GetSelectType() == ESelectType::Type);\
		Type##GroupWidget->SetEnabled(bEnabled); \
		Type##CheckBox->SetIsChecked(bEnabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked); \
	}
	
	SETUP(Scalar);
	SETUP(Vector);
	SETUP(Texture);
	SETUP(StaticSwitch);
#undef SETUP
}
void SCMTParameterSearcher::OnCheckBoxVectorRChanged(ECheckBoxState InValue)
{
	SetCheckBoxStateVectorR(InValue);
	VectorRGroupWidget->SetEnabled(InValue == ECheckBoxState::Checked);
}
void SCMTParameterSearcher::OnCheckBoxVectorGChanged(ECheckBoxState InValue)
{
	SetCheckBoxStateVectorG(InValue);
	VectorGGroupWidget->SetEnabled(InValue == ECheckBoxState::Checked);
}
void SCMTParameterSearcher::OnCheckBoxVectorBChanged(ECheckBoxState InValue)
{
	SetCheckBoxStateVectorB(InValue);
	VectorBGroupWidget->SetEnabled(InValue == ECheckBoxState::Checked);
}
void SCMTParameterSearcher::OnCheckBoxVectorAChanged(ECheckBoxState InValue)
{
	SetCheckBoxStateVectorA(InValue);
	VectorAGroupWidget->SetEnabled(InValue == ECheckBoxState::Checked);
}
void SCMTParameterSearcher::OnCheckBoxStaticSwitchValueChanged(ECheckBoxState InValue)
{
	SetCheckBoxStateStaticSwitchValue(InValue);
}
void SCMTParameterSearcher::OnCheckBoxOverrideOnlyChanged(ECheckBoxState InValue)
{
	SetCheckBoxStateOverrideOnly(InValue);
}
// Check box --- End

// SSpinBox --- Begin
void SCMTParameterSearcher::OnSpinBoxScalarValueChanged(float InValue)
{
	SetScalarValue(InValue);
}
void SCMTParameterSearcher::OnSpinBoxVectorRValueChanged(float InValue)
{
	GetVectorValueRef().R = InValue;
}
void SCMTParameterSearcher::OnSpinBoxVectorGValueChanged(float InValue)
{
	GetVectorValueRef().G = InValue;
}
void SCMTParameterSearcher::OnSpinBoxVectorBValueChanged(float InValue)
{
	GetVectorValueRef().B = InValue;
}
void SCMTParameterSearcher::OnSpinBoxVectorAValueChanged(float InValue)
{
	GetVectorValueRef().A = InValue;
}
// SSpinBox --- End

// TextureValue --- Begin
void SCMTParameterSearcher::OnTextureValueCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	SetTextureValue(Text.ToString());
}
// TextureValue --- End

// SComboBox --- Begin
TSharedRef<SWidget> SCMTParameterSearcher::GenerateVectorFunctionItem(TSharedPtr<ECMTParameterSearcherVectorFunction> InFunction)
{
	return SNew(STextBlock) 
		.Text(GetVectorFunctionText(InFunction));
}

FText SCMTParameterSearcher::GetVectorFunctionText(TSharedPtr<ECMTParameterSearcherVectorFunction> InFunction) const
{
	return GetVectorFunctionText(*InFunction);
}
FText SCMTParameterSearcher::GetVectorFunctionText(ECMTParameterSearcherVectorFunction InFunction) const
{
	FText EnumText;

	switch (InFunction)
	{ 
	case ECMTParameterSearcherVectorFunction::All:
		EnumText = LOCTEXT("ECMTParameterSearcherVectorFunction::All", "全てマッチ(&&)");
		break;
	case ECMTParameterSearcherVectorFunction::Any:
		EnumText = LOCTEXT("ECMTParameterSearcherVectorFunction::Any", "どれかマッチ(||)");
		break;
	}
	return EnumText;
}
FText SCMTParameterSearcher::GetSelectionVectorFunctionText() const
{
	return GetVectorFunctionText(GetVectorFunction());
}
void SCMTParameterSearcher::OnSelectionChangedVectorFunction(TSharedPtr<ECMTParameterSearcherVectorFunction> InFunction, ESelectInfo::Type InSelectInfo)
{
	SetVectorFunction(*InFunction);
}

TSharedRef<SWidget> SCMTParameterSearcher::GenerateFunctionItem(TSharedPtr<ECMTParameterSearcherFunction> InFunction)
{
	return SNew(STextBlock) 
		.Text(GetFunctionText(InFunction));
}

FText SCMTParameterSearcher::GetFunctionText(TSharedPtr<ECMTParameterSearcherFunction> InFunction) const
{
	return GetFunctionText(*InFunction);
}
FText SCMTParameterSearcher::GetFunctionText(ECMTParameterSearcherFunction InFunction) const
{
	FText EnumText;

	switch (InFunction)
	{ 
	case ECMTParameterSearcherFunction::Equals:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::Equals", "等しい");
		break;
	case ECMTParameterSearcherFunction::NotEquals:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::NotEquals", "等しくない");
		break;
	case ECMTParameterSearcherFunction::GreaterThan:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::GreaterThan", "より大きい");
		break;
	case ECMTParameterSearcherFunction::GreaterThanEquals:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::GreaterThanEquals", "以上");
		break;
	case ECMTParameterSearcherFunction::LessThan:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::LessThan", "より小さい");
		break;
	case ECMTParameterSearcherFunction::LessThanEquals:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::LessThanEquals", "以下");
		break;
	case ECMTParameterSearcherFunction::All:
		EnumText = LOCTEXT("ECMTParameterSearcherFunction::All", "全て表示");
		break;
	}
	return EnumText;
}
FText SCMTParameterSearcher::GetSelectionFunctionText() const
{
	return GetFunctionText(GetMatchFunction());
}
void SCMTParameterSearcher::OnSelectionChangedFunction(TSharedPtr<ECMTParameterSearcherFunction> InFunction, ESelectInfo::Type InSelectInfo)
{
	SetMatchFunction(*InFunction);
}
// SComboBox --- End


// Result Tree --- Begin

TSharedRef<ITableRow> SCMTParameterSearcher::OnGenerateRow(FCMTParameterSearcherResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(STableRow< TSharedPtr<FCMTParameterSearcherResultShare> >, OwnerTable)
		[
			SNew(SBorder)
			.BorderImage(FComUnrealToolsStyle::Get().GetBrush(InItem->GetBackgroundBrushName()))
			.BorderBackgroundColor(FLinearColor::Transparent)
			[
				SNew(SHorizontalBox)
				
				// icon
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					InItem->CreateIcon()
				]
				
				// data name
				+SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(2,0)
				[
					SNew(STextBlock)
					.Text(InItem->GetDisplayText())
					.ColorAndOpacity(InItem->IsError() ? FLinearColor(0.8f, 0.3f, 0.3f, 1.0f) : FLinearColor::White)
					.HighlightText(HighlightText)
				]
				
				// comments
				+SHorizontalBox::Slot()
				.FillWidth(1)
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(2,0)
				[
					SNew(STextBlock)
					.Text(InItem->GetValueText())
					.ColorAndOpacity(FLinearColor::Yellow)
					.HighlightText(HighlightText)
				]
			]
		];
}
void SCMTParameterSearcher::OnGetChildren(FCMTParameterSearcherResultShare InItem, TArray<FCMTParameterSearcherResultShare>& OutChildren)
{
//	OutChildren += InItem->GetChildren();
}

void SCMTParameterSearcher::OnTreeSelectionDoubleClicked(FCMTParameterSearcherResultShare Item)
{
	if(Item.IsValid())
	{
		Item->OnClick();
	}
}

// Result Tree --- End

// Progress Bar --- Begin

EVisibility SCMTParameterSearcher::GetProgressBarVisibility() const
{
	return MaterialSearcher.IsAsyncLoading() ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> SCMTParameterSearcher::GetProgressBarPercent() const
{
	if (!MaterialSearcher.IsAsyncLoading())
	{
		return 1.0f;
	}
	
	return MaterialSearcher.GetProgress();
}

// Progress Bar --- End

/** Search Start */
FReply SCMTParameterSearcher::ButtonSearchStartClicked()
{
	SearchStart();
	
	return FReply::Handled();
}

/* CopyClipboard clicked event */
FReply SCMTParameterSearcher::ButtonCopyClipBoardClicked()
{
	FString Clipboard = GetResultText();
	FCUTUtility::ExportClipboard(Clipboard);
	
	return FReply::Handled();
}

/* ExportText clicked event */
FReply SCMTParameterSearcher::ButtonExportTextClicked()
{
	FString Clipboard = GetResultText();
	FCUTUtility::ExportTxt("ParameterSearcher", "CMTParameterSearcher.txt", Clipboard, TEXT("Text |*.txt"));
	
	return FReply::Handled();
}

/** ExportCSV clicked event */
FReply SCMTParameterSearcher::ButtonExportCSVClicked()
{
	FString Clipboard = GetResultCSV();
	FCUTUtility::ExportTxt("ParameterSearcher", "CMTParameterSearcher.csv", Clipboard, TEXT("CSV |*.csv"));
	
	return FReply::Handled();
}


FString SCMTParameterSearcher::GetResultText()
{
	FString RetString;
	RetString = FString::Printf(TEXT("Search Path: %s\n"), *GetSearchPath());
	RetString += FString::Printf(TEXT("Search Name: %s\n\n"), *GetSearchValue());
	
	
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		RetString += FString::Printf(TEXT("- %s : %s\n"), *(*It)->GetDisplayText().ToString(), *(*It)->GetValueText().ToString());
	}
	return RetString;
}
FString SCMTParameterSearcher::GetResultCSV()
{
	FString RetString;
	for (auto It = ItemsFound.CreateConstIterator() ; It ; ++It)
	{
		RetString += FString::Printf(TEXT("%s,%s\n"), *(*It)->GetDisplayText().ToString(), *(*It)->GetValueText().ToString());
	}
	return RetString;
}

bool SCMTParameterSearcher::CheckMaterialParameterInfoScalar(UMaterialInterface* InMaterial, FText& OutValueText)
{
	bool bRetMatch = false;
	if (!IsValid(InMaterial))
	{
		return bRetMatch;
	}
	TArray<FMaterialParameterInfo> ParameterInfos;
	TArray<FGuid> ParameterIds;
	InMaterial->GetAllScalarParameterInfo(ParameterInfos, ParameterIds);
	for (const FMaterialParameterInfo& ParameterInfo : ParameterInfos)
	{
		float ParameterValue = 0.0f;
		if (InMaterial->GetScalarParameterValue(ParameterInfo, ParameterValue))
		{
			if (CheckMaterialParameterValueScalar(ParameterInfo, ParameterValue, OutValueText))
			{
				bRetMatch = true;
				break;
			}
		}
	}
	return bRetMatch;
}
bool SCMTParameterSearcher::CheckMaterialParameterInfoVector(UMaterialInterface* InMaterial, FText& OutValueText)
{
	bool bRetMatch = false;
	if (!IsValid(InMaterial))
	{
		return bRetMatch;
	}
	TArray<FMaterialParameterInfo> ParameterInfos;
	TArray<FGuid> ParameterIds;
	InMaterial->GetAllVectorParameterInfo(ParameterInfos, ParameterIds);
	for (const FMaterialParameterInfo& ParameterInfo : ParameterInfos)
	{
		FLinearColor ParameterValue = FLinearColor::Black;
		if (InMaterial->GetVectorParameterValue(ParameterInfo, ParameterValue))
		{
			if (CheckMaterialParameterValueVector(ParameterInfo, ParameterValue, OutValueText))
			{
				bRetMatch = true;
				break;
			}
		}
	}
	return bRetMatch;
}
bool SCMTParameterSearcher::CheckMaterialParameterInfoTexture(UMaterialInterface* InMaterial, FText& OutValueText)
{
	bool bRetMatch = false;
	if (!IsValid(InMaterial))
	{
		return bRetMatch;
	}
	TArray<FMaterialParameterInfo> ParameterInfos;
	TArray<FGuid> ParameterIds;
	InMaterial->GetAllTextureParameterInfo(ParameterInfos, ParameterIds);
	for (const FMaterialParameterInfo& ParameterInfo : ParameterInfos)
	{
		UTexture* ParameterValue = nullptr;
		if (InMaterial->GetTextureParameterValue(ParameterInfo, ParameterValue))
		{
			if (CheckMaterialParameterValueTexture(ParameterInfo, ParameterValue, OutValueText))
			{
				bRetMatch = true;
				break;
			}
		}
	}
	return bRetMatch;
}
bool SCMTParameterSearcher::CheckMaterialParameterInfoStaticSwitch(UMaterialInterface* InMaterial, FText& OutValueText)
{
	bool bRetMatch = false;
	if (!IsValid(InMaterial))
	{
		return bRetMatch;
	}
	TArray<FMaterialParameterInfo> ParameterInfos;
	TArray<FGuid> ParameterIds;
	InMaterial->GetAllStaticSwitchParameterInfo(ParameterInfos, ParameterIds);
	for (const FMaterialParameterInfo& ParameterInfo : ParameterInfos)
	{
		bool bParameterValue = false;
		FGuid ExpressionGuid;
		if (InMaterial->GetStaticSwitchParameterValue(ParameterInfo, bParameterValue, ExpressionGuid))
		{
			if (CheckMaterialParameterValueStaticSwitch(ParameterInfo, bParameterValue, OutValueText))
			{
				bRetMatch = true;
				break;
			}
		}
	}
	return bRetMatch;
}
bool SCMTParameterSearcher::CheckMaterialParameterValueScalar(const FMaterialParameterInfo& InParameterInfo, float InParameterValue, FText& OutValueText)
{
	bool bRetMatch = false;
	// 該当のパラメーター名の変数が有るか
	if (InParameterInfo.Name.Compare(*GetSearchValue()) == 0)
	{
		// 比較関数が一致するか
		if (CheckFunction(GetScalarValue(), InParameterValue))
		{
			bRetMatch = true;
			OutValueText = FText::FromString(FString::Printf(TEXT("%f"), InParameterValue));
		}
	}
	return bRetMatch;
}
bool SCMTParameterSearcher::CheckMaterialParameterValueVector(const FMaterialParameterInfo& InParameterInfo, FLinearColor InParameterValue, FText& OutValueText)
{
	bool bRetMatch = false;
	// 該当のパラメーター名の変数が有るか
	if (InParameterInfo.Name.Compare(*GetSearchValue()) == 0)
	{
		// 比較関数が一致するか
		if (CheckFunctionVector(InParameterValue))
		{
			bRetMatch = true;
			OutValueText = FText::FromString(FString::Printf(TEXT("R=%f,G=%f,B=%f,A=%f"),
				InParameterValue.R, InParameterValue.G, InParameterValue.B, InParameterValue.A));
		}
	}
	return bRetMatch;
}

bool SCMTParameterSearcher::CheckMaterialParameterValueTexture(const FMaterialParameterInfo& InParameterInfo, UTexture* InParameterValue, FText& OutValueText)
{
	bool bRetMatch = false;
	// 該当のパラメーター名の変数が有るか
	if (InParameterInfo.Name.Compare(*GetSearchValue()) == 0)
	{
		// 比較関数が一致するか
		if (CheckFunctionTexture(InParameterValue))
		{
			bRetMatch = true;
			OutValueText = FText::FromString(FString::Printf(TEXT("%s"),
				InParameterValue ? *InParameterValue->GetPathName() : TEXT("no texture")));
		}
	}
	return bRetMatch;
}

bool SCMTParameterSearcher::CheckMaterialParameterValueStaticSwitch(const FMaterialParameterInfo& InParameterInfo, bool bInParameterValue, FText& OutValueText)
{
	bool bRetMatch = false;
	// 該当のパラメーター名の変数が有るか
	if (InParameterInfo.Name.Compare(*GetSearchValue()) == 0)
	{
		// 比較関数が一致するか
		if (CheckFunctionBool(GetCheckBoxStateStaticSwitchValue() == ECheckBoxState::Checked, bInParameterValue))
		{
			bRetMatch = true;
			OutValueText = FText::FromString(FString::Printf(TEXT("%s"),
				bInParameterValue ? TEXT("ON") : TEXT("OFF")));
		}
	}
	return bRetMatch;
}

bool SCMTParameterSearcher::CheckFunction(float MatchValue, float Value)
{
	bool bRetMatch = false;
	switch (GetMatchFunction())
	{
	case ECMTParameterSearcherFunction::Equals:
		bRetMatch = FMath::IsNearlyEqual(MatchValue, Value); 
		break;
	case ECMTParameterSearcherFunction::NotEquals:
		bRetMatch = !FMath::IsNearlyEqual(MatchValue, Value); 
		break;
	case ECMTParameterSearcherFunction::GreaterThan:
		bRetMatch = (Value > MatchValue);
		break;
	case ECMTParameterSearcherFunction::GreaterThanEquals:
		bRetMatch = (Value >= (MatchValue-SMALL_NUMBER));
		break;
	case ECMTParameterSearcherFunction::LessThan:
		bRetMatch = (Value < MatchValue);
		break;
	case ECMTParameterSearcherFunction::LessThanEquals:
		bRetMatch = (Value <= (MatchValue+SMALL_NUMBER));
		break;
	case ECMTParameterSearcherFunction::All:
		bRetMatch = true;
		break;
	}
	return bRetMatch;
}

bool SCMTParameterSearcher::CheckFunctionBool(bool bMatchBalue, bool bValue)
{
	bool bRetMatch = false;
	switch (GetMatchFunction())
	{
	case ECMTParameterSearcherFunction::Equals:
		bRetMatch = (bMatchBalue == bValue);
		break;
	case ECMTParameterSearcherFunction::NotEquals:
		bRetMatch = (bMatchBalue != bValue);
		break;
	case ECMTParameterSearcherFunction::GreaterThan:
	case ECMTParameterSearcherFunction::GreaterThanEquals:
	case ECMTParameterSearcherFunction::LessThan:
	case ECMTParameterSearcherFunction::LessThanEquals:
		ensure(false);
		break;
	case ECMTParameterSearcherFunction::All:
		bRetMatch = true;
		break;
	}
	return bRetMatch;
}

bool SCMTParameterSearcher::CheckFunctionVector(FLinearColor Value)
{
	bool bRetMatch = false;
	switch (GetVectorFunction())
	{
	case ECMTParameterSearcherVectorFunction::All:
		{
			bool bMatch = true;
			bool bCheck = false;	// チェック処理したか（全部チェックOFFの際の対処）
			if (GetCheckBoxStateVectorR() == ECheckBoxState::Checked)	{ bCheck = true; bMatch = bMatch && CheckFunction(GetVectorValue().R, Value.R); }
			if (GetCheckBoxStateVectorG() == ECheckBoxState::Checked)	{ bCheck = true; bMatch = bMatch && CheckFunction(GetVectorValue().G, Value.G); }
			if (GetCheckBoxStateVectorB() == ECheckBoxState::Checked)	{ bCheck = true; bMatch = bMatch && CheckFunction(GetVectorValue().B, Value.B); }
			if (GetCheckBoxStateVectorA() == ECheckBoxState::Checked)	{ bCheck = true; bMatch = bMatch && CheckFunction(GetVectorValue().A, Value.A); }
			bRetMatch = bCheck && bMatch;
		}
		break;
	case ECMTParameterSearcherVectorFunction::Any:
		{
			bool bMatch = false;
			if (GetCheckBoxStateVectorR() == ECheckBoxState::Checked)	bMatch = bMatch || CheckFunction(GetVectorValue().R, Value.R);
			if (GetCheckBoxStateVectorG() == ECheckBoxState::Checked)	bMatch = bMatch || CheckFunction(GetVectorValue().G, Value.G);
			if (GetCheckBoxStateVectorB() == ECheckBoxState::Checked)	bMatch = bMatch || CheckFunction(GetVectorValue().B, Value.B);
			if (GetCheckBoxStateVectorA() == ECheckBoxState::Checked)	bMatch = bMatch || CheckFunction(GetVectorValue().A, Value.A);
			bRetMatch = bMatch;
		}
		break;
	}
	return bRetMatch;
}

bool SCMTParameterSearcher::CheckFunctionTexture(UTexture* Value)
{
	bool bRetMatch = false;
	FString InTexturePath(TEXT(""));
	if (Value)
	{
		InTexturePath = Value->GetPathName();
	}
	switch (GetMatchFunction())
	{
	case ECMTParameterSearcherFunction::Equals:
		if (InTexturePath.IsEmpty())
		{
			bRetMatch = GetTextureValue().IsEmpty();
		}
		else
		{
			bRetMatch = (InTexturePath.Find(GetTextureValue()) != INDEX_NONE);
		}
		break;
	case ECMTParameterSearcherFunction::NotEquals:
		if (InTexturePath.IsEmpty())
		{
			bRetMatch = !GetTextureValue().IsEmpty();
		}
		else
		{
			bRetMatch = (InTexturePath.Find(GetTextureValue()) == INDEX_NONE);
		}
		break;
	case ECMTParameterSearcherFunction::GreaterThan:
	case ECMTParameterSearcherFunction::GreaterThanEquals:
	case ECMTParameterSearcherFunction::LessThan:
	case ECMTParameterSearcherFunction::LessThanEquals:
		ensure(false);
		break;
	case ECMTParameterSearcherFunction::All:
		bRetMatch = true;
		break;
	}
	return bRetMatch;
}

#undef LOCTEXT_NAMESPACE

