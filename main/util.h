#pragma once

template< class T > struct remove_reference      {typedef T type;};
template< class T > struct remove_reference<T&>  {typedef T type;};
template< class T > struct remove_reference<T&&> {typedef T type;};

template< class T >
typename remove_reference<T>::type&& move( T&& t ) noexcept {
  return static_cast<typename remove_reference<T>::type &&>(t);
}

// FUNCTION TEMPLATE forward
template <class _Ty>
constexpr _Ty&& forward(
    remove_reference<_Ty>& _Arg) noexcept { // forward an lvalue as either an lvalue or an rvalue
    return static_cast<_Ty&&>(_Arg);
}

template <class _Ty>
constexpr _Ty&& forward(remove_reference<_Ty>&& _Arg) noexcept { // forward an rvalue as an rvalue
    return static_cast<_Ty&&>(_Arg);
}
