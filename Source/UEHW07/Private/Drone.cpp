#include "Drone.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DroneController.h"
#include "EnhancedInputComponent.h"


ADrone::ADrone()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	SetRootComponent(BoxComp);

	FTransform Transform(FRotator(FRotator::ZeroRotator), FVector(40.0f, 60.0f, -10.0), FVector(0.05f, 0.05f, 0.05f)) ;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(BoxComp);
	StaticMeshComp->SetRelativeTransform(Transform);

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(BoxComp);

	//SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArmComp->SetupAttachment(BoxComp);
	//SpringArmComp->TargetArmLength = 300.0f;
	//SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(BoxComp);
	//CameraComp->bUsePawnControlRotation = false;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Game/Fab/Parrot_Camo_drone/parrot_camo_drone.parrot_camo_drone"));

	if (Mesh.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(Mesh.Object);
	}

	BoxComp->SetSimulatePhysics(false);


	Speed = 500.0f; // 공중이 아닐 때 이동속도
	bIsFly = false; // 공중인지 판단
	FlySpeed = Speed * 0.3; // 공중 이동속도
	Gravity = -9.8f; // 중력
	MouseSensitivity = 45.0f; // 회전 속도(마우스 감도)
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bool IsFloor = HitFloor();
	FVector DropVector = GetActorLocation();

	if(IsFloor)
	{
		DropVector.Z = 0;
		bIsFly = false;
	}
	else
	{
		bIsFly = true;
	}

	if (bIsFly)
	{
		DropVector.Z += Gravity * DeltaTime;
		SetActorLocation(DropVector);
	}
}

// Called to bind functionality to input
void ADrone::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ADroneController* DroneController = GetWorld() ? GetWorld()->GetFirstPlayerController<ADroneController>() : nullptr;

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (EnhancedInput && DroneController)
		{
			EnhancedInput->BindAction(DroneController->DroneMoveAction, ETriggerEvent::Triggered, this, &ADrone::Move);
			EnhancedInput->BindAction(DroneController->DroneLookAction, ETriggerEvent::Triggered, this, &ADrone::Look);
			EnhancedInput->BindAction(DroneController->DroneRollAction, ETriggerEvent::Triggered, this, &ADrone::Roll);
		}
	}

}

void ADrone::Move(const FInputActionValue& Value)
{
	FVector MoveInput(Value.Get<FVector>());
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

	if (!FMath::IsNearlyZero(MoveInput.Z)) 
	{
		MoveDirection += GetActorUpVector() * MoveInput.Z;
	}

	MoveDirection.Normalize();
	AddActorWorldOffset(MoveDirection * DeltaTime * (bIsFly ? FlySpeed : Speed));
}

void ADrone::Look(const FInputActionValue& Value)
{
	ADroneController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController<ADroneController>() : nullptr;
	FVector2D LookInput(Value.Get<FVector2D>());
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FRotator DeltaRot(LookInput.Y * MouseSensitivity * DeltaTime, LookInput.X * MouseSensitivity * DeltaTime, 0.f);
	//FRotator DeltaRot(LookInput.Y * MouseSensitivity * DeltaTime, 0.f, 0.f);
	
	//if (PlayerController)
	//{
	//	FRotator CurrentRot = PlayerController->GetControlRotation();
	//	CurrentRot.Yaw += LookInput.X * MouseSensitivity * DeltaTime;
	//	CurrentRot.Pitch += LookInput.Y * MouseSensitivity * DeltaTime;
	//	PlayerController->SetControlRotation(CurrentRot);

	//	AddActorLocalRotation(DeltaRot);
	//}

	AddActorLocalRotation(DeltaRot);

}

void ADrone::Roll(const FInputActionValue& Value) // 드론 기울기
{
	ADroneController* PlayerController = GetWorld() ? GetWorld()->GetFirstPlayerController<ADroneController>() : nullptr;
	float RollInput(Value.Get<float>());
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	FRotator DeltaRot(0.f, 0.f, RollInput * MouseSensitivity * DeltaTime);

	//if (PlayerController)
	//{
	//	FRotator CurrentRot = PlayerController->GetControlRotation();
	//	CurrentRot.Roll += RollInput * MouseSensitivity * DeltaTime;
	//	PlayerController->SetControlRotation(CurrentRot);

	//}
	AddActorLocalRotation(DeltaRot);
	
}

bool ADrone::HitFloor()
{
	FHitResult HitResult;
	FVector LineTraceEndPos = GetActorLocation() - FVector(0.0f, 0.0f, 92.f);
	return GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(), LineTraceEndPos, ECC_WorldStatic); // 라인 트레이스 바닥 검출
}

