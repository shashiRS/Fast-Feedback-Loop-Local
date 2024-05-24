#if defined(_WIN32)
#define DECLSPEC_ECO extern "C" __declspec(dllexport)
#else
#define DECLSPEC_ECO
#endif

DECLSPEC_ECO int nextDllMain(int, const char *[]) { return 0; }

DECLSPEC_ECO const char *nextDllName() { return "sicore"; }

int main(int argc, const char *argv[]) { return nextDllMain(argc, argv); }