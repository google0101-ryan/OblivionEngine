#include <Framework/Filesystem/FileSystem.h>
#include <Framework/Filesystem/InternalFile.h>
#include <Framework/Cvar/Cvar.h>
#include <Framework/Platform/Platform.h>

#include <string>
#include <vector>

#define MODULE "FS"
#include <Util/Logger.h>

struct FS_SearchPath
{
    std::string path; // absolute prefix, i.e. C:\gamedir
    std::string dir; // path relative to the prefix, i.e. base
};

class FileSystem : public IFileSystem
{
public:
    virtual void Initialize();
    virtual IFile* OpenFileRead(const char* name);

    void AddGameDirectory(std::string dir, std::string gameName);
private:
    std::vector<FS_SearchPath> m_searchPaths;
};

FileSystem localFilesystem;
IFileSystem* g_pFilesystem = &localFilesystem;

CVar fs_basepath("fs_basepath", "", CVAR_FLAG_DEFAULT);
CVar fs_savepath("fs_savepath", "", CVAR_FLAG_DEFAULT);
CVar fs_debug("fs_debug", "0", CVAR_FLAG_DEFAULT);
// Allows the user to add extra resource paths
// Especially useful for modding
CVar fs_game("fs_game", "", CVAR_FLAG_DEFAULT);

#define FS_DEBUG(fmt, ...) \
    do { if (fs_debug.GetBool()) Logger::Log(LogLevel::DEBUG, fmt, ##__VA_ARGS__); } while (0)

void FileSystem::Initialize()
{
    Logger::Log(LogLevel::INFO, "------- FS INIT -------");

    if (!fs_basepath.Get()[0])
    {
        std::string defaultBasepath = Sys_GetBasePath();
        FS_DEBUG("Falling back to default basepath '%s'", defaultBasepath.c_str());
        fs_basepath.Set(defaultBasepath);
    }
    if (!fs_savepath.Get()[0])
    {
        std::string defaultSavePath = Sys_DefaultSavePath();
        FS_DEBUG("Falling back to default savepath");
        fs_savepath.Set(defaultSavePath);
    }

    AddGameDirectory(fs_basepath.Get(), GAMEDIR_NAME);
    AddGameDirectory(fs_savepath.Get(), GAMEDIR_NAME);

    if (fs_game.Get()[0])
    {
        auto str = fs_game.Get();
        if (str[str.length()-1] != *PATH_SEPERATOR)
            str.push_back(*PATH_SEPERATOR);
        fs_game.Set(str);
        AddGameDirectory(fs_game.Get(), GAMEDIR_NAME);
    }

    // Print all the search paths
    for (auto& sp : m_searchPaths)
    {
        Logger::Log(LogLevel::INFO, "Searchpath: %s", (sp.path + sp.dir).c_str());
    }
    Logger::Log(LogLevel::INFO, "---- FS INIT DONE ----");

    // Sanity check
    IFile* file = OpenFileRead("default.cfg");
    if (!file)
    {
        Logger::Log(LogLevel::FATAL, "default.cfg missing! Make sure all game components are present and installed!");
    }
    CloseFile(file);
}

std::string BuildOSPath(std::string path, std::string gameDir, std::string relativePath)
{
    if (path[path.length()-1] != '/' && gameDir[0] != '/')
        path.push_back('/');
    path += gameDir;
    if (gameDir[gameDir.length()-1] != '/' && relativePath[0] != '/')
        path.push_back('/');
    return path + relativePath;
}

IFile *FileSystem::OpenFileRead(const char *name)
{
    // TODO: Support for copying smaller files into memory

    // Find which searchpath the file exists in
    FS_SearchPath* chosen = nullptr;
    for (auto& sp : m_searchPaths)
    {
        auto path = BuildOSPath(sp.path, sp.dir, name);

        FS_DEBUG("Attempting to open '%s'", path.c_str());
        
        // Check if file exists
        FILE* f = fopen(path.c_str(), "r");
        if (!f)
        {
            continue;
        }
        fclose(f);
        chosen = &sp;
        break;
    }

    if (!chosen)
    {
        FS_DEBUG("%s: No such file or directory", name);
        return nullptr;
    }

    return new RegularFile(BuildOSPath(chosen->path, chosen->dir, name).c_str(), FILE_MODE_READ);
}

void FileSystem::AddGameDirectory(std::string path, std::string dir)
{
    FS_SearchPath sp;
    sp.path = path;
    sp.dir = dir;
    m_searchPaths.push_back(sp);
}

void IFileSystem::CloseFile(IFile *file)
{
    file->Close();
    delete file;
}
