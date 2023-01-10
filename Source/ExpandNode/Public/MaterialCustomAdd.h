// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "MaterialExpressionIO.h"
#include "Materials/MaterialExpression.h"
#include "MaterialCustomAdd.generated.h"



UCLASS(MinimalAPI)
class UMaterialExpressionCustomAdd : public UMaterialExpression
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(EditAnywhere, Category=MaterialExpressionCustomAdd)
	TArray<struct FCustomInput> Inputs;
	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual FText GetKeywords() const override { return FText::FromString(TEXT("+")); }
	virtual const TArray<FExpressionInput*> GetInputs()override;
	virtual FName GetInputName(int32 InputIndex) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface
};
