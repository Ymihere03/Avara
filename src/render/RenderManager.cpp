#include "RenderManager.h"

#include "AvaraDefines.h"
#include "CAvaraGame.h"
#include "FastMat.h"
#include "Preferences.h"

RenderManager::RenderManager(RenderMode mode, SDL_Window *window, std::optional<NVGcontext*> nvg)
{
    this->window = window;
    if (nvg) {
        this->nvg = *nvg;
        if (this->nvg) {
            ui = std::make_unique<CHUD>(gCurrentGame);
            ui->LoadImages(this->nvg);
        }
    }

    switch (mode) {
        case RenderMode::GL3:
            renderer = std::make_unique<ModernOpenGLRenderer>(this);
            break;
        case RenderMode::Headless:
            renderer = std::make_unique<NullRenderer>(this);
            break;
    }

    skyParams = new CWorldShader();
    skyParams->skyShadeCount = 12;

    viewParams = new CViewParameters();
    viewParams->hitherBound = FIX3(600);
    viewParams->yonBound = LONGYON;
    viewParams->horizonBound = FIX(16000); // 16 km
    ResetLights();

    staticWorld = new CBSPWorldImpl(100);
    dynamicWorld = new CBSPWorldImpl(100);
    hudWorld = new CBSPWorldImpl(30);
}

RenderManager::~RenderManager()
{
    delete skyParams;
    delete viewParams;
    delete staticWorld;
    delete dynamicWorld;
    delete hudWorld;
}

void RenderManager::AddHUDPart(CBSPPart *part)
{
    hudWorld->AddPart(part);
}

void RenderManager::AddPart(CBSPPart *part)
{
    dynamicWorld->AddPart(part);
}

void RenderManager::LevelReset()
{
    skyParams->Reset();
    staticWorld->DisposeParts();
    dynamicWorld->DisposeParts();
    hudWorld->DisposeParts();
    ResetLights();
}

void RenderManager::OverheadPoint(Fixed *pt, Fixed *extent)
{
    dynamicWorld->OverheadPoint(pt, extent);
}

void RenderManager::RefreshWindow()
{
    renderer->RefreshWindow();
}

void RenderManager::RemoveHUDPart(CBSPPart *part)
{
    hudWorld->RemovePart(part);
}

void RenderManager::RemovePart(CBSPPart *part)
{
    dynamicWorld->RemovePart(part);
}

void RenderManager::RenderFrame()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    AvaraGLShadeWorld(skyParams, viewParams);
    
    staticWorld->Render(Shader::World);
    dynamicWorld->Render(Shader::World);
    AvaraGLSetDepthTest(false);
    hudWorld->Render(Shader::HUD);

    if (ui) {
        if (gApplication ? gApplication->Get<bool>(kShowNewHUD) : true) {
            ui->RenderNewHUD(nvg);
        } else {
            ui->Render(nvg);
        }
    }

    AvaraGLSetDepthTest(true);
}

void RenderManager::SetFOV(float fov)
{
    this->fov = fov;
}

void RenderManager::UpdateViewRect(int width, int height, float pixelRatio)
{
    viewParams->SetViewRect(width, height, width / 2, height / 2);
    viewParams->viewPixelRatio = pixelRatio;
    viewParams->CalculateViewPyramidCorners();
}

void RenderManager::ResetLights()
{
    viewParams->ambientLight = 0.4f;
    viewParams->ambientLightColor = DEFAULT_LIGHT_COLOR;
    viewParams->SetLight(0, 0.4f, 45.0f, 20.0f, DEFAULT_LIGHT_COLOR, kLightGlobalCoordinates);
    viewParams->SetLight(1, 0.3f, 20.0f, 200.0f, DEFAULT_LIGHT_COLOR, kLightGlobalCoordinates);
    viewParams->SetLight(2, 0, 0, 0, DEFAULT_LIGHT_COLOR, kLightOff);
    viewParams->SetLight(3, 0, 0, 0, DEFAULT_LIGHT_COLOR, kLightOff);
}

RenderManager *gRenderer;
