#pragma once

#include "Application.hpp"

class AppContext : public SampleRenderV2::Application
{
public:
	AppContext(int argc, char** argv);
	AppContext(int argc, wchar_t** wargv);
	~AppContext() = default;
};