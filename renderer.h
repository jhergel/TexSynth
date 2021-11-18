
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>



struct TexDescriptor {
	GLuint handle;
	ImageRGB_Ptr tex;
};

struct drawing {
	TexDescriptor tDex;
	v3f size;
	v3f pos;
};

void initShader();
void LoadImageToGpu(ImageRGB_Ptr image, TexDescriptor& texture);
void LoadResultToGpu(ImageRGB_Ptr image, TexDescriptor& texture);

void renderScene(int g_3D_W, int g_3D_H);