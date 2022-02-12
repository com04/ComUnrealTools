// Copyright com04 All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Engine/StreamableManager.h"
#include "SlateFwd.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STreeView.h"

#include "CMTParameterSearcherResult.h"
#include "MaterialTools/Utility/CMTMaterialSearcher.h"


typedef TSharedPtr<class FCMTParameterSearcherResult> FCMTParameterSearcherResultShare;
typedef STreeView<FCMTParameterSearcherResultShare>  SCMTParameterSearcherTreeViewType;
struct FAssetData;
class UMaterialExpressionComment;
class SBox;
class SButton;
class SCheckBox;

/** Vectorパラメーターのマッチ条件 */
UENUM()
enum class ECMTParameterSearcherVectorFunction : uint8
{
	All,
	Any
};
/** パラメーターのマッチ条件 */
UENUM()
enum class ECMTParameterSearcherFunction : uint8
{
	Equals,
	NotEquals,
	GreaterThan,
	GreaterThanEquals,
	LessThan,
	LessThanEquals,
	All
};

/**
 * Implements the launcher application
 */
class SCMTParameterSearcher
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SCMTParameterSearcher) { }
	SLATE_END_ARGS()

public:
	/**
	 * Constructs the application.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(const FArguments& InArgs);


	/** Destructor. */
	~SCMTParameterSearcher();

public:

protected:

	// SCompoundWidget overrides

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

