#pragma once
#include "CommonInclude.h"
#include "ShaderInterop.h"
#include "wiEnums.h"
#include "wiGraphicsAPI.h"
#include "wiWindowRegistration.h"
#include "wiFrustum.h"
#include "wiSceneSystem_Decl.h"
#include "wiECS.h"

#include <unordered_set>
#include <deque>

class  wiSprite;
class  PHYSICS;
class  wiRenderTarget;
class  wiOcean;
struct wiOceanParameter;

typedef std::unordered_map<wiECS::Entity, std::vector<wiECS::Entity>> CulledCollection;

struct RAY;

class wiRenderer
{
public:
	static wiGraphicsTypes::GraphicsDevice* graphicsDevice;
	static wiGraphicsTypes::GraphicsDevice* GetDevice() { assert(graphicsDevice != nullptr);  return graphicsDevice; }


	static void Present(std::function<void()> drawToScreen1=nullptr, std::function<void()> drawToScreen2=nullptr, std::function<void()> drawToScreen3=nullptr);

	
	static wiGraphicsTypes::Sampler				*samplers[SSLOT_COUNT];
	static wiGraphicsTypes::VertexShader		*vertexShaders[VSTYPE_LAST];
	static wiGraphicsTypes::PixelShader			*pixelShaders[PSTYPE_LAST];
	static wiGraphicsTypes::GeometryShader		*geometryShaders[GSTYPE_LAST];
	static wiGraphicsTypes::HullShader			*hullShaders[HSTYPE_LAST];
	static wiGraphicsTypes::DomainShader		*domainShaders[DSTYPE_LAST];
	static wiGraphicsTypes::ComputeShader		*computeShaders[CSTYPE_LAST];
	static wiGraphicsTypes::VertexLayout		*vertexLayouts[VLTYPE_LAST];
	static wiGraphicsTypes::RasterizerState		*rasterizers[RSTYPE_LAST];
	static wiGraphicsTypes::DepthStencilState	*depthStencils[DSSTYPE_LAST];
	static wiGraphicsTypes::BlendState			*blendStates[BSTYPE_LAST];
	static wiGraphicsTypes::GPUBuffer			*constantBuffers[CBTYPE_LAST];
	static wiGraphicsTypes::GPUBuffer			*resourceBuffers[RBTYPE_LAST];
	static wiGraphicsTypes::Texture				*textures[TEXTYPE_LAST];
	static wiGraphicsTypes::Sampler				*customsamplers[SSTYPE_LAST];

	static wiGraphicsTypes::GPURingBuffer		*dynamicVertexBufferPool;

	static const wiGraphicsTypes::FORMAT RTFormat_ldr = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_hdr = wiGraphicsTypes::FORMAT_R16G16B16A16_FLOAT;
	static const wiGraphicsTypes::FORMAT RTFormat_gbuffer_0 = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_gbuffer_1 = wiGraphicsTypes::FORMAT_R16G16B16A16_FLOAT;
	static const wiGraphicsTypes::FORMAT RTFormat_gbuffer_2 = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_gbuffer_3 = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;
	// NOTE: Light buffer precision seems OK when using FORMAT_R11G11B10_FLOAT format
	// But the environmental light now also writes the AO to ALPHA so it has been changed to FORMAT_R16G16B16A16_FLOAT
	static const wiGraphicsTypes::FORMAT RTFormat_deferred_lightbuffer = wiGraphicsTypes::FORMAT_R16G16B16A16_FLOAT;
	static const wiGraphicsTypes::FORMAT RTFormat_lineardepth = wiGraphicsTypes::FORMAT_R16_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_ssao = wiGraphicsTypes::FORMAT_R8_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_waterripple = wiGraphicsTypes::FORMAT_R8G8B8A8_SNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_normalmaps = wiGraphicsTypes::FORMAT_R8G8B8A8_SNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_depthresolve = wiGraphicsTypes::FORMAT_R32_FLOAT;
	static const wiGraphicsTypes::FORMAT RTFormat_voxelradiance = wiGraphicsTypes::FORMAT_R16G16B16A16_FLOAT;
	static const wiGraphicsTypes::FORMAT RTFormat_envprobe = wiGraphicsTypes::FORMAT_R16G16B16A16_FLOAT;
	static const wiGraphicsTypes::FORMAT RTFormat_impostor_albedo = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_impostor_normal = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;
	static const wiGraphicsTypes::FORMAT RTFormat_impostor_surface = wiGraphicsTypes::FORMAT_R8G8B8A8_UNORM;

