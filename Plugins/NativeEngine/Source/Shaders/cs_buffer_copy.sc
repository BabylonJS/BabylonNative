#include <bgfx_compute.sh>

BUFFER_RAW_RO(source, 0);
BUFFER_RAW_WO(destination, 1);

uniform vec4 u_copyCount;

NUM_THREADS(64, 1, 1)
void main()
{
    uint word = gl_GlobalInvocationID.x;
    if (word >= uint(u_copyCount.x))
    {
        return;
    }

    rawStoreUint(destination, word, rawLoadUint(source, word));
}
