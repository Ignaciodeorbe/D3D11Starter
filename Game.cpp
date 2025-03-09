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
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(0.0f, 0.0f, -5.0f), 5.0f, 0.01f, XM_PIDIV4, Window::AspectRatio(), true));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(5.0f, 0.0f, -5.0f), 5.0f, 0.01f, XM_PIDIV2, Window::AspectRatio(), false));
	cameras.push_back(std::make_shared<Camera>(XMFLOAT3(-2.0f, 0.0f, -7.0f), 5.0f, 0.01f, 1.0f, Window::AspectRatio(), false));

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


	std::shared_ptr<SimpleVertexShader> vs = std::make_shared<SimpleVertexShader>(
		Graphics::Device, Graphics::Context, FixPath(L"VertexShader.cso").c_str());
	std::shared_ptr<SimplePixelShader> ps = std::make_shared<SimplePixelShader>(
		Graphics::Device, Graphics::Context, FixPath(L"PixelShader.cso").c_str());

	// Creating materials with different tints
	std::shared_ptr<Material> material1 = std::make_shared<Material>(
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), vs, ps); 
	std::shared_ptr<Material> material2 = std::make_shared<Material>(
		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), vs, ps);
	std::shared_ptr<Material> material3 = std::make_shared<Material>(
		XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), vs, ps);

	//--------------------
	// Initializing Shapes
	//--------------------
	
	std::shared_ptr<Mesh> mesh1 = std::make_shared<Mesh>(FixPath("../../Assets/Models/sphere.obj").c_str());
	std::shared_ptr<Mesh> mesh2 = std::make_shared<Mesh>(FixPath("../../Assets/Models/helix.obj").c_str());
	std::shared_ptr<Mesh> mesh3 = std::make_shared<Mesh>(FixPath("../../Assets/Models/cube.obj").c_str());




	// Add meshes to entitty list
	//entities.push_back(Entity(mesh1, material1));
	//entities.push_back(Entity(mesh2, material2));
	entities.push_back(Entity(mesh3, material3));
	

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

	// Update cameras with the window aspect ratio
	for (int i = 0; i < cameras.size(); i++)
	{
		if (cameras[i]->IsActive())
		{
			cameras[i]->Update(deltaTime);
		}
	}

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

	//VertexShaderData vertexShaderData;


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
		
		// Color picker for mesh tinit and UI element to drag the meshes
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
				// Turn all cameras to inactive
				for (int j = 0; j < cameras.size(); j++)
				{
					cameras[j]->SetActive(false);
				}
				// Activate the right camera
				cameras[i]->SetActive(true);
			}

			ImGui::PopID();

		}


		ImGui::Unindent(20.0f);
	}
	ImGui::End();
}
