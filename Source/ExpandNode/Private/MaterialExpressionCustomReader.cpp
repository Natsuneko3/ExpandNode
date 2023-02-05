// Copyright 2020 Jorge CR. All Rights Reserved.
#include "MaterialExpressionCustomReader.h"



#if WITH_EDITOR
#include "MaterialEditor/Public/MaterialEditingLibrary.h"
#endif
#include <Misc/FileHelper.h>
#include <Async/Async.h>
#include <Materials/MaterialExpressionAdd.h>

#include "ExpandNode.h"
#include "FileScanner.h"


#define LOCTEXT_NAMESPACE "MaterialExpressionReader"

UMaterialExpressionCustomReader::UMaterialExpressionCustomReader(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Custom = nullptr;
	Code = TEXT("return 1;");
	NodeName = TEXT("Reader");
	OutputType = CMOT_Float3;

#if WITH_EDITORONLY_DATA
	MenuCategories.Add(LOCTEXT("Custom", "Custom"));
#endif

#if WITH_EDITORONLY_DATA
	bCollapsed = false;
#endif // WITH_EDITORONLY_DATA
	Custom = CreateDefaultSubobject<UMaterialExpressionCustom>(TEXT("CustomNode_0"));

	Update();
}

UMaterialExpressionCustomReader::~UMaterialExpressionCustomReader()
{
	
}

#if WITH_EDITOR
void UMaterialExpressionCustomReader::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property->GetFName().ToString() == "FilePath")
	{
		if (FPaths::FileExists(File.FilePath))
		{
			FExpandNodeModule::GetFileScanner()->RegisterFile(File.FilePath);
		}
		
	}

	// strip any spaces from input name
	FProperty* PropertyThatChanged = PropertyChangedEvent.Property;
	if (PropertyThatChanged && PropertyThatChanged->GetFName() == GET_MEMBER_NAME_CHECKED(FCustomInput, InputName))
	{
		for (FCustomInput& Input : Inputs)
		{
			FString InputName = Input.InputName.ToString();
			if (InputName.ReplaceInline(TEXT(" "), TEXT("")) > 0)
			{
				Input.InputName = *InputName;
			}
		}
	}

	RebuildOutputs();

	if (PropertyChangedEvent.MemberProperty && GraphNode && ContainExpression())
	{
		const FName PropertyName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UMaterialExpressionCustom, Inputs) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UMaterialExpressionCustom, AdditionalOutputs))
		{
			GraphNode->ReconstructNode();
		}
	}
	Custom->IncludeFilePaths = IncludeFilePaths;
	

	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UMaterialExpressionCustomReader::RebuildOutputs()
{
	Outputs.Reset(Custom->AdditionalOutputs.Num() + 1);
	if (Custom->AdditionalOutputs.Num() == 0)
	{
		bShowOutputNameOnPin = false;
		Outputs.Add(FExpressionOutput(TEXT("")));
	}
	else
	{
		bShowOutputNameOnPin = true;
		Outputs.Add(FExpressionOutput(TEXT("return")));
		for (const FCustomOutput& CustomOutput : Custom->AdditionalOutputs)
		{
			if (!CustomOutput.OutputName.IsNone())
			{
				Outputs.Add(FExpressionOutput(CustomOutput.OutputName));
			}
		}
	}
}

#endif // WITH_EDITOR

void UMaterialExpressionCustomReader::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	if (FPaths::FileExists(File.FilePath))
	{
		FExpandNodeModule::GetFileScanner()->RegisterFile(File.FilePath);
	}
	
	
#endif
}

void UMaterialExpressionCustomReader::Update()
{
	if (!Custom)
	{
		Custom = NewObject<UMaterialExpressionCustom>(this, TEXT("CustomTransientNode"));
	}

	Custom->Code = Code;
	Custom->OutputType = OutputType;
	Custom->Description = NodeName;
	Custom->Inputs = Inputs;
	Custom->AdditionalOutputs = CustomOutputs;
}

#if WITH_EDITOR
int32 UMaterialExpressionCustomReader::Compile(class FMaterialCompiler* Compiler, int32 OutputIndex)
{
	if (!Custom) return 0;
	Update();
	return Custom->Compile(Compiler, OutputIndex);
}

void UMaterialExpressionCustomReader::GetCaption(TArray<FString>& OutCaptions) const
{
	if (!Custom) return;
	Custom->GetCaption(OutCaptions);
	OutCaptions.Add(NodeName);
}

