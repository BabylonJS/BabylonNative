#include <bgfx_compute.sh>

BUFFER_RAW_RO(params, 0);
BUFFER_RAW_RO(source, 1);
BUFFER_RAW_WO(destination, 2);

NUM_THREADS(64, 1, 1)
void main()
{
    uint instance = gl_GlobalInvocationID.x;
    if (instance >= rawLoadUint(params, 0))
    {
        return;
    }

    uint attributeCount = rawLoadUint(params, 1);
    uint destinationStride = rawLoadUint(params, 2);
    for (uint attr = 0u; attr < attributeCount; ++attr)
    {
        uint base = 4u + attr * 4u;
        uint sourceOffset = rawLoadUint(params, base);
        uint sourceStride = rawLoadUint(params, base + 1u);
        uint wordCount = rawLoadUint(params, base + 2u);
        uint destinationOffset = rawLoadUint(params, base + 3u);
        for (uint word = 0u; word < wordCount; ++word)
        {
            rawStoreUint(destination, destinationOffset + instance * destinationStride + word,
                rawLoadUint(source, sourceOffset + instance * sourceStride + word));
        }
    }
}
