#include "FileSystem.h"

#include <fstream>
#include <filesystem>

#include "Logger.h"

#include "ExternalTools/mmgr/mmgr.h"

namespace fs = std::filesystem;

FileSystem::FileSystem() 
{
}

FileSystem::~FileSystem()
{
}

nlohmann::json FileSystem::OpenJSONFile(const char* path)
{
	std::ifstream f(path);
	if (f.good()) {
		nlohmann::json j;
		f >> j;
		f.close();
		return j;
	}
    LOGE("Cannot open json with path %s", path);

	return nlohmann::json();
}

void FileSystem::SaveJSONFile(const char* path, const nlohmann::json& file)
{
    std::ofstream o(path);
    o << std::setw(4) << file << std::endl;
}

std::string FileSystem::OpenTextFile(const char* path)
{
    try {
        std::ifstream in(path, std::ios::in | std::ios::binary);
        if (in)
        {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        throw path;
    }
    catch (...) {
        LOGE("Cannot open file with path %s", path);
        throw path;
    }
}

bool FileSystem::Exists(const char* path)
{
    return fs::exists(path);
}

uint64_t FileSystem::LastTimeWrite(const char* path)
{
    struct stat time;
    if (stat(path, &time) == 0) {
        return time.st_mtime;
    }
    return 0ULL;
}

bool FileSystem::CreateFolder(const char* path)
{
    return fs::create_directory(path);
}

bool FileSystem::DeleteFile(const char* path)
{
    if (fs::remove(path) != 0) {
        LOGW("Could not delete %s", path);
        return false;
    }
    
    return true;
}

bool FileSystem::CopyTo(const char* source, const char* dst)
{
    std::error_code err;
    fs::copy(source, dst, err);
    if (err.value() != 0)
        LOGE("Copying from %s to %s failed: %s", source, dst, err.message().c_str());
    return err.value() == 0;
}

bool FileSystem::IsFileInFolderRecursive(const char* file, const char* folder)
{
    Folder parent = GetFilesRecursive(folder);

    return IsFileInFolder(file, parent);
}

bool FileSystem::IsFileInFolder(const char* file, const Folder& folder)
{
    for (auto i = folder.files.begin(); i != folder.files.end(); ++i) {
        if ((*i).compare(file) == 0)
            return true;
    }
    
    for (auto i = folder.folders.begin(); i != folder.folders.end(); ++i) {
        if (IsFileInFolder(file, *i))
            return true;
    }

    return false;
}

Folder FileSystem::GetFilesRecursive(const char* path)
{
    Folder parent(path);
    
    GetFiles(parent);
    
    return parent;
}

std::string FileSystem::GetFileExtension(const char* file, bool with_dot)
{
    std::string f(file);
    std::string ext;

    for (auto i = f.rbegin(); i != f.rend(); ++i) {
        if (*i != '.')
            ext.push_back(std::tolower(*i));
        else {
            if (with_dot)
                ext.push_back(std::tolower(*i));
            break;
        }
    }

    std::reverse(ext.begin(), ext.end());
    return ext;
}

std::string FileSystem::GetNameFile(const char* file, bool with_extension)
{
    std::string f(file);
    std::string name;

    for (auto i = f.rbegin(); i != f.rend(); ++i) {
        if (*i != '/' && *i != '//' && *i != '\\')
            name.push_back(*i);
        else
            break;
    }

    if (!with_extension) {
        if (std::find(name.begin(), name.end(), '.') != name.end()) {
            auto i = name.begin();
            int size = 0;
            while (i != name.end()) {
                if (*i == '.') {
                    ++size;
                    break;
                }
                else {
                    ++size;
                    ++i;
                }
            }
            name.erase(name.begin(), name.begin() + size);
        }
        else
            LOGW("File %s has not extension", name.c_str());
    }

    std::reverse(name.begin(), name.end());
    return name;
}

void FileSystem::GetFiles(Folder& parent) {
    for (const auto& entry : fs::directory_iterator(parent.full_path)) {
        if (entry.is_directory()) {
            Folder f((entry.path().u8string() + '/').c_str());
            GetFiles(f);
            parent.folders.push_back(f);
        }
        else {
            parent.files.push_back(entry.path().filename().string());
        }
    }
}

Folder::Folder(const char* n) :full_path(n)
{
    for (auto i = full_path.rbegin(); i != full_path.rend(); ++i) {
        if (i != full_path.rbegin()) {
            if (*i == '/' || *i == '\\' || *i == '//')
                break;
            else
                name.push_back(*i);
        }
    }
    std::reverse(name.begin(), name.end());
}
