#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

#define M 800 // Dizinin eleman sayısı
#define N 1600 // Üretilebilecek en fazla sayı

unsigned char kullanilmislar[N] = { 0 };

typedef struct {
	int baslangic;
	int bitis;
} dongu;

int* ilk_dizi;
int* son_dizi;

void ilk_diziyi_yazdir();
void* sirala(void* parametre);
void* birlestir(void* parametre);
void son_diziyi_yazdir(FILE* dosya);

void main()
{
	int i, j = 0;

	//Dizilerin boyutu dinamik olarak oluşturuldu
	ilk_dizi = (int*) malloc(M * sizeof(*ilk_dizi));
	son_dizi = (int*) malloc(M * sizeof(*son_dizi));
	
	//Benzersiz rastgele sayılarla dizi üretimi için
	//Floyd algoritması kullanıldı

	srand((unsigned)time(0));
	for (i = N - M; i < N && j < M; ++i)
	{
		int r = rand() % (i + 1); //Rastgele sayı üretilir
		if (kullanilmislar[r]) 
			r = i; //Üretilen sayı kullanılmışsa i sayısını kullan
		assert(!kullanilmislar[r]); //Sayı kullanılmamış olmalı
		ilk_dizi[j++] = r + 1;
		kullanilmislar[r] = 1;
	}	
	assert(j == M);

	ilk_diziyi_yazdir();

	//3 uzunluğunda bir iş parçacığı dizisi oluşturuldu	

	pthread_t ip_dizi[3];
	
	//İlk 300 elemanı sıralamak için iş parçacığı oluşturuldu ve beklendi	
	
	dongu* d = (dongu*) malloc(sizeof(dongu));
	d->baslangic = 0;
	d->bitis = (M * 3) / 8;
	pthread_create(&ip_dizi[0], NULL, sirala, d);
	pthread_join(ip_dizi[0], NULL);
	
	//Son 500 elemanı sıralamak için iş parçacığı oluşturuldu ve beklendi	

	d = (dongu*) malloc(sizeof(dongu));
	d->baslangic = (M * 3) / 8;
	d->bitis = M;
	pthread_create(&ip_dizi[1], NULL, sirala, d);
	pthread_join(ip_dizi[1], NULL);

	//Birleştirme için son iş parçacığı oluşturuldu ve beklendi

	d = (dongu*) malloc(sizeof(dongu));
	d->baslangic = 0;
	d->bitis = M;
	pthread_create(&ip_dizi[2], NULL, birlestir, d);
	pthread_join(ip_dizi[2], NULL);

	//İlk dizi temizlendi

	free(ilk_dizi);

	//İlgili dosya açıldı

	FILE* dosya = fopen("son.txt", "wb");

	//Dosya sistemde yoksa verilecek hata mesajı
	//Eğer varsa, oluşturulan yeni diziyi yazdırarak dosyayı kapattı

	if (dosya == NULL)
		printf("Sistemde dosya bulunamadığı için açılamadı!\n");
	else {
		son_diziyi_yazdir(dosya);
		fclose(dosya);
	}

	//Son dizi temizlendi

	free(son_dizi);
}

void ilk_diziyi_yazdir()
{
	int i;

	//Elemanları yazdırdı

	printf("Başta Üretilen %d Eleman\n\n", M);

	for (i = 0; i < M; i++)
	{
		printf("%d ", ilk_dizi[i]);
		if ((i + 1) % 10 == 0) printf("\n");
	}

	printf("\n");
}

void* sirala(void* parametre)
{
	dongu* d = parametre;
	int i, j, gecici;
	int baslangic = d->baslangic;
	int bitis = d->bitis;
	
	//Dizi sıralandı

	for (i = baslangic; i < bitis; i++)
	{
		for (j = baslangic; j < bitis - 1; j++)
		{
			if (ilk_dizi[j] > ilk_dizi[j + 1])
			{
				gecici = ilk_dizi[j];
				ilk_dizi[j] = ilk_dizi[j + 1];
				ilk_dizi[j + 1] = gecici;
			}
		}
	}

	printf("%d-%d Arasındaki Sıralanmış Elemanlar\n\n", baslangic + 1, bitis);

	for (i = baslangic; i < bitis; i++)
	{
		printf("%d ", ilk_dizi[i]);
		if ((i + 1) % 10 == 0) printf("\n");
	}

	printf("\n");

	//İş parçacığı sonlandı

	pthread_exit(0);
}

void* birlestir(void* parametre)
{
	dongu* d = parametre;
	int baslangic = d->baslangic;
	int bitis = d->bitis;
	int i = baslangic, j = baslangic, k = (bitis * 3) / 8;
	
	//Dizi sıralandı

	for (i = baslangic; i < bitis; i++)
	{
		if (j == (bitis * 3) / 8) 
			son_dizi[i] = ilk_dizi[k++];
		else if (k == bitis) 
			son_dizi[i] = ilk_dizi[j++];
		else
		{
			if (ilk_dizi[j] < ilk_dizi[k]) 
				son_dizi[i] = ilk_dizi[j++];
			else
				son_dizi[i] = ilk_dizi[k++];
		}
	}

	//İş parçacığı sonlandı

	pthread_exit(0);
}

void son_diziyi_yazdir(FILE *dosya)
{
	int i;

	//Dizinin son hali yazdırıldı
	
	printf("Sıralanmış ve Birleştirilmiş %d Eleman\n\n", M);
	fprintf(dosya, "Sıralanmış ve Birleştirilmiş %d Eleman\n\n", M);

	for (i = 0; i < M; i++)
	{
		printf("%d ", son_dizi[i]);
		fprintf(dosya, "%d ", son_dizi[i]);
		if ((i + 1) % 10 == 0) 
		{
			printf("\n"); fprintf(dosya, "\n");
		} 
	}
}
