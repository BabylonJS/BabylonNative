#pragma once

#ifndef BABYLON_API
#ifdef _WIN32
#define BABYLON_API __cdecl
#else
#define BABYLON_API
#endif
#endif
