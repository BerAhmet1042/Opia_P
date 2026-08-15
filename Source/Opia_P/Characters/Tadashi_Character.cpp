// Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/Tadashi_Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

// Sets default values
ATadashi_Character::ATadashi_Character()
{
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate the character with the controller
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character moves in the direction of input
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ATadashi_Character::BeginPlay()
{
	Super::BeginPlay();

	// Seviyeye onceden yerlestirilmis ve zaten possess edilmis durum icin.
	SetupInputMapping();
}

// Motor, bu pawn bir Controller tarafindan ele gecirildiginde burayi cagirir.
// Tadashi zaten possess edilmis dogsa da, iki yol da guvenli olsun diye burada da cagiriyoruz.
void ATadashi_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SetupInputMapping();
}

void ATadashi_Character::SetupInputMapping()
{
	// GetController() bu pawn'i kontrol eden nesneyi verir. Kimse etmiyorsa nullptr doner.
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	if (!Subsystem)
	{
		return;
	}

	if (!DefaultMappingContext)
	{
		UE_LOG(LogTemp, Error,
			TEXT("[Opia] %s: DefaultMappingContext atanmamis. Blueprint'te Input kategorisinden ata."), *GetName());
		return;
	}

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(DefaultMappingContext, 0);

	UE_LOG(LogTemp, Log, TEXT("[Opia] %s icin mapping context yuklendi (%s)."),
		*GetName(), *DefaultMappingContext->GetName());
}

// Called every frame
void ATadashi_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATadashi_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] %s: EnhancedInputComponent yok. Project Settings > Input > Default Input Component Class kontrol et."), *GetName());
		return;
	}

	// Null bir InputAction ile BindAction cagirmak sessizce basarisiz olur.
	// Her birini ayri ayri kontrol ediyoruz ki hangisinin eksik oldugunu logdan gorebilesin.
	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATadashi_Character::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATadashi_Character::DoJumpEnd);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] %s: JumpAction atanmamis."), *GetName());
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATadashi_Character::Move);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[Opia] %s: MoveAction atanmamis."), *GetName());
	}

	if (LookAction)
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATadashi_Character::Look);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Opia] %s: LookAction atanmamis (gamepad sag cubuk calismaz)."), *GetName());
	}

	if (MouseLookAction)
	{
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ATadashi_Character::Look);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[Opia] %s: MouseLookAction atanmamis (mouse ile bakma calismaz)."), *GetName());
	}
}

void ATadashi_Character::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void ATadashi_Character::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void ATadashi_Character::DoMove(float Right, float Forward)
{
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void ATadashi_Character::DoLook(float Yaw, float Pitch)
{
	if (Controller)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void ATadashi_Character::DoJumpStart()
{
	Jump();
}

void ATadashi_Character::DoJumpEnd()
{
	StopJumping();
}
