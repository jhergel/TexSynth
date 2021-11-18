
#include <LibSL/LibSL.h>
#include <LibSL/LibSL_gl.h>



struct ImageAndFFT {
	ImageRGB_Ptr imageRe;
	ImageRGB_Ptr fftRe;

	ImageRGB_Ptr imageIm;
	ImageRGB_Ptr fftIm;
};


struct Kernel {
	ImageFloat1_Ptr shape;//weights
	v2i center;//position of the center of the kernel
};

class Synthetizer {
	virtual ImageRGB_Ptr Synthetize(ImageRGB_Ptr source, int sizeU, int sizeV) = 0;

};

class SynthetizerTSVQ :Synthetizer {
public:
	Kernel m_kernel;
	void setKernel(Kernel k) {
		m_kernel = k;
	}
	ImageRGB_Ptr Synthetize(ImageRGB_Ptr source, int sizeU, int sizeV);

};

class SynthetizerPBR : Synthetizer {
	Kernel m_kernel;
	void setKernel(Kernel k) {
		m_kernel = k;
	}
	ImageRGB_Ptr Synthetize(ImageRGB_Ptr source, int sizeU, int sizeV);
};