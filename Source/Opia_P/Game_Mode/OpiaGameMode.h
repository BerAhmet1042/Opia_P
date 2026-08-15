// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpiaGameMode.generated.h"

/**
 * 
 */
UCLASS()
class OPIA_P_API AOpiaGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AOpiaGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Opia|Characters")
	TSubclassOf<APawn> TadashiPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Opia|Characters")
	TSubclassOf<APawn> AiriPawnClass;

private:
	void SetupSecondPlayer();
};
