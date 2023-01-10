// Fill out your copyright notice in the Description page of Project Settings.


#include "MaterialExpressionCustomSubtract.h"

#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionCustom.h"

#if WITH_EDITOR
#include "MaterialGraph/MaterialGraphNode.h"
#endif
#if WITH_EDITOR
int32 UMaterialExpressionCustomSubtract::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	int32 Current = 0;
	if (Inputs.Num() > 0)
	{
		Current = Inputs[0].Input.Compile(Compiler);
		for (int i = 1; i < Inputs.Num(); i++)
		{
			if (Inputs[i].Input.GetTracedInput().Expression)
			{
				Current = Compiler->Sub(Current, Inputs[i].Input.Compile(Compiler));
			}
			else
			{
				Current = Compiler->Sub(Current, Compiler->Constant(0));
			}
		}
	}
	return Current;
}

void UMaterialExpressionCustomSubtract::GetCaption(TArray<FString>& OutCaptions) const
{
	FString ret = TEXT("CustomAdd");


	/*if (Inputs.Num() <= 2) 
	{
		FExpressionInput ATraced = A.GetTracedInput();
		FExpressionInput BTraced = B.GetTracedInput();
		if (!ATraced.Expression || !BTraced.Expression)
		{
			ret += TEXT("(");
			ret += ATraced.Expression ? TEXT(",") : FString::Printf(TEXT("%.4g,"), ConstA);
			ret += BTraced.Expression ? TEXT(")") : FString::Printf(TEXT("%.4g)"), ConstB);
		}
	};*/

	OutCaptions.Add(ret);
}

const TArray<FExpressionInput*> UMaterialExpressionCustomSubtract::GetInputs()
{
	//AddinputValues.SetNum(Valuecount);
	TArray<FExpressionInput*> Result;
	for (int i = 0; i < Inputs.Num(); i++)
	{
		Result.Add(&Inputs[i].Input);
	}
	return Result;
}

FName UMaterialExpressionCustomSubtract::GetInputName(int32 InputIndex) const
{
	if (InputIndex < Inputs.Num())
	{
		return Inputs[InputIndex].InputName;
	}
	return NAME_None;
}

void UMaterialExpressionCustomSubtract::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (UMaterialGraphNode* MatGraphNode = Cast<UMaterialGraphNode>(GraphNode))
	{
		MatGraphNode->RecreateAndLinkNode();
	}
}

#endif // WITH_EDITOR;
UMaterialExpressionCustomSubtract::UMaterialExpressionCustomSubtract(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCustomInput A;
	FCustomInput B;
	A.InputName = "A";
	B.InputName = "B";
	Inputs.Add(A);
	Inputs.Add(B);
}
