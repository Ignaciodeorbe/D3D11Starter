#include "Game.h"
#include "Graphics.h"
#include "Vertex.h"
#include "Input.h"
#include "PathHelpers.h"
#include "Window.h"

#include <DirectXMath.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include "Mesh.h"
#include <memory>
#include <vector>
#include "BufferStructs.h"
#include "SimpleShader.h"
#include "Material.h"
#include "WICTextureLoader.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Called once per program, after the window and graphics API
// are initialized but before the game loop begins
// --------------------------------------------------------
void Game::Initialize()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateGeometry();


	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		Graphics::Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Ensure the pipeline knows how to interpret all the numbers stored in
		// the vertex buffer. For this course, all of your vertices will probably
		// have the same layout, so we can just set this once at startup.

		// Set the active vertex and pixel shaders
		//  - Once you start applying different shaders to different objects,
		//    these calls will need to happen multiple times per frame
		
	}

	// Making the constant buffer size a multiple of 16
	unsigned int constantBufferSize = sizeof(VertexShaderData);
	constantBufferSize = (constantBufferSize + 15) / 16 * 16;

	// Describe the constant buffer
	D3D11_BUFFER_DESC cbDesc = {}; 
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.ByteWidth = constantBufferSize; 
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	//Graphics::Device->CreateBuffer(&cbDesc, 0, constantBuffer.GetAddressOf());-----------------------------------------------------------------

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(Window::Handle());
	ImGui_ImplDX11_Init(Graphics::Device.Get(), Graphics::Context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	// Giving the offset and tint some default values
	translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	colorTint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Initialize camera
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(0.0f, 9.0f, -15.0f), 5.0f, 0.01f, XM_PIDIV4, Window::AspectRatio()));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(5.0f, 0.0f, -5.0f), 5.0f, 0.01f, XM_PIDIV2, Window::AspectRatio()));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(-2.0f, 0.0f, -7.0f), 5.0f, 0.01f, 1.0f, Window::AspectRatio()));

	// Set the current camera
	currentCamera = cameras[0];

	// Set ambient color
	ambientColor = XMFLOAT3(0.1f, 0.1f, 0.25f);


}


// --------------------------------------------------------
// Clean up memory or objects created by this class
// 
// Note: Using smart pointers means there probably won't
//       be much to manually clean up here!
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


