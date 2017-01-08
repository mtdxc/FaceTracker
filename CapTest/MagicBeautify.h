#ifndef _MAGIC_BEAUTIFY_H_
#define _MAGIC_BEAUTIFY_H_
#include <stdint.h>

class MagicBeautify
{
public:
	void initMagicBeautify(uint8_t* argb, int width, int height);
	void unInitMagicBeautify();

	void startSkinSmooth(float smoothlevel);
	void startWhiteSkin(float whitenlevel);

	static MagicBeautify* getInstance();
	~MagicBeautify();

private:
	void Clear();
	static MagicBeautify * instance;
	MagicBeautify();

	uint64_t *mIntegralMatrix;
	uint64_t *mIntegralMatrixSqr;

	// rgb32 原图
	uint32_t *storedBitmapPixels;
	// 内部申明的图片
	uint32_t *mImageData_rgb;

	// yccbr图像(利用色度进行磨皮)
	uint8_t *mImageData_yuv;
	// 皮肤矩阵 表示非皮肤，255表示是皮肤
	uint8_t *mSkinMatrix;

	int mImageWidth;
	int mImageHeight;

	float mSmoothLevel;
	float mWhitenLevel;
	
	void initIntegral();
	
	void initSkinMatrix();

	void _startBeauty(float smoothlevel, float whitenlevel);
	void _startSkinSmooth(float smoothlevel);
	void _startWhiteSkin(float whitenlevel);
};
#endif
