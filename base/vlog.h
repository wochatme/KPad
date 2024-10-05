#ifndef __base_vlog_h__
#define __base_vlog_h__

#include <vector>
#include "basic_types.h"
#include "string_piece.h"

namespace base
{
    class VlogInfo
    {
    public:
        static const int kDefaultVlogLevel;

        VlogInfo(const std::string& v_switch,
            const std::string& vmodule_switch,
            int* min_log_level);
        ~VlogInfo();

        int GetVlogLevel(const base::StringPiece& file);

    private:
        void SetMaxVlogLevel(int level);
        int GetMaxVlogLevel() const;

        struct VmodulePattern;
        std::vector<VmodulePattern> vmodule_levels_;
        int* min_log_level_;

        DISALLOW_COPY_AND_ASSIGN(VlogInfo);
    };

    bool MatchVlogPattern(const base::StringPiece& string,
        const base::StringPiece& vlog_pattern);

} //namespace base

#endif //__base_vlog_h__