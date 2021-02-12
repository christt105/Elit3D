#pragma once

#include <string>
#include <map>
#include <functional>

class StyleSelector
{
public:
	StyleSelector();
	~StyleSelector() = default;

	void SetStyle(const std::string& id);

private:
	std::map<std::string, std::function<void()>> styles;

	void SetMain();
};

