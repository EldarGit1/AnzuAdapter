#include "Engine/AnzuCamera.h"

#include "Kismet/GameplayStatics.h"

namespace anzu
{
    AnzuCamera& AnzuCamera::Get()
    {
        static AnzuCamera Instance;
        return Instance;
    }

    void AnzuCamera::Update(UObject* WorldContextObject, int32 PlayerIndex)
    {
        if (WorldContextObject == nullptr)
        {
            _currentActiveCamera = nullptr;
            return;
        }

        _currentActiveCamera = UGameplayStatics::GetPlayerCameraManager(WorldContextObject, PlayerIndex);
    }

    APlayerCameraManager* AnzuCamera::GetCurrentActiveCamera() const
    {
        
        return _manualCamera ? _manualCamera : _currentActiveCamera;
    }

    void AnzuCamera::SetManualCamera(APlayerCameraManager* newCam)
    {
        _manualCamera = newCam;
    }

    void AnzuCamera::ResetManualCamera()
    {
        _manualCamera = nullptr;
    }
}
