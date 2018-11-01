#ifndef SHEAP_HPP
#define SHEAP_HPP

#include <utility>
#include <new>
#include "sheap.h"

#ifdef __cplusplus

template <typename T>
class secure_pointer;


template <typename T>
class unlocked_pointer {

    friend secure_pointer<T>;

  public:

    ~unlocked_pointer<T>() {
        if (__ptr) {
            sh_lock(__ptr);
        }
    }

    unlocked_pointer<T>(const unlocked_pointer<T>&) = delete;

    unlocked_pointer<T>(unlocked_pointer<T>&& other) {
        __ptr = other.__ptr;
        other.__ptr = nullptr;
    }

    unlocked_pointer<T>& operator=(const unlocked_pointer<T>&) = delete;

    unlocked_pointer<T>& operator=(unlocked_pointer<T>&& other) {
        return *this;
    }

    T* operator->() {
        return __ptr;
    }

    void lock() {
        sh_lock(__ptr);
        __ptr = nullptr;
    }

  private:

    explicit unlocked_pointer<T>(T* w) {
        __ptr = w;
    }

    T* __ptr;
};


template <typename T>
class secure_pointer {

  public:

    secure_pointer<T>() {
        __ptr = nullptr;
    }

    explicit secure_pointer<T>(const T* p) {
        sh_validate(p);
        __ptr = p;
    }

    ~secure_pointer<T>() {
        shfree(__ptr);
    };

    secure_pointer<T>(const secure_pointer<T>&) = delete;

    secure_pointer<T>(secure_pointer<T>&& other) {
        __ptr = other.__ptr;
        other.__ptr = nullptr;
    }

    secure_pointer<T>& operator=(const secure_pointer<T>&) = delete;

    secure_pointer<T>& operator=(secure_pointer<T>&& other) {
        return *this;
    }

    const T* operator->() {
        sh_validate(__ptr);
        return __ptr;
    }

    unlocked_pointer<T> unlock() {
        T* ptr_w = reinterpret_cast<T*>(sh_unlock(__ptr));
        return unlocked_pointer<T>(ptr_w);
    }


  private:

    const T* __ptr;
};


template <typename T, typename... Arg>
secure_pointer<T> make_secure(Arg&&... args) {
    const void* r = shalloc(sizeof(T));
    void* w = sh_unlock(r);
    new (w) T(std::forward<Arg>(args)...);
    sh_lock(w);
    return secure_pointer<T>(reinterpret_cast<const T*>(r));
};


#endif /* __cplusplus */

#endif //SHEAP_HPP
