#pragma once
#include "Core/ProgramManager.h"
#include "Core/Renderer.h"
#include "MeshLaunchParams.h"

namespace Wayland
{

class MeshProgramManager : public ProgramManager
{
public:
    MeshProgramManager(Renderer *r_) : renderer(r_) {}

    void Setup()
    {
        param.frameID = 0;
        param.fbSize.x = renderer->window.size.w;
        param.fbSize.y = renderer->window.size.h;
        param.colorBuffer = (glm::u8vec4 *)renderer->device.deviceFrameBuffer;
        param.traversable = renderer->device.GetTraversableHandle();

        Camera *cam = renderer->scene.camera.get();
        assert(cam);
        assert(cam->type() == CameraType::Pinhole);
        PinholeCamera *pCam = reinterpret_cast<PinholeCamera *>(cam);
        param.camera.pos = pCam->position;
        param.camera.lookAt = pCam->lookAt;
        param.camera.up = pCam->up;
        param.camera.right =
            pCam->right * (float(param.fbSize.x) / param.fbSize.y);

    }

    void Update()
    {
        param.frameID += 1;
        param.fbSize.x = renderer->window.size.w;
        param.fbSize.y = renderer->window.size.h;
        param.colorBuffer = (glm::u8vec4 *)renderer->device.deviceFrameBuffer;
        param.traversable = renderer->device.GetTraversableHandle();
    }

    void *GetParamPtr() { return &param; }
    size_t GetParamSize() { return sizeof(param); };

    Optix::ShaderBindingTable GenerateSBT(const Optix::ProgramGroupArray &pg)
    {
        using namespace Optix;
        SBTData<void> raygenData{};
        SBTData<int> missData{};
        vector<SBTData<int>> hitData;
        hitData.resize(renderer->scene.meshes.size());
        vector<std::size_t> hitIdx(renderer->scene.meshes.size(),2);
        return ShaderBindingTable{
            raygenData, 0, missData, 1, std::span(hitData), hitIdx.data(), pg
        };
    }

private:
    Renderer *renderer;
    MeshLaunchParams param;
};

} // namespace Wayland