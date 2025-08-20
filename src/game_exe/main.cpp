
#ifdef POSIX
int main()
{
    std::cout << "Hello world!" << std::endl;
}
#endif

#ifdef WIN32
#include "Windows.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    
    return EXIT_SUCCESS;
}


#endif