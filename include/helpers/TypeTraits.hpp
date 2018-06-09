#ifndef ENGINE_TYPETRAITS_H
#define ENGINE_TYPETRAITS_H
#include "helpers/Storage.hpp"

namespace helpers::type_traits
{
    template <class T>
    struct has_const_iterator
    {
        template <class U>
        static auto has_ci(U const&) -> decltype(typename U::const_iterator{}, std::true_type{});
        static std::false_type has_ci(...);
        static constexpr bool value = decltype(has_ci(T()))::value;
    };
    template<class T>
    inline constexpr bool has_const_iterator_v = has_const_iterator<T>::value;

    template <class T>
    struct storage_value_index
    {
        template <class U>
        static typename U::const_iterator choose_type(U const&);
        static std::nullptr_t choose_type(...);
        using type = decltype(choose_type(T()));
    };
    template <class T>
    using storage_value_index_t = typename storage_value_index<T>::type;

    template <class T>
    struct get_deleter
    {
        using type = std::default_delete<T>;
    };
    template <>
    struct get_deleter<helpers::storage::ProxyStorage>
    {
        using type = helpers::storage::ProxyStorageDeleter;
    };
    template <class T>
    using get_deleter_t = typename get_deleter<T>::type;
}

#endif //ENGINE_TYPETRAITS_H
