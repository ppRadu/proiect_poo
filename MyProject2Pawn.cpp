// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MyProject2Pawn.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

AMyProject2Pawn::AMyProject2Pawn()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Flying/Meshes/UFO.UFO"))
		{
		}
	};
	static FConstructorStatics ConstructorStatics;

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());	// Set static mesh
	RootComponent = PlaneMesh;

	//SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	MyMesh->SetupAttachment(RootComponent);

	//Atasam mesh-ul elicelor dronei
	Elice1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice1"));
	Elice1->SetupAttachment(MyMesh);

	Elice2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice2"));
	Elice2->SetupAttachment(MyMesh);

	Elice3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice3"));
	Elice3->SetupAttachment(MyMesh);

	Elice4 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Elice4"));
	Elice4->SetupAttachment(MyMesh);

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->SetupAttachment(RootComponent);	// Attach SpringArm to RootComponent
	SpringArm->TargetArmLength = 160.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;	// Do not allow camera to lag
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);	// Attach the camera
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	// Set handling parameters
	Acceleration = 5000.f;
	TurnSpeed = 150.f;
	MaxSpeed = 4000.f;
	MinSpeed = 0.f;
	//CurrentForwardSpeed = 500.f;

	//GetOwner()->AddActorLocalRotation({ 10.0f,10.0f,5.0f });

	
}

void AMyProject2Pawn::Tick(float DeltaSeconds)
{
	const FVector LocalMove = FVector(CurrentXSpeed * DeltaSeconds, CurrentYSpeed * DeltaSeconds, CurrentZSpeed * DeltaSeconds);

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);
	//AddActorWorldOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation);

	//FRotator Rot = { 0.0f, 30.0f, 0.0f };
	Elice1->AddLocalRotation({ 0.0f, -5 * RotationRate, 0.0f });
	Elice2->AddLocalRotation({ 0.0f, 5 * RotationRate, 0.0f });
	Elice3->AddLocalRotation({ 0.0f, -5 * RotationRate , 0.0f });
	Elice4->AddLocalRotation({ 0.0f, 5 * RotationRate, 0.0f });



	

	if (PlaneMesh->GetComponentRotation() != Balance)
		PlaneMesh->SetWorldRotation({ 0.0f, PlaneMesh->GetComponentRotation().Yaw, 0.0f});
	





	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AMyProject2Pawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// Deflect along the surface when we collide.
	FRotator CurrentRotation = GetActorRotation();
	SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}


void AMyProject2Pawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    // Check if PlayerInputComponent is valid (not NULL)
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveX", this, &AMyProject2Pawn::MoveXInput);
	PlayerInputComponent->BindAxis("MoveY", this, &AMyProject2Pawn::MoveYInput);
	PlayerInputComponent->BindAxis("MoveZ", this, &AMyProject2Pawn::MoveZInput);
	PlayerInputComponent->BindAxis("Yaw", this, &AMyProject2Pawn::YawInput);
}

void AMyProject2Pawn::MoveXInput(float Val)
{
	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	

	float CurrentAcc;
	if (bHasInput)
	{
		CurrentRotation = { -RotationRate * FMath::Sign(Val), 0.0f, 0.0f };
		CurrentAcc = Val * Acceleration;
	}
	else
	{
		CurrentRotation = { -RotationRate * FMath::Sign(MyMesh->GetComponentRotation().Pitch), 0.0f, 0.0f };
		CurrentAcc = -CurrentXSpeed;
	}
	
	FRotator NewRotation = MyMesh->GetComponentRotation() + CurrentRotation;

	MyMesh->SetRelativeRotation({ FMath::Clamp(NewRotation.Pitch, -MaxRotation.Pitch, MaxRotation.Pitch), 0.0f,  MyMesh->GetComponentRotation().Roll });


	float NewXSpeed = CurrentXSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	CurrentXSpeed = FMath::Clamp(NewXSpeed, -MaxSpeed, MaxSpeed);
}

void AMyProject2Pawn::MoveYInput(float Val)
{
	// Target yaw speed is based on input
	// float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	// CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	// const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value.
	// float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	// CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	
	float CurrentAcc;
	if (bHasInput)
	{
		CurrentRotation = { 0.0f, 0.0f, RotationRate * FMath::Sign(Val) };
		CurrentAcc = Val * Acceleration;
	}
	else
	{
		CurrentRotation = { 0.0f, 0.0f, -RotationRate * FMath::Sign(MyMesh->GetComponentRotation().Roll) };
		CurrentAcc = -CurrentYSpeed;
	}

	FRotator NewRotation = MyMesh->GetComponentRotation() + CurrentRotation;

	MyMesh->SetRelativeRotation({ MyMesh->GetComponentRotation().Pitch, 0.0f, FMath::Clamp(NewRotation.Roll, -MaxRotation.Roll, MaxRotation.Roll) });


	float NewYSpeed = CurrentYSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	CurrentYSpeed = FMath::Clamp(NewYSpeed, -MaxSpeed, MaxSpeed);
}

void AMyProject2Pawn::MoveZInput(float Val)
{
	// Target pitch speed is based in input
	//float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	// TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	//CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);

	float CurrentAcc;
	if (bHasInput)
	{
		CurrentAcc = Val * Acceleration;
	}
	else
	{
		CurrentAcc = -CurrentZSpeed;
	}

	float NewZSpeed = CurrentZSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);

	CurrentZSpeed = FMath::Clamp(NewZSpeed, -MaxSpeed, MaxSpeed);
}



void AMyProject2Pawn::YawInput(float Val)
{
	float TargetYawSpeed = (Val * TurnSpeed);
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
	
	//const bool bIsTurning = FMath::Abs(Val) > 0.2f;
	//float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);
	//CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}
