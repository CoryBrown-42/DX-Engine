/*
Precompiled Header to speed up build times.
This file should include anything that doesn't change often.

IF YOU CHANGE A FILE THAT IS IN HERE OR INCLUDED IN A FILE INCLUDED HERE IT WON"T TRACK THE CHANGES UNTIL YOU CLEAN AND REBUILD
You HAVE to comment out files and dependent files here when you are working on them!

Note that files with global const variables with assigned values will not be included

This is Force Included in all files! Properties->C / C++->Advanced
*/

#define _WIN32_WINNT 0x0601//Minimum System supported is Windows 7

//System
#include <WindowsX.h>
#include <Windows.h>
#include <string>
#include <d3d11.h>
#include <assert.h>
#include <vector>
#include <sstream> 
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <math.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <mmsystem.h>
#include <unordered_map>

//Third Party
#include "dxerr.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include "SpriteBatch.h"
#include "SpriteFont.h" 
#include "GameTimer.h" 

//Our Engine
#include "AlgorithmTimer.h" 
#include "BaseGame.h"
#include "InputDevice.h"
#include "Keyboard.h"
#include "Math.h"
#include "Mouse.h"
#include "UsageMonitor.h"
#include "Utility.h"