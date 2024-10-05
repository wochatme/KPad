#ifndef __base_environment_h__
#define __base_environment_h__

#include <string>

namespace base
{
    class Environment
    {
    public:
        virtual ~Environment();

        static Environment* Create();

        virtual bool GetVar(const char* variable_name, std::string* result) = 0;

        virtual bool HasVar(const char* variable_name);

        virtual bool SetVar(const char* variable_name,
            const std::string& new_value) = 0;

        virtual bool UnSetVar(const char* variable_name) = 0;
    };

} //namespace base

#endif //__base_environment_h__