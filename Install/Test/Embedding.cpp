#include <Babylon/Embedding/Runtime.h>

int main()
{
    Babylon::Embedding::Runtime runtime{};
    return runtime.IsSuspended() ? 1 : 0;
}
