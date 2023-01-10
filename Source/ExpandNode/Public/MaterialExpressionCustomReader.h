// Copyright 2020 Jorge CR. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <Materials/MaterialExpression.h>
#include <Tickable.h>
#include <Materials/MaterialExpressionCustom.h>
#include "MaterialExpressionCustomReader.generated.h"


/**
 *	Custom Reader node of HLSL code inside material editor
 */
 UCLASS()
class EXPANDNODE_API UMaterialExpressionCustomReader : public UMaterialExpression
{
    GENERATED_UCLASS_BODY()

	~UMaterialExpressionCustomReader();

private:
	//~ Pointer to the material custom node used by this reader
	UPROPERTY()
		UMaterialExpressionCustom* Custom;

public:
	//~ Name for this node
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustom)
		FString NodeName;

	//~ Filepath to selected a usf file
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustom)//,meta = (RelativeToGameDir)
		FFilePath File;

	//~ Code read from file, not modifiable but visible
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustom, meta = (MultiLine = true))
		FString Code;

	//~ Output type
	UPROPERTY(EditAnywhere, Category = MaterialExpressionCustom)
		TEnumAsByte<enum ECustomMaterialOutputType> OutputType;

	//~ Inputs, this will go inside the material expression custom node
	UPROPERTY()
		TArray<struct FCustomInput> Inputs;

	//~ Begin UObject Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	virtual void PostLoad() override;

	void Update();

	//~ Begin UMaterialExpression Interface
#if WITH_EDITOR
	virtual int32 Compile(class FMaterialCompiler* Compiler, int32 OutputIndex) override;
	virtual void GetCaption(TArray<FString>& OutCaptions) const override;
	virtual const TArray<FExpressionInput*> GetInputs() override;
	virtual FExpressionInput* GetInput(int32 InputIndex) override;
	virtual FName GetInputName(int32 InputIndex) const override;
	virtual uint32 GetInputType(int32 InputIndex) override;
	virtual uint32 GetOutputType(int32 OutputIndex) override;
#endif // WITH_EDITOR
	//~ End UMaterialExpression Interface


#if WITH_EDITOR
public:
	/**
	 * Parse a new file and return the code and the variables found inside
	 *
	 * @param: File			- File to parse
	 * @param: OutCode		- String containing the code
	 * @param: OutVariables - String containing the variables found
	*/
	static bool ParseFile(const FString& File, FString& OutCode, TArray<FString>& OutVariables);

private:
	/**
	 * Check if the file has different variables from before
	 *
	 * @param: InVariables	- New set of variables to compare when the previous ones
	*/
	bool HasDifferentVariables(const TArray<FString>& InVariables);

	/**
	 * update the variables inputs for this node
	 *
	 * @param: InVariables	- New set of variables to set 
	*/
	void UpdateInputs(const TArray<FString>& InVariables);

	//~ Check if the owner material contain this node expression 
	//~ (note UE does not remove nodes until refresh the material editor, so this lead to crashes trying to refresh a node that was remove from the material)
	bool ContainExpression();

public:
	
	/**
	 *	Update this node with new code and variables
	 *
	 * @param: InCode		- Code to update
	 * @param: InVariables	- Variables to update
	*/
	void UpdateNode(const FString& InCode, const TArray<FString>& InVariables);
	

	
	/*Open your file in the default editor if the file is available*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = MaterialExpressionCustom)
		void EditFile();

	/*Open your file in explorer if the file is available*/
	UFUNCTION(BlueprintCallable, CallInEditor, Category = MaterialExpressionCustom)
		void OpenFileInExplorer();

#endif // WITH_EDITOR
};
