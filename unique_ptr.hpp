#ifndef _UNIQUE_PTR_HPP_
#define _UNIQUE_PTR_HPP_

#include <memory>

#if __cplusplus >= 201402L
using std::make_unique;
#else
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args){
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

#endif //_UNIQUE_PTR_HPP_

