//
// Copyright 2015 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// DebugAnnotator11.cpp: D3D11 helpers for adding trace annotations.
//

#include "libANGLE/renderer/d3d/d3d11/DebugAnnotator11.h"

#include "common/debug.h"
#include "libANGLE/renderer/d3d/d3d11/renderer11_utils.h"

namespace rx
{

DebugAnnotator11::DebugAnnotator11()
    : mInitialized(false),
      mD3d11Module(nullptr),
      mUserDefinedAnnotation(nullptr)
{
    // D3D11 devices can't be created during DllMain.
    // We defer device creation until the object is actually used.
}

DebugAnnotator11::~DebugAnnotator11()
{
    if (mInitialized)
    {
#if defined(ANGLE_ENABLE_D3D11_1)
        SafeRelease(mUserDefinedAnnotation);
#endif

#if !defined(ANGLE_ENABLE_WINDOWS_STORE)
        FreeLibrary(mD3d11Module);
#endif // !ANGLE_ENABLE_WINDOWS_STORE
    }
}

void DebugAnnotator11::beginEvent(const wchar_t *eventName)
{
    initializeDevice();

    if (mUserDefinedAnnotation != nullptr)
    {
#if defined(ANGLE_ENABLE_D3D11_1)
        mUserDefinedAnnotation->BeginEvent(eventName);
#endif
    }
}

void DebugAnnotator11::endEvent()
{
    initializeDevice();

    if (mUserDefinedAnnotation != nullptr)
    {
#if defined(ANGLE_ENABLE_D3D11_1)
        mUserDefinedAnnotation->EndEvent();
#endif
    }
}

void DebugAnnotator11::setMarker(const wchar_t *markerName)
{
    initializeDevice();

    if (mUserDefinedAnnotation != nullptr)
    {
#if defined(ANGLE_ENABLE_D3D11_1)
        mUserDefinedAnnotation->SetMarker(markerName);
#endif
    }
}

bool DebugAnnotator11::getStatus()
{
#if defined(ANGLE_ENABLE_WINDOWS_STORE)
#if (NTDDI_VERSION == NTDDI_WIN10)
    initializeDevice();

    if (mUserDefinedAnnotation != nullptr)
    {
        return !!(mUserDefinedAnnotation->GetStatus());
    }

    return true;  // Default if initializeDevice() failed
#elif defined(_DEBUG) && (WINAPI_FAMILY != WINAPI_FAMILY_PHONE_APP)
    static bool underCapture = true;

    // ID3DUserDefinedAnnotation::GetStatus doesn't work with the Graphics Diagnostics tools in
    // Windows 8.1/Visual Studio 2013. We can use IDXGraphicsAnalysis, though.
    // The call to GetDebugInterface1 only succeeds if the app is under capture.
    // This should only be called in DEBUG mode.
    // If an app links against DXGIGetDebugInterface1 in release mode then it will fail Windows
    // Store ingestion checks.

    // Cache the result to reduce the number of calls to DXGIGetDebugInterface1
    static bool triedIDXGraphicsAnalysis = false;

    if (!triedIDXGraphicsAnalysis)
    {
        IDXGraphicsAnalysis *graphicsAnalysis = nullptr;

        HRESULT result = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&graphicsAnalysis));
        if (SUCCEEDED(result))
        {
            underCapture = (graphicsAnalysis != nullptr);
        }

        SafeRelease(graphicsAnalysis);
        triedIDXGraphicsAnalysis = true;
    }

    return underCapture;
#else
    // We can't detect GetStatus() on release WinRT 8.1 builds, so always return true.
    return true;
#endif  // (NTDDI_VERSION == NTDDI_WIN10) or _DEBUG
#else
    // We can't detect GetStatus() on desktop ANGLE builds so always return true.
    return true;
#endif  // ANGLE_ENABLE_WINDOWS_STORE
}

void DebugAnnotator11::initializeDevice()
{
    if (!mInitialized)
    {
#if !defined(ANGLE_ENABLE_WINDOWS_STORE)
        mD3d11Module = LoadLibrary(TEXT("d3d11.dll"));
        ASSERT(mD3d11Module);

        PFN_D3D11_CREATE_DEVICE D3D11CreateDevice = (PFN_D3D11_CREATE_DEVICE)GetProcAddress(mD3d11Module, "D3D11CreateDevice");
        ASSERT(D3D11CreateDevice != nullptr);
#endif // !ANGLE_ENABLE_WINDOWS_STORE

        ID3D11Device *device = nullptr;
        ID3D11DeviceContext *context = nullptr;

        HRESULT hr = E_FAIL;

        // Create a D3D_DRIVER_TYPE_NULL device, which is much cheaper than other types of device.
        hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_NULL, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &device, nullptr, &context);
        ASSERT(SUCCEEDED(hr));
        if (SUCCEEDED(hr))
        {
#if defined(ANGLE_ENABLE_D3D11_1)
            mUserDefinedAnnotation = d3d11::DynamicCastComObject<ID3DUserDefinedAnnotation>(context);
            ASSERT(mUserDefinedAnnotation != nullptr);
#endif
            mInitialized = true;
        }

        SafeRelease(device);
        SafeRelease(context);
    }
}

}