// --------------------------------------------------------
// Creates the geometry we're going to draw
// --------------------------------------------------------
void Game::CreateGeometry()
{
	// Local variables
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Graphics::Device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

	//--------------
	// Load Textures
	//--------------
	
	// Load Lava Rock texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> lavaRockSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/LavaRockTexture.png").c_str(),
		nullptr,
		lavaRockSRV.GetAddressOf());

	// Load sand texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sandSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/Sand.png").c_str(),
		nullptr,
		sandSRV.GetAddressOf());

	// Load distortion texture 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> distortionSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/shaderNoise.png").c_str(),
		nullptr,
		distortionSRV.GetAddressOf());

	// Load distortion texture 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> fireSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/Explosion.png").c_str(),
		nullptr,
		fireSRV.GetAddressOf());

	// Load energy texture 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> energySRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/Plasma2.png").c_str(),
		nullptr,
		energySRV.GetAddressOf());

	// Load cobblestone texture 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cobblestone.png").c_str(),
		nullptr,
		cobblestoneSRV.GetAddressOf());

	// Load cobblestone normal texture 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cobblestoneNormalsSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str(),
		nullptr,
		cobblestoneNormalsSRV.GetAddressOf());


	// Load metal floor PBR textures 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorSRV;
	std::wstring floorAlbedoFilePath = L"../../Assets/Textures/PBR/floor_albedo.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorNormalsSRV;
	std::wstring floorNormalFilePath = L"../../Assets/Textures/PBR/floor_normals.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorRoughnessSRV;
	std::wstring floorRoughnessFilePath = L"../../Assets/Textures/PBR/floor_roughness.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> floorMetalSRV;
	std::wstring floorMetalFilePath = L"../../Assets/Textures/PBR/floor_metal.png";

	LoadPBRTexturesFromFile(
		floorSRV, floorAlbedoFilePath, 
		floorNormalsSRV, floorNormalFilePath, 
		floorRoughnessSRV, floorRoughnessFilePath, 
		floorMetalSRV, floorMetalFilePath);


	// Load bronze PBR textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeSRV;
	std::wstring bronzeAlbedoFilePath = L"../../Assets/Textures/PBR/bronze_albedo.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeNormalsSRV;
	std::wstring bronzeNormalFilePath = L"../../Assets/Textures/PBR/bronze_normals.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeRoughnessSRV;
	std::wstring bronzeRoughnessFilePath = L"../../Assets/Textures/PBR/bronze_roughness.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> bronzeMetalSRV;
	std::wstring bronzeMetalFilePath = L"../../Assets/Textures/PBR/bronze_metal.png";

	LoadPBRTexturesFromFile(
		bronzeSRV, bronzeAlbedoFilePath,
		bronzeNormalsSRV, bronzeNormalFilePath,
		bronzeRoughnessSRV, bronzeRoughnessFilePath,
		bronzeMetalSRV, bronzeMetalFilePath);


	// Load scratched metal PBR textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedSRV;
	std::wstring scratchedAlbedoFilePath = L"../../Assets/Textures/PBR/scratched_albedo.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedNormalsSRV;
	std::wstring scratchedNormalFilePath = L"../../Assets/Textures/PBR/scratched_normals.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedRoughnessSRV;
	std::wstring scratchedRoughnessFilePath = L"../../Assets/Textures/PBR/scratched_roughness.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedMetalSRV;
	std::wstring scratchedMetalFilePath = L"../../Assets/Textures/PBR/scratched_metal.png";

	LoadPBRTexturesFromFile(
		scratchedSRV, scratchedAlbedoFilePath,
		scratchedNormalsSRV, scratchedNormalFilePath,
		scratchedRoughnessSRV, scratchedRoughnessFilePath,
		scratchedMetalSRV, scratchedMetalFilePath);


	// Load paint PBR textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintSRV;
	std::wstring paintAlbedoFilePath = L"../../Assets/Textures/PBR/paint_albedo.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintNormalsSRV;
	std::wstring paintNormalFilePath = L"../../Assets/Textures/PBR/paint_normals.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintRoughnessSRV;
	std::wstring paintRoughnessFilePath = L"../../Assets/Textures/PBR/paint_roughness.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> paintMetalSRV;
	std::wstring paintMetalFilePath = L"../../Assets/Textures/PBR/paint_metal.png";

	LoadPBRTexturesFromFile(
		paintSRV, paintAlbedoFilePath,
		paintNormalsSRV, paintNormalFilePath,
		paintRoughnessSRV, paintRoughnessFilePath,
		paintMetalSRV, paintMetalFilePath);


	// Load rough PBR textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughSRV;
	std::wstring roughAlbedoFilePath = L"../../Assets/Textures/PBR/rough_albedo.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughNormalsSRV;
	std::wstring roughNormalFilePath = L"../../Assets/Textures/PBR/rough_normals.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughRoughnessSRV;
	std::wstring roughRoughnessFilePath = L"../../Assets/Textures/PBR/rough_roughness.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughMetalSRV;
	std::wstring roughMetalFilePath = L"../../Assets/Textures/PBR/rough_metal.png";

	LoadPBRTexturesFromFile(
		roughSRV, roughAlbedoFilePath,
		roughNormalsSRV, roughNormalFilePath,
		roughRoughnessSRV, roughRoughnessFilePath,
		roughMetalSRV, roughMetalFilePath);


	// Load wood PBR textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodSRV;
	std::wstring woodAlbedoFilePath = L"../../Assets/Textures/PBR/wood_albedo.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodNormalsSRV;
	std::wstring woodNormalFilePath = L"../../Assets/Textures/PBR/wood_normals.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodRoughnessSRV;
	std::wstring woodRoughnessFilePath = L"../../Assets/Textures/PBR/wood_roughness.png";

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> woodMetalSRV;
	std::wstring woodMetalFilePath = L"../../Assets/Textures/PBR/wood_metal.png";

	LoadPBRTexturesFromFile(
		woodSRV, woodAlbedoFilePath,
		woodNormalsSRV, woodNormalFilePath,
		woodRoughnessSRV, woodRoughnessFilePath,
		woodMetalSRV, woodMetalFilePath);


	//-----------------
	// Making Materials
	//-----------------

	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> uvPixelShader = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugUVsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> normalPixelShader = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"DebugNormalsPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> customPixelShader = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"CustomPS.cso").c_str());
	std::shared_ptr<SimplePixelShader> texturePixelShader = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"TexturePS.cso").c_str());
	std::shared_ptr<SimplePixelShader> energyPixelShader = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"EnergyPS.cso").c_str());
	shadowVS = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"ShadowMapVertexShader.cso").c_str());

	// Creating materials with different tints
	std::shared_ptr<Material> basicMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);
	basicMaterial->AddSampler("BasicSampler", samplerState);
	std::shared_ptr<Material> basicMaterial2 = std::make_shared<Material>(
		XMFLOAT4(0.5f, 0.0f, 0.70f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);
	std::shared_ptr<Material> uvMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), vs, uvPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);
	std::shared_ptr<Material> normalMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vs, normalPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);
	std::shared_ptr<Material> customMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vs, customPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);

	// Creating materials with textures from files
	std::shared_ptr<Material> lavaRockMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);
	lavaRockMaterial->AddSampler("BasicSampler", samplerState);
	lavaRockMaterial->AddTextureSRV("SurfaceTexture", lavaRockSRV);

	std::shared_ptr<Material> sandMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, texturePixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	sandMaterial->AddSampler("BasicSampler", samplerState);
	sandMaterial->AddTextureSRV("SurfaceTexture", sandSRV);
	sandMaterial->AddTextureSRV("DistortionSurfaceTexture", distortionSRV);


	std::shared_ptr<Material> fireMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, energyPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f, 0.0f);
	fireMaterial->AddSampler("BasicSampler", samplerState);
	fireMaterial->AddTextureSRV("SurfaceTexture", fireSRV);
	fireMaterial->AddTextureSRV("EnergySurfaceTexture", energySRV);


	std::shared_ptr<Material> cobblestoneMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	cobblestoneMaterial->AddSampler("BasicSampler", samplerState);
	cobblestoneMaterial->AddTextureSRV("Albedo", cobblestoneSRV);
	cobblestoneMaterial->AddTextureSRV("NormalMap", cobblestoneNormalsSRV);

	// PBR materials
	std::shared_ptr<Material> floorMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	PBR_SRV_TextureInitialize(floorMaterial, floorSRV, floorNormalsSRV, floorRoughnessSRV, floorMetalSRV);

	std::shared_ptr<Material> bronzeMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	PBR_SRV_TextureInitialize(bronzeMaterial, bronzeSRV, bronzeNormalsSRV, bronzeRoughnessSRV, bronzeMetalSRV);

	std::shared_ptr<Material> scractchedMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	PBR_SRV_TextureInitialize(scractchedMaterial, scratchedSRV, scratchedNormalsSRV, scratchedRoughnessSRV, scratchedMetalSRV);

	std::shared_ptr<Material> paintMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	PBR_SRV_TextureInitialize(paintMaterial, paintSRV, paintNormalsSRV, paintRoughnessSRV, paintMetalSRV);

	std::shared_ptr<Material> roughMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	PBR_SRV_TextureInitialize(roughMaterial, roughSRV, roughNormalsSRV, roughRoughnessSRV, roughMetalSRV);

	std::shared_ptr<Material> woodMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 0.05f, 0.0f);
	PBR_SRV_TextureInitialize(woodMaterial, woodSRV, woodNormalsSRV, woodRoughnessSRV, woodMetalSRV);


	// Adding materials to a list
	materials.push_back(lavaRockMaterial);
	materials.push_back(sandMaterial);
	materials.push_back(fireMaterial);
	materials.push_back(cobblestoneMaterial);
	materials.push_back(floorMaterial);
	materials.push_back(bronzeMaterial);
	materials.push_back(scractchedMaterial);
	materials.push_back(paintMaterial);
	materials.push_back(roughMaterial);
	materials.push_back(woodMaterial);




	//-----------------------
	// Initializing 3D meshes
	//-----------------------
	
	std::shared_ptr<Mesh> cube = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str());
	std::shared_ptr<Mesh> cylinder = std::make_shared<Mesh>(FixPath("../../Assets/Models/cylinder.obj").c_str());
	std::shared_ptr<Mesh> helix = std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str());
	std::shared_ptr<Mesh> sphere = std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str());
	std::shared_ptr<Mesh> torus = std::make_shared<Mesh>(FixPath("../../Assets/Models/torus.obj").c_str());
	std::shared_ptr<Mesh> quad = std::make_shared<Mesh>(FixPath("../../Assets/Models/quad.obj").c_str());
	std::shared_ptr<Mesh> quadDoubleSided = std::make_shared<Mesh>(FixPath("../../Assets/Models/quad_double_sided.obj").c_str());


	//// Add meshes to entitty list with normal material
	//entities.push_back(Entity(sphere, floorMaterial));
	//entities.push_back(Entity(sphere, bronzeMaterial));
	//entities.push_back(Entity(sphere, scractchedMaterial));
	//entities.push_back(Entity(sphere, paintMaterial));
	//entities.push_back(Entity(sphere, roughMaterial));
	//entities.push_back(Entity(sphere, woodMaterial));
	//entities.push_back(Entity(sphere, bronzeMaterial));
	//
	//// Number of shapes in each row, used for spacing out shapes without hard coding values
	//int numberOfShapesForRow = (int)entities.size();
	//
	//// Add meshes to entitty list with UV material
	//entities.push_back(Entity(cube, floorMaterial));
	//entities.push_back(Entity(cylinder, floorMaterial));
	//entities.push_back(Entity(helix, floorMaterial));
	//entities.push_back(Entity(sphere, floorMaterial));
	//entities.push_back(Entity(torus, floorMaterial));
	//entities.push_back(Entity(quad, floorMaterial));
	//entities.push_back(Entity(quadDoubleSided, floorMaterial));
	//
	//// Add meshes to entitty list with custom material
	//entities.push_back(Entity(cube, fireMaterial));
	//entities.push_back(Entity(cylinder, sandMaterial));
	//entities.push_back(Entity(helix, sandMaterial));
	//entities.push_back(Entity(sphere, fireMaterial));
	//entities.push_back(Entity(torus, customMaterial));
	//entities.push_back(Entity(quad, customMaterial));
	//entities.push_back(Entity(quadDoubleSided, customMaterial));
	//
	//// Number of shapes in each row, used for spacing out shapes without hard coding values
	////int numberOfShapesForRow = (int)entities.size();
	//
	//// Offset to make rows
	//float verticalOffset = -1.0f;
	//
	//// Spacing out the entities
	//for (int i = 0; i < entities.size(); i++)
	//{
	//	// Makes a new row once each shape has been displayed
	//	if (i % numberOfShapesForRow == 0)
	//	{
	//		verticalOffset++;
	//	}
	//
	//	entities[i].GetTransform()->SetPosition(XMFLOAT3((3.0f * (i % numberOfShapesForRow)) - 9.0f, (5.0f - (verticalOffset * 3)) , 0.0f));
	//
	//}


	//------------------------------
	// Meshs for shadow mapping test
	//------------------------------
	entities.push_back(Entity(cube, woodMaterial));
	entities.push_back(Entity(torus, floorMaterial));
	entities.push_back(Entity(sphere, paintMaterial));
	entities.push_back(Entity(helix, roughMaterial));

	entities[0].GetTransform()->SetScale(XMFLOAT3(20.0f, 0.01f, 20.0f));
	entities[1].GetTransform()->SetPosition(XMFLOAT3(0.0f, 2.0f, 0.0f));
	entities[2].GetTransform()->SetPosition(XMFLOAT3(-3.0f, 2.0f, 0.0f));
	entities[3].GetTransform()->SetPosition(XMFLOAT3(3.0f, 2.0f, 0.0f));






	//------------------
	// Initialize lights
	//------------------

	Light directionalLight1 = {};
	directionalLight1.Color = XMFLOAT3(1, 1, 1);
	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Intensity = 0.7f;
	directionalLight1.Direction = XMFLOAT3(0, -1, 1); 
	lights.push_back(directionalLight1);

	Light directionalLight2 = {};
	directionalLight2.Color = XMFLOAT3(0, 0, 1);
	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Intensity = 0.7f;
	directionalLight2.Direction = XMFLOAT3(0, 0, 1); 
	//lights.push_back(directionalLight2);

	Light pointLight1 = {};
	pointLight1.Color = XMFLOAT3(0, 1, 0);
	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Intensity = 1.0f;
	pointLight1.Position = XMFLOAT3(5, 5, 0); 
	pointLight1.Range = 10.0f;
	//lights.push_back(pointLight1);

	Light spotLight1 = {};
	spotLight1.Color = XMFLOAT3(0.5f, 0.5f, 0.5f);
	spotLight1.Type = LIGHT_TYPE_SPOT;
	spotLight1.Intensity = 1.0f;
	spotLight1.Direction = XMFLOAT3(0, -1, 0);
	spotLight1.Position = XMFLOAT3(-6, 5, 0);
	spotLight1.Range = 10.0f;
	spotLight1.SpotInnerAngle = XMConvertToRadians(20.0f);
	spotLight1.SpotOuterAngle = XMConvertToRadians(40.0f);
	//lights.push_back(spotLight1);

	Light spotLight2 = {};
	spotLight2.Color = XMFLOAT3(1, 1, 1);
	spotLight2.Type = LIGHT_TYPE_SPOT;
	spotLight2.Intensity = 1.0f;
	spotLight2.Direction = XMFLOAT3(0, -1, 0);
	spotLight2.Position = XMFLOAT3(-9, 7, 0);
	spotLight2.Range = 10.0f;
	spotLight2.SpotInnerAngle = XMConvertToRadians(1.0f);
	spotLight2.SpotOuterAngle = XMConvertToRadians(10.0f);
	//lights.push_back(spotLight2);


	//----------------------------
	// Creating Shadow Map Texture
	//----------------------------


	// Create the actual texture that will be the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapResolution;
	shadowDesc.Height = shadowMapResolution;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	Graphics::Device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());


	// Create the depth/stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	Graphics::Device->CreateDepthStencilView(
		shadowTexture.Get(),
		&shadowDSDesc,
		shadowDSV.GetAddressOf());

	// Create the SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	Graphics::Device->CreateShaderResourceView(
		shadowTexture.Get(),
		&srvDesc,
		shadowSRV.GetAddressOf());


	// Create rasterizer state for depth biasing
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Min. precision units, not world units!
	shadowRastDesc.SlopeScaledDepthBias = 1.0f; // Bias more based on slope
	Graphics::Device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);


	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f; // Only need the first component
	Graphics::Device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	XMVECTOR lightDirection = XMVectorSet(directionalLight1.Direction.x, directionalLight1.Direction.y, directionalLight1.Direction.z, 0);

	XMMATRIX lightView = XMMatrixLookToLH(
		-lightDirection * 20, // Position: "Backing up" 20 units from origin
		lightDirection, // Direction: light's direction
		XMVectorSet(0, 1, 0, 0)); // Up: World up vector (Y axis)

	float lightProjectionSize = 15.0f; 
	XMMATRIX lightProjection = XMMatrixOrthographicLH(
		lightProjectionSize,
		lightProjectionSize,
		1.0f,
		100.0f);

	// Store the light matricies
	XMStoreFloat4x4(&lightViewMatrix, lightView);
	XMStoreFloat4x4(&lightProjectionMatrix, lightProjection);


	//------------------------------
	// Creating Post Process Texture
	//------------------------------

	ppVS = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"FullscreenVertexShader.cso").c_str());
	ppPS = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PostProcessPixelShader.cso").c_str());

	// Sampler state for post processing
	D3D11_SAMPLER_DESC ppSampDesc = {};
	ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&ppSampDesc, ppSampler.GetAddressOf());

	// Describe the texture we're creating
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = Window::Width();
	textureDesc.Height = Window::Height();;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	// Create the resource (no need to track it after the views are created below)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	Graphics::Device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());


	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Graphics::Device->CreateRenderTargetView(
		ppTexture.Get(),
		&rtvDesc,
		ppRTV.ReleaseAndGetAddressOf());

	// Create the Shader Resource View
	// By passing it a null description for the SRV, we
	// get a "default" SRV that has access to the entire resource
	Graphics::Device->CreateShaderResourceView(
		ppTexture.Get(),
		0,
		ppSRV.ReleaseAndGetAddressOf());



	//------------------
	// Initialize Skybox
	//------------------

	skybox = std::make_shared<Skybox>(cube, samplerState, FixPath(L"../../Assets/Textures/right.png").c_str(),
		FixPath(L"../../Assets/Textures/left.png").c_str(),
		FixPath(L"../../Assets/Textures/up.png").c_str(),
		FixPath(L"../../Assets/Textures/down.png").c_str(),
		FixPath(L"../../Assets/Textures/front.png").c_str(),
		FixPath(L"../../Assets/Textures/back.png").c_str());
}


