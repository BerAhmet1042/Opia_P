// Fill out your copyright notice in the Description page of Project Settings.

#include "Game_Mode/OpiaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

AOpiaGameMode::AOpiaGameMode()
{
}

void AOpiaGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetupSecondPlayer();
}

void AOpiaGameMode::SetupSecondPlayer()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] GameInstance yok, ikinci oyuncu kurulamadi."));
		return;
	}

	// Zaten iki oyuncu varsa tekrar kurma.
	if (GameInstance->GetNumLocalPlayers() >= 2)
	{
		UE_LOG(LogTemp, Log, TEXT("[Opia] Ikinci oyuncu zaten var, atlaniyor."));
		return;
	}

	// AiriPawnClass GameMode Blueprint'inde atanmis mi?
	if (!AiriPawnClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] AiriPawnClass BOS. GameMode Blueprint'inde BP_Airi'yi ata."));
		return;
	}

	FString Error;
	ULocalPlayer* SecondPlayer = GameInstance->CreateLocalPlayer(1, Error, true);
	if (!SecondPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] Ikinci local player olusturulamadi: %s"), *Error);
		return;
	}

	APlayerController* SecondPC = SecondPlayer->GetPlayerController(GetWorld());
	if (!SecondPC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] Ikinci PlayerController bulunamadi."));
		return;
	}

	// CreateLocalPlayer, ikinci oyuncuya GameMode'un varsayilan pawn'ini (BP_Tadashi)
	// otomatik dogurur. Airi'yi koymadan once onu temizle, yoksa sahnede sahipsiz
	// bir Tadashi kalir.
	if (APawn* AutoSpawnedPawn = SecondPC->GetPawn())
	{
		UE_LOG(LogTemp, Log, TEXT("[Opia] Otomatik dogan pawn temizleniyor: %s"), *AutoSpawnedPawn->GetName());
		SecondPC->UnPossess();
		AutoSpawnedPawn->Destroy();
	}

	// Airi'nin dogacagi yer: ikinci PlayerStart varsa orasi.
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (PlayerStarts.Num() > 1)
	{
		SpawnLocation = PlayerStarts[1]->GetActorLocation();
		SpawnRotation = PlayerStarts[1]->GetActorRotation();
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Opia] Ikinci PlayerStart yok. Airi (0,0,0) noktasinda dogacak — zeminin altinda kalabilir."));
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* AiriPawn = GetWorld()->SpawnActor<APawn>(AiriPawnClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!AiriPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] Airi pawn spawn edilemedi."));
		return;
	}

	SecondPC->Possess(AiriPawn);

	UE_LOG(LogTemp, Log, TEXT("[Opia] Airi olusturuldu ve possess edildi. Konum: %s"), *SpawnLocation.ToString());
}
