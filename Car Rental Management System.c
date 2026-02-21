#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct tanimlari
typedef struct {
    int id;             // Arac kimlik numarasi
    char brand[50];     // Marka
    char model[50];     // Model
    int year;           // Yil
    int status;         // 1: Musait, 0: Kirada 
} Car;

typedef struct {
    char id[12];        // 11 haneli uye kimlik numarası
    char name[50];      // Uye adi
    char phone[20];     // Telefon numarası
} Customer;

typedef struct {
    int carID;          // Kiralanan aracın kimlik numarası
    char customerID[12];// Kiralayan musterinin kimlik numarası
    char rentDate[20];  // Kiralama tarihi
    char returnDate[20];// Teslim tarihi
} Rental;

// Degiskenler ve pointerlar
Car *cars = NULL;
int carCount = 0;

Customer *customers = NULL;
int customerCount = 0;

Rental *rentals = NULL;
int rentalCount = 0;

// Fonksiyon prototipleri
void loadFiles(); // Dosyalari yukle
void saveFiles(); // Dosyalara kaydet
void menu();      // Ana menu

// Arac yonetimi
void addCar(); // Yeni arac ekle
void listCars(); // Tum araclari listele
void searchCar(); // ID'ye gore
void deleteCar(); // ID'ye gore

// Musteri yonetimi
void addCustomer();    // Yeni musteri ekle
void listCustomers();  // Tum musterileri listele
void searchCustomer(); // ID'ye gore

// Kiralama islemleri
void rentCar();     // Arac kirala
void returnCar();   // Arac teslim al
void listRentals(); // Kiralama listesi

// Yardimci fonksiyonlar
int getLineCount(const char *filename); // Dosyadaki satir sayisini bulur
void clearInputBuffer(); // Korunumlu girdiyi temizle

// Main fonksiyonun minimal hali
int main() {
    loadFiles(); // Program basinda dosyalari oku ve malloc ile yer ayir 
    menu();      // Kullanici etkilesimi
    return 0;
}

// Dosya islemleri

// Dosyadaki satir sayisini bulur
int getLineCount(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return 0;
    
    int count = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') {
            count++;
        }
    }
    fclose(fp); // Dosyayi kapat
    return count;
}

void loadFiles() { // Dosyalardan verileri yukle
    // 1. Araclari yukle
    FILE *fp = fopen("araclar.txt", "r");
    if (fp != NULL) {
        // Once dosyadaki kayit sayisini basitce dogrudan okur ve malloc ile yer ayiririz
        
        Car temp;
        while(fscanf(fp, "%d %s %s %d %d", &temp.id, temp.brand, temp.model, &temp.year, &temp.status) != EOF) {
            carCount++;
            cars = (Car*)realloc(cars, carCount * sizeof(Car));
            cars[carCount-1] = temp;
        }
        fclose(fp); // Dosyayi kapat
    }

    // 2. Musterileri yukle
    fp = fopen("musteriler.txt", "r");
    if (fp != NULL) {
        Customer temp;
        // %[^;] anlami: ';' gorene kadar tum karakterleri (bosluk dahil) oku demektir.
        // Bu sekilde isimlerdeki bosluklar korunur.
        // Aralara bosluk yerine ; koyuyoruz
        // Ornegin: 14419137998;Berke Yalçın;05526579259
        // fscanf ile okurken de buna uygun sekilde ayiriyoruz.
        // Bu sayede isimdeki bosluklar korunmus olur.
        while(fscanf(fp, "%[^;];%[^;];%s\n", temp.id, temp.name, temp.phone) != EOF) {
            customerCount++;
            customers = (Customer*)realloc(customers, customerCount * sizeof(Customer));
            customers[customerCount-1] = temp;
        }
        fclose(fp); // Dosyayi kapat
    }

    // 3. Kiralamalari yukle
    fp = fopen("kiralamalar.txt", "r");
    if (fp != NULL) {
        Rental temp; 
        while(fscanf(fp, "%d %s %s %s", &temp.carID, temp.customerID, temp.rentDate, temp.returnDate) != EOF) { // Kiralama kayitlarini oku
            rentalCount++;
            rentals = (Rental*)realloc(rentals, rentalCount * sizeof(Rental));
            rentals[rentalCount-1] = temp;
        }
        fclose(fp); // Dosyayi kapat
    }
    
    printf("Veriler dosyalardan yuklendi.\n");
    printf("Arac Sayisi: %d, Musteri Sayisi: %d, Kiralama Sayisi: %d\n", carCount, customerCount, rentalCount);
}

