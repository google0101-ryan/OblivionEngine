#include <engine/main/EngineApp.h>

int main(int argc, char** argv)
{
    CEngineApp app;
    app.SetArgs(argc, argv);
    app.Run();
    return 0;
}