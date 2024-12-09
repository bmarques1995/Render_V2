#include "ApplicationStarter.hpp"
#include "FileHandler.hpp"

const std::unordered_map<std::string, SampleRenderV2::GraphicsAPI> SampleRenderV2::ApplicationStarter::s_GraphicsAPIMapper =
{
	{"SAMPLE_RENDER_GRAPHICS_API_VK", SampleRenderV2::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_VK},
#ifdef RENDER_USES_WINDOWS
	{"SAMPLE_RENDER_GRAPHICS_API_D3D12", SampleRenderV2::GraphicsAPI::SAMPLE_RENDER_GRAPHICS_API_D3D12},
#endif
};

SampleRenderV2::ApplicationStarter::ApplicationStarter(std::string_view jsonFilepath)
{
	if (!FileHandler::FileExists(jsonFilepath.data()))
	{
		m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
		m_API = SAMPLE_RENDER_GRAPHICS_API_VK;
		FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
	}
	else
	{
		Json::Reader reader;
		std::string jsonResult;
		bool healthy = FileHandler::ReadTextFile(jsonFilepath.data(), &jsonResult);
		healthy &= reader.parse(jsonResult, m_Starter);

		auto it = s_GraphicsAPIMapper.find(m_Starter["GraphicsAPI"].asString());
		if ((it == s_GraphicsAPIMapper.end()) && (!healthy))
		{
			m_Starter["GraphicsAPI"] = "SAMPLE_RENDER_GRAPHICS_API_VK";
			m_API = SAMPLE_RENDER_GRAPHICS_API_VK;
			FileHandler::WriteTextFile(jsonFilepath.data(), m_Starter.toStyledString());
		}
		else
		{
			m_API = it->second;
		}

	}
}

SampleRenderV2::ApplicationStarter::~ApplicationStarter()
{
}

SampleRenderV2::GraphicsAPI SampleRenderV2::ApplicationStarter::GetCurrentAPI()
{
	return m_API;
}