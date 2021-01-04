#include "Tools/System/OSUtils.h"

#include <Windows.h>
#include <shellapi.h>
#include <ShlObj.h>
#include <SDL.h>
#include <string>

#include "Tools/System/Logger.h"


void OSUtils::ExecuteURL(const char* url)
{
	// TODO: set it for linux and mac
	int err = (int)ShellExecute(0, 0, url, 0, 0, SW_SHOW);
	if (err <= 32)
		CheckShellErrors(err);
}

void OSUtils::Open(const char* complete_path)
{
	int err = (int)ShellExecute(0, "open", complete_path, 0, 0, SW_SHOW);
	if (err <= 32)
		CheckShellErrors(err);
}

void OSUtils::OpenAndSelect(const char* complete_path)
{
	ITEMIDLIST* pidl = ILCreateFromPath(complete_path);
	if (pidl) {
		SHOpenFolderAndSelectItems(pidl, 0, 0, 0);
		ILFree(pidl);
	}
}

void OSUtils::SetClipboardText(const char* txt)
{
	if (SDL_SetClipboardText(txt) < 0) {
		LOGW("%s", SDL_GetError());
	}
}

void OSUtils::CheckShellErrors(int err)
{
	switch (err)
	{
	case 0:
		break;
	case (int)ERROR_FILE_NOT_FOUND:
		LOGE("ShellExecute error %i ERROR_FILE_NOT_FOUND", err);
		break;
	case (int)ERROR_PATH_NOT_FOUND:
		LOGE("ShellExecute error %i ERROR_PATH_NOT_FOUND", err);
		break;
	case (int)ERROR_BAD_FORMAT:
		LOGE("ShellExecute error %i ERROR_BAD_FORMAT", err);
		break;
	case SE_ERR_ACCESSDENIED:
		LOGE("ShellExecute error %i SE_ERR_ACCESSDENIED", err);
		break;
	case SE_ERR_ASSOCINCOMPLETE:
		LOGE("ShellExecute error %i SE_ERR_ASSOCINCOMPLETE", err);
		break;
	case SE_ERR_DDEBUSY:
		LOGE("ShellExecute error %i SE_ERR_DDEBUSY", err);
		break;
	case SE_ERR_DDEFAIL:
		LOGE("ShellExecute error %i SE_ERR_DDEFAIL", err);
		break;
	case SE_ERR_DDETIMEOUT:
		LOGE("ShellExecute error %i SE_ERR_DDETIMEOUT", err);
		break;
	case SE_ERR_DLLNOTFOUND:
		LOGE("ShellExecute error %i SE_ERR_DLLNOTFOUND", err);
		break;
	case SE_ERR_NOASSOC:
		LOGE("ShellExecute error %i SE_ERR_NOASSOC", err);
		break;
	case SE_ERR_OOM:
		LOGE("ShellExecute error %i SE_ERR_OOM", err);
		break;
	case SE_ERR_SHARE:
		LOGE("ShellExecute error %i SE_ERR_SHARE", err);
		break;
	default:
		LOGE("ShellExecute error %i", err);
		break;
	}
}
