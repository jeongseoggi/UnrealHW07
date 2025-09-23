#include "DroneController.h"
#include "EnhancedInputSubsystems.h"

ADroneController::ADroneController() :
	DroneMoveAction(nullptr),
	DroneLookAction(nullptr),
	DroneRollAction(nullptr),
	InputMappingContext(nullptr)
{
}

void ADroneController::BeginPlay()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* SubSystems = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (SubSystems)
			{
				SubSystems->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
