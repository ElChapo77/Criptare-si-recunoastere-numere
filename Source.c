#include<stdio.h>
#include<stdlib.h>
#include<math.h>

typedef struct
{
	unsigned char b, g, r;
}pixel;

typedef struct
{
	unsigned int width, height;
	pixel *p;
	unsigned char *header;
	unsigned char nrSablon; //daca este sablon
}imagine;

typedef struct
{
	unsigned int x, y;
}punct;

typedef struct
{
	imagine i;
	double corr;
	punct upleft;
	unsigned int width, height;
	unsigned char nrSablon;
}fereastra;

void XORSHIFT32(unsigned int seed, unsigned int **v, unsigned int dim)
{
	unsigned int r, i;

	*v = (unsigned int*)malloc(dim * sizeof(unsigned int));
	r = seed;
	for (i = 1; i < dim; i++)
	{
		r = r ^ r << 13;
		r = r ^ r >> 17;
		r = r ^ r << 5;
		(*v)[i] = r;
	}
}

char incarcareImagine(char *bmp, imagine *I)
{
	unsigned int i, j, padding;
	FILE *f;

	f = fopen(bmp, "rb");
	if (f == NULL)
	{
		printf("Eroare la deschiderea fisierului %s\n", bmp);
		return 0;
	}
	
	fseek(f, 18, SEEK_SET);
	fread(&(I->width), sizeof(I->width), 1, f);
	fread(&(I->height), sizeof(I->height), 1, f);

	padding = (4 - ((I->width * 3) % 4)) % 4;
	
	fseek(f, 0, SEEK_SET);
	I->header = (unsigned char*)malloc(54);
	fread(I->header, 1, 54, f);

	I->p = (pixel*)malloc(I->height*I->width * sizeof(pixel));
	for (i = I->height - 1;; i--)
	{
		for (j = 0; j < I->width; j++)
			fread(&I->p[i*I->width + j], sizeof(I->p[i*I->width + j]), 1, f);
		fseek(f, padding, SEEK_CUR);
		if (i == 0)
			break;
	}

	fclose(f);
	return 1;
}

char salvareImagine(char *bmp, imagine I)
{
	unsigned int i, j;
	unsigned char k, padding;
	FILE *f;

	f = fopen(bmp, "wb");
	if (f == NULL)
	{
		printf("Eroare la deschiderea fisierului %s\n", bmp);
		return 0;
	}

	padding = (4 - ((I.width * 3) % 4)) % 4;

	fwrite(I.header, 1, 54, f);
	for (i = I.height - 1;; i--)
	{
		for (j = 0; j < I.width; j++)
			fwrite(&I.p[i*I.width + j], sizeof(I.p[i*I.width + j]), 1, f);
		for (k = 0; k < padding; k++)
			fwrite(&k, sizeof(k), 1, f);
		if (i == 0)
			break;
	}

	fclose(f);
	return 1;
}

void generarePermutare(unsigned int **sigma, unsigned int dim, unsigned int *R)
{
	unsigned int i, j, r, aux;

	*sigma = (unsigned int*)malloc(dim * sizeof(unsigned int));
	for (i = 0; i < dim; i++)
		(*sigma)[i] = i;
	for (i = dim - 1, j = 1; i >= 1; i--, j++)
	{
		r = R[j] % (i + 1);
		aux = (*sigma)[r];
		(*sigma)[r] = (*sigma)[i];
		(*sigma)[i] = aux;
	}
}

void intTo3Chars(unsigned int y, unsigned char *x2, unsigned char *x1, unsigned char *x0)
{
	unsigned int z;

	z = y;
	z = z << 24;
	z = z >> 24;
	*x0 = z;

	z = y;
	z = z << 16;
	z = z >> 24;
	*x1 = z;

	z = y;
	z = z << 8;
	z = z >> 24;
	*x2 = z;
}