// Verileri dosyalara geri yazar 
void saveFiles() {
    FILE *fp;

    // Araclari kaydet
    fp = fopen("araclar.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < carCount; i++) {
            fprintf(fp, "%d %s %s %d %d\n", cars[i].id, cars[i].brand, cars[i].model, cars[i].year, cars[i].status);
        }
        fclose(fp); // Dosyayi kapat
    }

    // Musterileri kaydet
    fp = fopen("musteriler.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < customerCount; i++) {
            // Aralara bosluk yerine ; koyuyoruz
            fprintf(fp, "%s;%s;%s\n", customers[i].id, customers[i].name, customers[i].phone);
        }
        fclose(fp); // Dosyayi kapat
    }

    // Kiralamalari kaydet
    fp = fopen("kiralamalar.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < rentalCount; i++) {
            fprintf(fp, "%d %s %s %s\n", rentals[i].carID, rentals[i].customerID, rentals[i].rentDate, rentals[i].returnDate);
        }
        fclose(fp); // Dosyayi kapat
    }
}

// Menu ve Kontrol

void menu() {
    int choice;
    while (1) { // Cikis secilene kadar devam et 
        printf("\n--- ARAC KIRALAMA SISTEMI ---\n");
        printf("1. Arac Ekle\n");
        printf("2. Arac Listele\n");
        printf("3. Arac Ara (ID)\n");
        printf("4. Arac Sil\n");
        printf("---------------------------\n");
        printf("5. Musteri Ekle\n");
        printf("6. Musteri Listele\n");
        printf("7. Musteri Ara (ID)\n");
        printf("---------------------------\n");
        printf("8. Arac Kirala\n");
        printf("9. Arac Teslim Al\n");
        printf("10. Kiralama Listesi\n");
        printf("---------------------------\n");
        printf("0. Cikis\n");
        printf("Seciminiz: ");
        
        if (scanf("%d", &choice) != 1) { // Girdi kontrolu
            printf("Gecersiz giris! Lutfen sayi giriniz.\n");
            clearInputBuffer();
            continue;
        }
        
        // Secime gore fonksiyon cagir
        switch (choice) {
            case 1: addCar(); break;
            case 2: listCars(); break;
            case 3: searchCar(); break;
            case 4: deleteCar(); break;
            case 5: addCustomer(); break;
            case 6: listCustomers(); break;
            case 7: searchCustomer(); break;
            case 8: rentCar(); break;
            case 9: returnCar(); break;
            case 10: listRentals(); break;
            case 0:
                saveFiles(); // Cikarken kaydet
                // Bellekleri serbest birak
                free(cars);
                free(customers);
                free(rentals);
                printf("Cikis yapiliyor...\n");
                return;
            default:
                printf("Gecersiz secim!\n");
        }
    }
}