// --------------------------------------------------------
// Handle resizing to match the new window size
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Check for if camera is null
	if (cameras.size() != 0)
	{
		// Update cameras with the window aspect ratio
		for (int i = 0; i < cameras.size(); i++)
		{
			cameras[i]->UpdateProjectionMatrix(Window::AspectRatio());
		}
	}

	if (Graphics::Device)
	{
		// Describe the texture we're creating
		D3D11_TEXTURE2D_DESC textureDesc = {};
		textureDesc.Width = Window::Width();
		textureDesc.Height = Window::Height();;
		textureDesc.ArraySize = 1;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.MipLevels = 1;
		textureDesc.MiscFlags = 0;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;

		// Create the resource (no need to track it after the views are created below)
		Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
		Graphics::Device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());


		// Create the Render Target View
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = textureDesc.Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		Graphics::Device->CreateRenderTargetView(
			ppTexture.Get(),
			&rtvDesc,
			ppRTV.ReleaseAndGetAddressOf());

		// Create the Shader Resource View
		// By passing it a null description for the SRV, we
		// get a "default" SRV that has access to the entire resource
		Graphics::Device->CreateShaderResourceView(
			ppTexture.Get(),
			0,
			ppSRV.ReleaseAndGetAddressOf());
	}
}


// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Refresh the ImGui
	RefreshUI(deltaTime);

	CreateUI();

	//// Update cameras with the window aspect ratio
	currentCamera->Update(deltaTime);

	// Move all meshes
	//for (int i = 0; i < entities.size(); i++)
	//{
	//	// Global movment of entities back a forth
	//	XMFLOAT3 pos = entities[i].GetTransform()->GetPosition();
	//	pos.x += (float)(sin(totalTime) * 0.5f * deltaTime);  
	//
	//	// Appling the updated global movement
	//	entities[i].GetTransform()->SetPosition(pos);
	//
	//	// Global scale of entities back a forth
	//	XMFLOAT3 scl = entities[i].GetTransform()->GetScale();
	//	scl.y += (float)(sin(totalTime) * 0.5f * deltaTime);
	//
	//	// Appling the updated global scale
	//	entities[i].GetTransform()->SetScale(scl);
	//}


	// Move meshes for shadow mapping test
	entities[1].GetTransform()->SetRotation(XMFLOAT3(totalTime, totalTime, 0.0f));

	float move = (float)(sin(totalTime) * 10.0f);

	entities[2].GetTransform()->SetPosition(XMFLOAT3(-3.0f, 2.0f, move));

	entities[3].GetTransform()->SetPosition(XMFLOAT3(3.0f, move / 5.0f + 3.0f, 0.0f));





	// Example input checking: Quit if the escape key is pressed
	if (Input::KeyDown(VK_ESCAPE))
		Window::Quit();
}


// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erase what's on screen) and depth buffer
		//float color[4] = bgColor[];
		Graphics::Context->ClearRenderTargetView(Graphics::BackBufferRTV.Get(),	bgColor);
		Graphics::Context->ClearDepthStencilView(Graphics::DepthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	}

	// Clear shadow map
	Graphics::Context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Set up shadow map output merger
	ID3D11RenderTargetView* nullRTV{};
	Graphics::Context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());


	// Enable shadow rasterizer state
	Graphics::Context->RSSetState(shadowRasterizer.Get());

	// Deactivate pixel shader
	Graphics::Context->PSSetShader(0, 0, 0);

	// Change viewport size to match the shadowmaps resolution
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)shadowMapResolution;
	viewport.Height = (float)shadowMapResolution;
	viewport.MaxDepth = 1.0f;
	Graphics::Context->RSSetViewports(1, &viewport);

	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", lightViewMatrix);
	shadowVS->SetMatrix4x4("projection", lightProjectionMatrix);



	// Loop and draw all entities
	for (int i = 0; i < entities.size(); i++)
	{
		shadowVS->SetMatrix4x4("world", entities[i].GetTransform()->GetWorldMatrix());
		shadowVS->CopyAllBufferData();

		// Draw the mesh directly to avoid the entity's material
		entities[i].GetMesh()->Draw();
	}


	// Change pipeline settings back so that the screen can be rendered
	viewport.Width = (float)Window::Width();
	viewport.Height = (float)Window::Height();
	Graphics::Context->RSSetViewports(1, &viewport);
	Graphics::Context->OMSetRenderTargets(
		1,
		Graphics::BackBufferRTV.GetAddressOf(),
		Graphics::DepthBufferDSV.Get());

	Graphics::Context->RSSetState(0);

	// Clear the post process effect
	Graphics::Context->ClearRenderTargetView(ppRTV.Get(), bgColor);
	Graphics::Context->OMSetRenderTargets(1, ppRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());

	// Draw Geometry
	for (int i = 0; i < entities.size(); i++)
	{
		// Setting shadowmap vertex shader data
		entities[i].GetMaterial()->VertexShader()->SetMatrix4x4("lightView", lightViewMatrix);
		entities[i].GetMaterial()->VertexShader()->SetMatrix4x4("lightProjection", lightProjectionMatrix);


		// Pass in values to the shader for lighting
		entities[i].GetMaterial()->PixelShader()->SetFloat3("ambient", ambientColor);
		entities[i].GetMaterial()->PixelShader()->SetInt("lightsCount", (int)lights.size());

		entities[i].GetMaterial()->PixelShader()->SetShaderResourceView("ShadowMap", shadowSRV);
		entities[i].GetMaterial()->PixelShader()->SetSamplerState("ShadowSampler", shadowSampler);


		// Add the lights to the pixel shader
		entities[i].GetMaterial()->PixelShader()->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

		entities[i].Draw(currentCamera, totalTime);
	}

	skybox->Draw(currentCamera);

	
	// Post Process
	Graphics::Context->OMSetRenderTargets(1, Graphics::BackBufferRTV.GetAddressOf(), 0);

	// Activate shaders and bind resources
	// Also set any required cbuffer data (not shown)
	ppVS->SetShader();
	ppPS->SetShader();
	ppPS->SetShaderResourceView("Pixels", ppSRV.Get());
	ppPS->SetSamplerState("ClampSampler", ppSampler.Get());

	ppPS->SetFloat("pixelWidth", 1.0f / Window::Width());
	ppPS->SetFloat("pixelHeight", 1.0f / Window::Height());
	ppPS->SetInt("blurRadius", 10);
	ppPS->CopyAllBufferData();

	Graphics::Context->Draw(3, 0); // Draw exactly 3 vertices (one triangle)

	// Unbind shadow depth buffer and shadowsrv
	ID3D11ShaderResourceView* nullSRVs[128] = {};
	Graphics::Context->PSSetShaderResources(0, 128, nullSRVs);


	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen


	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present at the end of the frame
		bool vsync = Graphics::VsyncState();
		Graphics::SwapChain->Present(
			vsync ? 1 : 0,
			vsync ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Re-bind back buffer and depth buffer after presenting
		Graphics::Context->OMSetRenderTargets(
			1,
			Graphics::BackBufferRTV.GetAddressOf(),
			Graphics::DepthBufferDSV.Get());
	}
}

