/* Copyright (C) 2013-2015 Zeex
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SAMPGDK_INTEROP_H
#define SAMPGDK_INTEROP_H

#include <stdarg.h>

#include <sampgdk/bool.h>
#include <sampgdk/export.h>

/**
 * \addtogroup interop
 * @{
 */

/**
 * \brief Returns all currently registered native functions
 *
 * This function can be used to get the names and addresses of all native
 * functions that have been registered with amx_Register(), by both the
 * server and plugins.
 *
 * \note The returned array is NULL-terminated.
 *
 * \param number where to store the number of natives (optional).
 *
 * \returns pointer to array of registered native functions
 *
 * \see sampgdk_FindNative()
 * \see sampgdk_CallNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(const AMX_NATIVE_INFO *, sampgdk_GetNatives(int *number));

/**
 * \brief Finds a native function by name
 *
 * Searches for a native function with the specified name and returns its
 * address. In order to be found the function must be registered with
 * amx_Register() prior to the call.
 *
 * \param name name of the native function
 *
 * \returns function's address or \c NULL if not found
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_CallNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(AMX_NATIVE, sampgdk_FindNative(const char *name));

/**
 * \brief Calls a native function
 *
 * This function is suitable for calling simple natives that either have only
 * value parameters or don't have any parameters at all. If you have to pass
 * a reference or a string use sampgdk_InvokeNative() instead.
 *
 * \note The first element of \p params must contain the number of arguments
 * multiplied by \c sizeof(cell).
 *
 * \param native pointer to the native function
 * \param params parameters to be passed to the function as its second argument
 *
 * \returns function's return value
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_FindNative()
 * \see sampgdk_InvokeNative()
 */
SAMPGDK_API(cell, sampgdk_CallNative(AMX_NATIVE native, cell *params));

/**
 * \brief Calls a native function with arguments
 *
 * Argument types are specified via \p format where each character, or
 * *specifier*, corresponds to a single argument. The following format
 * specifiers are supported:
 *
 * Specifier | C/C++ type    | Description
 * :-------- | :------------ | :------------------------------------------
 * i         | int           | integer value
 * d         | int           | integer value (same as 'i')
 * b         | bool          | boolean value
 * f         | double        | floating-point value
 * r         | const cell *  | const reference (input only)
 * R         | cell *        | non-const reference (both input and output)
 * s         | const char *  | const string (input only)
 * S         | char *        | non-const string (both input and output)
 * a         | const cell *  | const array (input only)
 * A         | cell *        | non-const array (both input and output)
 *
 * \remarks For the 'S', 'a' and 'A' specifiers you have to specify the size
 * of the string/array in square brackets, e.g. "a[100]" (fixed size)
 * or s[*2] (size passed via 2nd argument).
 *
 * \note In Pawn variadic functions always take their variable arguments
 * (those represented by "...") by reference. This means that for such
 * functions you have to use the 'r' specifier where you would normally
 * use 'b', 'i' 'd' or 'f'.
 *
 * \param native pointer to the native function.
 * \param format argument types
 * \param ... arguments themselves
 *
 * \returns function's return value
 *
 * \see sampgdk_GetNatives()
 * \see sampgdk_FindNative()
 * \see sampgdk_InvokeNativeV()
 * \see sampgdk_InvokeNativeArray()
 */
SAMPGDK_API(cell, sampgdk_InvokeNative(AMX_NATIVE native,
    const char *format, ...));

/**
* \brief Calls a native function with arguments
*
* This function is identical to sampgdk_InvokeNative() except it takes
* \c va_list instead of variable arguments.
*
* \see sampgdk_GetNatives()
* \see sampgdk_FindNative()
* \see sampgdk_InvokeNative()
* \see sampgdk_InvokeNativeArray()
*/
SAMPGDK_API(cell, sampgdk_InvokeNativeV(AMX_NATIVE native,
    const char *format, va_list args));

