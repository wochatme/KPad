#ifndef __base_registry_h__
#define __base_registry_h__

#include <windows.h>
#include <string>

#include "base/basic_types.h"

namespace base
{
    namespace win
    {
        class RegKey
        {
        public:
            RegKey();
            RegKey(HKEY rootkey, const wchar_t* subkey, REGSAM access);
            ~RegKey();

            LONG Create(HKEY rootkey, const wchar_t* subkey, REGSAM access);

            LONG CreateWithDisposition(HKEY rootkey, const wchar_t* subkey,
                DWORD* disposition, REGSAM access);

            LONG Open(HKEY rootkey, const wchar_t* subkey, REGSAM access);

            LONG CreateKey(const wchar_t* name, REGSAM access);

            LONG OpenKey(const wchar_t* name, REGSAM access);

            void Close();

            DWORD ValueCount() const;

            LONG ReadName(int index, std::wstring* name);

            bool Valid() const { return key_ != NULL; }

            LONG DeleteKey(const wchar_t* name);

            LONG DeleteValue(const wchar_t* name);

            bool ValueExists(const wchar_t* name);

            LONG ReadValue(const wchar_t* name, void* data, DWORD* dsize,
                DWORD* dtype) const;
            LONG ReadValue(const wchar_t* name, std::wstring* value) const;
            LONG ReadValueDW(const wchar_t* name, DWORD* value) const;
            LONG ReadInt64(const wchar_t* name, int64* value) const;

            LONG WriteValue(const wchar_t* name, const void* data, DWORD dsize,
                DWORD dtype);
            LONG WriteValue(const wchar_t* name, const wchar_t* value);
            LONG WriteValue(const wchar_t* name, DWORD value);

            LONG StartWatching();

            bool HasChanged();

            LONG StopWatching();

            inline bool IsWatching() const { return watch_event_ != 0; }
            HANDLE watch_event() const { return watch_event_; }
            HKEY Handle() const { return key_; }

        private:
            HKEY key_; 
            HANDLE watch_event_;

            DISALLOW_COPY_AND_ASSIGN(RegKey);
        };

        class RegistryValueIterator
        {
        public:
            RegistryValueIterator(HKEY root_key, const wchar_t* folder_key);
            ~RegistryValueIterator();

            DWORD ValueCount() const;

            bool Valid() const;

            void operator++();

            const wchar_t* Name() const { return name_; }
            const wchar_t* Value() const { return value_; }
            DWORD ValueSize() const { return value_size_; }
            DWORD Type() const { return type_; }

            int Index() const { return index_; }

        private:
            bool Read();

            HKEY key_;

            int index_;

            wchar_t name_[MAX_PATH];
            wchar_t value_[MAX_PATH];
            DWORD value_size_;
            DWORD type_;

            DISALLOW_COPY_AND_ASSIGN(RegistryValueIterator);
        };

        class RegistryKeyIterator
        {
        public:
            RegistryKeyIterator(HKEY root_key, const wchar_t* folder_key);
            ~RegistryKeyIterator();

            DWORD SubkeyCount() const;

            bool Valid() const;

            void operator++();

            const wchar_t* Name() const { return name_; }

            int Index() const { return index_; }

        private:
            bool Read();

            HKEY key_;

            int index_;

            wchar_t name_[MAX_PATH];

            DISALLOW_COPY_AND_ASSIGN(RegistryKeyIterator);
        };

    } //namespace win
} //namespace base

#endif //__base_registry_h__