	static const wiGraphicsTypes::FORMAT DSFormat_full = wiGraphicsTypes::FORMAT_D32_FLOAT_S8X24_UINT;
	static const wiGraphicsTypes::FORMAT DSFormat_full_alias = wiGraphicsTypes::FORMAT_R32G8X24_TYPELESS;
	static const wiGraphicsTypes::FORMAT DSFormat_small = wiGraphicsTypes::FORMAT_D16_UNORM;
	static const wiGraphicsTypes::FORMAT DSFormat_small_alias = wiGraphicsTypes::FORMAT_R16_TYPELESS;
	
	static float GAMMA;
	static int SHADOWRES_2D, SHADOWRES_CUBE, SHADOWCOUNT_2D, SHADOWCOUNT_CUBE, SOFTSHADOWQUALITY_2D;
	static bool HAIRPARTICLEENABLED, EMITTERSENABLED;
	static float SPECULARAA;
	static float renderTime, renderTime_Prev, deltaTime;
	static XMFLOAT2 temporalAAJitter, temporalAAJitterPrev;
	static float RESOLUTIONSCALE;
	static bool TRANSPARENTSHADOWSENABLED;
	static bool ALPHACOMPOSITIONENABLED;

	static void SetShadowProps2D(int resolution, int count, int softShadowQuality);
	static void SetShadowPropsCube(int resolution, int count);

protected:

	static bool	wireRender, debugBoneLines, debugPartitionTree, debugEnvProbes, debugEmitters, debugForceFields, debugCameras, gridHelper, voxelHelper, advancedLightCulling, advancedRefractions;
	static bool ldsSkinningEnabled;
	static bool requestReflectionRendering;


	static wiGraphicsTypes::Texture2D* enviroMap;
	static void LoadBuffers();
	static void LoadShaders();
	static void SetUpStates();


	static float GameSpeed;

	static wiSceneSystem::Scene* scene;

	static XMFLOAT4 waterPlane;

	static bool debugLightCulling;
	static bool occlusionCulling;
	static bool temporalAA, temporalAADEBUG;
	static bool freezeCullingCamera;

	struct VoxelizedSceneData
	{
		bool enabled;
		int res;
		float voxelsize;
		XMFLOAT3 center;
		XMFLOAT3 extents;
		int numCones;
		float rayStepSize;
		bool secondaryBounceEnabled;
		bool reflectionsEnabled;
		bool centerChangedThisFrame;
		UINT mips;

		VoxelizedSceneData() :enabled(false), res(128), voxelsize(1.0f), center(XMFLOAT3(0, 0, 0)), extents(XMFLOAT3(0, 0, 0)), numCones(8),
			rayStepSize(0.5f), secondaryBounceEnabled(true), reflectionsEnabled(false), centerChangedThisFrame(true), mips(7)
		{}
	} static voxelSceneData;

	static wiGraphicsTypes::GPUQuery occlusionQueries[256];

	static UINT entityArrayOffset_Lights, entityArrayCount_Lights;
	static UINT entityArrayOffset_Decals, entityArrayCount_Decals;
	static UINT entityArrayOffset_ForceFields, entityArrayCount_ForceFields;
	static UINT entityArrayOffset_EnvProbes, entityArrayCount_EnvProbes;

public:
	static std::string SHADERPATH;

	static void SetUpStaticComponents();
	static void CleanUpStatic();
	
