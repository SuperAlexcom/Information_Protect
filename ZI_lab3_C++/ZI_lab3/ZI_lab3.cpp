#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdafx.h"
#include "windows.h"
#include <share.h>

static unsigned short read_u16(FILE *fp)
{
	unsigned char b0, b1;

	b0 = getc(fp);
	b1 = getc(fp);

	return ((b1 << 8) | b0);
}
static unsigned int read_u32(FILE *fp)
{
	unsigned char b0, b1, b2, b3;

	b0 = getc(fp);
	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);

	return ((((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}
static int read_s32(FILE *fp)
{
	unsigned char b0, b1, b2, b3;

	b0 = getc(fp);
	b1 = getc(fp);
	b2 = getc(fp);
	b3 = getc(fp);

	return ((int)(((((b3 << 8) | b2) << 8) | b1) << 8) | b0);
}
void readfileheader(FILE *fp){
// считываем заголовок файла
BITMAPFILEHEADER header;

header.bfType = read_u16(fp);
header.bfSize = read_u32(fp);
header.bfReserved1 = read_u16(fp);
header.bfReserved2 = read_u16(fp);
header.bfOffBits = read_u32(fp);

// считываем заголовок изображения
BITMAPINFOHEADER bmiHeader;

bmiHeader.biSize = read_u32(fp);
bmiHeader.biWidth = read_s32(fp);
bmiHeader.biHeight = read_s32(fp);
bmiHeader.biPlanes = read_u16(fp);
bmiHeader.biBitCount = read_u16(fp);
bmiHeader.biCompression = read_u32(fp);
bmiHeader.biSizeImage = read_u32(fp);
bmiHeader.biXPelsPerMeter = read_s32(fp);
bmiHeader.biYPelsPerMeter = read_s32(fp);
bmiHeader.biClrUsed = read_u32(fp);
bmiHeader.biClrImportant = read_u32(fp);

}
void hide(RGBQUAD *pixel, unsigned char hide_byte){
	pixel->rgbBlue &= (0xFC);
	pixel->rgbBlue |= (hide_byte >> 6) & 0x3;
	pixel->rgbGreen &= (0xFC);
	pixel->rgbGreen |= (hide_byte >> 4) & 0x3;
	pixel->rgbRed &= (0xFC);
	pixel->rgbRed |= (hide_byte >> 2) & 0x3;
	pixel->rgbReserved &= (0xFC);
	pixel->rgbReserved |= (hide_byte)& 0x3;

}
unsigned char dec(RGBQUAD *pixel){
	unsigned char hide_byte;
	hide_byte = (hide_byte & 0x3f) | ((pixel->rgbBlue << 6) & (0x03 << 6));
	hide_byte = (hide_byte & 0xcf) | ((pixel->rgbGreen << 4) & (0x03 << 4));
	hide_byte = (hide_byte & 0xf3) | ((pixel->rgbRed << 2) & (0x03 << 2));
	hide_byte = (hide_byte & 0xfc) | (pixel->rgbReserved & 0x03);
	return hide_byte;
}
//режим 1 - расшифровка, 2 - сокрытие
void decoding(int mmode, char* fromOneBmp, const char* toAnoher, const char* secBmp){

	errno_t errFile;
	FILE*  fip;
	errFile = fopen_s(&fip, fromOneBmp, "r+b");
	FILE*  fip2;
	errFile = fopen_s(&fip2, secBmp, "r+b");
	if (fip2 == NULL){
		printf("not open fip2 first.\n");
		exit(1);
	}
	FILE*  mfile;
	fopen_s(&mfile, toAnoher, "r+b");
	if (mfile == NULL){
		printf("error opening mfile.\n");
		exit(1);
	}

	if (errFile != 0){
		printf("error opening fip.\n");
		exit(1);
	}
	if (fip == NULL){
		printf("not open fip first.\n");
		exit(1);
	}

	//считывание заголовков
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER bmiHeader;
	fread(&header, sizeof(header), 1, fip);
	fread(&bmiHeader, sizeof(bmiHeader), 1, fip);

	if (mmode == 1){
		RGBQUAD bgr;
		unsigned char mbyte;
		fread(&bgr, 4, 1, fip);
		mbyte = dec(&bgr);
		fwrite(&mbyte, 1, 1, mfile);
		while (mbyte != 0xFF){
			fread(&bgr, 4, 1, fip);
			mbyte = dec(&bgr);
			if (mbyte != 0xFF){ fwrite(&mbyte, 1, 1, mfile); }
			fwrite(&mbyte, 1, 1, mfile);
		}
	}
	else if (mmode == 2){

		//запись одного файла в другой
		fwrite(&header, sizeof(header), 1, fip2);
		fwrite(&bmiHeader, sizeof(bmiHeader), 1, fip2);

		unsigned char mbyte;

		RGBQUAD bgr1;

		size_t padding = 0;
		if ((bmiHeader.biWidth * 3) % 4)
			padding = 4 - (bmiHeader.biWidth * 3) % 4;
		for (int i = 0; i< bmiHeader.biHeight; i++){
			for (int j = 0; j< bmiHeader.biWidth; j++){

				fread(&bgr1, sizeof(bgr1), 1, fip);
				if (!feof(mfile)){
					fread(&mbyte, 1, 1, mfile);
				}
				else {
					mbyte = 0xff;
				}
				hide(&bgr1, mbyte);
				fwrite(&bgr1, sizeof(bgr1), 1, fip2);
			}
			if (padding != 0) {
				if (!feof(mfile))
					fread(&mbyte, 1, 1, mfile);

				fread(&bgr1, sizeof(bgr1), 1, fip);
				hide(&bgr1, mbyte);
				fwrite(&bgr1, sizeof(bgr1), 1, fip2);
			}

		}

		fread(&mbyte, 1, 1, mfile);
		fread(&bgr1, sizeof(bgr1), 1, fip);
		hide(&bgr1, mbyte);
		fwrite(&bgr1, sizeof(bgr1), 1, fip2);
		if (padding != 0) {
			fread(&mbyte, 1, 1, mfile);
			fread(&bgr1, padding, 1, fip);
			hide(&bgr1, mbyte);
			fwrite(&bgr1, padding, 1, fip2);
		}
	}


	fclose(fip);
	fclose(fip2);
	fclose(mfile);




}



int main()
{
	decoding(1, "5.bmp", "message.txt", "Bitmap.bmp");
	//decoding(2, "tony_cr.bmp", "message.txt", "Bitmap.bmp");
	//decoding(1, "bitfile2.bmp", "message2.txt", "tony_cr.bmp");
	system("pause");
	return 0;
}

