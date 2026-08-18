// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpiaGameMode.generated.h"

class APlayerStart;

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

	// Motor 1. oyuncuyu (Tadashi) dogururken hangi PlayerStart'i secegini burada belirliyoruz.
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	UPROPERTY(EditDefaultsOnly, Category = "Opia|Characters")
	TSubclassOf<APawn> TadashiPawnClass;

	UPROPERTY(EditDefaultsOnly, Category = "Opia|Characters")
	TSubclassOf<APawn> AiriPawnClass;

	// Seviyedeki PlayerStart'lari isimle ayirmak icin kullanilan etiketler.
	// Editorde her PlayerStart'in "Player Start Tag" alanina bu degerler yazilir.
	UPROPERTY(EditDefaultsOnly, Category = "Opia|Spawn")
	FName TadashiStartTag = FName("Tadashi");

	UPROPERTY(EditDefaultsOnly, Category = "Opia|Spawn")
	FName AiriStartTag = FName("Airi");

private:
	void SetupSecondPlayer();

	// Verilen etikete sahip PlayerStart'i bulur, yoksa nullptr doner.
	APlayerStart* FindPlayerStartByTag(FName Tag) const;
};
