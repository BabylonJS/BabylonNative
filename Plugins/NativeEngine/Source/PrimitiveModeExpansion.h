#pragma once

#include <gsl/gsl>

#include <cstdint>
#include <vector>

namespace Babylon
{
    namespace PrimitiveModeExpansion
    {
        enum class Mode : uint32_t
        {
            LineLoop = 5,
            TriangleFan = 8,
        };

        class IndexData final
        {
        public:
            IndexData(gsl::span<const uint8_t> bytes, bool index32);

            void Update(gsl::span<const uint8_t> bytes, uint32_t startIndex);
            void Clear();

            gsl::span<const uint8_t> Bytes() const;
            bool Is32Bit() const;
            uint64_t Revision() const;

        private:
            std::vector<uint8_t> m_bytes{};
            bool m_index32{};
            uint64_t m_revision{};
        };

        struct Result final
        {
            std::vector<uint8_t> Bytes{};
            bool Index32{};
            uint32_t IndexCount{};
        };

        Result ExpandIndexed(const IndexData& data, uint32_t firstIndex, uint32_t indexCount, Mode mode);
        Result ExpandUnindexed(uint32_t vertexCount, Mode mode);
    }
}