void criptare(char *imagineInitiala, char *imagineCriptata, char *cheieSecreta)
{
	imagine P, Pprim, C;
	unsigned int R0, SV, *R, *sigma, i;
	unsigned char x0, x1, x2;
	FILE *f;

	if (incarcareImagine(imagineInitiala, &P) == 0)
		return;
	f = fopen(cheieSecreta, "r");
	if (f == NULL)
	{
		printf("Eroare la deschiderea fisierului %s\n", cheieSecreta);
		return;
	}
	fscanf(f, "%u %u", &R0, &SV);
	fclose(f);

	XORSHIFT32(R0, &R, 2 * P.width*P.height);
	generarePermutare(&sigma, P.width*P.height, R);

	Pprim.p = (pixel*)malloc(P.width*P.height * sizeof(pixel));
	for (i = 0; i < P.width*P.height; i++)
		Pprim.p[sigma[i]] = P.p[i];
	Pprim.header = P.header;
	Pprim.width = P.width;
	Pprim.height = P.height;

	free(P.p);
	free(sigma);

	C.p = (pixel*)malloc(P.width*P.height * sizeof(pixel));
	intTo3Chars(SV, &x2, &x1, &x0);
	C.p[0].r = x2 ^ Pprim.p[0].r;
	C.p[0].g = x1 ^ Pprim.p[0].g;
	C.p[0].b = x0 ^ Pprim.p[0].b;
	intTo3Chars(R[P.width*P.height], &x2, &x1, &x0);
	C.p[0].r = C.p[0].r ^ x2;
	C.p[0].g = C.p[0].g ^ x1;
	C.p[0].b = C.p[0].b ^ x0;
	for (i = 1; i < P.width*P.height; i++)
	{
		C.p[i].r = C.p[i - 1].r ^ Pprim.p[i].r;
		C.p[i].g = C.p[i - 1].g ^ Pprim.p[i].g;
		C.p[i].b = C.p[i - 1].b ^ Pprim.p[i].b;
		intTo3Chars(R[P.width*P.height + i], &x2, &x1, &x0);
		C.p[i].r = C.p[i].r ^ x2;
		C.p[i].g = C.p[i].g ^ x1;
		C.p[i].b = C.p[i].b ^ x0;
	}
	C.header = P.header;
	C.width = P.width;
	C.height = P.height;

	free(Pprim.p);
	free(R);
	if (salvareImagine(imagineCriptata, C) == 0)
		return;
	free(P.header);
	free(C.p);
}

void inversarePermutare(unsigned int **sigma, unsigned int dim)
{
	unsigned int *o, i;

	o = (unsigned int*)malloc(dim * sizeof(unsigned int));
	for (i = 0; i < dim; i++)
		o[(*sigma)[i]] = i;
	free(*sigma);
	*sigma = o;
}

void decriptare(char *imagineCriptata, char *imagineInitiala, char *cheieSecreta)
{
	imagine C, Cprim, D;
	unsigned int R0, SV, *R, *sigma, i;
	unsigned char x0, x1, x2;
	FILE *f;

	if (incarcareImagine(imagineCriptata, &C) == 0)
		return;
	f = fopen(cheieSecreta, "r");
	if (f == NULL)
	{
		printf("Eroare la deschiderea fisierului %s\n", cheieSecreta);
		return;
	}
	fscanf(f, "%u %u", &R0, &SV);
	fclose(f);

	XORSHIFT32(R0, &R, 2 * C.width*C.height);
	generarePermutare(&sigma, C.width*C.height, R);
	inversarePermutare(&sigma, C.width*C.height);

	Cprim.p = (pixel*)malloc(C.width*C.height * sizeof(pixel));
	intTo3Chars(SV, &x2, &x1, &x0);
	Cprim.p[0].r = x2 ^ C.p[0].r;
	Cprim.p[0].g = x1 ^ C.p[0].g;
	Cprim.p[0].b = x0 ^ C.p[0].b;
	intTo3Chars(R[C.width*C.height], &x2, &x1, &x0);
	Cprim.p[0].r = Cprim.p[0].r ^ x2;
	Cprim.p[0].g = Cprim.p[0].g ^ x1;
	Cprim.p[0].b = Cprim.p[0].b ^ x0;
	for (i = 1; i < C.width*C.height; i++)
	{
		Cprim.p[i].r = C.p[i - 1].r ^ C.p[i].r;
		Cprim.p[i].g = C.p[i - 1].g ^ C.p[i].g;
		Cprim.p[i].b = C.p[i - 1].b ^ C.p[i].b;
		intTo3Chars(R[C.width*C.height + i], &x2, &x1, &x0);
		Cprim.p[i].r = Cprim.p[i].r ^ x2;
		Cprim.p[i].g = Cprim.p[i].g ^ x1;
		Cprim.p[i].b = Cprim.p[i].b ^ x0;
	}
	Cprim.header = C.header;
	Cprim.width = C.width;
	Cprim.height = C.height;

	free(C.p);
	free(R);

	D.p = (pixel*)malloc(C.width*C.height * sizeof(pixel));
	for (i = 0; i < C.width*C.height; i++)
		D.p[sigma[i]] = Cprim.p[i];
	D.header = C.header;
	D.width = C.width;
	D.height = C.height;

	free(sigma);
	free(Cprim.p);
	if (salvareImagine(imagineInitiala, D) == 0)
		return;
	free(C.header);
	free(D.p);
}

