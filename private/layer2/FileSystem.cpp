#include <layer2/FileSystem.h>
#include <layer2/layer2.h>
#include <filesystem>
#include <array>

#include "File.h"

#define MAX_SEARCHPATHS 20

struct SSearchpath_t
{
    fs::path m_basePath; // The root path, i.e. C:\Users\Oblivion
    fs::path m_gameDir; // The base dir, i.e. saves, base, mods, etc...
};

class CFileSystem : public CLayer2AppSystem<IFileSystem>
{
public:
    virtual bool Init();
    virtual void Shutdown();
    virtual IFile* OpenFileRead( const char* pName );
    virtual IFile* OpenFileWrite( const char* pName ) { return nullptr; }
    virtual void CloseFile( IFile* pFile );
    virtual bool ListDirectory(const char* dir, std::vector<std::string>& list, const char* ext = nullptr);
private:
    void AddGameDirectory( const char* pDirName );
private:
    void PrintSearchPaths();
    static void PrintSearchpaths( CmdArgs_t& );

    std::array<SSearchpath_t, MAX_SEARCHPATHS> m_Searchpaths;
    int m_NumSearchpaths;
    // directory names of various search paths
    const ICVar* m_pBasepath;
    const ICVar* m_pSavepath;
    const ICVar* m_pGamepath; // Optional
    const ICVar* m_pFsDebug;

#define FS_DEBUG(x, ...) if (m_pFsDebug->GetBool()) do { printf("FS_DEBUG: "); printf(x, ##__VA_ARGS__); } while (0)
};

CFileSystem g_FileSystem;
EXPOSE_INTERFACE_GLOBALVAR(CFileSystem, IFileSystem, g_FileSystem, FILESYSTEM_INTERFACE_NAME);

bool CFileSystem::Init()
{
    m_NumSearchpaths = 0;

    printf("----- FileSystem Init -----\n");
    m_pBasepath = GetCvarSystem()->GetVar( "fs_basepath", "base" );
    m_pSavepath = GetCvarSystem()->GetVar( "fs_savepath", "saves" );
    m_pGamepath = GetCvarSystem()->GetVar( "fs_gamepath", "" );
    m_pFsDebug = GetCvarSystem()->GetVar( "fs_debug", "0" );

    if (m_pGamepath->GetString()[0] != 0)
    {
        AddGameDirectory( m_pGamepath->GetString() );
    }

    // Add basepath, then savepath
    AddGameDirectory( m_pBasepath->GetString() );
    AddGameDirectory( m_pSavepath->GetString() );

    GetCommandSystem()->AddCommand("fs_printsp", CFileSystem::PrintSearchpaths);

    PrintSearchPaths();

    // Sanity test
    IFile* pFile = OpenFileRead( "default.cfg" );
    if (!pFile)
    {
        printf( "###############################################\n"
                "#        ERROR: no default.cfg found.         #\n"
                "#  Make sure the game is properly installed.  #\n"
                "#     If it is, try restarting the game.      #\n"
                "###############################################\n"
        );
        return false;
    }
    CloseFile( pFile );

    printf("---------------------------\n");

    return true;
}

void CFileSystem::Shutdown()
{
    m_NumSearchpaths = 0;
}

IFile *CFileSystem::OpenFileRead(const char *pName)
{
    SSearchpath_t* pSearchpath = nullptr;
    for (int i = 0; i < m_NumSearchpaths; i++)
    {
        auto& sp = m_Searchpaths[i];
        auto path = sp.m_basePath / sp.m_gameDir;
        FS_DEBUG("Checking for \"%s\" in \"%s\"\n", pName, path.string().c_str());

        if (fs::exists(path / pName))
        {
            pSearchpath = &sp;
            break;
        }
    }

    if (!pSearchpath)
    {
        FS_DEBUG("Failed to find file \"%s\"\n", pName);
        return nullptr;
    }

    // The .string() is necessary because Windows uses wchar_t by default for paths
    return new CRegularFile((pSearchpath->m_basePath / pSearchpath->m_gameDir / pName).string().c_str(), true);
}

void CFileSystem::CloseFile(IFile *pFile)
{
    delete pFile;
}

bool CFileSystem::ListDirectory(const char *dir, std::vector<std::string> &list, const char *ext)
{
    list.reserve(1024); // Try not to allocate during this function

    // For now we assume base path is fs_basepath
    // TODO: don't
    fs::path base = m_pBasepath->GetString();
    base /= dir;

    FS_DEBUG("Listing all files in \"%s\"\n", base.string().c_str());

    for (const auto& entry : fs::directory_iterator(base))
    {
        if (ext && entry.is_regular_file() && entry.path().extension() == ext)
        {
            list.push_back(entry.path().string().c_str());
        }
        else if (!ext)
            list.push_back(entry.path().string().c_str());
    }

    return !list.empty();
}

void CFileSystem::AddGameDirectory(const char *pDirName)
{
    // Sanitize the filename
    while (*pDirName == '/' || *pDirName == '\\')
        pDirName++; // Skip leading slashes, we only support relative paths

    FS_DEBUG("Adding %s to searchpath\n", pDirName);

    // This should get initialized the first time this function is called
    // Get the path of the executable, minus the game exe on the end
    // i.e. C:\Program Files (x86)\Oblivion
    static fs::path basePath = fs::current_path();

    m_Searchpaths[m_NumSearchpaths].m_basePath = basePath;
    m_Searchpaths[m_NumSearchpaths++].m_gameDir = pDirName;

    // Enumerate directory looking for .res files
    // TODO: actually support .res files
}

void CFileSystem::PrintSearchPaths()
{
    printf("Current searchpath:\n");
    for (int i = 0; i < m_NumSearchpaths; i++)
    {
        auto& sp = m_Searchpaths[i];
        printf("\t%s\n", (sp.m_basePath / sp.m_gameDir).string().c_str());
    }
    printf("%i search paths total\n", m_NumSearchpaths);
}

void CFileSystem::PrintSearchpaths(CmdArgs_t &)
{
    g_FileSystem.PrintSearchPaths();
}
