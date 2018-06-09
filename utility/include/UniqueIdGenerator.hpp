#ifndef UNIQUEIDGENERATOR_HPP
#define UNIQUEIDGENERATOR_HPP

#include <memory>
#include <cassert>
#include "Log.h"

namespace utils
{
    template<class T>
    class UniqueIdGenerator
    {
    public:
        using unique_id_type = T;
        static unique_id_type generate()
        {
            auto instance = get_instance();
            return generate_impl(&instance->_data);
        }
        static void free(const unique_id_type &id)
        {
            auto instance = get_instance();
            free_impl(id, &instance->_data);
        }
    private:
        UniqueIdGenerator() = default;
        // NOTE: we won't allow to call that without right instantiation
        static unique_id_type generate_impl(void **data);
//        { assert(false); }
        static void free_impl(const unique_id_type &id, void **data);
//        { assert(false); }
    protected:
        static UniqueIdGenerator *get_instance()
        {
            if (!_instance)
            {
                LOG_D("UniqueIdGenerator created.")
                _instance.reset(new UniqueIdGenerator);
            }
            return _instance.get();
        }
        static std::unique_ptr <UniqueIdGenerator> _instance;
        static void *_data;
    };
}

#endif //UNIQUEIDGENERATOR_HPP