/**
* \brief Calls a native function with an array of arguments
*
* This function is similar to sampgdk_InvokeNative() but the arguments
* are passed as an array where each element is a pointer pointing to
* the actual value.
*
* Argument types are specified via \p format where each character, or
* *specifier*, corresponds to a single argument. See sampgdk_InvokeNative()
* for the list of supported format specifiers.
*
* \param native pointer to the native function.
* \param format argument types
* \param args arguments themselves
*
* \returns function's return value
*
* \see sampgdk_GetNatives()
* \see sampgdk_FindNative()
* \see sampgdk_InvokeNative()
*/
SAMPGDK_API(cell, sampgdk_InvokeNativeArray(AMX_NATIVE native,
    const char *format, void **args));

/**
 * \brief A generic catch-all callback that gets called whenever some
 * AMX public function is executed
 *
 * This is the publics "filter" callback. It is called whenever the
 * server calls \c amx_Exec(), which practically means that you can
 * use it to hook *any* callback, even those that are called by other
 * plugins.
 *
 * \param amx AMX on which the function is called
 * \param name function name
 * \param params function arguments as stored on the AMX stack, with
 *        \c params[0] being set to the number of arguments multiplied
 *        by \c sizeof(cell)
 * \param retval where to store the return value (can be \c NULL)
 *
 * \returns \c true if the public is allowed to execute
 */
SAMPGDK_CALLBACK(bool, OnPublicCall(AMX *amx, const char *name,
    cell *params, cell *retval));

/**
 * \brief A generic catch-all callback that gets called whenever some
 * AMX public function is executed
 *
 * This callback is similar to \c OnPublicCall but also allows you to
 * stop the call from being propagated to other plugins or the gamemode
 * by setting the \c stop parameter to \c true.
 *
 * \param amx AMX on which the function is called
 * \param name function name
 * \param params function arguments as stored on the AMX stack, with
 *        \c params[0] being set to the number of arguments multiplied
 *        by \c sizeof(cell)
 * \param retval where to store the return value (can be \c NULL)
 * \param stop whether to stop public call propagation (\c false by default)
 *
 * \returns \c true if the public is allowed to execute
 */
SAMPGDK_CALLBACK(bool, OnPublicCall2(AMX *amx, const char *name,
    cell *params, cell *retval, bool *stop));

/** @} */

#ifdef __cplusplus

namespace sampgdk {

/**
  * \addtogroup interop
  * @{
  */

/// \brief C++ wrapper around sampgdk_GetNatives()
inline const AMX_NATIVE_INFO *GetNatives(int &number) {
  return sampgdk_GetNatives(&number);
}

/// \brief C++ wrapper around sampgdk_GetNatives()
inline const AMX_NATIVE_INFO *GetNatives() {
  return sampgdk_GetNatives(0);
}

/// \brief C++ wrapper around sampgdk_FindNative()
inline AMX_NATIVE FindNative(const char *name) {
  return sampgdk_FindNative(name);
}

/// \brief C++ wrapper around sampgdk_CallNative()
inline cell CallNative(AMX_NATIVE native, cell *params) {
  return sampgdk_CallNative(native, params);
}

/// \brief C++ wrapper around sampgdk_InvokeNative()
inline cell InvokeNative(AMX_NATIVE native, const char *format, ...) {
  va_list args;
  va_start(args, format);
  cell retval = sampgdk_InvokeNativeV(native, format, args);
  va_end(args);
  return retval;
}

/// \brief C++ wrapper around sampgdk_InvokeNativeV()
inline cell InvokeNativeV(AMX_NATIVE native, const char *format,
    va_list args) {
  return sampgdk_InvokeNativeV(native, format, args);
}

/// \brief C++ wrapper around sampgdk_InvokeNativeArray()
inline cell InvokeNativeArray(AMX_NATIVE native, const char *format,
    void **args) {
  return sampgdk_InvokeNativeArray(native, format, args);
}

/** @} */

} // namespace sampgdk

#endif /* __cplusplus */

#endif /* !SAMPGDK_INTEROP_H */
