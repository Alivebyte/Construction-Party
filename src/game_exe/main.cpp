#include <stdio.h>

#define LIB_DIR "bin/"

#if defined(WIN32)
#include <Windows.h>
#define LIB_ENGINE "engine.dll"
#elif defined(_LINUX)
#define LIB_ENGINE "libengine.so"
#endif

#define LIB_ENGINE_PATH LIB_DIR LIB_ENGINE

int App_Start();
void App_LoadEngine();
void App_Shutdown();

static HINSTANCE hEngineDll;

static void Exe_Error(const char *szFmt, ...)
{
    static char buffer[16384];
    va_list args;
    va_start(args, szFmt);
    vsnprintf(buffer, sizeof(buffer), szFmt, args);
    va_end(args);

#ifdef WIN32
    MessageBoxA(NULL, buffer, "Launcher Error!", MB_OK);
#else
    fprintf(stderr, "Launch Error: %s", buffer);
#endif

    exit(1);
}


const char* GetErrorString()
{
#ifdef WIN32
    static char buf[1024];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, sizeof(buf), NULL);
    return buf;
#else
    return dlerror();
#endif 
}

int App_Start()
{
    App_LoadEngine();

    return 0;
}

void App_Shutdown()
{

}

void App_LoadEngine()
{
    hEngineDll = LoadLibrary(LIB_ENGINE_PATH);
    if (!hEngineDll)
        Exe_Error("Couldn't load engine.dll!\n%s", GetErrorString());

    
}

#ifdef _LINUX
int main()
{
    std::cout << "Hello world!" << std::endl;
}
#endif

#ifdef WIN32

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    int ret;

    ret = App_Start();
    
    return ret;
}




#endif