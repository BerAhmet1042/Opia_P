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

// Seviyedeki butun PlayerStart'lari tarar, "Player Start Tag" alani verilen
// etikete esit olani dondurur. Bulamazsa nullptr doner.
// GetAllActorsOfClass sirasiz calisir; bu yuzden indeks (PlayerStarts[1]) yerine
// isim kullaniyoruz. Indeks kullanildiginda iki karakter ayni noktada dogabiliyordu.
APlayerStart* AOpiaGameMode::FindPlayerStartByTag(FName Tag) const
{
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	for (AActor* Actor : PlayerStarts)
	{
		APlayerStart* Start = Cast<APlayerStart>(Actor);
		if (Start && Start->PlayerStartTag == Tag)
		{
			return Start;
		}
	}

	return nullptr;
}

// Motor 1. oyuncunun pawn'ini kendi doguruyor ve normalde PlayerStart'lardan
// rastgele birini seciyor. Burada onu "Tadashi" etiketli olana sabitliyoruz.
AActor* AOpiaGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (APlayerStart* TadashiStart = FindPlayerStartByTag(TadashiStartTag))
	{
		return TadashiStart;
	}

	UE_LOG(LogTemp, Warning,
		TEXT("[Opia] '%s' etiketli PlayerStart bulunamadi, motorun varsayilani kullaniliyor."),
		*TadashiStartTag.ToString());

	return Super::ChoosePlayerStart_Implementation(Player);
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

	// Airi'nin dogacagi yer: "Airi" etiketli PlayerStart.
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (APlayerStart* AiriStart = FindPlayerStartByTag(AiriStartTag))
	{
		SpawnLocation = AiriStart->GetActorLocation();
		SpawnRotation = AiriStart->GetActorRotation();
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[Opia] '%s' etiketli PlayerStart yok. Airi (0,0,0) noktasinda dogacak — zeminin altinda kalabilir."),
			*AiriStartTag.ToString());
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
