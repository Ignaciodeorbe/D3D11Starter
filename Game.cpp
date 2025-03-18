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
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(0.0f, 5.0f, -15.0f), 5.0f, 0.01f, XM_PIDIV4, Window::AspectRatio()));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(5.0f, 0.0f, -5.0f), 5.0f, 0.01f, XM_PIDIV2, Window::AspectRatio()));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(-2.0f, 0.0f, -7.0f), 5.0f, 0.01f, 1.0f, Window::AspectRatio()));

	currentCamera = cameras[0];

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

	// Load distortion texture texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> distortionSRV;
	CreateWICTextureFromFile(
		Graphics::Device.Get(),
		Graphics::Context.Get(),
		FixPath(L"../../Assets/Textures/shaderNoise.png").c_str(),
		nullptr,
		distortionSRV.GetAddressOf());


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

	// Creating materials with different tints
	std::shared_ptr<Material> basicMaterial = std::make_shared<Material>(
		XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	std::shared_ptr<Material> basicMaterial2 = std::make_shared<Material>(
		XMFLOAT4(0.5f, 0.0f, 0.70f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	std::shared_ptr<Material> uvMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), vs, uvPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	std::shared_ptr<Material> normalMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vs, normalPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	std::shared_ptr<Material> customMaterial = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vs, customPixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);


	// Creating materials with textures from files
	std::shared_ptr<Material> lavaRockMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, ps, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	lavaRockMaterial->AddSampler("BasicSampler", samplerState);
	lavaRockMaterial->AddTextureSRV("SurfaceTexture", lavaRockSRV);

	std::shared_ptr<Material> sandMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, texturePixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	sandMaterial->AddSampler("BasicSampler", samplerState);
	sandMaterial->AddTextureSRV("SurfaceTexture", sandSRV);

	std::shared_ptr<Material> distortionMaterial = std::make_shared<Material>(
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), vs, texturePixelShader, XMFLOAT2(1, 1), XMFLOAT2(0, 0), 1.0f);
	sandMaterial->AddSampler("BasicSampler", samplerState);
	sandMaterial->AddTextureSRV("DistortionSurfaceTexture", distortionSRV);

	// Adding materials to a list
	materials.push_back(lavaRockMaterial);
	materials.push_back(sandMaterial);


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
	//entities.push_back(Entity(cube, normalMaterial));
	//entities.push_back(Entity(cylinder, normalMaterial));
	//entities.push_back(Entity(helix, normalMaterial));
	//entities.push_back(Entity(sphere, normalMaterial));
	//entities.push_back(Entity(torus, normalMaterial));
	//entities.push_back(Entity(quad, normalMaterial));
	//entities.push_back(Entity(quadDoubleSided, normalMaterial));
	//
	//// Number of shapes in each row, used for spacing out shapes without hard coding values
	//int numberOfShapesForRow = (int)entities.size();
	//
	//// Add meshes to entitty list with UV material
	//entities.push_back(Entity(cube, uvMaterial));
	//entities.push_back(Entity(cylinder, uvMaterial));
	//entities.push_back(Entity(helix, uvMaterial));
	//entities.push_back(Entity(sphere, uvMaterial));
	//entities.push_back(Entity(torus, uvMaterial));
	//entities.push_back(Entity(quad, uvMaterial));
	//entities.push_back(Entity(quadDoubleSided, uvMaterial));

	// Add meshes to entitty list with custom material
	entities.push_back(Entity(cube, lavaRockMaterial));
	entities.push_back(Entity(cylinder, sandMaterial));
	entities.push_back(Entity(helix, lavaRockMaterial));
	entities.push_back(Entity(sphere, sandMaterial));
	entities.push_back(Entity(torus, lavaRockMaterial));
	entities.push_back(Entity(quad, sandMaterial));
	entities.push_back(Entity(quadDoubleSided, lavaRockMaterial));

	// Number of shapes in each row, used for spacing out shapes without hard coding values
	int numberOfShapesForRow = (int)entities.size();

	// Offset to make rows
	float verticalOffset = -1.0f;

	// Spacing out the entities
	for (int i = 0; i < entities.size(); i++)
	{
		// Makes a new row once each shape has been displayed
		if (i % numberOfShapesForRow == 0)
		{
			verticalOffset++;
		}

		entities[i].GetTransform()->SetPosition(XMFLOAT3((3.0f * (i % numberOfShapesForRow)) - 9.0f, (5.0f - (verticalOffset * 3)) , 0.0f));

	}
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


	// Draw Geometry
	for (int i = 0; i < entities.size(); i++)
	{
			entities[i].Draw(currentCamera);
	}



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
			if (ImGui::DragFloat("Distortion", &materialDistortionStrength, 0.01f))
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
	ImGui::End();
}