const TArray<FExpressionInput*> UMaterialExpressionCustomReader::GetInputs()
{
	TArray<FExpressionInput*> Result;
	for (int32 i = 0; i < Inputs.Num(); i++)
	{
		Result.Add(&Inputs[i].Input);
	}
	return Result;
}

FExpressionInput* UMaterialExpressionCustomReader::GetInput(int32 InputIndex)
{
	if (InputIndex < Inputs.Num())
	{
		return &Inputs[InputIndex].Input;
	}
	return NULL;
}

FName UMaterialExpressionCustomReader::GetInputName(int32 InputIndex) const
{
	if (!Custom) return NAME_None;

	if (InputIndex < Inputs.Num())
	{
		Custom->GetInputName(InputIndex);
		return Inputs[InputIndex].InputName;
	}
	return NAME_None;
}

uint32 UMaterialExpressionCustomReader::GetInputType(int32 InputIndex)
{
	return MCT_Unknown;
}

uint32 UMaterialExpressionCustomReader::GetOutputType(int32 OutputIndex)
{
	if (!Custom) return 0;
	return Custom->GetOutputType(OutputIndex);
}

bool UMaterialExpressionCustomReader::ParseFile(const FString& File, FString& OutCode,TArray<FString>& InputVariables, TArray<FString>& OutputVariables,TArray<ECustomMaterialOutputType>&OutputTypes)
{
	if (!FPaths::FileExists(File)) return false;

	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *File)) return false;

	if(!Lines.Num()) return false;

	InputVariables.Empty();
	OutputVariables.Empty();
	OutputTypes.Empty();
	int32 CodeStart = -1;
	for (int32 i = 0; i < Lines.Num(); i++)
	{
		CodeStart++;
		if ((Lines[i].Contains("float") || Lines[i].Contains("int") || Lines[i].Contains("texture")
			|| Lines[i].Contains("double")|| Lines[i].Contains("uint")|| Lines[i].Contains("half")) && Lines[i].Contains(";"))
		{
			FString Left;
			FString right;
			FString Variable;
			Lines[i].Split(" ", &Left, &right);
			if(Left == "out")
			{
				
				FString End;
				FString OutputType;
				right.Split(" ", &OutputType, &Variable);
				//switch OutputType
				if(OutputType=="float")
				{
					OutputTypes.Add(ECustomMaterialOutputType::CMOT_Float1);
				}
				else if(OutputType=="float2")
				{
					OutputTypes.Add(ECustomMaterialOutputType::CMOT_Float2);
				}
				else if(OutputType=="float3")
				{
					OutputTypes.Add(ECustomMaterialOutputType::CMOT_Float3);
				}
				else if(OutputType=="float4")
				{
					OutputTypes.Add(ECustomMaterialOutputType::CMOT_Float4);
				}
				else
				{
					OutputTypes.Add(ECustomMaterialOutputType::CMOT_Float3);
				}
				
				Variable.Split(";",&End,nullptr);
				if (!End.IsEmpty())
				{
					OutputVariables.Add(End);
				}
				continue;
			}
			
			//Lines[i].Split(" ", nullptr, &right);
			right.Split(";", &Variable, nullptr);
			if (!Variable.IsEmpty())
			{
				InputVariables.Add(Variable);
			}
		}
		else
		{
			break;
		}
	}

	OutCode = FString();
	for (int32 i = CodeStart; i < Lines.Num(); i++)
	{
		if (i == Lines.Num() - 1)
		{
			OutCode += Lines[i];
		}
		else
		{
			OutCode += Lines[i] + "\n";
		}
	}

	return (OutCode.IsEmpty()) ? false : true;
}

bool UMaterialExpressionCustomReader::HasDifferentVariables(const TArray<FString>& InVariables)
{
	if (InVariables.Num() == Inputs.Num())
	{
		for (int32 i = 0; i < InVariables.Num(); i++)
		{
			bool bFound = false;

			for (auto& input : Inputs)
			{
				if (InVariables[i] == input.InputName.ToString())
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				return true;
			}
		}
		return false;
	}
	return true;
}

bool UMaterialExpressionCustomReader::HasDifferentOutVariables(const TArray<FString>& InVariables)
{
	if (InVariables.Num() == CustomOutputs.Num())
	{
		for (int32 i = 0; i < InVariables.Num(); i++)
		{
			bool bFound = false;

			for (auto& Output : CustomOutputs)
			{
				if (InVariables[i] == Output.OutputName.ToString())
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				return true;
			}
		}
		return false;
	}
	return true;
}


