#include "Application.hpp"
#include "Console.hpp"
#include <imgui/imgui.h>
#include "CompilerExceptions.hpp"
#include <UniformsLayout.hpp>

SampleRenderV2::Application* SampleRenderV2::Application::s_AppSingleton = nullptr;
bool SampleRenderV2::Application::s_SingletonEnabled = false;

SampleRenderV2::Application::Application()
{
	EnableSingleton(this);

	m_CompleteMVP = {
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity(),
		Eigen::Matrix4f::Identity()
	};

	m_SmallMVP = {
		Eigen::Matrix4f::Identity()
	};

	SampleRenderV2::Console::Init();
	m_Starter.reset(new ApplicationStarter("render.json"));
	m_Window.reset(Window::Instantiate());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	//m_Context->SetClearColor(0x27/255.0f, 0xae/255.0f, 0x60/255.0f, 1.0f);
	m_Window->ConnectResizer(std::bind(&GraphicsContext::WindowResize, m_Context.get(), std::placeholders::_1, std::placeholders::_2));
	std::stringstream buffer;
	buffer << "SampleRender Window [" << (m_Starter->GetCurrentAPI() == GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK ? "Vulkan" : "D3D12") << "]";
	m_Window->ResetTitle(buffer.str());

	ImguiContext::StartImgui();

	m_ImguiWindowController.reset(ImguiWindowController::Instantiate(&m_Window));
	m_ImguiContext.reset(ImguiContext::Instantiate(&m_Context));

	try
	{
		m_SPVCompiler.reset(new SPVCompiler("_main", "_6_8", "1.3"));
		m_CSOCompiler.reset(new CSOCompiler("_main", "_6_8"));
		m_SPVCompiler->PushShaderPath("./assets/shaders/HelloTriangle.hlsl");
		m_CSOCompiler->PushShaderPath("./assets/shaders/HelloTriangle.hlsl");
		m_SPVCompiler->CompilePackedShader();
		m_CSOCompiler->CompilePackedShader();
	}
	catch (CompilerException e)
	{
		Console::CoreError("{}", e.what());
	}

	InputBufferLayout layout(
		{
			{ShaderDataType::Float3, "POSITION", false},
			{ShaderDataType::Float4, "COLOR", false}
		});

	SmallBufferLayout smallBufferLayout(
		{
			//size_t offset, size_t size, uint32_t bindingSlot, uint32_t smallAttachment
			{ 0, 64, 0, m_Context->GetSmallBufferAttachment() }
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	//UniformLayout uniformsLayout(
	//	{
	//		//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t bufferAttachment
	//		{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::ROOT_BUFFER, 1, m_Context->GetUniformAttachment() }, //
	//		{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 2, 0, 2, AccessLevel::ROOT_BUFFER, 1, m_Context->GetUniformAttachment() } //
	//	}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	UniformLayout uniformsLayout(
		{
			//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t bufferAttachment
			{ BufferType::UNIFORM_CONSTANT_BUFFER, 256, 1, 0, 1, AccessLevel::DESCRIPTOR_BUFFER, 2, m_Context->GetUniformAttachment() } //
		}, AllowedStages::VERTEX_STAGE | AllowedStages::PIXEL_STAGE);

	m_Shader.reset(Shader::Instantiate(&m_Context, "./assets/shaders/HelloTriangle", layout, smallBufferLayout, uniformsLayout));
	/*m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 1);
	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 2, 1);*/
	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 1);
	m_Shader->UpdateCBuffer(&m_CompleteMVP.model(0, 0), sizeof(m_CompleteMVP), 1, 2);
	m_VertexBuffer.reset(VertexBuffer::Instantiate(&m_Context, (const void*)&vBuffer[0], sizeof(vBuffer), layout.GetStride()));
	m_IndexBuffer.reset(IndexBuffer::Instantiate(&m_Context, (const void*)&iBuffer[0], sizeof(iBuffer) / sizeof(uint32_t)));
}

SampleRenderV2::Application::~Application()
{
	m_IndexBuffer.reset();
	m_VertexBuffer.reset();
	m_Shader.reset();
	m_ImguiContext.reset();
	m_ImguiWindowController.reset();
	ImguiContext::EndImgui();
	m_Context.reset();
	m_Window.reset();
	SampleRenderV2::Console::End();
}

void SampleRenderV2::Application::Run()
{
	bool show_demo_window = true;
	while (!m_Window->ShouldClose())
	{
		m_Window->Update();
		if (!m_Window->IsMinimized())
		{
			try
			{
				m_Context->ReceiveCommands();
				m_Context->StageViewportAndScissors();
				m_ImguiWindowController->ReceiveInput();
				{
					m_ImguiContext->ReceiveInput();
					ImguiContext::StartFrame();
					{
						for (Layer* layer : m_LayerStack)
							layer->OnUpdate();
						//m_Context->Draw(m_IndexBuffer->GetCount());
						m_Shader->Stage();
						m_Shader->BindSmallBuffer(&m_SmallMVP.model(0, 0), sizeof(m_SmallMVP), 0);
						m_Shader->BindDescriptors();
						m_VertexBuffer->Stage();
						m_IndexBuffer->Stage();
						m_Context->Draw(m_IndexBuffer->GetCount());
					}
					ImguiContext::EndFrame();
					m_ImguiContext->DispatchInput();
				}
				m_Context->DispatchCommands();
				m_Context->Present();
			}
			catch (GraphicsException e)
			{
				Console::CoreError("Caught error: {}", e.what());
				exit(2);
			}
		}
	}
}

void SampleRenderV2::Application::EnableSingleton(Application* ptr)
{
	if (!s_SingletonEnabled)
	{
		s_SingletonEnabled = true;
		s_AppSingleton = ptr;
	}
}

SampleRenderV2::Application* SampleRenderV2::Application::GetInstance()
{
	return s_AppSingleton;
}