//---------------
// Helper Methods
//---------------

// Loads PBR textures from their files
void Game::LoadPBRTexturesFromFile(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& albedoSRV, std::wstring& albedoRelativeFilePath, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& normalMapSRV, std::wstring& normalRelativeFilePath, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& roughnessMapSRV, std::wstring& roughnessRelativeFilePath, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& metalnessMapSRV, std::wstring& metalnessRelativeFilePath)
{
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(albedoRelativeFilePath).c_str(),
		nullptr,
		albedoSRV.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(normalRelativeFilePath).c_str(),
		nullptr,
		normalMapSRV.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(roughnessRelativeFilePath).c_str(),
		nullptr,
		roughnessMapSRV.GetAddressOf());

	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(metalnessRelativeFilePath).c_str(),
		nullptr,
		metalnessMapSRV.GetAddressOf());
	
}

// Helper method to set the PBR texture resources
void Game::PBR_SRV_TextureInitialize(std::shared_ptr<Material> material, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalMapSRV, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessMapSRV, 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessMapSRV)
{

	material->AddSampler("BasicSampler", samplerState);
	material->AddTextureSRV("Albedo", albedoSRV);
	material->AddTextureSRV("NormalMap", normalMapSRV);
	material->AddTextureSRV("RoughnessMap", roughnessMapSRV);
	material->AddTextureSRV("MetalnessMap", metalnessMapSRV);
}

