#ifndef SPRITE
#define SPRITE

#include "Mesh.h" 
#include "ConstantBufferDefinitions.h"
#include "Material.h"
#include "Globals.h"

/*
A Sprite is like an GameObject, except for 2D objects.
All sprites use the same mesh - a square with verts from (0,0) to (1,1)
Scale it to fit any textures dimensions, transform it to move across screen where (0,0)
is the top left of the screen.  Its position.z represents its depth, so sprites 
with greater depth are drawn under other sprites
*/
class Sprite
{
public:
	static BaseMesh *mesh; 

	XMFLOAT3 position, rotation, scale; 
	XMFLOAT4X4 world;
	XMFLOAT4 color;
	UINT postEffect;
	Material* material = nullptr;
	XMFLOAT4 dimensions;//x, y = where in the texture to start, z, w = width and height of the texture to use

	//Constructor
	Sprite()
		: dimensions(XMFLOAT4(0, 0, 1, 1))
	{ 
		position = XMFLOAT3(0, 0, 0);
		rotation = XMFLOAT3(0, 0, 0);
		scale = XMFLOAT3(1, 1, 1);
		color = XMFLOAT4(1, 1, 1, 1); 
		postEffect = rand() % 8;
	} 

	void Draw();
};

#endif