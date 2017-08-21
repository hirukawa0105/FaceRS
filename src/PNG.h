#include "lodepng.h"

//テクスチャクラス
class TEXTURE{
protected:
	LodePNG_Decoder decoder;//デコーダ
	unsigned char* buffer;//バッファ
	size_t buffersize, imagesize;//サイズ
public:
	TEXTURE();
	TEXTURE(const char* FileName);//コンストラクタ
	void LOAD_PNG(const char* FileName);//PNG読み込み
	unsigned char* image;//イメージポインタ
	unsigned int Width, Height;//画像サイズ
};
TEXTURE::TEXTURE(){
}
TEXTURE::TEXTURE(const char* FileName){
	LOAD_PNG(FileName);
}
void TEXTURE::LOAD_PNG(const char* FileName){
	LodePNG_Decoder_init(&decoder);
	//ロード
	LodePNG_loadFile(&buffer, &buffersize, FileName);
	//デコード
	LodePNG_decode(&decoder, &image, &imagesize, buffer, buffersize);
	//幅,高さ
	Width = decoder.infoPng.width; Height = decoder.infoPng.height;
}