#include "MaterialCustomMultiply.h"
#include "MaterialCompiler.h"
#include "Materials/MaterialExpressionCustom.h"
#if WITH_EDITOR
#include "MaterialGraph/MaterialGraphNode.h"
#endif


#if WITH_EDITOR
//const float ConstAA = 0.f;
//const float ConstBB = 1.f;
int32 UMaterialExpressionCustomMultiply::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	int32 Current = 1;
	if(Inputs.Num()>0)
	{
		Current = Inputs[0].Input.Compile(Compiler);
		for (int i = 1; i < Inputs.Num(); i++)
		{
			if(Inputs[i].Input.GetTracedInput().Expression)
			{
				Current = Compiler->Mul(Current, Inputs[i].Input.Compile(Compiler));
			}
			else
			{
				Current = Compiler->Mul(Current,Compiler->Constant(1));
			}
		
		}
	}
	
	return Current;
}
void UMaterialExpressionCustomMultiply::GetCaption(TArray<FString>& OutCaptions) const
{
	FString ret = TEXT("CustomMultiply");
	OutCaptions.Add(ret);
}
const TArray<FExpressionInput*> UMaterialExpressionCustomMultiply::GetInputs()
{
	TArray<FExpressionInput*> Result;
	for (int i = 0; i < Inputs.Num(); i++)
	{
		Result.Add(&Inputs[i].Input);
	}
	return Result;
}

FName UMaterialExpressionCustomMultiply::GetInputName(int32 InputIndex) const
{
	if( InputIndex < Inputs.Num() )
	{
		return Inputs[InputIndex].InputName;
	}
	return NAME_None;
}

void UMaterialExpressionCustomMultiply::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (UMaterialGraphNode* MatGraphNode = Cast<UMaterialGraphNode>(GraphNode))
	{

		MatGraphNode->RecreateAndLinkNode();
	}

}

#endif // WITH_EDITOR;
UMaterialExpressionCustomMultiply::UMaterialExpressionCustomMultiply(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FCustomInput A;
	FCustomInput B;
	A.InputName = "A";
	B.InputName = "B";
	Inputs.Add(A);
	Inputs.Add(B);
	
}
