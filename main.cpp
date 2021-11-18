
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>
#include <time.h>

#include "imgui/imgui.h"
#include "FileDialog.h"
#include "renderer.h"
#include "Synthetizer.h"

#include <ctime>
#include <chrono>

#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <Eigen/SparseCholesky>
#include <Eigen/LU>



LIBSL_WIN32_FIX

m4x4f g_lightPos = lookatMatrix(v3f(-1.0, -1.0, 1.0), v3f(0.0f, 0.0f, 0.0f), v3f(0.0f, 1.0f, 0.0f));

using namespace std;

int g_3D_W = 1280;
int g_3D_H = 800;

int g_SM_W = 1280;
int g_SM_H = 800;
int g_cluster = 0;
bool g_TakeScreenShot = false;

int g_U = 128;
int g_V = 128;

v2d g_mousePos;

ImageRGB_Ptr g_source;
ImageRGB_Ptr g_dest;


// --------------------------------------------------------------


void drawImgUIStuff() {
	bool mouseDown = ImGui::IsMouseClicked(0);
	ImVec2 offsetGUI = ImVec2(100, 100);

	static bool show = true;
	ImGui::Begin("Menu", &show, ImGuiWindowFlags_AlwaysAutoResize);
	{
		if (ImGui::InputInt("U", &g_U)) {
		}
		if (ImGui::InputInt("V", &g_V)) {
		}
		if (ImGui::Button("LoadImage")) {
			string path = openFileDialog("");
			g_source = ImageRGB_Ptr(loadImage<ImageRGB>(path.c_str()));
			TexDescriptor tDesc;
			LoadImageToGpu(g_source, tDesc);
		}
		if (ImGui::Button("export result")) {
			string path = saveFileDialog("");
		}
		if (ImGui::Button("Go!")) {
			SynthetizerTSVQ tsvq;
			Kernel k;
			k.shape = ImageFloat1_Ptr(new ImageFloat1(11, 11, 1.0));
			k.center = v2i(5, 5);
			float tot = 0;
			//create the kernel, I know, I should move this...
			for (int i = 0; i < k.shape->w(); i++) {
				for (int j = 0; j < k.shape->h(); j++) {
					k.shape->pixel(i, j) = 1;// / length(v2i(i, j) - k.center);
						if (v2i(i, j) == k.center) {
							k.shape->pixel(i, j) = 1;

						}
						if (i > k.center[0] || (i >= k.center[0]  && j >= k.center[1])) {
							k.shape->pixel(i, j) = 0;
						}
						cerr << k.shape->pixel(i, j)[0] << " ";
				}
				cerr << endl;
			}


			tsvq.setKernel(k);

			g_dest = tsvq.Synthetize(g_source,g_U,g_V);
			TexDescriptor tDesc;

			LoadResultToGpu(g_dest, tDesc);
		}
	}
	ImGui::End();
	ImGui::Render();
}


void mainRender() {

	double renderingTime = 0;

	renderScene(g_3D_W, g_3D_H);
	drawImgUIStuff();


}

void screenShot() {
	//RenderTarget2DRGB_Ptr RTDField = RenderTarget2DRGB_Ptr(new RenderTarget2DRGB(g_3D_W, g_3D_H));

	//RTDField->bind();
	//renderScene(g_3D_W, g_3D_H, g_model, g_support, g_Shield, g_SupportPoints, g_SupportNormal, g_lightPos, TrackballUI::matrix(), g_renderingOptions);
	//RTDField->unbind();


	//ImageRGB_Ptr img = ImageRGB_Ptr(new ImageRGB(g_3D_W, g_3D_H));
	//RTDField->readBack(img->pixels());
	//img->flipH();
	//static int cnt = 0;
	//while (LibSL::System::File::exists(sprint(PATHTOSRC"\\img\\shot%04d.png", cnt))) { cnt++; }
	//saveImage(sprint(PATHTOSRC"\\img\\shot%04d.png", cnt), img);

}
// --------------------------------------------------------------

void mainKeyPressed(uchar k)
{

	if (k == ' ') {
		//compute();
	}

	if (k = 'S') {
		screenShot();
	}
}


// --------------------------------------------------------------

void mainMousePressed(uint x, uint y, uint button, uint flags)
{
	if (button == LIBSL_LEFT_BUTTON) {
		if (flags == LIBSL_BUTTON_DOWN) {
			g_mousePos = v2d(x, y);

		}
	}
}

// --------------------------------------------------------------

void mainMouseMotion(uint x, uint y)
{
}


// --------------------------------------------------------------
void mainMouseWheel(int incr)
{

}

// --------------------------------------------------------------
int main(int argc, char** argv)
{

	try {
		TrackballUI::onRender = mainRender;
		TrackballUI::onKeyPressed = mainKeyPressed;
		TrackballUI::onMouseButtonPressed = mainMousePressed;
		TrackballUI::onMouseMotion = mainMouseMotion;
		TrackballUI::init(g_3D_W, g_3D_H);

		// GL init

		// imgui
		SimpleUI::bindImGui();
		TrackballUI::trackball().set(v3f(0), v3f(0), quatf(v3f(1, 0, 0), -1.0f) * quatf(v3f(0, 0, 1), 2.5f));
		//tracking ball position, using quaternions.
		SimpleUI::initImGui();
		SimpleUI::onReshape(g_3D_W, g_3D_H);
		initShader();

		TrackballUI::loop();
		SimpleUI::terminateImGui();
		TrackballUI::shutdown();
	}
	catch (Fatal & e) {
		cerr << Console::red << e.message() << Console::gray << endl;
		return (-1);
	}

	return (0);
}
