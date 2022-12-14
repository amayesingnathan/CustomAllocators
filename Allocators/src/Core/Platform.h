#pragma once

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
#    define FUNC_SIG __PRETTY_FUNCTION__
#    define FUNC_SIG_PREFIX '='
#    define FUNC_SIG_SUFFIX ']'
#elif defined(__DMC__) && (__DMC__ >= 0x810)
#    define FUNC_SIG __PRETTY_FUNCTION__
#    define FUNC_SIG_PREFIX '='
#    define FUNC_SIG_SUFFIX ']'
#elif (defined(__FUNCSIG__) || (_MSC_VER))
#    define FUNC_SIG __FUNCSIG__
#    define FUNC_SIG_PREFIX '<'
#    define FUNC_SIG_SUFFIX '>'
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
#   define FUNC_SIG __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
#   define FUNC_SIG __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#   define FUNC_SIG __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
#   define FUNC_SIG __func__
#else
#   define FUNC_SIG "FUNC_SIG unknown!"
#endif