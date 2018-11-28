#include "Material.h"

Material* Material::appliedMaterial = nullptr;
ID3D11PixelShader* Material::appliedPShader = nullptr;
ID3D11VertexShader* Material::appliedVShader = nullptr;
ID3D11InputLayout* Material::appliedInputLayout = nullptr;
ID3D11DepthStencilState* Material::appliedDepthState = nullptr;
ID3D11BlendState* Material::appliedBlendState = nullptr;
ID3D11RasterizerState* Material::appliedRasterizerState = nullptr;

vector<ID3D11ShaderResourceView*> Material::appliedTextures;
vector<ID3D11SamplerState*> Material::appliedSamplers;