	static void FixedUpdate();
	// Render data that needs to be updated on the main thread!
	static void UpdatePerFrameData(float dt);
	static void UpdateRenderData(GRAPHICSTHREAD threadID);
	static void OcclusionCulling_Render(GRAPHICSTHREAD threadID);
	static void OcclusionCulling_Read();
	static void PutWaterRipple(const std::string& image, const XMFLOAT3& pos);
	static void ManageWaterRipples();
	static void DrawWaterRipples(GRAPHICSTHREAD threadID);
	static void SetGameSpeed(float value){GameSpeed=value; if(GameSpeed<0) GameSpeed=0;};
	static float GetGameSpeed();
	static void SetResolutionScale(float value) { RESOLUTIONSCALE = value; }
	static float GetResolutionScale() { return RESOLUTIONSCALE; }
	static void SetTransparentShadowsEnabled(float value) { TRANSPARENTSHADOWSENABLED = value; }
	static float GetTransparentShadowsEnabled() { return TRANSPARENTSHADOWSENABLED; }
	static XMUINT2 GetInternalResolution() { return XMUINT2((UINT)ceilf(GetDevice()->GetScreenWidth()*GetResolutionScale()), (UINT)ceilf(GetDevice()->GetScreenHeight()*GetResolutionScale())); }
	static bool ResolutionChanged();
	static void SetGamma(float value) { GAMMA = value; }
	static float GetGamma() { return GAMMA; }
	static void SetWireRender(bool value) { wireRender = value; }
	static bool IsWireRender() { return wireRender; }
	static void SetToDrawDebugBoneLines(bool param) { debugBoneLines = param; }
	static bool GetToDrawDebugBoneLines() { return debugBoneLines; }
	static void SetToDrawDebugPartitionTree(bool param){debugPartitionTree=param;}
	static bool GetToDrawDebugPartitionTree(){return debugPartitionTree;}
	static bool GetToDrawDebugEnvProbes() { return debugEnvProbes; }
	static void SetToDrawDebugEnvProbes(bool value) { debugEnvProbes = value; }
	static void SetToDrawDebugEmitters(bool param) { debugEmitters = param; }
	static bool GetToDrawDebugEmitters() { return debugEmitters; }
	static void SetToDrawDebugForceFields(bool param) { debugForceFields = param; }
	static bool GetToDrawDebugForceFields() { return debugForceFields; }
	static void SetToDrawDebugCameras(bool param) { debugCameras = param; }
	static bool GetToDrawDebugCameras() { return debugCameras; }
	static bool GetToDrawGridHelper() { return gridHelper; }
	static void SetToDrawGridHelper(bool value) { gridHelper = value; }
	static bool GetToDrawVoxelHelper() { return voxelHelper; }
	static void SetToDrawVoxelHelper(bool value) { voxelHelper = value; }
	static void SetDebugLightCulling(bool enabled) { debugLightCulling = enabled; }
	static bool GetDebugLightCulling() { return debugLightCulling; }
	static void SetAdvancedLightCulling(bool enabled) { advancedLightCulling = enabled; }
	static bool GetAdvancedLightCulling() { return advancedLightCulling; }
	static void SetAlphaCompositionEnabled(bool enabled) { ALPHACOMPOSITIONENABLED = enabled; }
	static bool GetAlphaCompositionEnabled() { return ALPHACOMPOSITIONENABLED; }
	static void SetOcclusionCullingEnabled(bool enabled); // also inits query pool!
	static bool GetOcclusionCullingEnabled() { return occlusionCulling; }
	static void SetLDSSkinningEnabled(bool enabled) { ldsSkinningEnabled = enabled; }
	static bool GetLDSSkinningEnabled() { return ldsSkinningEnabled; }
	static void SetTemporalAAEnabled(bool enabled) { temporalAA = enabled; }
	static bool GetTemporalAAEnabled() { return temporalAA; }
	static void SetTemporalAADebugEnabled(bool enabled) { temporalAADEBUG = enabled; }
	static bool GetTemporalAADebugEnabled() { return temporalAADEBUG; }
	static void SetFreezeCullingCameraEnabled(bool enabled) { freezeCullingCamera = enabled; }
	static bool GetFreezeCullingCameraEnabled() { return freezeCullingCamera; }
	static void SetVoxelRadianceEnabled(bool enabled) { voxelSceneData.enabled = enabled; }
	static bool GetVoxelRadianceEnabled() { return voxelSceneData.enabled; }
	static void SetVoxelRadianceSecondaryBounceEnabled(bool enabled) { voxelSceneData.secondaryBounceEnabled = enabled; }
	static bool GetVoxelRadianceSecondaryBounceEnabled() { return voxelSceneData.secondaryBounceEnabled; }
	static void SetVoxelRadianceReflectionsEnabled(bool enabled) { voxelSceneData.reflectionsEnabled = enabled; }
	static bool GetVoxelRadianceReflectionsEnabled() { return voxelSceneData.reflectionsEnabled; }
	static void SetVoxelRadianceVoxelSize(float value) { voxelSceneData.voxelsize = value; }
	static float GetVoxelRadianceVoxelSize() { return voxelSceneData.voxelsize; }
	//static void SetVoxelRadianceResolution(int value) { voxelSceneData.res = value; }
	static int GetVoxelRadianceResolution() { return voxelSceneData.res; }
	static void SetVoxelRadianceNumCones(int value) { voxelSceneData.numCones = value; }
	static int GetVoxelRadianceNumCones() { return voxelSceneData.numCones; }
	static float GetVoxelRadianceRayStepSize() { return voxelSceneData.rayStepSize; }
	static void SetVoxelRadianceRayStepSize(float value) { voxelSceneData.rayStepSize = value; }
	static void SetSpecularAAParam(float value) { SPECULARAA = value; }
	static float GetSpecularAAParam() { return SPECULARAA; }
	static void SetAdvancedRefractionsEnabled(bool value) { advancedRefractions = value; }
	static bool GetAdvancedRefractionsEnabled(); // also needs additional driver support for now...
	static bool IsRequestedReflectionRendering() { return requestReflectionRendering; }
	static void SetEnviromentMap(wiGraphicsTypes::Texture2D* tex){ enviroMap = tex; }
	static wiGraphicsTypes::Texture2D* GetEnviromentMap(){ return enviroMap; }
	static wiGraphicsTypes::Texture2D* GetLuminance(wiGraphicsTypes::Texture2D* sourceImage, GRAPHICSTHREAD threadID);
	static const XMFLOAT4& GetWaterPlane();