void UMaterialExpressionCustomReader::UpdateInputs(const TArray<FString>& InVariables)
{
	while (HasDifferentVariables(InVariables))
	{
		for (int32 i = 0; i < InVariables.Num(); i++)
		{
			bool bFound = false;

			for (auto& input : Inputs)
			{
				if (InVariables[i] == input.InputName.ToString())
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				FCustomInput Input;
				Input.InputName = FName(*InVariables[i]);
				Inputs.Add(Input);
			}
		}

		for(int32 i = 0; i < Inputs.Num(); i++)
		{
			bool bFound = false;

			for (auto& var : InVariables)
			{
				if (var == Inputs[i].InputName.ToString())
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				Inputs.RemoveAt(i);
			}
		}
	}

}

void UMaterialExpressionCustomReader::UpdateOutputs(const TArray<FString>& InVariables,const TArray<ECustomMaterialOutputType>& OutputTypes)
{
	while (HasDifferentOutVariables(InVariables))
	{
		for (int32 i = 0; i < InVariables.Num(); i++)
		{
			bool bFound = false;

			for (auto& Output : CustomOutputs)
			{
				if (InVariables[i] == Output.OutputName.ToString())
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				FCustomOutput Output;
				Output.OutputName = FName(*InVariables[i]);
				Output.OutputType = OutputTypes[i];
				CustomOutputs.Add(Output);
				//Outputs.Add(FExpressionOutput(FName(InVariables[i])));
			}
		}

		for(int32 i = 0; i < CustomOutputs.Num(); i++)
		{
			bool bFound = false;

			for (auto& var : InVariables)
			{
				if (var == CustomOutputs[i].OutputName.ToString())
				{
					bFound = true;
				}
			}

			if (!bFound)
			{
				CustomOutputs.RemoveAt(i);
			}
		}
	}
}

void UMaterialExpressionCustomReader::UpdateNode(const FString& InCode, const TArray<FString>& InVariables,const TArray<FString>& OutVariables,const TArray<ECustomMaterialOutputType>& OutputtypesArray)
{
	if(!ContainExpression()) return;

	bool bCompileMaterial = false;
	if (Code != InCode)
	{
		Code = InCode;
		Custom->Code = Code;
		bCompileMaterial = true;
	}

	if (!Inputs.Num())
	{
		for (auto& var : InVariables)
		{
			FCustomInput Input;
			Input.InputName = FName(*var);
			Inputs.Add(Input);
		}
		Custom->Inputs = Inputs;
		if (GraphNode && ContainExpression())
		{
			GraphNode->ReconstructNode();
		}
	}
	
	if(!CustomOutputs.Num())
	{
		int OutputNum = 0;
		for (auto& var : OutVariables)
		{
			
			FCustomOutput Output;
			Output.OutputName = FName(*var);
			Output.OutputType = OutputtypesArray[OutputNum];
			CustomOutputs.Add(Output);
			OutputNum++;
			
		}
		Custom->AdditionalOutputs = CustomOutputs;
		RebuildOutputs();
		
		
		if (GraphNode && ContainExpression())
		{
			GraphNode->ReconstructNode();
		}
		
	}
	
	
	
	if (HasDifferentVariables(InVariables))
	{
		UpdateInputs(InVariables);
		Custom->Inputs = Inputs;
		//~ Refresh node
		if (GraphNode && ContainExpression())
		{
			GraphNode->ReconstructNode();
		}
	}
	
	if(HasDifferentOutVariables(OutVariables))
	{
		UpdateOutputs(OutVariables,OutputtypesArray);
		Custom->AdditionalOutputs = CustomOutputs;
		RebuildOutputs();
		//Custom->RebuildOutputs();
		//~ Refresh node
		if (GraphNode && ContainExpression())
		{
			GraphNode->ReconstructNode();
		}
	}

	if (bCompileMaterial)
	{
		UMaterialEditingLibrary::RecompileMaterial(Material);
	}
}

void UMaterialExpressionCustomReader::OpenFileInExplorer()
{
	if (FPaths::FileExists(File.FilePath))
	{
		FPlatformProcess::ExploreFolder(*File.FilePath);
	}
}

void UMaterialExpressionCustomReader::EditFile()
{
	if (FPaths::FileExists(File.FilePath))
	{
		FPlatformProcess::LaunchFileInDefaultExternalApplication(*File.FilePath);
	}
}

bool UMaterialExpressionCustomReader::ContainExpression()
{
	for (auto& e : Material->GetExpressions())
	{
		if (e == this)
		{
			return true;
		}
	}

	return false;
}

#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE