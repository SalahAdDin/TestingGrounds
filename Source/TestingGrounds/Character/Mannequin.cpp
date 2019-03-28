// Fill out your copyright notice in the Description page of Project Settings.

#include "Mannequin.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "../Weapons/Gun.h"

// Sets default values
AMannequin::AMannequin()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-17.5f, -9.25f, 60.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Default offset from the character location for projectiles to spawn
	// TODO Delete or move GunOffset to the Gun class (if needed)
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

}

// Called when the game starts or when spawned
void AMannequin::BeginPlay()
{
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	if (GunBlueprint == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Gun blueprint missing."));
		return;
	}
	Gun = GetWorld()->SpawnActor<AGun>(GunBlueprint);
	
	//Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
	if (IsPlayerControlled())	Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	else Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
	
	// Set Gun's AnimInstance to the same AnimInstance as defined in the Mesh1P
	Gun->AnimInstance1P = Mesh1P->GetAnimInstance();
	Gun->AnimInstance3P = GetMesh()->GetAnimInstance();

	if (InputComponent != nullptr) InputComponent->BindAction("Fire", IE_Pressed, this, &AMannequin::PullTrigger);
	
}

// Called every frame
void AMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMannequin::UnPossessed() 
{
	Super::UnPossessed();

	if (!ensure(Gun))
	{
		UE_LOG(LogTemp, Error, TEXT("Gun is not available"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Gun re-attached"));

	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));
}

void AMannequin::PullTrigger()
{
	Gun->OnFire();
}

