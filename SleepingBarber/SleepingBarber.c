#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

//Müşteri limit sayısı

#define MUSTERI_LIMIT 10

//Berber limit sayısı

#define BERBER_LIMIT 5

//Semafor tanımlamaları

sem_t berberler;
sem_t musteriler;
sem_t mutex; 

//Değişkenler

int musteri_sayisi = 0;
int berber_sayisi = 0;
int koltuk_sayisi = 0;
int bekleme_koltugu, uyuyan_berber;

//Fonksiyon prototipleri

void musteri(void *parametre);
void berber(void *parametre);
void bekle();

int main()
{
	//Berber sayısı doğru girilene kadar

	while (berber_sayisi < 1 || berber_sayisi > BERBER_LIMIT)
	{
		printf("Berber Sayısı: ");
		scanf("%d", &berber_sayisi);
		if (berber_sayisi < 1 || berber_sayisi > BERBER_LIMIT)
			printf("Doğru bir değer giriniz!\n\n");
		uyuyan_berber = berber_sayisi;
	}

	//Müşteri sayısı doğru girilene kadar

	while (musteri_sayisi < 1 || musteri_sayisi > MUSTERI_LIMIT)
	{
		printf("Müşteri Sayısı: ");
		scanf("%d", &musteri_sayisi);
		if (musteri_sayisi < 1 || musteri_sayisi > MUSTERI_LIMIT)
			printf("Doğru bir değer giriniz!\n\n");
	}

	//Müşteri koltuk sayısı doğru girilene kadar

	while (koltuk_sayisi < 1 || koltuk_sayisi > MUSTERI_LIMIT)
	{
		printf("Müşteri Koltuk Sayısı: ");
		scanf("%d", &koltuk_sayisi);
		if (koltuk_sayisi < musteri_sayisi || koltuk_sayisi > MUSTERI_LIMIT)
			printf("Doğru bir değer giriniz!\n\n");
		bekleme_koltugu = koltuk_sayisi;
	}
	
	//İş parçacıkları oluşturulur
	
	pthread_t berberler[berber_sayisi];
	pthread_t musteriler[koltuk_sayisi];

	//Semaforlar oluşturulur

	sem_init(&berberler, 0, berber_sayisi);
    sem_init(&musteriler, 0, koltuk_sayisi);
	sem_init(&mutex, 0, 1);

	printf("\nBerber dükkanı açılıyor...\n\n");

	//Berber iş parçacıkları oluşturulur

	for (int i = 0; i < berber_sayisi; i++)
	{
		pthread_create(&berberler[i], NULL, (void*)berber, (void*)&i);
		sleep(1);
	}

	//Müşteri iş parçacıkları oluşturulur

	for (int i = 0; i < musteri_sayisi; i++)
	{
		pthread_create(&musteriler[i], NULL, (void*)musteri, (void*)&i);
		bekle(); //Müşterilerin rastgele zamanlarda girmesini sağlamak
	}
	
	//Dükkan kapanmadan önce tüm müşteriler ile ilgilenilir

	for (int i = 0; i < musteri_sayisi; i++)
	{
		pthread_join(musteriler[i], NULL);
	}

	printf("Tüm müşterilere hizmet verildi.\nBerber dükkanı kapanıyor...\n");
	return 0;
}

void berber(void* parametre)
{
	int sayi = *(int*)parametre;
	printf("%d. berber dükkana geldi.\n", sayi + 1);
	printf("%d. berber uyumaya gitti.\n\n", sayi + 1);
	
	//Müşteriler devam ettiği sürece berberler uyandırılır
	//Aksi halde berberler uyumaya devam eder
	
	while (1) 
    {
		sem_wait(&berberler); //Berber müşteriyi bekler
       	sem_wait(&mutex);
       	printf("%d. berber uyandı.\n", sayi + 1); //Gelen müşteri berberi uyandırır
       	sem_post(&mutex);
       	sem_post(&musteriler); //Berber müşteriyle haberleşir
       	printf("%d. berber saçı kesiyor.\n", sayi + 1);
		sleep(1);
	}
}

void musteri(void* parametre)
{
	sem_wait(&mutex);
	int sayi = *(int*)parametre;
	printf("%d. müşteri dükkana geldi.\n", sayi + 1);
	if (uyuyan_berber > 0)  //Uyuyan berber uyandırılır
    {
		uyuyan_berber--;
        printf("%d. müşteri uyuyan berberi uyandırıyor.\n\n", sayi + 1);
        sem_post(&mutex);
        sem_post(&berberler);  //Müşteri berberle haberleşir
        sem_wait(&musteriler); //Müşteri hizmet alır
        printf("%d. müşteri saçını kestiriyor.\n", sayi + 1);
        uyuyan_berber++;
        printf("%d. müşteri dükkanı terk etti ve berber uyuma gitti.\n\n", sayi + 1);
    }
    else if (bekleme_koltugu <= koltuk_sayisi && bekleme_koltugu > 0)
    {
		bekleme_koltugu--;
        printf("%d. müşteri berberi bekliyor.\n", sayi + 1);
        sem_post(&mutex);
        sem_post(&berberler);  //Müşteri berberle haberleşir
        sem_wait(&musteriler); //Müşteri hizmet alır
        printf("%d. müşteri saçını kestiriyor.\n", sayi + 1);
        bekleme_koltugu++;
        printf("%d. müşteri dükkanı terk etti ve berber uyumaya gitti.\n\n", sayi + 1);
	}
	pthread_exit(0);
}

void bekle()
{
	int sure = rand() % (250000 - 50000 + 1) + 50000;
    srand(time(NULL));
	usleep(sure); 
}
