#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
void InverseImage(BYTE* Img, BYTE* Out, int W, int H)
{                  //�Ķ���� : �Է�, ���
	int ImgSize = W * H;    //������ ���μ��� ������
	for (int i = 0; i < ImgSize; i++)    //����ó���ϴ� �ڵ�
	{
		Out[i] = 255 - Img[i];
	}
}
void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{//���ϰų� ����, ���
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)  //���� �˻�, �ִ� 255
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] = Img[i] + Val;      //0�� 255���̸� �׳� �����ض�
	}
}
void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{  //���ϰų� ������, ��� -> �ʹ� ū ���� �����ϸ� �ȵ�, 1���� �ణ ū��, 1���� �ణ ���� ��. 0���� �۾����� ���� �Ұ���
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);   //����*�Ǽ��̹Ƿ� ����ȯ �� �������ֱ�
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	double Temp[256] = { 0 };    //Ȯ���� ���ϱ�
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
	for (int i = 0; i < 256; i++) {
		Temp[i] = (double)Histo[i] / ImgSize;
	}
	FILE* fp = fopen("histogram.txt", "wt");     //����� histo�� Ȯ���� �� �ִ� �ڵ�(52~54)
	for(int i=0; i<256; i++) fprintf(fp, "%d\n", Histo[i]);    //�󵵰����� ���ϱ�
	//for(int i=0; i<256; i++) fprintf(fp, "%lf\n", Temp[i]);    //Ȯ���� ���ϱ�
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
{    //���� ������ �Ǹ鼭 ������ ���� ������
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {   //���� ��ο� ȭ�Ұ� ã��
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {    //���� �ڿ������� ���� ���� ȭ�Ұ� ã��
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {   //������ low�� high���� ������ ������׷� ��Ʈ��Ī ���ִ� �ڵ�
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);    //0~255�� �븻�������̼�
	}
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{//��� ȭ�Ұ����� ���� ���� �� �ֵ��� ��й����ִ� �� -> å���� ��ũ��ġ���� ����(�Ⱥ��̴� �͵� ����). �̹��� ���� ȿ��
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;     //Gmax : �ִ� ���� �� �ִ� ȭ�Ұ�
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
{     //����ȣ : ȭ�Ұ� 0 Ȥ�� 255�� ���
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
	int g1, g2, sum1, sum2, temp = 0, T;//g1 g2�� ������ ������ ������ ��
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


		if ((T - temp < 3) && (T - temp > -3)) {//ex) T1�� temp0�� if���� ���Ե� �׷��Ƿ� ����� ������ T�� ���;���
			break;
		}
		temp = T;
	} while (1);

	return T; //temp ��ȯ�� if�� ������� �ϳ� �۾������� ������ ������ T��ȯ
}

int main()
{
	BITMAPFILEHEADER hf; // 14����Ʈ
	BITMAPINFOHEADER hInfo; // 40����Ʈ
	RGBQUAD hRGB[256]; // 1024����Ʈ
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

	int Histo[256] = { 0 };     //������׷� : ȭ�Ұ����� ��� �����ϴ��� ���� (0~255) ȭ�Ұ��� �ϳ� ������ ������ ȭ�Ұ� ���� ++����
	int AHisto[256] = { 0 };

	ObtainHistogram(Image, Histo, hInfo.biWidth, hInfo.biHeight);   //������׷� ���ϴ� �ڵ�
	//ObtainAHistogram(Histo, AHisto);   //���� ������׷� ���ϴ� �ڵ�. �Ǹ����� ���� ������׷� = ȭ�� ����
	//HistogramEqualization(Image, Output, AHisto, hInfo.biWidth, hInfo.biHeight);
	int Thres = GozalezBinThresh(Histo);
	printf("%d", Thres);
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);   //����ȭ
	//HistogramStretching(Image, Output, Histo, hInfo.biWidth, hInfo.biHeight);
	//InverseImage(Image, Output, hInfo.biWidth, hInfo.biHeight);
	//�Է�, ��� ������ ���λ�����, ���λ�����
	//BrightnessAdj(Image, Output, hInfo.biWidth, hInfo.biHeight, -120);
	//����, ���, ����, ����
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


//������׷� ���� ��� ���Ѱ� : ������ ȭ�Ұ