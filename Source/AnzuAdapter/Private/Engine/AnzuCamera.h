#pragma once

#include "CoreMinimal.h"

class APlayerCameraManager;
class UObject;

namespace anzu
{
    class AnzuCamera
    {
    public:
        static AnzuCamera& Get();

        void Update(UObject* WorldContextObject, int32 PlayerIndex = 0);
        APlayerCameraManager* GetCurrentActiveCamera() const;
        void SetManualCamera(APlayerCameraManager* newCam);
        void ResetManualCamera();

    private:
        AnzuCamera() = default;

        APlayerCameraManager* _currentActiveCamera = nullptr;
        APlayerCameraManager* _manualCamera = nullptr;
    };
}
