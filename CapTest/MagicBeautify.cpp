#include "MagicBeautify.h"
#include "Math.h"
#include "convert.h"
#include <memory.h>
#define  LOG_TAG    "MagicBeautify"
#define  LOGD(...)  //__android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  //__android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define div255(x) (x * 0.003921F)
#define abs(x) (x>=0 ? x:(-x))
struct RGB
{
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};
MagicBeautify* MagicBeautify::instance;
MagicBeautify* MagicBeautify::getInstance()
{
	if (instance == NULL)
		instance = new MagicBeautify();
	return instance;
}

void MagicBeautify::unInitMagicBeautify() {
	if (instance != NULL)
		delete instance;
	instance = NULL;
}

MagicBeautify::MagicBeautify()
{
	LOGE("MagicBeautify");
	mIntegralMatrix = NULL;
	mIntegralMatrixSqr = NULL;
	mImageData_yuv = NULL;
	mSkinMatrix = NULL;
	mImageData_rgb = NULL;
	mSmoothLevel = 0.0;
	mWhitenLevel = 0.0;
}

MagicBeautify::~MagicBeautify()
{
	LOGE("~MagicBeautify");
	Clear();

}

void MagicBeautify::Clear()
{
	if (mIntegralMatrix != NULL) {
		delete[] mIntegralMatrix;
		mIntegralMatrix = NULL;
	}
	if (mIntegralMatrixSqr != NULL) {
		delete[] mIntegralMatrixSqr;
		mIntegralMatrixSqr = NULL;
	}
	if (mImageData_yuv != NULL) {
		delete[] mImageData_yuv;
		mImageData_yuv = NULL;
	}
	if (mSkinMatrix != NULL) {
		delete[] mSkinMatrix;
		mSkinMatrix = NULL;
	}
	if (mImageData_rgb != NULL) {
		delete[] mImageData_rgb;
		mImageData_rgb = NULL;
	}
}

void MagicBeautify::initMagicBeautify(uint8_t* argb, int width, int height){
	LOGE("initMagicBeautify");
	// 保持旧缓冲区引用
	storedBitmapPixels = argb;

	if (mImageWidth != width || mImageHeight != height)
		Clear();
	mImageWidth = width;
	mImageHeight = height;
	// 拷贝一份用于美化
	if(mImageData_rgb == NULL)
		mImageData_rgb = new uint8_t[sizeof(RGB) * mImageWidth * mImageHeight];
	memcpy(mImageData_rgb, argb, sizeof(RGB) * mImageWidth * mImageHeight);
	// 转成YCbCr
	if(mImageData_yuv == NULL)
		mImageData_yuv = new uint8_t[mImageWidth * mImageHeight * 3];
	RGBToYCbCr(mImageData_rgb, mImageData_yuv, mImageWidth * mImageHeight);
	initSkinMatrix();
	initIntegral();
}

void MagicBeautify::startSkinSmooth(float smoothlevel){
	_startBeauty(smoothlevel,mWhitenLevel);
}

void MagicBeautify::startWhiteSkin(float whitenlevel){
	_startBeauty(mSmoothLevel,whitenlevel);
}

void MagicBeautify::_startBeauty(float smoothlevel, float whitenlevel){
	LOGE("smoothlevel=%f---whitenlevel=%f",smoothlevel,whitenlevel);
	if(smoothlevel >= 10.0 && smoothlevel <= 510.0){
		mSmoothLevel = smoothlevel;
		_startSkinSmooth(mSmoothLevel);
	}
	if(whitenlevel >= 1.0 && whitenlevel <= 5.0){
		mWhitenLevel = whitenlevel;
		_startWhiteSkin(mWhitenLevel);
	}
}

void MagicBeautify::_startWhiteSkin(float whitenlevel){
	float a = log(whitenlevel);
	RGB* pRgb = (RGB*)storedBitmapPixels;
	for(int i = 0; i < mImageHeight; i++){
		for(int j = 0; j < mImageWidth; j++){
			int offset = i*mImageWidth+j;
			RGB* RGB = pRgb + offset;
			if(a != 0){
				RGB->red = 255 * (log(div255(RGB->red) * (whitenlevel - 1) + 1) / a);
				RGB->green = 255 * (log(div255(RGB->green) * (whitenlevel - 1) + 1) / a);
				RGB->blue = 255 * (log(div255(RGB->blue) * (whitenlevel - 1) + 1) / a);
			}
			//storedBitmapPixels[offset] = BitmapOperation::convertArgbToInt(RGB);
		}
	}
}

