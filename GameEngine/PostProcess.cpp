#include "PostProcess.h"

float PostProcess::clearcolor[4];
ID3D11RenderTargetView **PostProcess::screenView;
ID3D11DepthStencilView **PostProcess::screenDepthView;