	static void ReloadShaders(const std::string& path = "");
	static void BindPersistentState(GRAPHICSTHREAD threadID);

	inline static XMUINT3 GetEntityCullingTileCount()
	{
		return XMUINT3(
			(UINT)ceilf((float)GetInternalResolution().x / (float)TILED_CULLING_BLOCKSIZE),
			(UINT)ceilf((float)GetInternalResolution().y / (float)TILED_CULLING_BLOCKSIZE),
			1);
	}
	
public:
	

	static void UpdateWorldCB(GRAPHICSTHREAD threadID);
	static void UpdateFrameCB(GRAPHICSTHREAD threadID);
	static void UpdateCameraCB(const wiSceneSystem::CameraComponent& camera, GRAPHICSTHREAD threadID);
	static void SetClipPlane(const XMFLOAT4& clipPlane, GRAPHICSTHREAD threadID);
	static void SetAlphaRef(float alphaRef, GRAPHICSTHREAD threadID);
	static void ResetAlphaRef(GRAPHICSTHREAD threadID) { SetAlphaRef(0.75f, threadID); }
	static void BindGBufferTextures(wiGraphicsTypes::Texture2D* slot0, wiGraphicsTypes::Texture2D* slot1, wiGraphicsTypes::Texture2D* slot2, wiGraphicsTypes::Texture2D* slot3, wiGraphicsTypes::Texture2D* slot4, GRAPHICSTHREAD threadID);
	static void BindDepthTextures(wiGraphicsTypes::Texture2D* depth, wiGraphicsTypes::Texture2D* linearDepth, GRAPHICSTHREAD threadID);
	
	static void RenderMeshes(const XMFLOAT3& eye, const CulledCollection& culledRenderer, SHADERTYPE shaderType, UINT renderTypeFlags, GRAPHICSTHREAD threadID, 
		bool tessellation = false, bool occlusionCulling = false, uint32_t layerMask = 0xFFFFFFFF);
	static void DrawSky(GRAPHICSTHREAD threadID);
	static void DrawSun(GRAPHICSTHREAD threadID);
	static void DrawWorld(const wiSceneSystem::CameraComponent& camera, bool tessellation, GRAPHICSTHREAD threadID, SHADERTYPE shaderType, bool grass, bool occlusionCulling, uint32_t layerMask = 0xFFFFFFFF);
	static void DrawForShadowMap(GRAPHICSTHREAD threadID, uint32_t layerMask = 0xFFFFFFFF);
	static void DrawWorldTransparent(const wiSceneSystem::CameraComponent& camera, SHADERTYPE shaderType, GRAPHICSTHREAD threadID, bool grass, bool occlusionCulling, uint32_t layerMask = 0xFFFFFFFF);
	static void DrawDebugWorld(const wiSceneSystem::CameraComponent& camera, GRAPHICSTHREAD threadID);
	static void DrawSoftParticles(const wiSceneSystem::CameraComponent& camera, bool distortion, GRAPHICSTHREAD threadID);
	static void DrawTrails(GRAPHICSTHREAD threadID, wiGraphicsTypes::Texture2D* refracRes);
	static void DrawLights(const wiSceneSystem::CameraComponent& camera, GRAPHICSTHREAD threadID);
	static void DrawLightVisualizers(const wiSceneSystem::CameraComponent& camera, GRAPHICSTHREAD threadID);
	static void DrawVolumeLights(const wiSceneSystem::CameraComponent& camera, GRAPHICSTHREAD threadID);
	static void DrawLensFlares(GRAPHICSTHREAD threadID);
	static void DrawDecals(const wiSceneSystem::CameraComponent& camera, GRAPHICSTHREAD threadID);
	static void RefreshEnvProbes(GRAPHICSTHREAD threadID);
	static void VoxelRadiance(GRAPHICSTHREAD threadID);