void MagicBeautify::_startSkinSmooth(float smoothlevel){
	if(mIntegralMatrix == NULL || mIntegralMatrixSqr == NULL || mSkinMatrix == NULL){
		LOGE("not init correctly");
		return;
	}
	RGBToYCbCr((uint8_t*)mImageData_rgb, mImageData_yuv, mImageWidth * mImageHeight);

	int radius = mImageWidth > mImageHeight ? mImageWidth * 0.02 : mImageHeight * 0.02;

	for(int i = 1; i < mImageHeight; i++){
		for(int j = 1; j < mImageWidth; j++){
			int offset = i * mImageWidth + j;
			if(mSkinMatrix[offset]){
				int iMax = i + radius >= mImageHeight-1 ? mImageHeight-1 : i + radius;
				int jMax = j + radius >= mImageWidth-1 ? mImageWidth-1 :j + radius;
				int iMin = i - radius <= 1 ? 1 : i - radius;
				int jMin = j - radius <= 1 ? 1 : j - radius;

				int squar = (iMax - iMin + 1)*(jMax - jMin + 1);
				int i4 = iMax*mImageWidth+jMax;
				int i3 = (iMin-1)*mImageWidth+(jMin-1);
				int i2 = iMax*mImageWidth+(jMin-1);
				int i1 = (iMin-1)*mImageWidth+jMax;

				float m = (mIntegralMatrix[i4]
						+ mIntegralMatrix[i3]
						- mIntegralMatrix[i2]
						- mIntegralMatrix[i1]) / squar;

				float v = (mIntegralMatrixSqr[i4]
						+ mIntegralMatrixSqr[i3]
						- mIntegralMatrixSqr[i2]
						- mIntegralMatrixSqr[i1]) / squar - m*m;
				float k = v / (v + smoothlevel);

				mImageData_yuv[offset * 3] = ceil(m - k * m + k * mImageData_yuv[offset * 3]);
			}
		}
	}
	YCbCrToRGB(mImageData_yuv, storedBitmapPixels, mImageWidth * mImageHeight);
}

void MagicBeautify::initSkinMatrix(){
	LOGE("initSkinMatrix");
	if(mSkinMatrix == NULL)
		mSkinMatrix = new uint8_t[mImageWidth * mImageHeight];
	RGB* pRgb = (RGB*)mImageData_rgb;
	for(int i = 0; i < mImageHeight; i++){
		for(int j = 0; j < mImageWidth; j++){
			int offset = i * mImageWidth + j;
			RGB& RGB = pRgb[offset];
			if ((RGB.blue>95 && RGB.green>40 && RGB.red>20 &&
					RGB.blue-RGB.red>15 && RGB.blue-RGB.green>15)||
					(RGB.blue>200 && RGB.green>210 && RGB.red>170 &&
					abs(RGB.blue-RGB.red)<=15 && RGB.blue>RGB.red&& RGB.green>RGB.red))
				mSkinMatrix[offset] = 255;
			else
				mSkinMatrix[offset] = 0;
		}
	}
}

void MagicBeautify::initIntegral(){
	LOGE("initIntegral");
	if(mIntegralMatrix == NULL)
		mIntegralMatrix = new uint64_t[mImageWidth * mImageHeight];
	if(mIntegralMatrixSqr == NULL)
		mIntegralMatrixSqr = new uint64_t[mImageWidth * mImageHeight];

	uint64_t *columnSum = new uint64_t[mImageWidth];
	uint64_t *columnSumSqr = new uint64_t[mImageWidth];

	columnSum[0] = mImageData_yuv[0];
	columnSumSqr[0] = mImageData_yuv[0] * mImageData_yuv[0];

	mIntegralMatrix[0] = columnSum[0];
	mIntegralMatrixSqr[0] = columnSumSqr[0];

	for(int i = 1;i < mImageWidth;i++){
		columnSum[i] = mImageData_yuv[3 * i];
		columnSumSqr[i] = mImageData_yuv[3 * i] * mImageData_yuv[3 * i];

		mIntegralMatrix[i] = columnSum[i];
		mIntegralMatrix[i] += mIntegralMatrix[i - 1];
		mIntegralMatrixSqr[i] = columnSumSqr[i];
		mIntegralMatrixSqr[i] += mIntegralMatrixSqr[i - 1];
	}
	for (int i = 1;i < mImageHeight; i++){
		int offset = i * mImageWidth;

		columnSum[0] += mImageData_yuv[3*offset];
		columnSumSqr[0] += mImageData_yuv[3*offset] * mImageData_yuv[3*offset];

		mIntegralMatrix[offset] = columnSum[0];
		mIntegralMatrixSqr[offset] = columnSumSqr[0];

		for(int j = 1; j < mImageWidth; j++){
			columnSum[j] += mImageData_yuv[3*(offset+j)];
			columnSumSqr[j] += mImageData_yuv[3*(offset+j)] * mImageData_yuv[3*(offset+j)];

			mIntegralMatrix[offset+j] = mIntegralMatrix[offset+j-1] + columnSum[j];
			mIntegralMatrixSqr[offset+j] = mIntegralMatrixSqr[offset+j-1] + columnSumSqr[j];
		}
	}
	delete[] columnSum;
	delete[] columnSumSqr;
	LOGE("initIntegral~end");
}