private:
	// 現在の選択型種類
	enum class ESelectType : uint8
	{
		Scalar,
		Vector,
		Texture,
		StaticSwitch,
	};
	
	// Search  --- Begin
	void OnSearchPathCommitted(const FText& Text, ETextCommit::Type CommitType);
	void OnSearchTextCommitted(const FText& Text, ETextCommit::Type CommitType);
	// Search  --- End
	
	
	/** Search */
	void SearchStart();
	
	/** search finish callback */
	void FinishSearch();
	
	
	// Check box --- Begin
	void OnCheckBoxScalarChanged(ECheckBoxState InValue);
	void OnCheckBoxVectorChanged(ECheckBoxState InValue);
	void OnCheckBoxTextureChanged(ECheckBoxState InValue);
	void OnCheckBoxStaticSwitchChanged(ECheckBoxState InValue);
	void SetupCheckBoxType();
	void OnCheckBoxVectorRChanged(ECheckBoxState InValue);
	void OnCheckBoxVectorGChanged(ECheckBoxState InValue);
	void OnCheckBoxVectorBChanged(ECheckBoxState InValue);
	void OnCheckBoxVectorAChanged(ECheckBoxState InValue);
	void OnCheckBoxStaticSwitchValueChanged(ECheckBoxState InValue);
	void OnCheckBoxOverrideOnlyChanged(ECheckBoxState InValue);
	// Check box --- End
	
	// SSpinBox --- Begin
	void OnSpinBoxScalarValueChanged(float InValue);
	void OnSpinBoxVectorRValueChanged(float InValue);
	void OnSpinBoxVectorGValueChanged(float InValue);
	void OnSpinBoxVectorBValueChanged(float InValue);
	void OnSpinBoxVectorAValueChanged(float InValue);
	// SSpinBox --- End
	
	// TextureValue --- Begin
	void OnTextureValueChanged(const FText& Text);
	void OnTextureValueCommitted(const FText& Text, ETextCommit::Type CommitType);
	// TextureValue --- End
	
	// SComboBox --- Begin
	TSharedRef<SWidget> GenerateVectorFunctionItem(TSharedPtr<ECMTParameterSearcherVectorFunction> InFunction);
	FText GetVectorFunctionText(TSharedPtr<ECMTParameterSearcherVectorFunction> InFunction) const;
	FText GetVectorFunctionText(ECMTParameterSearcherVectorFunction InFunction) const;
	FText GetSelectionVectorFunctionText() const;
	void OnSelectionChangedVectorFunction(TSharedPtr<ECMTParameterSearcherVectorFunction> InFunction, ESelectInfo::Type InSelectInfo);
	
	TSharedRef<SWidget> GenerateFunctionItem(TSharedPtr<ECMTParameterSearcherFunction> InFunction);
	FText GetFunctionText(TSharedPtr<ECMTParameterSearcherFunction> InFunction) const;
	FText GetFunctionText(ECMTParameterSearcherFunction InFunction) const;
	FText GetSelectionFunctionText() const;
	void OnSelectionChangedFunction(TSharedPtr<ECMTParameterSearcherFunction> InFunction, ESelectInfo::Type InSelectInfo);
	// SComboBox --- End
	
	
	// Result Tree --- Begin
	
	/** row generate */
	TSharedRef<ITableRow> OnGenerateRow(FCMTParameterSearcherResultShare InItem, const TSharedRef<STableViewBase>& OwnerTable);
	/** tree child */
	void OnGetChildren(FCMTParameterSearcherResultShare InItem, TArray<FCMTParameterSearcherResultShare>& OutChildren);
	/** item double click event */
	void OnTreeSelectionDoubleClicked(FCMTParameterSearcherResultShare Item);
	
	// Result Tree --- End
	
	// Progress Bar --- Begin
	
	/** progress bar is now search? */
	EVisibility GetProgressBarVisibility() const;
	
	/** progress bar progress */
	TOptional<float> GetProgressBarPercent() const;

	// Progress Bar --- End
	
	
	// Button --- Begin
	
	/** Search Start */
	FReply ButtonSearchStartClicked();
	
	/** CopyClipboard clicked event */
	FReply ButtonCopyClipBoardClicked();
	
	/** ExportText clicked event */
	FReply ButtonExportTextClicked();
	
	/** ExportCSV clicked event */
	FReply ButtonExportCSVClicked();
	
	// Button --- End
	
	FString GetResultText();
	FString GetResultCSV();
	
	// マテリアルのパラメーターがマッチするか
	bool CheckMaterialParameterInfoScalar(UMaterialInterface* InMaterial, FText& OutValueText);
	bool CheckMaterialParameterInfoVector(UMaterialInterface* InMaterial, FText& OutValueText);
	bool CheckMaterialParameterInfoTexture(UMaterialInterface* InMaterial, FText& OutValueText);
	bool CheckMaterialParameterInfoStaticSwitch(UMaterialInterface* InMaterial, FText& OutValueText);
	bool CheckMaterialParameterValueScalar(const FMaterialParameterInfo& InParameterInfo, float InParameterValue, FText& OutValueText);
	bool CheckMaterialParameterValueVector(const FMaterialParameterInfo& InParameterInfo, FLinearColor InParameterValue, FText& OutValueText);
	bool CheckMaterialParameterValueTexture(const FMaterialParameterInfo& InParameterInfo, UTexture* InParameterValue, FText& OutValueText);
	bool CheckMaterialParameterValueStaticSwitch(const FMaterialParameterInfo& InParameterInfo, bool bInParameterValue, FText& OutValueText);
	
	/** 値が比較関数にマッチするか */
	bool CheckFunction(float MatchValue, float Value);
	bool CheckFunctionBool(bool bMatchBalue, bool bValue);
	bool CheckFunctionVector(FLinearColor Value);
	bool CheckFunctionTexture(UTexture* Value);
	
	
	
	TArray<TSharedPtr<ECMTParameterSearcherVectorFunction>> VectorFunctionSource;
	TArray<TSharedPtr<ECMTParameterSearcherFunction>> FunctionSource;
	
	TSharedPtr<SCheckBox> ScalarCheckBox;
	TSharedPtr<SCheckBox> VectorCheckBox;
	TSharedPtr<SCheckBox> TextureCheckBox;
	TSharedPtr<SCheckBox> StaticSwitchCheckBox;
	TSharedPtr<SBox> ScalarGroupWidget;
	TSharedPtr<SBox> VectorGroupWidget;
	TSharedPtr<SBox> VectorRGroupWidget;
	TSharedPtr<SBox> VectorGGroupWidget;
	TSharedPtr<SBox> VectorBGroupWidget;
	TSharedPtr<SBox> VectorAGroupWidget;
	TSharedPtr<SBox> TextureGroupWidget;
	TSharedPtr<SBox> StaticSwitchGroupWidget;
	/** result tree widget instance*/
	TSharedPtr<SCMTParameterSearcherTreeViewType> TreeView;
	
	TSharedPtr<SButton> SearchStartButton;
	TSharedPtr<SButton> CopyClipBoardButton;
	TSharedPtr<SButton> ExportTextButton;
	TSharedPtr<SButton> ExportCsvButton;
	
	/** changed SearchPath */
	bool bDirtySearchPath;
	
	/** changed SearchPath */
	bool bDirtySearchText;
	
	/** search string list. from SearchPath */
	TArray<FString> SearchPathTokens;
	
	/** search string list. from SearchValue */
	TArray<FString> SearchTokens;
	
	
	/** Result tree highlighting text (= Search text) */
	FText HighlightText;
	
	
	/** material search utility class */
	FCMTMaterialSearcher MaterialSearcher;
	
	
	/** Find result list */
	TArray<FCMTParameterSearcherResultShare> ItemsFound;
	
	
	
	/** search path text */
	static FString SearchPath;
	
	/** search text */
	static FString SearchValue;
	
	/** パラメーター数値 */
	static float ScalarValue;
	static FLinearColor VectorValue;
	static FString TextureValue;
	
	/** Vectorの検索関数 */
	static ECMTParameterSearcherVectorFunction VectorFunction;
	/** 比較関数 */
	static ECMTParameterSearcherFunction MatchFunction;
	
	/** 検索型種類 */
	static ESelectType SelectType;
	static ECheckBoxState CheckBoxStateVectorR;
	static ECheckBoxState CheckBoxStateVectorG;
	static ECheckBoxState CheckBoxStateVectorB;
	static ECheckBoxState CheckBoxStateVectorA;
	static ECheckBoxState CheckBoxStateStaticSwitchValue;
	static ECheckBoxState CheckBoxStateOverrideOnly;
};