void chiPatrat(char *imagineBMP)
{
	double R, G, B, fbarat;
	imagine I;
	unsigned int *frecventaR, *frecventaG, *frecventaB, i;

	if (incarcareImagine(imagineBMP, &I) == 0)
		return;
	free(I.header);

	fbarat = (I.width*I.height) / 256.0;
	frecventaR = (unsigned int*)calloc(256, sizeof(unsigned int));
	frecventaG = (unsigned int*)calloc(256, sizeof(unsigned int));
	frecventaB = (unsigned int*)calloc(256, sizeof(unsigned int));
	for (i = 0; i < I.width*I.height; i++)
	{
		frecventaR[I.p[i].r]++;
		frecventaG[I.p[i].g]++;
		frecventaB[I.p[i].b]++;
	}

	R = G = B = 0;
	for (i = 0; i <= 255; i++)
	{
		R += ((frecventaR[i] - fbarat)*(frecventaR[i] - fbarat)) / fbarat;
		G += ((frecventaG[i] - fbarat)*(frecventaG[i] - fbarat)) / fbarat;
		B += ((frecventaB[i] - fbarat)*(frecventaB[i] - fbarat)) / fbarat;
	}

	free(frecventaR);
	free(frecventaG);
	free(frecventaB);

	printf("Testul chi-patrat pentru imaginea %s este:\nR: %.2f\nG: %.2f\nB: %.2f\n", imagineBMP, R, G, B);
}

void citireCale(char **s)
{
	char c;
	int dim = 0;

	*s = (char*)malloc(1);
	while ((c = fgetc(stdin)) != '\n')
	{
		dim++;
		*s = (char*)realloc(*s, dim);
		(*s)[dim - 1] = c;
	}
	dim++;
	*s = (char*)realloc(*s, dim);
	(*s)[dim - 1] = '\0';
}

void citireCaleDinFisier(char **s, FILE *f)
{
	char c;
	int dim = 0;

	*s = (char*)malloc(1);
	while ((c = fgetc(f)) != '\n')
	{
		if (feof(f) != 0)
			break;
		dim++;
		*s = (char*)realloc(*s, dim);
		(*s)[dim - 1] = c;
	}
	dim++;
	*s = (char*)realloc(*s, dim);
	(*s)[dim - 1] = '\0';
}

void mainCriptografie()
{
	char *imagineInitiala, *imagineCriptata, *imagineDecriptata, *cheieSecreta, *fisierText;
	FILE *f;

	printf("Calea fisierului text ce contine caile imaginii initiale, criptate, decriptate si a cheii secrete este: ");
	citireCale(&fisierText);

	f = fopen(fisierText, "r");
	if (f == NULL)
	{
		printf("Eroare la deschiderea fisierului %s\n", fisierText);
		return;
	}

	citireCaleDinFisier(&imagineInitiala, f);
	citireCaleDinFisier(&imagineCriptata, f);
	citireCaleDinFisier(&imagineDecriptata, f);
	citireCaleDinFisier(&cheieSecreta, f);

	fclose(f);
	free(fisierText);

	criptare(imagineInitiala, imagineCriptata, cheieSecreta);
	decriptare(imagineCriptata, imagineDecriptata, cheieSecreta);

	printf("\n");
	chiPatrat(imagineInitiala);
	printf("\n");
	chiPatrat(imagineCriptata);
	printf("\n");

	free(imagineInitiala);
	free(imagineCriptata);
	free(imagineDecriptata);
	free(cheieSecreta);
}

