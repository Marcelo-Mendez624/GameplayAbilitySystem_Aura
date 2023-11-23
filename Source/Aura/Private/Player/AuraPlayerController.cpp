// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaSeconds)
{
	Super::PlayerTick(DeltaSeconds);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;

	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	
	
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	APawn* ControlledPawn = GetPawn<APawn>();
	if(!ControlledPawn) return;
	
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
	
	ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if(!CursorHit.bBlockingHit) return;

	LastActor = CurrentActor;
 	CurrentActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if(LastActor == nullptr && CurrentActor)
	{
		CurrentActor->HighlightActor();
	}

	if(LastActor && CurrentActor == nullptr)
	{
		LastActor->UnHighlightActor();
	}

	if((LastActor && CurrentActor) && (CurrentActor != LastActor))
	{
		CurrentActor->HighlightActor();
		LastActor->UnHighlightActor();
	}
		
}
