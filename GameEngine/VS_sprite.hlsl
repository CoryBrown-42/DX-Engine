#include "Common.hlsli"

VSpriteToPixel main(VSpriteInput input)
{
	VSpriteToPixel output;

	//Notice, we are NOT multiplying this by the viewProjection matrix because its 2D
	output.position = mul(float4(input.position, 1.0f), objectTransform);
	
	//VIEW space has an origin (0, 0) at the CENTER of the window.
	//This converts from SCREEN space (how we position our sprites, (0,0) is top left corner) to view space without view/projection matrix
	output.position.x = output.position.x * (1.0f / screenSize.x) * 2.0f - 1.0f;
	output.position.y = output.position.y * (1.0f / screenSize.y) * -2.0f + 1.0f;
	output.position.z = spriteDepth;
	
	output.texCoords = spriteTexPos + input.index * spriteTexSize;

	return output;
}