void grayscale(imagine *I)
{
	unsigned int i;
	unsigned char aux;

	for (i = 0; i < I->height*I->width; i++)
	{
		aux = (unsigned char)(0.299*I->p[i].r + 0.587*I->p[i].g + 0.114*I->p[i].b);
		I->p[i].r = I->p[i].g = I->p[i].b = aux;
	}
}

double corelatie(imagine S, fereastra f)
{
	unsigned int n, i, j;
	double Sbarat, fbarat, corr, sigmaS, sigmaf;

	n = S.width * S.height;

	Sbarat = 0;
	fbarat = 0;
	for (i = 0; i < S.height; i++)
		for (j = 0; j < S.width; j++)
		{
			Sbarat += S.p[i*S.width + j].r;
			fbarat += f.i.p[(f.upleft.y + i)*f.i.width + (f.upleft.x + j)].r;
		}
	Sbarat = (double)Sbarat / n;
	fbarat = (double)fbarat / n;

	sigmaS = 0;
	sigmaf = 0;
	for (i = 0; i < S.height; i++)
		for (j = 0; j < S.width; j++)
		{
			sigmaS += (S.p[i*S.width + j].r - Sbarat)*(S.p[i*S.width + j].r - Sbarat);
			sigmaf += (f.i.p[(f.upleft.y + i)*f.i.width + (f.upleft.x + j)].r - fbarat)*(f.i.p[(f.upleft.y + i)*f.i.width + (f.upleft.x + j)].r - fbarat);
		}
	sigmaS = sqrt((1.0 / (n - 1))*sigmaS);
	sigmaf = sqrt((1.0 / (n - 1))*sigmaf);

	corr = 0;
	for (i = 0; i < S.height; i++)
		for (j = 0; j < S.width; j++)
			corr += (1.0 / (sigmaf*sigmaS))*(f.i.p[(f.upleft.y + i)*f.i.width + (f.upleft.x + j)].r - fbarat)*(S.p[i*S.width + j].r - Sbarat);
	corr = (1.0 / n)*corr;
	return corr;
}

void templateMatching(imagine I, imagine S, float ps, fereastra **D, unsigned int *nrFerestre)
{
	unsigned i, j;
	fereastra f;

	grayscale(&S);

	for (i = 0; i <= I.height - S.height; i++)
		for (j = 0; j <= I.width - S.width; j++)
		{
			f.upleft.y = i;
			f.upleft.x = j;
			f.width = S.width;
			f.height = S.height;
			f.i = I;
			f.corr = corelatie(S, f);
			f.nrSablon = S.nrSablon;
			if (f.corr > ps)
			{
				(*nrFerestre)++;
				*D = (fereastra*)realloc(*D, (*nrFerestre) * sizeof(fereastra));
				(*D)[*nrFerestre - 1] = f;
			}
		}
}

void desenare(imagine *I, fereastra f, pixel C)
{
	unsigned int i, j, up, down, left, right;

	up = f.upleft.y;
	down = f.upleft.y + f.height;
	left = f.upleft.x;
	right = f.upleft.x + f.width;

	for (j = left; j < right; j++)
		I->p[up*I->width + j] = C;
	for (i = up + 1; i < down; i++)
		I->p[i*I->width + right - 1] = C;
	for (j = right - 2; j >= left; j--)
		I->p[(down - 1)*I->width + j] = C;
	for (i = down - 2; i > up; i--)
		I->p[i*I->width + left] = C;
}

int cmp(const void *a, const void *b)
{
	fereastra x, y;
	x = *(fereastra*)a;
	y = *(fereastra*)b;
	if (x.corr > y.corr)
		return-1;
	if (x.corr < y.corr)
		return 1;
	return 0;
}

void sortare(fereastra **D, unsigned int nrFerestre)
{
	qsort(*D, nrFerestre, sizeof(fereastra), cmp);
}