// This refreshs the UI every frame
void Game::RefreshUI(float deltaTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)Window::Width();
	io.DisplaySize.y = (float)Window::Height();
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input::SetKeyboardCapture(io.WantCaptureKeyboard);
	Input::SetMouseCapture(io.WantCaptureMouse);

}

// Creates a UI holding general data and some customizations for the window
void Game::CreateUI()
{
	// Variables
	float framerate = ImGui::GetIO().Framerate;
	float windowWidth = static_cast<float>(Window::Width());
	float windowHeight = static_cast<float>(Window::Height());



	// Create a ImGui window to display UI elements
	ImGui::Begin("UI Window");

	// Expandable/collapasble section for orginization
	if (ImGui::CollapsingHeader("App Details"))
	{
		// Displays the fps
		ImGui::Text("Framerate - %f fps", framerate);

		// Displays the window width and height
		ImGui::Text("Width - %f px : Height - %f px", windowWidth, windowHeight);

		// Allows user to pick and change the color
		ImGui::ColorEdit4("RGBA background color picker", bgColor);

		// Button to enable/disable the demo window
		if (ImGui::Checkbox("Show Demo Window", &showDemoWindow)){}
		

		// If true, then show the demo window
		if (showDemoWindow)
		{
			ImGui::ShowDemoWindow();
		}

		// Increases/decreases font size by incrementing/decrementing by 0.1 when the button is clicked
		if (ImGui::Button("Increase Font"))
		{
			fontSize += 0.1f; 
		}
		if (ImGui::Button("Decrease Font"))
		{
			fontSize -= 0.1f; 
		}


		ImGui::Text("Press + or - to increase/decrease font size");

		// Press equals key (Plus) to increase font size
		if (ImGui::IsKeyDown(ImGuiKey_Equal))
		{
			fontSize += 0.005f;

		}

		// Press Minus key to decrease font size
		if (ImGui::IsKeyDown(ImGuiKey_Minus))
		{
			fontSize -= 0.005f;

		}

		// Cap the font size at 0.1 to avoid the font size from becoming negative
		if (fontSize < 0.1f)
		{
			fontSize += 0.01f;
		}

		// Adjust the font size
		ImGui::SetWindowFontScale(fontSize);
		
		// Starts/pauses the stop watch
		if (ImGui::IsKeyPressed(ImGuiKey_Space, true))
		{
			stopwatch = !stopwatch;
		}

		// Adds to deltatime to time to track the amount of time since the stopwatch was started
		if (stopwatch)
		{
			time += ImGui::GetIO().DeltaTime;

		}

		// Resets the stop watch
		if (ImGui::IsKeyPressed(ImGuiKey_R, true))
		{
			stopwatch = false;
			time = 0.0f;
		}

		// Stopwatch instructions and display
		ImGui::Text("Press SPACE to START and PAUSE the stopwatch");
		ImGui::Text("Press R to reset");
		ImGui::Text("Stopwatch - %f Seconds", time);
		
		// Color picker for mesh tint and UI element to drag the meshes
		ImGui::DragFloat3("Shader Offset", &translation.x, 0.001f);			 
		ImGui::ColorEdit4("RGBA mesh tint color picker", &colorTint.x);
	}

	// Displays mesh useful info
	if (ImGui::CollapsingHeader("Mesh Information"))
	{
		ImGui::Indent(20.0f); // Indent to make the data more organized
		for (int i = 0; i < entities.size(); i++)
		{
			// Creating a mesh label because I don't have names for my meshes
			std::string meshLabel = "Mesh " + std::to_string(i + 1);

			// Shows the basic mesh info for each mesh displayed on screen
			if (ImGui::CollapsingHeader(meshLabel.c_str()))
			{
				ImGui::Text("Triangles - %d", entities[i].GetMesh()->GetIndexCount() / 3);
				ImGui::Text("Vertices - %d", entities[i].GetMesh()->GetVertexCount());
				ImGui::Text("Indices - %d", entities[i].GetMesh()->GetIndexCount());

			}
		}
		ImGui::Unindent(20.0f);

	}

	// Shows individual entities position, rotation, and scale and allows user to edit them
	if (ImGui::CollapsingHeader("Scene Entities"))
	{
		ImGui::Indent(20.0f); // Indent to make the data more organized

		for (int i = 0; i < entities.size(); i++)
		{
			ImGui::PushID(i);

			// Creating a label for entities
			std::string entityLabel = "Entity " + std::to_string(i + 1);

			if (ImGui::CollapsingHeader(entityLabel.c_str()))
			{
				// Get current transform data
				DirectX::XMFLOAT3 pos = entities[i].GetTransform()->GetPosition();
				DirectX::XMFLOAT3 rot = entities[i].GetTransform()->GetPitchYawRoll();
				DirectX::XMFLOAT3 scale = entities[i].GetTransform()->GetScale();

				// Position
				if (ImGui::DragFloat3("Position", &pos.x, 0.01f))
					entities[i].GetTransform()->SetPosition(pos);

				// Rotation
				if (ImGui::DragFloat3("Rotation", &rot.x, 0.01f))
					entities[i].GetTransform()->SetRotation(rot);

				// Scale
				if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
					entities[i].GetTransform()->SetScale(scale);

			}

			ImGui::PopID(); 

		}
		ImGui::Unindent(20.0f);
	}

	// Allows user to switch between cameras
	if (ImGui::CollapsingHeader("Cameras"))
	{
		ImGui::Indent(20.0f); // Indent to make the data more organized


		for (int i = 0; i < cameras.size(); i++)
		{
			// Creating a label for cameras
			std::string cameraLabel = "Camera " + std::to_string(i + 1);

			ImGui::PushID(i);

			// Set the respective camera active if button is clicked
			if (ImGui::Button(cameraLabel.c_str()))
			{				
				// Activate the right camera			
				currentCamera = cameras[i];
			}

			// Display each cameras position
			XMFLOAT3 cameraTransform = cameras[i]->GetTransform().GetPosition();
			ImGui::Text("Position: X - %f Y - %f Z - %f", cameraTransform.x, cameraTransform.y, cameraTransform.z);

			ImGui::PopID();
		}


		ImGui::Unindent(20.0f);
	}


	// Allows user to switch between materials and adjust materials
	if (ImGui::CollapsingHeader("Materials"))
	{
		ImGui::Indent(20.0f); // Indent to make the data more organized

		for (int i = 0; i < materials.size(); i++)
		{
			ImGui::PushID(i);

			// Storing changable material values
			XMFLOAT4 materialTint = materials[i]->Tint();
			XMFLOAT2 materialScale = materials[i]->Scale();
			XMFLOAT2 materialOffset = materials[i]->Offset();
			float materialDistortionStrength = materials[i]->DistortionStrength();

			// Set the material color
			ImGui::ColorEdit4("RGBA material tint color picker", &materialTint.x);
			materials[i]->SetTint(materialTint);

			// Set the material scale
			if (ImGui::DragFloat2("Scale", &materialScale.x, 0.01f))
				materials[i]->SetScale(materialScale);

			// Set the Offset
			if (ImGui::DragFloat2("Offset", &materialOffset.x, 0.01f))
				materials[i]->SetOffset(materialOffset);

			// Set the Distortion
			if (ImGui::DragFloat("Distortion", &materialDistortionStrength, 0.001f, 0.0f, 1.0f))
				materials[i]->SetDistortionStrength(materialDistortionStrength);


			// Display the texture
			auto textureSRV = materials[i]->GetTextureSRV();
			if (textureSRV)
			{
				ImGui::Image((ImTextureID)textureSRV.Get(), ImVec2(256, 256));
			}
			else
			{
				ImGui::Text("No Texture");
			}


			ImGui::PopID();

		}

		ImGui::Unindent(20.0f);
	}

	// Allows user to change aspects of lights
	if (ImGui::CollapsingHeader("Lights"))
	{
		ImGui::Indent(20.0f); // Indent to make the data more organized

		for (int i = 0; i < lights.size(); i++)
		{
			ImGui::PushID(i);

			// Changes the color of the lights
			ImGui::ColorEdit4("RGBA light color picker", &lights[i].Color.x);

			// Change spot lights and point lights position
			if(lights[i].Type == LIGHT_TYPE_POINT || lights[i].Type == LIGHT_TYPE_SPOT)
				ImGui::DragFloat3("Position", &lights[i].Position.x, 0.01f);

			// Change spot lights rotation (direction)
			if (lights[i].Type == LIGHT_TYPE_SPOT)
				ImGui::DragFloat3("Rotation", &lights[i].Direction.x, 0.01f);


			ImGui::PopID();

		}
	}

	// Allows user to shadow map
	if (ImGui::CollapsingHeader("Shadow Map"))
	{
		ImGui::Image((ImTextureID)shadowSRV.Get(), ImVec2(512, 512));
	}
	ImGui::End();
}
