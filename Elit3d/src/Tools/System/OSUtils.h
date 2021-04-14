#pragma once
class OSUtils
{
public:
	static void ExecuteURL(const char* url);
	static void Open(const char* complete_path);
	static void OpenAndSelect(const char* complete_path);
	static void SetClipboardText(const char* txt);

private:
	static void CheckShellErrors(int err);
};