void clearInputBuffer() { // Korunumlu girdiyi temizle
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Arac fonksiyonlari

void addCar() { // Yeni arac ekle
    Car newCar;
    printf("\n--- Yeni Arac Ekle ---\n");
    
    printf("Arac ID: ");
    scanf("%d", &newCar.id);
    
    // ID kontrolu (Var mi?)
    for(int i=0; i<carCount; i++) {
        if(cars[i].id == newCar.id) {
            printf("HATA: Bu ID'ye sahip arac zaten var!\n");
            return;
        }
    }

    printf("Marka: ");
    scanf("%s", newCar.brand);
    printf("Model: ");
    scanf("%s", newCar.model);
    printf("Yil: ");
    scanf("%d", &newCar.year);
    
    newCar.status = 1; // Varsayilan olarak musait 

    // Bellekte yer ac ve ekle
    carCount++;
    cars = (Car*)realloc(cars, carCount * sizeof(Car));
    cars[carCount-1] = newCar;
    
    saveFiles(); // Dosyaya yansit
    printf("Arac basariyla eklendi.\n");
}

void listCars() { // Tum araclari listele
    printf("\n--- Arac Listesi ---\n");
    printf("ID\tMarka\tModel\tYil\tDurum\n");
    for (int i = 0; i < carCount; i++) {
        printf("%d\t%s\t%s\t%d\t%s\n", 
            cars[i].id, cars[i].brand, cars[i].model, cars[i].year, 
            cars[i].status == 1 ? "Musait" : "Kirada");
    }
}

void searchCar() { // ID'ye gore arac ara
    int searchID;
    printf("Aranacak Arac ID: ");
    scanf("%d", &searchID);
    
    int found = 0;
    for (int i = 0; i < carCount; i++) {
        if (cars[i].id == searchID) {
            printf("Bulundu -> Marka: %s, Model: %s, Yil: %d, Durum: %d\n", 
                   cars[i].brand, cars[i].model, cars[i].year, cars[i].status);
            found = 1;
            break;
        }
    }
    if (!found) printf("Arac bulunamadi.\n");
}

void deleteCar() { // ID'ye gore arac sil
    int deleteID;
    printf("Silinecek Arac ID: ");
    scanf("%d", &deleteID);
    
    int foundIndex = -1; // Silinecek aracin indeksi
    for (int i = 0; i < carCount; i++) {// Araci bul
        if (cars[i].id == deleteID) {
            foundIndex = i;
            break;
        }
    }
    
    if (foundIndex == -1) {
        printf("Arac bulunamadi.\n");
        return;
    }
    
    if (cars[foundIndex].status == 0) {
        printf("HATA: Kirada olan arac silinemez!\n");
        return;
    }

    // Silme islemi: Son elemani silinecek yere kopyala ve boyutu azalt
    cars[foundIndex] = cars[carCount - 1];
    carCount--;
    cars = (Car*)realloc(cars, carCount * sizeof(Car));
    
    saveFiles(); // Dosyaya yansit
    printf("Arac silindi.\n");
}

// Musteri fonksiyonlari

void addCustomer() { // Yeni musteri ekle
    Customer newCust;
    printf("\n--- Yeni Musteri Ekle ---\n");
    
    printf("TC Kimlik No (11 Hane): ");
    scanf("%s", newCust.id);
    
    // Kontrol: 11 hane mi?
    if (strlen(newCust.id) != 11) {
        printf("HATA: TC Kimlik numarasi 11 haneli olmalidir!\n");
        return;
    }
    
    // Benzersizlik kontrolu
    for(int i=0; i<customerCount; i++) {
        if(strcmp(customers[i].id, newCust.id) == 0) {
            printf("HATA: Bu TC numarasi zaten kayitli!\n");
            return;
        }
    }

    printf("Ad Soyad: ");
    clearInputBuffer(); // Onceki scanf'ten kalan enter tusunu temizle
    fgets(newCust.name, 50, stdin); //isim al

    // fgets sonunda gelen enter karakterini (\n) sil
    int len = strlen(newCust.name);
    if (len > 0 && newCust.name[len - 1] == '\n') {
        newCust.name[len - 1] = '\0'; // \n yerine bitis karakteri koyduk
    }

    // Telefon numarasi al
    printf("Telefon: ");
    scanf("%s", newCust.phone);
    
    // Bellekte yer ac ve ekle
    customerCount++; // Musteri sayisini artir
    customers = (Customer*)realloc(customers, customerCount * sizeof(Customer)); // Bellekte yer ac
    customers[customerCount-1] = newCust; // Yeni musteriyi ekle
    
    saveFiles(); // Dosyaya yansit
    printf("Musteri eklendi.\n");
}

void listCustomers() { // Tum musterileri listele
    printf("\n--- Musteri Listesi ---\n"); 
    for (int i = 0; i < customerCount; i++) { 
        printf("ID: %s, Isim: %s, Tel: %s\n", customers[i].id, customers[i].name, customers[i].phone);
    }
}

void searchCustomer() { // ID'ye gore musteri ara
    char searchID[20];
    printf("Aranacak Musteri ID: ");
    scanf("%s", searchID);
    
    int found = 0;
    for (int i = 0; i < customerCount; i++) { // Musterileri tara
        if (strcmp(customers[i].id, searchID) == 0) {
            printf("Bulundu -> Isim: %s, Tel: %s\n", customers[i].name, customers[i].phone);
            found = 1;
            break;
        }
    }
    if (!found) printf("Musteri bulunamadi.\n");
}

// Kiralama fonksiyonlari

void rentCar() { // Arac kirala
    int cID;
    char custID[20];
    
    printf("\n--- Arac Kirala ---\n");
    printf("Kiralanacak Arac ID: ");
    scanf("%d", &cID);
    
    // Arac kontrolu
    int carIndex = -1;
    for (int i = 0; i < carCount; i++) {
        if (cars[i].id == cID) {
            carIndex = i;
            break;
        }
    }
    
    if (carIndex == -1) {
        printf("HATA: Arac bulunamadi.\n");
        return;
    }
    if (cars[carIndex].status == 0) {
        printf("HATA: Bu arac su an kirada!\n");
        return;
    }
    
    // Musteri ID al
    printf("Musteri TC No: "); 
    scanf("%s", custID);
    
    // Musteri kontrolu
    int custIndex = -1;
    for (int i = 0; i < customerCount; i++) {
        if (strcmp(customers[i].id, custID) == 0) {
            custIndex = i;
            break;
        }
    }
    
    if (custIndex == -1) {
        printf("HATA: Musteri bulunamadi. Once musteri ekleyiniz.\n");
        return;
    }
    
    // Kiralama islemi
    Rental newRental;
    newRental.carID = cID;
    strcpy(newRental.customerID, custID);
    printf("Kiralama Tarihi (GG.AA.YYYY): ");
    scanf("%s", newRental.rentDate); // Kiralama tarihi al
    printf("Teslim Tarihi (GG.AA.YYYY): ");
    scanf("%s", newRental.returnDate); // Teslim tarihi al
    
    // Diziyi genislet
    rentalCount++; 
    rentals = (Rental*)realloc(rentals, rentalCount * sizeof(Rental));
    rentals[rentalCount-1] = newRental;
    
    // Aracin durumunu guncelle (0: Kirada)
    cars[carIndex].status = 0;
    
    saveFiles(); // Dosyalara kaydet
    printf("Kiralama islemi basarili.\n");
}

void returnCar() { // Arac teslim al
    int cID; 
    printf("\n--- Arac Teslim Al ---\n");
    printf("Teslim edilecek Arac ID: ");
    scanf("%d", &cID);
    
    // Araci bul
    int carIndex = -1;
    for (int i = 0; i < carCount; i++) {
        if (cars[i].id == cID) {
            carIndex = i;
            break;
        }
    }
    
    if (carIndex == -1 || cars[carIndex].status == 1) {
        printf("HATA: Bu arac zaten musait veya bulunamadi.\n");
        return;
    }
    
    // Araci musait yap
    cars[carIndex].status = 1;
    
    // Kiralamayi bul ve sil
    int rentIndex = -1;
    for (int i = 0; i < rentalCount; i++) {
        if (rentals[i].carID == cID) {
            rentIndex = i;
            break;
        }
    }
    
    if (rentIndex != -1) {
        // Listeden sil
        rentals[rentIndex] = rentals[rentalCount - 1];
        rentalCount--;
        rentals = (Rental*)realloc(rentals, rentalCount * sizeof(Rental));
    }
    // Dosyalara kaydet
    saveFiles();
    printf("Arac teslim alindi ve musait duruma getirildi.\n");
}

void listRentals() { // Kiralama listesini goster
    printf("\n--- Aktif Kiralamalar ---\n");
    printf("AracID\tMusteriID\tAlisTarihi\tTeslimTarihi\n");
    for (int i = 0; i < rentalCount; i++) {
        printf("%d\t%s\t%s\t%s\n", rentals[i].carID, rentals[i].customerID, rentals[i].rentDate, rentals[i].returnDate);
    }
}