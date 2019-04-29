#pragma once
#include <memory>
#include <string>

class Configuration
{
public:
	Configuration(struct IPrint& iPrint, const std::wstring& configurationFile);
	~Configuration();

	struct IGetConfiguration& IGetConfiguration();


private:
	std::unique_ptr<class ImplConfiguration> m_ImplConfiguration;
};