double suprapunere(fereastra a, fereastra b)
{
	unsigned int ariaIntersectie, ariaA, ariaB;

	ariaIntersectie = 0;
	if (b.upleft.x >= a.upleft.x && b.upleft.x < a.upleft.x + a.width && b.upleft.y >= a.upleft.y && b.upleft.y < a.upleft.y + a.height)
		ariaIntersectie = (a.upleft.x + a.width - b.upleft.x)*(a.upleft.y + a.height - b.upleft.y);
	else if (a.upleft.x + a.width > b.upleft.x && a.upleft.x + a.width <= b.upleft.x + b.width && a.upleft.y >= b.upleft.y && a.upleft.y < b.upleft.y + b.height)
		ariaIntersectie = (a.upleft.x + a.width - b.upleft.x)*(b.upleft.y + b.height - a.upleft.y);
	else if (a.upleft.x >= b.upleft.x && a.upleft.x < b.upleft.x + b.width && a.upleft.y >= b.upleft.y && a.upleft.y < b.upleft.y + b.height)
		ariaIntersectie = (b.upleft.x + b.width - a.upleft.x)*(b.upleft.y + b.height - a.upleft.y);
	else if (b.upleft.x + b.width > a.upleft.x && b.upleft.x + b.width <= a.upleft.x + a.width && b.upleft.y >= a.upleft.y && b.upleft.y < a.upleft.y + a.height)
		ariaIntersectie = (b.upleft.x + b.width - a.upleft.x)*(a.upleft.y + a.height - b.upleft.y);

	ariaA = a.width*a.height;
	ariaB = b.width*b.height;

	return (double)ariaIntersectie / (ariaA + ariaB - ariaIntersectie);
}

void eliminareNonMaxime(fereastra **D, unsigned int *nrFerestre)
{
	unsigned int i, j, k;

	sortare(&(*D), *nrFerestre);
	for (i = 0; i < *nrFerestre - 1; i++)
		for (j = i + 1; j < *nrFerestre; j++)
			if (suprapunere((*D)[i], (*D)[j]) > 0.2)
			{
				for (k = j + 1; k < *nrFerestre; k++)
					(*D)[k - 1] = (*D)[k];
				(*nrFerestre)--;
				*D = (fereastra*)realloc(*D, (*nrFerestre) * sizeof(fereastra));
				j--;
			}
}

void mainRecunoasterePatternuri()
{
	float ps = 0.5;
	FILE *f;
	char *fisierText, *img, *sablon;
	fereastra *D;
	unsigned int nrFerestre, i;
	imagine I, S;
	pixel *C, rosu = { 0,0,255 }, galben = { 0,255,255 }, verde = { 0,255,0 }, cyan = { 255,255,0 }, magenta = { 255,0,255 }, albastru = { 255,0,0 }, argintiu = { 192,192,192 }, portocaliu = { 0,140,255 }, violet = { 128,0,128 }, maro = { 0,0,128 };

	C = (pixel*)malloc(10 * sizeof(pixel));
	C[0] = rosu; C[1] = galben; C[2] = verde; C[3] = cyan; C[4] = magenta; C[5] = albastru; C[6] = argintiu; C[7] = portocaliu; C[8] = violet; C[9] = maro;

	printf("Calea fisierului ce contine caile imaginii si a sabloanelor este: ");
	citireCale(&fisierText);

	f = fopen(fisierText, "r");
	if (f == NULL)
	{
		printf("Eroare la deschiderea fisierului %s\n", fisierText);
		return;
	}
	citireCaleDinFisier(&img, f);
	if (incarcareImagine(img, &I) == 0)
		return;

	D = (fereastra*)malloc(sizeof(fereastra));
	nrFerestre = 0;
	grayscale(&I);
	for (i = 0; i <= 9; i++)
	{
		citireCaleDinFisier(&sablon, f);
		if (incarcareImagine(sablon, &S) == 0)
			return;
		S.nrSablon = i;
		templateMatching(I, S, ps, &D, &nrFerestre);
		free(sablon);
		free(S.header);
		free(S.p);
	}
	fclose(f);
	eliminareNonMaxime(&D, &nrFerestre);
	
	free(I.header);
	free(I.p);
	if (incarcareImagine(img, &I) == 0)
		return;

	for (i = 0; i < nrFerestre; i++)
		desenare(&I, D[i], C[D[i].nrSablon]);

	salvareImagine(img, I);

	free(fisierText);
	free(img);
	free(I.header);
	free(I.p);
	free(C);
}

int main()
{
	mainCriptografie();
	mainRecunoasterePatternuri();
}