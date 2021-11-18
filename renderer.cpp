#include "renderer.h"
#include <ctime>

#include "render.h"

RenderTarget2DRGB_Ptr g_RenderTarget0;

int g_pickedId = 0;
using namespace std;

bool isSet = false;
bool isSynth = false;

TexDescriptor g_texture0;
TexDescriptor g_texture1;

AutoBindShader::render g_render;


// --------------------------------------------------------------

void quad2d(v2f pos, v2f size) {
	v2f a = pos;
	v2f b = v2f(pos[0] + size[0], pos[1]);
	v2f c = pos + size;
	v2f d = v2f(pos[0], pos[1] + size[1]);
	/*glTexCoord2f(0, 0); glVertex2fv(&a[0]);
	glTexCoord2f(0, 1); glVertex2fv(&b[0]);
	glTexCoord2f(1, 1); glVertex2fv(&c[0]);
	glTexCoord2f(1, 0); glVertex2fv(&d[0]);*/
	std::vector<float> points;
	std::vector<float> colors;
	std::vector<float> texCoord;
	points.push_back(a[0]);
	points.push_back(a[1]);
	points.push_back(.0);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	texCoord.push_back(0);
	texCoord.push_back(0);

	points.push_back(b[0]);
	points.push_back(b[1]);
	points.push_back(.0);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	texCoord.push_back(0);
	texCoord.push_back(1);

	points.push_back(c[0]);
	points.push_back(c[1]);
	points.push_back(.0);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	texCoord.push_back(1);
	texCoord.push_back(1);

	points.push_back(d[0]);
	points.push_back(d[1]);
	points.push_back(.0);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	colors.push_back(0.4f);
	texCoord.push_back(1);
	texCoord.push_back(0);

	GLuint vboCoord = 0;
	glGenBuffers(1, &vboCoord);
	glBindBuffer(GL_ARRAY_BUFFER, vboCoord);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(float), points.data(), GL_STATIC_DRAW);

	GLuint vboColor = 0;
	glGenBuffers(1, &vboColor);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);

	GLuint vboTex = 0;
	glGenBuffers(1, &vboTex);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glBufferData(GL_ARRAY_BUFFER, texCoord.size() * sizeof(float), texCoord.data(), GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vboCoord);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glDrawArrays(GL_QUADS, 0, points.size());
}




void renderModel(ImageRGB_Ptr textureA, ImageRGB_Ptr textureB, ImageRGB_Ptr synthesized) {


}

void initShader() {
	g_render.init();

}


void LoadImageToGpu(ImageRGB_Ptr image, TexDescriptor& texture)
{
	isSet = false;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	cerr << image->pixels().xsize() << endl;
	texture.tex = image;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_2D, texture.handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.tex->w(), texture.tex->h(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.tex->raw());
	g_texture0 = texture;
	isSet = true;

}

void LoadResultToGpu(ImageRGB_Ptr image, TexDescriptor& texture)
{
	isSynth = false;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	cerr << image->pixels().xsize() << endl;
	texture.tex = image;
	glGenTextures(1, &texture.handle);
	glBindTexture(GL_TEXTURE_2D, texture.handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.tex->w(), texture.tex->h(), 0, GL_RGB, GL_UNSIGNED_BYTE, texture.tex->raw());
	g_texture1 = texture;
	isSynth = true;

}


void renderScene(int g_3D_W, int g_3D_H)
{
	glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isSet) {
		glViewport(0, 0, g_texture0.tex->w(), g_texture0.tex->h());
		//glClearColor(0.4f, 0.4f, 0.4f, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		g_render.begin();
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_texture0.handle);
		//glColor3f(0.4f, 0.4f, 0.4f); 
		quad2d(v2f(-1.0, -1.0), v2f(2.0, 2.0));
		g_render.end();
	}

	if (isSynth && isSet) {
		glViewport(g_texture0.tex->w(), g_texture0.tex->h(), g_texture1.tex->w(), g_texture1.tex->h());
		//glClearColor(0.4f, 0.4f, 0.4f, 0);
		glEnable(GL_DEPTH_TEST);
		g_render.begin();
		glActiveTextureARB(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_texture1.handle);
		//glColor3f(0.4f, 0.4f, 0.4f); 
		quad2d(v2f(-1.0, -1.0), v2f(2.0, 2.0));
		g_render.end();
	}
}

