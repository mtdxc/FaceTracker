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

	// rgb32 ԭͼ
	uint32_t *storedBitmapPixels;
	// �ڲ�������ͼƬ
	uint32_t *mImageData_rgb;

	// yccbrͼ��(����ɫ�Ƚ���ĥƤ)
	uint8_t *mImageData_yuv;
	// Ƥ������ ��ʾ��Ƥ����255��ʾ��Ƥ��
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
