#pragma once

template <class T> using Vector     = std::vector<T>;
template <class T> using VectorRef  = std::vector<T>&;
template <class T> using CVector    = const std::vector<T>;
template <class T> using CVectorRef = const std::vector<T>&;

template <class T> using SharedPtr        = std::shared_ptr<T>;
template <class T> using SharedPtrRef     = std::shared_ptr<T>&;
template <class T> using CSharedPtr       = const std::shared_ptr<T>;
template <class T> using CSharedPtrRef    = const std::shared_ptr<T>&;

template <class T> using VecSharedPtr     = std::vector<std::shared_ptr<T>>;
template <class T> using VecSharedPtrRef  = std::vector<std::shared_ptr<T>>&;
template <class T> using CVecSharedPtr    = const std::vector<std::shared_ptr<T>>;
template <class T> using CVecSharedPtrRef = const std::vector<std::shared_ptr<T>>&;

class JDM_DLL HigherObject;
using SharedHigherObject  = std::shared_ptr<HigherObject>;
using CSharedHigherObject = const SharedHigherObject;

