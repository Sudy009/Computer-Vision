#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{                  //파라미터 : 입력, 결과
	int ImgSize = W * H;    //영상의 가로세로 사이즈
	for (int i = 0; i < ImgSize; i++)    //영상처리하는 코드
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{//더하거나 빼기, 밝기
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)  //먼저 검사, 최대 255
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] = Img[i] + Val;      //0과 255사이면 그냥 대입해라
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{  //곱하거나 나누기, 대비 -> 너무 큰 수로 조정하면 안됨, 1보다 약간 큰것, 1보다 약간 작은 것. 0보다 작아지는 것은 불가능
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);   //정수*실수이므로 형변환 후 대입해주기
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	double Temp[256] = { 0 };    //확률로 구하기
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
	for (int i = 0; i < 256; i++) {
		Temp[i] = (double)Histo[i] / ImgSize;
	}
	FILE* fp = fopen("histogram.txt", "wt");     //여기는 histo값 확인할 수 있는 코드(52~54)
	for(int i=0; i<256; i++) fprintf(fp, "%d\n", Histo[i]);    //빈도값으로 구하기
	//for(int i=0; i<256; i++) fprintf(fp, "%lf\n", Temp[i]);    //확률로 구하기
	fclose(fp);
}

void ObtainAHistogram(int* Histo, int* AHisto)
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] = AHisto[i] + Histo[j];
		}
	}
	/*FILE* fp = fopen("Ahistogram.txt", "wt");
	for (int i = 0; i < 256; i++) fprintf(fp, "%d\n", AHisto[i]);
	fclose(fp);*/
}

void HistogramStretching(BYTE* Img, BYTE* Out, int* Histo, int W, int H)
{    //고대비를 가지게 되면서 영상의 질이 좋아짐
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {   //가장 어두운 화소값 찾기
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {    //제일 뒤에서부터 가방 밝은 화소값 찾기
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {   //구해진 low와 high값을 가지고 히스토그램 스트레칭 해주는 코드
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);    //0~255로 노말라이제이션
	}
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{//모든 화소값들이 고르게 쓰일 수 있도록 재분배해주는 것 -> 책상의 스크레치까지 보임(안보이던 것도 보임). 이미지 개선 효과
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;     //Gmax : 최대 가질 수 있는 화소값
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	/*FILE* fp = fopen("NormSum.txt", "wt");
	for (int i = 0; i < 256; i++) fprintf(fp, "%d\n", NormSum[i]);
	fclose(fp);*/
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}

void Binarization(BYTE* Img, BYTE* Out, int W, int H, BYTE Threshold)
{     //이진호 : 화소값 0 혹은 255만 사용
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int GozalezBinThresh(int* Histo)
{
	BYTE Low = 0, High = 0;
	double m1 = 0.0, m2 = 0.0;
	int g1, g2, sum1, sum2, temp = 0, T;//g1 g2로 영역을 나눈후 각각의 합
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}

	T = (int)((High + Low) / 2);

	do {
		sum1 = 0;
		g1 = 0;
		sum2 = 0;
		g2 = 0;

		for (int i = Low; i <= T; i++) {
			sum1 += i * Histo[i];
			g1 += Histo[i];
		}
		m1 = sum1 / (double)g1;

		for (int i = T + 1; i <= High; i++) {
			sum2 += i * Histo[i];
			g2 += Histo[i];
		}
		m2 = sum2 / (double)g2;

		T = (int)((m1 + m2) / 2.0);


		if ((T - temp < 3) && (T - temp > -3)) {//ex) T1과 temp0이 if문을 들어가게됨 그러므로 출력은 현재인 T가 나와야함
			break;
		}
		temp = T;
	} while (1);

	return T; //temp 반환시 if문 통과보다 하나 작아진값이 나오게 됨으로 T반환
}

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("coin.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE* Image = (BYTE*)malloc(ImgSize);
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	int Histo[256] = { 0 };     //히스토그램 : 화소값들이 몇번 등장하는지 저장 (0~255) 화소값이 하나 등장할 때마다 화소값 변수 ++해줌
	int AHisto[256] = { 0 };

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);   //히스토그램 구하는 코드
	//ObtainAHistogram(Histo, AHisto);   //누적 히스토그램 구하는 코드. 맨마지막 누적 히스토그램 = 화소 갯수
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	int Thres = GozalezBinThresh(Histo);
	printf("%d", Thres);
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);   //이진화
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//입력, 출력 영상의 가로사이즈, 세로사이즈
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, -120);
	//원본, 출력, 넓이, 높이
	//ContrastAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, 0.5);

	fp = fopen("output.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}


//히스토그램 넓이 모두 더한것 : 영상의 화소값