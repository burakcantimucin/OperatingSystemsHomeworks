#include <stdio.h>
#include <stdlib.h>

//POSIX API Kütüphaneleri
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 8192

int main() {
    //Okunacak ve yazılacak dosyaların isimleri
    char input_file[50], output_file[50];
    //Okunacak ve yazılacak dosyaların tanımlayıcıları
    int input_fd, output_fd;

    printf("Okunacak dosyanın adını giriniz: ");
    scanf("%s", input_file);

    //Dosyaların okuma ve yazma modlarında açılması
    input_fd = open(input_file, 0_RDONLY);
    if(input_fd == -1) {
        printf("Okunacak dosya bulunamadı!\n");
        printf("Bu yüzden işleminiz sonlanmıştır.\n");
        exit(0);
    }

    printf("Yazılacak dosyanın adını giriniz: ");
    scanf("%s", output_file);

    output_fd = open(output_file, 0_WRONLY);
    if (output_fd == -1) {
        output_fd = open(output_file, 0_WRONLY | 0_CREAT, 0644);
        if (output_fd == -1)
            printf("Dosya oluşturma hatası oluştu!\n");
        else {
            printf("Dosya bulunamadı, fakat yeni bir dosya oluşturuldu.");
            printf("Bu yüzden işleminiz sonlanmıştır.\n");
            exit(0);
        }
    }
    printf("Okunacak ve yazılacak dosya kontrolleri başarıyla gerçekleşti!\n");

    //Okunacak ve yazılacak dosyaların içerisindeki byte değerleri
    ssize_t ret_in, ret_out;
    //Karakter buffer boyutu
    char buffer[BUFFER_SIZE];

    //Okuma ve yazma işlemleri
    while((ret_in = read(input_fd, &buffer, BUFFER_SIZE)) > 0) {
        ret_out = write(output_fd, &buffer, (ssize_t)ret_in);

        //Okunan verinin yazılacak veriyle eşleşmeme kontrolü
        if (ret_out != ret_in) {
            printf("Verilerde tutarsızlık tespit edildi!\n");
            printf("Bu yüzden işleminiz sonlanmıştır.\n");
            exit(0);
        }
    }
    printf("Okuma ve yazma işlemleri başarıyla gerçekleşti!\n");

    close(input_fd);
    close(output_fd);
    return (EXIT_SUCCESS)
}