#pragma once

#include "imp.h"

#if defined(_WIN32)
#if defined(__GNUC__)
#define IMP_API __attribute__((__dllexport__))
#else
#define IMP_API __declspec(dllexport)
#endif
#else
#if defined(__GNUC__)
#define IMP_API __attribute__((__visibility__("default")))
#else
#define IMP_API
#endif
#endif

IMP_API b32 imp_backend_init();

IMP_API b32 imp_backend_get_inputs(imp_Inputs* inputs);

IMP_API b32 imp_backend_run_commands(imp_CommandList command_list);

IMP_API b32 imp_backend_close();