	static void ComputeTiledLightCulling(bool deferred, GRAPHICSTHREAD threadID);
	static void ResolveMSAADepthBuffer(wiGraphicsTypes::Texture2D* dst, wiGraphicsTypes::Texture2D* src, GRAPHICSTHREAD threadID);

	static void BuildSceneBVH(GRAPHICSTHREAD threadID);
	static void DrawTracedScene(const wiSceneSystem::CameraComponent& camera, wiGraphicsTypes::Texture2D* result, GRAPHICSTHREAD threadID);

	enum MIPGENFILTER
	{
		MIPGENFILTER_POINT,
		MIPGENFILTER_LINEAR,
		MIPGENFILTER_LINEAR_MAXIMUM,
		MIPGENFILTER_GAUSSIAN,
	};
	static void GenerateMipChain(wiGraphicsTypes::Texture1D* texture, MIPGENFILTER filter, GRAPHICSTHREAD threadID, int arrayIndex = -1);
	static void GenerateMipChain(wiGraphicsTypes::Texture2D* texture, MIPGENFILTER filter, GRAPHICSTHREAD threadID, int arrayIndex = -1);
	static void GenerateMipChain(wiGraphicsTypes::Texture3D* texture, MIPGENFILTER filter, GRAPHICSTHREAD threadID, int arrayIndex = -1);

	enum BORDEREXPANDSTYLE
	{
		BORDEREXPAND_DISABLE,
		BORDEREXPAND_WRAP,
		BORDEREXPAND_CLAMP,
	};
	// Performs copy operation even between different texture formats
	//	Can also expand border region according to desired sampler func
	static void CopyTexture2D(wiGraphicsTypes::Texture2D* dst, UINT DstMIP, UINT DstX, UINT DstY, wiGraphicsTypes::Texture2D* src, UINT SrcMIP, GRAPHICSTHREAD threadID, 
		BORDEREXPANDSTYLE borderExpand = BORDEREXPAND_DISABLE);

	// dst: Texture2D with unordered access, the output will be written to this
	// refinementCount: 0: auto select, 1: perfect noise, greater numbers: smoother clouds, slower processing
	// randomness: random seed
	static void GenerateClouds(wiGraphicsTypes::Texture2D* dst, UINT refinementCount, float randomness, GRAPHICSTHREAD threadID);

	static void ManageDecalAtlas(GRAPHICSTHREAD threadID);

	// The scene holds all models, and world information
	static wiSceneSystem::Scene& GetScene();
	
	static std::deque<wiSprite*> waterRipples;
	static void ClearWorld();
	
	static wiSceneSystem::CameraComponent& GetCamera();
	static wiSceneSystem::CameraComponent& GetPrevCamera();
	static wiSceneSystem::CameraComponent& GetRefCamera();

	static RAY GetPickRay(long cursorX, long cursorY);

	struct RayIntersectWorldResult
	{
		wiECS::Entity entity = wiECS::INVALID_ENTITY;
		XMFLOAT3 position = XMFLOAT3(0, 0, 0);
		XMFLOAT3 normal = XMFLOAT3(0, 0, 0);
		float distance = FLT_MAX;
		int subsetIndex = -1;
	};
	static RayIntersectWorldResult RayIntersectWorld(const RAY& ray, UINT renderTypeMask = RENDERTYPE_OPAQUE, uint32_t layerMask = ~0);


	static PHYSICS* physicsEngine;
	static void SynchronizeWithPhysicsEngine(float dt = 1.0f / 60.0f);

	static wiOcean* ocean;
	static void SetOceanEnabled(bool enabled, const wiOceanParameter& params);
	static wiOcean* GetOcean() { return ocean; }

	static void CreateImpostor(wiECS::Entity, GRAPHICSTHREAD threadID);

	static std::vector<std::pair<XMFLOAT4X4,XMFLOAT4>> renderableBoxes;
	// Add box to render in next frame
	static void AddRenderableBox(const XMFLOAT4X4& boxMatrix, const XMFLOAT4& color = XMFLOAT4(1,1,1,1));

	struct RenderableLine
	{
		XMFLOAT3 start = XMFLOAT3(0, 0, 0);
		XMFLOAT3 end = XMFLOAT3(0, 0, 0);
		XMFLOAT4 color = XMFLOAT4(1, 1, 1, 1);
	};
	static std::vector<RenderableLine> renderableLines;
	static void AddRenderableLine(const RenderableLine& line);

	static void AddDeferredMIPGen(wiGraphicsTypes::Texture2D* tex);

};

