#ifndef __ui_base_clipboard_util_win_h__
#define __ui_base_clipboard_util_win_h__

#include <shlobj.h>

#include <string>
#include <vector>

namespace ui
{
    class ClipboardUtil
    {
    public:
        /////////////////////////////////////////////////////////////////////////////
        static FORMATETC* GetUrlFormat();
        static FORMATETC* GetUrlWFormat();
        static FORMATETC* GetMozUrlFormat();
        static FORMATETC* GetPlainTextFormat();
        static FORMATETC* GetPlainTextWFormat();
        static FORMATETC* GetFilenameFormat();
        static FORMATETC* GetFilenameWFormat();
        static FORMATETC* GetHtmlFormat();
        // Firefox text/html
        static FORMATETC* GetTextHtmlFormat();
        static FORMATETC* GetCFHDropFormat();
        static FORMATETC* GetFileDescriptorFormat();
        static FORMATETC* GetFileContentFormatZero();
        static FORMATETC* GetWebKitSmartPasteFormat();

        /////////////////////////////////////////////////////////////////////////////
        static bool HasUrl(IDataObject* data_object);
        static bool HasFilenames(IDataObject* data_object);
        static bool HasPlainText(IDataObject* data_object);
        static bool HasFileContents(IDataObject* data_object);
        static bool HasHtml(IDataObject* data_object);

        /////////////////////////////////////////////////////////////////////////////
        static bool GetUrl(IDataObject* data_object,
            std::wstring* url, std::wstring* title, bool convert_filenames);
        static bool GetFilenames(IDataObject* data_object,
            std::vector<std::wstring>* filenames);
        static bool GetPlainText(IDataObject* data_object, std::wstring* plain_text);
        static bool GetHtml(IDataObject* data_object, std::wstring* text_html,
            std::string* base_url);
        static bool GetFileContents(IDataObject* data_object,
            std::wstring* filename, std::string* file_contents);

        static std::string HtmlToCFHtml(const std::string& html,
            const std::string& base_url);
        static void CFHtmlToHtml(const std::string& cf_html, std::string* html,
            std::string* base_url);
    };

} //namespace ui

#endif //__ui_base_clipboard_util_win_h__
