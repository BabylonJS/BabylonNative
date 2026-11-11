#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>
#undef STB_TRUETYPE_IMPLEMENTATION

namespace
{
    constexpr stbtt_uint32 Tag(char a, char b, char c, char d)
    {
        return (static_cast<stbtt_uint32>(a) << 24) |
            (static_cast<stbtt_uint32>(b) << 16) |
            (static_cast<stbtt_uint32>(c) << 8) |
            static_cast<stbtt_uint32>(d);
    }

    stbtt_uint32 ReadTag(stbtt_uint8* value)
    {
        return (static_cast<stbtt_uint32>(value[0]) << 24) |
            (static_cast<stbtt_uint32>(value[1]) << 16) |
            (static_cast<stbtt_uint32>(value[2]) << 8) |
            static_cast<stbtt_uint32>(value[3]);
    }

    stbtt_uint32 ScriptTagForCodepoint(unsigned int codepoint)
    {
        if (codepoint <= 0x024f)
        {
            return Tag('l', 'a', 't', 'n');
        }
        if (codepoint >= 0x0370 && codepoint <= 0x03ff)
        {
            return Tag('g', 'r', 'e', 'k');
        }
        if (codepoint >= 0x0400 && codepoint <= 0x052f)
        {
            return Tag('c', 'y', 'r', 'l');
        }
        if (codepoint >= 0x0590 && codepoint <= 0x05ff)
        {
            return Tag('h', 'e', 'b', 'r');
        }
        return 0;
    }

    int GetPairAdjustment(stbtt_uint8* lookupList, int lookupIndex, int glyph1, int glyph2)
    {
        const auto lookupCount = ttUSHORT(lookupList);
        if (lookupIndex < 0 || lookupIndex >= lookupCount)
        {
            return 0;
        }

        auto* lookupTable = lookupList + ttUSHORT(lookupList + 2 + 2 * lookupIndex);
        if (ttUSHORT(lookupTable) != 2)
        {
            return 0;
        }

        const auto subTableCount = ttUSHORT(lookupTable + 4);
        auto* subTableOffsets = lookupTable + 6;
        for (int subTableIndex = 0; subTableIndex < subTableCount; ++subTableIndex)
        {
            auto* table = lookupTable + ttUSHORT(subTableOffsets + 2 * subTableIndex);
            const auto coverageIndex = stbtt__GetCoverageIndex(table + ttUSHORT(table + 2), glyph1);
            if (coverageIndex == -1)
            {
                continue;
            }

            const auto valueFormat1 = ttUSHORT(table + 4);
            const auto valueFormat2 = ttUSHORT(table + 6);
            if (valueFormat1 != 4 || valueFormat2 != 0)
            {
                continue;
            }

            switch (ttUSHORT(table))
            {
                case 1:
                {
                    const auto pairSetCount = ttUSHORT(table + 8);
                    if (coverageIndex >= pairSetCount)
                    {
                        continue;
                    }

                    auto* pairValueTable = table + ttUSHORT(table + 10 + 2 * coverageIndex);
                    const auto pairValueCount = ttUSHORT(pairValueTable);
                    auto* pairValueArray = pairValueTable + 2;
                    int left = 0;
                    int right = pairValueCount - 1;
                    while (left <= right)
                    {
                        const int middle = (left + right) >> 1;
                        auto* pairValue = pairValueArray + 4 * middle;
                        const int secondGlyph = ttUSHORT(pairValue);
                        if (glyph2 < secondGlyph)
                        {
                            right = middle - 1;
                        }
                        else if (glyph2 > secondGlyph)
                        {
                            left = middle + 1;
                        }
                        else
                        {
                            return ttSHORT(pairValue + 2);
                        }
                    }
                    break;
                }
                case 2:
                {
                    const int glyph1Class = stbtt__GetGlyphClass(table + ttUSHORT(table + 8), glyph1);
                    const int glyph2Class = stbtt__GetGlyphClass(table + ttUSHORT(table + 10), glyph2);
                    const auto class1Count = ttUSHORT(table + 12);
                    const auto class2Count = ttUSHORT(table + 14);
                    if (glyph1Class < 0 || glyph1Class >= class1Count || glyph2Class < 0 || glyph2Class >= class2Count)
                    {
                        continue;
                    }

                    auto* class1Records = table + 16;
                    auto* class2Records = class1Records + 2 * (glyph1Class * class2Count);
                    return ttSHORT(class2Records + 2 * glyph2Class);
                }
            }
        }

        return 0;
    }

    int GetScriptKernAdvance(const stbtt_fontinfo* font, int glyph1, int glyph2, stbtt_uint32 scriptTag)
    {
        if (!font->gpos || !scriptTag)
        {
            return stbtt_GetGlyphKernAdvance(font, glyph1, glyph2);
        }

        auto* gpos = font->data + font->gpos;
        if (ttUSHORT(gpos) != 1)
        {
            return stbtt_GetGlyphKernAdvance(font, glyph1, glyph2);
        }

        // stb_truetype scans every pair lookup and can apply one from the wrong OpenType script.
        auto* scriptList = gpos + ttUSHORT(gpos + 4);
        const auto scriptCount = ttUSHORT(scriptList);
        stbtt_uint8* script = nullptr;
        for (int scriptIndex = 0; scriptIndex < scriptCount; ++scriptIndex)
        {
            auto* scriptRecord = scriptList + 2 + 6 * scriptIndex;
            if (ReadTag(scriptRecord) == scriptTag)
            {
                script = scriptList + ttUSHORT(scriptRecord + 4);
                break;
            }
        }
        if (!script)
        {
            return stbtt_GetGlyphKernAdvance(font, glyph1, glyph2);
        }

        const auto defaultLangSysOffset = ttUSHORT(script);
        if (!defaultLangSysOffset)
        {
            return stbtt_GetGlyphKernAdvance(font, glyph1, glyph2);
        }

        auto* langSys = script + defaultLangSysOffset;
        auto* featureList = gpos + ttUSHORT(gpos + 6);
        const auto featureCount = ttUSHORT(featureList);
        auto* lookupList = gpos + ttUSHORT(gpos + 8);
        const auto langFeatureCount = ttUSHORT(langSys + 4);
        for (int langFeatureIndex = -1; langFeatureIndex < langFeatureCount; ++langFeatureIndex)
        {
            const int featureIndex = langFeatureIndex < 0 ? ttUSHORT(langSys + 2) : ttUSHORT(langSys + 6 + 2 * langFeatureIndex);
            if (featureIndex == 0xffff || featureIndex >= featureCount)
            {
                continue;
            }

            auto* featureRecord = featureList + 2 + 6 * featureIndex;
            if (ReadTag(featureRecord) != Tag('k', 'e', 'r', 'n'))
            {
                continue;
            }

            auto* feature = featureList + ttUSHORT(featureRecord + 4);
            const auto lookupIndexCount = ttUSHORT(feature + 2);
            for (int index = 0; index < lookupIndexCount; ++index)
            {
                const int adjustment = GetPairAdjustment(lookupList, ttUSHORT(feature + 4 + 2 * index), glyph1, glyph2);
                if (adjustment)
                {
                    return adjustment;
                }
            }
            return 0;
        }

        return 0;
    }
}

int fons__stb_getGlyphKernAdvanceForCodepoint(const stbtt_fontinfo* font, int glyph1, int glyph2, unsigned int codepoint)
{
    return GetScriptKernAdvance(font, glyph1, glyph2, ScriptTagForCodepoint(codepoint));
}
