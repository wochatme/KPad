#ifndef __ui_base_data_pack_h__
#define __ui_base_data_pack_h__

#include <map>

#include "base/basic_types.h"
#include "base/memory/scoped_ptr.h"

class FilePath;
class RefCountedStaticMemory;

namespace base
{
    class MemoryMappedFile;
    class StringPiece;
}

namespace ui
{

    class DataPack
    {
    public:
        DataPack();
        ~DataPack();

        bool Load(const FilePath& path);

        bool GetStringPiece(uint16 resource_id, base::StringPiece* data) const;

        RefCountedStaticMemory* GetStaticMemory(uint16 resource_id) const;

        static bool WritePack(const FilePath& path,
            const std::map<uint16, base::StringPiece>& resources);

    private:
        scoped_ptr<base::MemoryMappedFile> mmap_;

        size_t resource_count_;

        DISALLOW_COPY_AND_ASSIGN(DataPack);
    };

} //namespace ui

#endif //__ui_base_data_pack_h__
