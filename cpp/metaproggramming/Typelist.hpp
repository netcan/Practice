/*************************************************************************
    > File Name: Typelist.hpp
    > Author: Netcan
    > Descripton: Typelist for meta programming
    > Blog: http://www.netcan666.com
    > Mail: 1469709759@qq.com
    > Created Time: 2020-07-26 21:10
************************************************************************/
#include <type_traits>
#include <cstddef>

template <typename ...Ts>
struct TypeList {
    using type = TypeList<Ts...>;
    static constexpr size_t size = 0;
    template <typename T>
    using appendTo = typename TypeList<T>::type;

    template <typename ...T>
    using extends = typename TypeList<T...>::type;

    template <typename T>
    using prepend = typename TypeList<T>::type;

    template <template<typename...> typename T>
    using exportTo = T<Ts...>;
};

template <typename Head, typename ...Tails>
struct TypeList<Head, Tails...> {
    using type = TypeList<Head, Tails...>;
    using head = Head;
    using tails = TypeList<Tails...>;
    static constexpr size_t size = sizeof...(Tails) + 1;

    template <typename T>
    using appendTo = typename TypeList<Head, Tails..., T>::type;

    template <typename ...Ts>
    using extends = typename TypeList<Head, Tails..., Ts...>::type;

    template <typename T>
    using prepend = typename TypeList<T, Head, Tails...>::type;

    template <template<typename...> typename T>
    using exportTo = T<Head, Tails...>;
};

static_assert(std::is_same_v<TypeList<int, char>::type,
        TypeList<int>::appendTo<char>::type>);

template<typename IN,
    template<typename> typename P,
    typename S = TypeList<>,
    typename R = TypeList<>,
    typename = void>
struct Split {
    struct type {
        using satisfied = S;
        using rest = R;
    };
};

template<typename IN, template<typename> typename P, typename S, typename R>
class Split<IN, P, S, R, std::enable_if_t<P<typename IN::head>::value>> {
    using satisfied = typename S::template appendTo<typename IN::head>::type;
public:
    using type = typename Split<typename IN::tails, P, satisfied, R>::type;
};

template<typename IN,
    template<typename> typename P,
    typename S,
    typename R>
class Split<IN, P, S, R, std::enable_if_t<!P<typename IN::head>::value> >  {
    using rest = typename R::template appendTo<typename IN::head>::type;
public:
    using type = typename Split<typename IN::tails, P, S, rest>::type;
};

// Replace once
template<typename IN, typename FROM, typename TO, typename OUT = TypeList<>, typename = void>
struct Replace {
    using type = OUT;
};

template<typename IN, typename OUT, typename TO>
class Replace<IN, typename IN::head, TO, OUT, std::void_t<typename IN::head>> {
    using R = typename OUT::template appendTo<TO>;
public:
    using type = typename IN::tails::template exportTo<R::template extends>::type;
};

template<typename IN, typename FROM, typename OUT, typename TO>
class Replace<IN, FROM, TO, OUT, std::void_t<typename IN::head>> {
    using C = typename OUT::template appendTo<typename IN::head>;
public:
    using type = typename Replace<typename IN::tails, FROM, TO, C>::type;
};
