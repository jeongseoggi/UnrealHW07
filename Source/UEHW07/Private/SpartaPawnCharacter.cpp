#include "SpartaPawnCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/ArrowComponent.h"
#include "EnhancedInputComponent.h"
#include "SpartaPlayerController.h"

// Sets default values
ASpartaPawnCharacter::ASpartaPawnCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->SetCapsuleSize(34.0f, 95.0f); 
	CapsuleComp->SetSimulatePhysics(false);
	SetRootComponent(CapsuleComp);

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);
	SkeletalMeshComp->SetSimulatePhysics(false);
	SkeletalMeshComp->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -90.0f),
		FRotator(0.0f, -90.0f, 0.0f)
	);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(CapsuleComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->TargetArmLength = 300.0f;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;


	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkelMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny.SKM_Manny"));
	
	if (SkelMesh.Succeeded())
	{
		SkeletalMeshComp->SetSkeletalMesh(SkelMesh.Object);
	}

	Speed = 300.0f; // 속도
	MouseSensitivity = 45.0f; // 회전 속도(마우스 감도)
	bUseControllerRotationYaw = true;
}

// Called when the game starts or when spawned
void ASpartaPawnCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASpartaPawnCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASpartaPawnCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ASpartaPlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController<ASpartaPlayerController>() : nullptr;

	if (UEnhancedInputComponent* EnhancedComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (EnhancedComp)
		{
			if (PlayerController)
			{
				EnhancedComp->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &ASpartaPawnCharacter::MoveAction);
				EnhancedComp->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &ASpartaPawnCharacter::LookAction);
			}
		}
	}
}

void ASpartaPawnCharacter::MoveAction(const FInputActionValue& Value)
{
	FVector2D MoveInput(Value.Get<FVector2D>());
	FVector MoveDirection(FVector::ZeroVector);
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		MoveDirection += GetActorForwardVector() * MoveInput.X;
		
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		MoveDirection += GetActorRightVector() * MoveInput.Y;
	}

	MoveDirection.Normalize();
	AddActorWorldOffset(MoveDirection * DeltaTime * Speed);
}

void ASpartaPawnCharacter::LookAction(const FInputActionValue& Value)
{
	ASpartaPlayerController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController<ASpartaPlayerController>() : nullptr;
	FVector2D LookInput(Value.Get<FVector2D>());
	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// Pawn이 움직이는게 아니라 Controller가 움직인다
	if (PlayerController)
	{
		FRotator CurrentRot = PlayerController->GetControlRotation();
		CurrentRot.Yaw += LookInput.X * MouseSensitivity * DeltaTime;
		CurrentRot.Pitch += LookInput.Y * MouseSensitivity * DeltaTime;
		PlayerController->SetControlRotation(CurrentRot);
	}
}

