#include "Application.h"

int main(const int argc, char *argv[]) {
    const Application application(argc, argv);
    return application.Run();
}
