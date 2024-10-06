#ifndef __ui_gfx_font_h__
#define __ui_gfx_font_h__

#include "base/memory/ref_counted.h"
#include "base/string16.h"

namespace gfx
{

    class PlatformFont;

    class Font
    {
    public:
        enum FontStyle
        {
            NORMAL = 0,
            BOLD = 1,
            ITALIC = 2,
            UNDERLINED = 4,
        };

        Font();

        Font(const Font& other);
        Font& operator=(const Font& other);

        explicit Font(HFONT native_font);

        explicit Font(PlatformFont* platform_font);

        Font(const string16& font_name, int font_size);

        ~Font();

        Font DeriveFont(int size_delta) const;

        Font DeriveFont(int size_delta, int style) const;

        int GetHeight() const;

        int GetBaseline() const;

        int GetAverageCharacterWidth() const;

        int GetStringWidth(const string16& text) const;

        int GetExpectedTextWidth(int length) const;

        int GetStyle() const;

        string16 GetFontName() const;

        int GetFontSize() const;

        HFONT GetNativeFont() const;

        PlatformFont* platform_font() const { return platform_font_.get(); }

    private:
        scoped_refptr<PlatformFont> platform_font_;
    };

} //namespace gfx

#endif //__ui_gfx_font_h__