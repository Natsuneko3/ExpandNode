// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpression.h"
#include "MyPluginsAddNode.generated.h"



UCLASS(MinimalAPI)
class UMaterialExpressionCustomAdd : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()
		/** only used if A is not hooked up */
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustomAdd, meta = (OverridingInputProperty = "A"))
	float ConstA;
	/** only used if B is not hooked up */
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustomAdd, meta = (OverridingInputProperty = "B"))
	float ConstB;
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustomAdd)
	int Valuecount;
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstA' if not specified"))
	FExpressionInput A;
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstB' if not specified"))
	FExpressionInput B;
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstB' if not specified"))
	FExpressionInput C;
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstB' if not specified"))
	FExpressionInput D;
	UPROPERTY(meta = (RequiredInput = "false", ToolTip = "Defaults to 'ConstB' if not specified"))
	FExpressionInput E;

	
	UPROPERTY()
	TArray<FExpressionInput> AddinputValues;

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual FText GetKeywords() const override { return FText::FromString(TEXT("+")); }
	virtual const TArray<FExpressionInput*> GetInputs()override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface
};
