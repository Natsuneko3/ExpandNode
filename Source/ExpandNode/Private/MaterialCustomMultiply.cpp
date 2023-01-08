#include "MaterialCustomMultiply.h"
#include "MaterialCompiler.h"
#if WITH_EDITOR
#include "MaterialGraph/MaterialGraphNode.h"
#endif


#if WITH_EDITOR
//const float ConstAA = 0.f;
//const float ConstBB = 1.f;
int32 UMaterialExpressionCustomMultiply::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (AddinputValues.Num() <= 2)
	{
		int32 Arg1 = A.GetTracedInput().Expression ? A.Compile(Compiler) : Compiler->Constant(ConstA);

		int32 Arg2 = B.GetTracedInput().Expression ? B.Compile(Compiler) : Compiler->Constant(ConstB);

		return Compiler->Mul(Arg1, Arg2);
	}
	int32 Current = AddinputValues[0].Compile(Compiler);
	for (int i = 1; i < AddinputValues.Num(); i++)
	{
		
			Current = Compiler->Mul(Current, AddinputValues[i].Compile(Compiler));
		
	}
	return Current;
}
void UMaterialExpressionCustomMultiply::GetCaption(TArray<FString>& OutCaptions) const
{
	FString ret = TEXT("CustomMultiply");


	if (AddinputValues.Num() <= 2)
	{
		FExpressionInput ATraced = A.GetTracedInput();
		FExpressionInput BTraced = B.GetTracedInput();
		if (!ATraced.Expression || !BTraced.Expression)
		{
			ret += TEXT("(");
			ret += ATraced.Expression ? TEXT(",") : FString::Printf(TEXT("%.4g,"), ConstA);
			ret += BTraced.Expression ? TEXT(")") : FString::Printf(TEXT("%.4g)"), ConstB);
		}
	};

	OutCaptions.Add(ret);
}
const TArray<FExpressionInput*> UMaterialExpressionCustomMultiply::GetInputs()
{
	AddinputValues.SetNum(Valuecount);
	TArray<FExpressionInput*> Result;
	for (int i = 0; i < AddinputValues.Num(); i++)
	{
		Result.Add(&AddinputValues[i]);
	}
	return Result;
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
	ConstA = 0.0f;
	ConstB = 1.0f;
	Valuecount = 2;
}
