#pragma once

namespace cpp
{
// https://stackoverflow.com/questions/7510182/how-does-stdmove-transfer-values-into-rvalues

/// @brief Convert object to rvalue
template<typename T>
T&& move(T& arg)
{
  return static_cast<T&&>(arg);
}

/// @brief Convert object to rvalue
template<typename T>
T&& move(T&& arg)
{
  return static_cast<T&&>(arg);
}

};