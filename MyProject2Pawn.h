// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyProject2Pawn.generated.h"

UCLASS(Config=Game)
class AMyProject2Pawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* PlaneMesh;
	
	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* Camera;

	//UPROPERTY(EditAnyWhere)
	//USceneComponent* SceneComponent;

	UPROPERTY(EditAnyWhere)
	UStaticMeshComponent* MyMesh;

	// Declaram Elicea dronei
	UPROPERTY(EditAnyWhere)
		UStaticMeshComponent* Elice1;
	// Declaram Elicea dronei
	UPROPERTY(EditAnyWhere)
		UStaticMeshComponent* Elice2;
	// Declaram Elicea dronei
	UPROPERTY(EditAnyWhere)
		UStaticMeshComponent* Elice3;
	// Declaram Elicea dronei
	UPROPERTY(EditAnyWhere)
		UStaticMeshComponent* Elice4;

	
public:
	AMyProject2Pawn();

	// Begin AActor overrides
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	// End AActor overrides

protected:

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides


	void MoveXInput(float Val);
	void MoveYInput(float Val);
	void MoveZInput(float Val);
	void YawInput(float Val);

private:

	/** How quickly forward speed changes */
	UPROPERTY(Category=Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category = Plane, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category = Yaw, EditAnywhere)
	float MinSpeed;

	//UPROPERTY(EditAnyWhere)
	//FRotator RotationRate;

	UPROPERTY(EditAnywhere)
	FRotator MaxRotation;

	FRotator CurrentRotation;

	FRotator NewRotation;

	FRotator Balance = { 0.0f, 0.0f, 0.0f };
	
	UPROPERTY(EditAnyWhere)
	float RotationRate;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	float CurrentXSpeed;
	float CurrentYSpeed;
	float CurrentZSpeed;

public:
	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
};
