#include "Application.hpp"
#include "Console.hpp"
#include <imgui/imgui.h>
#include "CompilerExceptions.hpp"

SampleRenderV2::Application* SampleRenderV2::Application::s_AppSingleton = nullptr;
bool SampleRenderV2::Application::s_SingletonEnabled = false;

SampleRenderV2::Application::Application()
{
	EnableSingleton(this);
	m_Starter.reset(new ApplicationStarter("render.json"));
	m_Window.reset(Window::Instantiate());
	m_Context.reset(GraphicsContext::Instantiate(m_Window.get(), 3));
	m_Window->ConnectResizer(std::bind(&GraphicsContext::WindowResize, m_Context.get(), std::placeholders::_1, std::placeholders::_2));

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

	m_Shader.reset(Shader::Instantiate(&m_Context, "./assets/shaders/HelloTriangle", layout));
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
						if (show_demo_window)
							ImGui::ShowDemoWindow(&show_demo_window);
						//m_Context->Draw(m_IndexBuffer->GetCount());
						m_Shader->Stage();
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
