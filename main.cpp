#include <iostream>
#include <io.h>
#include <cstring>
#include <fcntl.h>
#include <ctime>
#include <limits>
#include <windows.h>
#include <cstdlib>
#include <iomanip>
#include <cstdio>

const int maxlenght = 30;
const char* reportfile = "report.txt";
int globalsize = 0;
char currentFilename[maxlenght] = "";

struct Date {
    int day;
    int month;
    int year;
};

struct Product {
    char name[maxlenght];
    int quantity;
    double pricePerOne;
    double weightPerOne;
    Date arrivalDate;
};

void clearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool isSameDate(const Product& p, int day, int month, int year) {
    return (p.arrivalDate.day == day && p.arrivalDate.month == month && p.arrivalDate.year == year); 
}

bool isDateAfter(const Product& p, int day, int month, int year) {
    if(p.arrivalDate.year != year)
        return p.arrivalDate.year > year;
    if(p.arrivalDate.month != month)
        return p.arrivalDate.month > month;
    return p.arrivalDate.day > day;
}

int getRecordCount(FILE* file) {
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    return (fileSize > 0) ? (fileSize / sizeof(Product)) : 0;
}

void createNewFile(FILE** file) {
    char filename[maxlenght];
    std::cout << "Введите имя для нового файла: ";
    std::cin >> filename;
    if(strcmp(filename, currentFilename) == 0){
        std::cout << "Файл уже создан и готов к работе\n";
        return;
    }
    FILE* test = fopen(filename, "rb");
    if(test) {
        fclose(test);
        std::cout << "Файл с таким именем уже существует, выберете другое название\n";
        return;
    }
    if(strlen(currentFilename) != 0){
        fclose(*file);
    }
    *file = fopen(filename, "wb+");
    globalsize = getRecordCount(*file);
    strcpy(currentFilename, filename);
    std::cout << "Создан файл: " << filename <<". Этот же файл сейчас активен и готов к работе." << "\n";
}

void selectFile(FILE** file, bool& flag) {
    char filename[maxlenght];
    std::cout << "Введите имя файла: ";
    std::cin >> filename;
    if(strcmp(filename, currentFilename) == 0){
        std::cout << "Файл уже выбран";
        return;
    }
    if(*file) {
        fclose(*file);
    }
    *file = fopen(filename, "rb+");
    if(!*file){
        std::cout << "Файл с таким названием не существует" << "\n";
        *file = fopen(currentFilename, "rb+");
        if(strlen(currentFilename) != 0){
            std::cout << "Текущим файлом остается " << "'" << currentFilename << "'" << "\n";
            return;
        }
        else{
            std::cout << "Выберете существующий файл или создайте новый\n";
            flag = true;
            return;
        }
    }
    globalsize = getRecordCount(*file);
    strcpy(currentFilename, filename);
    std::cout << "Выбран файл: " << filename << "\n";
}

void addRecord(FILE* file) {
    Product p;
    std::cout << "Наименование: ";
    std::cin >> p.name;
    while (true) {
        std::cout << "Количество: ";
        std::cin >> p.quantity;
        if(std::cin.fail() || p.quantity < 1){
            clearInputStream();
            std::cout << "Ошибка!\n";
        } else{
            break;
        }
    }
    while (true) {
        std::cout << "Цена: ";
        std::cin >> p.pricePerOne;
        if(std::cin.fail() || p.pricePerOne <= 0){
            clearInputStream();
            std::cout << "Ошибка!\n";
        }else{
            break;
        }
    }
    while (true) {
        std::cout << "Масса (кг): ";
        std::cin >> p.weightPerOne;
        if(std::cin.fail() || p.weightPerOne < 1){
            clearInputStream();
            std::cout << "Ошибка!\n";
        } else{
            break;
        }
    }
    while (true) {
        std::cout << "Дата (дд мм гггг): ";
        std::cin >> p.arrivalDate.day >> p.arrivalDate.month >> p.arrivalDate.year;
        if(p.arrivalDate.month == 02){
            int maxDay = 28;
            if(p.arrivalDate.year % 4 == 0) {
                maxDay = 29;
            }
            if(p.arrivalDate.day > maxDay) {
                clearInputStream();
                std::cout << "Ошибка!\n";
            }else{
                break;
            }
        }else{
            if(std::cin.fail() || p.arrivalDate.day > 31 || p.arrivalDate.month > 12){
                clearInputStream();
                std::cout << "Ошибка!\n"; 
            } else{
                break;
            }
        }
    }
    globalsize++;
    fseek(file, 0, SEEK_END);
    fwrite(&p, sizeof(Product), 1, file);
    std::cout << "Запись добавлена\n";
}

void viewRecords(FILE* file) {
    if(globalsize == 0) {
        std::cout << "Нет записей\n";
        return;
    }
    rewind(file);
    Product p;
    std::cout << "\n------------------------------- Содержимое файла --------------------------------\n";
    std::cout << std::setw(6) << std::left << "N" 
              << std::setw(30) << std::left << "Name" 
              << std::setw(10) << std::left << "Qty" 
              << std::setw(12) << std::left << "Price" 
              << std::setw(12) << std::left << "Weight" 
              << std::setw(12) << std::left << "Date" << std::endl;
    std::cout << "---------------------------------------------------------------------------------\n";
    int count = 0;
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        count++;
        std::cout << std::setw(6) << std::left << count 
                  << std::setw(30) << std::left << p.name 
                  << std::setw(10) << std::left << p.quantity 
                  << std::setw(12) << std::left << std::fixed << std::setprecision(2) << p.pricePerOne 
                  << std::setw(12) << std::left << std::fixed << std::setprecision(3) << p.weightPerOne 
                  << std::setw(2) << p.arrivalDate.day << "."
                  << std::setw(2) << std::setfill(' ')
                  << std::setw(2) << p.arrivalDate.month << "."
                  << std::setw(2) << std::setfill(' ')
                  << std::setw(4) << p.arrivalDate.year << std::setfill(' ') << std::endl;
    } 
}

void deleteRecord(FILE* file) {
    if(globalsize == 0) {
        std::cout << "Нет записей\n";
        return;
    }
    int num;
    std::cout << "Номер записи (1-" << globalsize << "): ";
    std::cin >> num;
    if(std::cin.fail() || num < 1 || num > globalsize) {
        clearInputStream();
        std::cout << "Неверный номер\n";
        return;
    }
    Product p;
    long size = sizeof(Product);
    for(int i = num; i < globalsize; i++) {
        fseek(file, i*size, SEEK_SET);
        fread(&p, size, 1, file);
        fseek(file, (i - 1)*size, SEEK_SET);
        fwrite(&p, size, 1, file);
    }
    long newSize = (globalsize - 1)*size;
    _chsize_s(fileno(file), newSize);
    globalsize--;
    std::cout << "Запись " << num << " удалена\n";
}

void editRecord(FILE* file) {
    if(globalsize == 0) {
        std::cout << "Нет записей\n";
        return;
    }
    int num;
    std::cout << "Номер записи (1-" << globalsize << "): ";
    std::cin >> num;
    if(std::cin.fail() || num < 1 || num > globalsize) {
        clearInputStream();
        std::cout << "Неверный номер\n";
        return;
    }

    fseek(file, (num - 1)*sizeof(Product), SEEK_SET);
    Product p;
    fread(&p, sizeof(Product), 1, file);

    while (true){
        std::cout << "\n--- Содержимое запси ---\n";
        std::cout << "1. Name\n";
        std::cout << "2. Qty\n";
        std::cout << "3. Price\n";
        std::cout << "4. Weight\n";
        std::cout << "5. Date\n";
        std::cout << "0. Сохранить и выйти\n";
        std::cout << "Введите номер записи: ";
        int choice;
        std::cin >> choice;
        if(choice == 0) {
            break;
        }
        switch (choice){
        case 1:
            std::cout << "Введите новое имя: ";
            std::cin >> p.name;
            break;
        case 2:
            std::cout << "Введите новое количество: ";
            std::cin >> p.quantity;
            break;
        case 3:
            std::cout << "Введите новую цену: ";
            std::cin >> p.pricePerOne;
            break;
        case 4:
            std::cout << "Введите новый вес: ";
            std::cin >> p.weightPerOne;
            break;
        case 5:
            std::cout << "Введите новую дату: ";
            std::cin >> p.arrivalDate.day >> p.arrivalDate.month >> p.arrivalDate.year;
        default:
            break;
        }
    }
    fseek(file, (num - 1)*sizeof(Product), SEEK_SET);
    fwrite(&p, sizeof(Product), 1, file);
    std::cout << "Запись изменена\n";
}

void quickSortQuantity(bool ascending, FILE* file, int count) {
    struct StackNode {
        int left;
        int right;
        StackNode* next;
    };
    StackNode* stack = new StackNode{0, count - 1, nullptr};
    while (stack) {
        int left = stack->left;
        int right = stack->right;
        StackNode* temp = stack;
        stack = stack->next;
        delete temp;
        if(left >= right) continue;
        Product pivot;
        fseek(file, ((left + right) / 2)*sizeof(Product), SEEK_SET);
        fread(&pivot, sizeof(Product), 1, file);
        int i = left, j = right;
        
        while (i <= j) {
            Product pi, pj;
            fseek(file, i*sizeof(Product), SEEK_SET);
            while (i <= right) {
                fread(&pi, sizeof(Product), 1, file);
                if(ascending){
                    if(pi.quantity >= pivot.quantity) break;
                } else{
                    if(pi.quantity <= pivot.quantity) break;
                }
                i++;
            }
            
            while (j >= left) {
                fseek(file, j*sizeof(Product), SEEK_SET);
                fread(&pj, sizeof(Product), 1, file);
                if(ascending){
                    if(pj.quantity <= pivot.quantity) break; 
                }
                else{ 
                    if(pj.quantity >= pivot.quantity) break;
                }
                j--;
            }
            
            if(i <= j) {
                if(i != j) {
                    Product t1, t2;
                    fseek(file, i*sizeof(Product), SEEK_SET);
                    fread(&t1, sizeof(Product), 1, file);
                    fseek(file, j*sizeof(Product), SEEK_SET);
                    fread(&t2, sizeof(Product), 1, file);
                    fseek(file, i*sizeof(Product), SEEK_SET);
                    fwrite(&t2, sizeof(Product), 1, file);
                    fseek(file, j*sizeof(Product), SEEK_SET);
                    fwrite(&t1, sizeof(Product), 1, file);
                }
                i++;
                j--;
            }
        }
        if(j - left > right - i) {
            if(left < j) stack = new StackNode{left, j, stack};
            if(i < right) stack = new StackNode{i, right, stack};
        } else {
            if(i < right) stack = new StackNode{i, right, stack};
            if(left < j) stack = new StackNode{left, j, stack};
        }
    }
    std::cout << "Сортировка по количеству выполнена\n";
}

void selectionSortByWeight(bool ascending, FILE* file, int count) {
    for(int i = 0; i < count - 1; i++) {
        int extrIdx = i;
        Product extr;
        fseek(file, i*sizeof(Product), SEEK_SET);
        fread(&extr, sizeof(Product), 1, file);
        fseek(file, (i + 1)*sizeof(Product), SEEK_SET);
        for(int j = i + 1; j < count; j++) {
            Product curr;
            fread(&curr, sizeof(Product), 1, file);
            bool condition = ascending ? (curr.weightPerOne < extr.weightPerOne):(curr.weightPerOne > extr.weightPerOne); 
            if(condition) {
                extrIdx = j;
                extr = curr;
            }
        }
        
        if(extrIdx != i) {
            Product t1, t2;
            fseek(file, i*sizeof(Product), SEEK_SET);
            fread(&t1, sizeof(Product), 1, file);
            fseek(file, extrIdx*sizeof(Product), SEEK_SET);
            fread(&t2, sizeof(Product), 1, file);
            fseek(file, i*sizeof(Product), SEEK_SET);
            fwrite(&t2, sizeof(Product), 1, file);
            fseek(file, extrIdx*sizeof(Product), SEEK_SET);
            fwrite(&t1, sizeof(Product), 1, file);
        }
    }
    std::cout << "Сортировка по массе выполнена\n";
}

void insertionSortByPrice(bool ascending, FILE* file, int count) {
    for(int i = 1; i < count; i++) {
        Product key;
        fseek(file, i*sizeof(Product), SEEK_SET);
        fread(&key, sizeof(Product), 1, file);
        int j = i - 1;
        while (j >= 0) {
            Product curr;
            fseek(file, j*sizeof(Product), SEEK_SET);
            fread(&curr, sizeof(Product), 1, file);
            bool needMove = ascending ? 
                (curr.pricePerOne > key.pricePerOne) :
                (curr.pricePerOne < key.pricePerOne);
            if(!needMove)break;
            fseek(file, (j + 1)*sizeof(Product), SEEK_SET);
            fwrite(&curr, sizeof(Product), 1, file);
            j--;
        }
        if(j + 1 != i) {
            fseek(file, (j + 1)*sizeof(Product), SEEK_SET);
            fwrite(&key, sizeof(Product), 1, file);
        }
    }
    std::cout << "Сортировка по цене выполнена\n";
}

void sortMenu(FILE* file) {
    if(globalsize < 1) {
        std::cout << "Недостаточно записей для сортировок\n";
        return;
    }

    int choice;
    std::cout << "\n--- Сортировка ---\n";
    std::cout << "1. По количеству (возр)\n";
    std::cout << "2. По количеству (уб)\n";
    std::cout << "3. По массе (возр)\n";
    std::cout << "4. По массе (уб)\n";
    std::cout << "5. По цене (возр)\n";
    std::cout << "6. По цене (уб)\n";
    std::cout << "Выбор: ";
    std::cin >> choice;

    switch (choice) {
        case 1: quickSortQuantity(true, file, globalsize); break;
        case 2: quickSortQuantity(false, file, globalsize); break;
        case 3: selectionSortByWeight(true, file, globalsize); break;
        case 4: selectionSortByWeight(false, file, globalsize); break;
        case 5: insertionSortByPrice(true, file, globalsize); break;
        case 6: insertionSortByPrice(false, file, globalsize); break;
        default: std::cout << "Неверный выбор\n";
    }
}

void linearSearchByName(FILE* file) {
    if(globalsize == 0){
        std::cout << "Нет записей\n";
        return;
    }
    rewind(file);
    char query[maxlenght];
    std::cout << "Имя для поиска: ";
    std::cin >> query;
    Product p;
    int found = 0;
    std::cout << "\nРезультаты поиска \"" << query << "\":\n";
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        if(strcmp(p.name, query) == 0) {
            found++;
            std::cout << found << ") " << p.name 
                      << ", кол-во: " << p.quantity
                      << ", цена: " << p.pricePerOne
                      << ", масса: " << p.weightPerOne
                      << ", дата: " << p.arrivalDate.day << "." 
                      << p.arrivalDate.month << "." << p.arrivalDate.year << std::endl;
        }
    }
    if(found == 0) std::cout << "Не найдено\n";
}

void binarySearchByPrice(FILE* file) {
    if(globalsize == 0) {
        std::cout << "Нет данных\n";
        return;
    }
    double target;
    std::cout << "Цена для поиска: ";
    std::cin >> target;
    insertionSortByPrice(true, file, globalsize);
    int left = 0, right = globalsize - 1;
    int foundIdx = -1;
    Product curr;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        fseek(file, mid*sizeof(Product), SEEK_SET);
        fread(&curr, sizeof(Product), 1, file);
        if(curr.pricePerOne == target) {
            foundIdx = mid;
            break;
        }
        else if(curr.pricePerOne < target) left = mid + 1;
        else right = mid - 1;
    }
    if(foundIdx != -1) {
        int first = foundIdx, last = foundIdx;
        while (first > 0) {
            Product prev;
            fseek(file, (first - 1)*sizeof(Product), SEEK_SET);
            fread(&prev, sizeof(Product), 1, file);
            if(prev.pricePerOne == target) first--;
            else break;
        }
        while (last < globalsize - 1) {
            Product next;
            fseek(file, (last + 1)*sizeof(Product), SEEK_SET);
            fread(&next, sizeof(Product), 1, file);
            if(next.pricePerOne == target) last++;
            else break;
        }
        std::cout << "\nНайдено " << (last - first + 1) << " записей:\n";
        for(int i = first; i <= last; i++) {
            fseek(file, i*sizeof(Product), SEEK_SET);
            fread(&curr, sizeof(Product), 1, file);
            std::cout << "#" << (i+1) << ": " << curr.name 
                      << ", кол-во: " << curr.quantity
                      << ", цена: " << curr.pricePerOne
                      << ", дата: " << curr.arrivalDate.day << "."
                      << curr.arrivalDate.month << "." << curr.arrivalDate.year << std::endl;
        }
    }
    else std::cout << "Не найдено\n";
}

void task1(FILE* file) {
    if(globalsize == 0){
        std::cout << "Нет записей\n";
        return;
    }
    rewind(file);

    char choice;
    std::cout << "Хотели бы вы сохранить данные?(Да - y | Нет - n)\n";
    std::cin >> choice;

    int qty, d, m, y;
    std::cout << "Количество для сравнения: ";
    std::cin >> qty;
    std::cout << "Дата (дд мм гггг): ";
    std::cin >> d >> m >> y;
    if(std::cin.fail()) {
        clearInputStream();
        std::cout << "Ошибка ввода\n";
        return;
    }

    FILE* out = nullptr;
    char fname[maxlenght] = "";
    if(choice == 'y'){
        while(true){
            std::cout << "Имя файла: ";
            std::cin >> fname;
            if(strcmp(fname, currentFilename) == 0){
                std::cout << "Это имя основного файла, нужно другое название\n";
                continue;
            }
            FILE* test = fopen(fname, "rb");
            if(test) {
                fclose(test);
                std::cout << "Файл с таким именем уже существует, выберете другое название\n";
                continue;
            }
            out = fopen(fname, "w");
            if(out) {
                fprintf(out, "Результаты: кол-во < %d, дата %02d.%02d.%04d\n\n", qty, d, m, y);
                break;
            }
        }
    }

    Product p;
    int idx = 0, count = 0;
    std::cout << "\nТовары (кол-во < " << qty << ", дата " << d << "." << m << "." << y << "):\n";
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        idx++;
        if(p.quantity < qty && isSameDate(p, d, m, y)){
            count++;
            std::cout << "#" << idx << ": " << p.name 
                      << ", кол-во: " << p.quantity
                      << ", цена: " << p.pricePerOne
                      << ", дата: " << p.arrivalDate.day << "."
                      << p.arrivalDate.month << "." << p.arrivalDate.year << std::endl;
            if(out){
                fprintf(out, "%d: %s, кол-во: %d, цена: %.2f, масса: %.3f, дата: %02d.%02d.%04d\n",
                                idx, p.name, p.quantity, p.pricePerOne, p.weightPerOne,
                                p.arrivalDate.day, p.arrivalDate.month, p.arrivalDate.year);
            }
        }
    }
    if(count == 0) {
        std::cout << "Не найдено\n";
        if(out) {
            fclose(out);
            remove(fname);
        }
    } else {
        std::cout << "\nНайдено: " << count << "\n";
        if(out) {
            fclose(out);
            std::cout << "Сохранено\n";
        }
    }
}

void task2(FILE* file){
    if(globalsize == 0){
        std::cout << "Нет записей\n";
        return;
    }
    rewind(file);

    char choice;
    std::cout << "Хотели бы вы сохранить данные?(Да - y | Нет - n)\n";
    std::cin >> choice;

    char name[maxlenght];
    int d, m, y;
    std::cout << "Наименование: ";
    std::cin >> name;
    std::cout << "Дата (дд мм гггг): ";
    std::cin >> d >> m >> y;
    if(std::cin.fail()) {
        clearInputStream();
        std::cout << "Ошибка ввода\n";
        return;
    }

    FILE* out = nullptr;
    char fname[30] = "";
    if(choice == 'y'){
        while(true){
            std::cout << "Имя файла: ";
            std::cin >> fname;
            if(strcmp(fname, currentFilename) == 0){
                std::cout << "Это имя основного файла, нужно другое название\n";
                continue;
            }
            FILE* test = fopen(fname, "rb");
            if(test) {
                fclose(test);
                std::cout << "Файл с таким именем уже существует, выберете другое название\n";
                continue;
            }
            out = fopen(fname, "w");
            if(out) {
                fprintf(out, "Результаты: товар \"%s\", после %02d.%02d.%04d\n\n", name, d, m, y);
                break;
            }
        }
    }

    Product p;
    int idx = 0, found = 0;
    std::cout << "\nТовары \"" << name << "\" после " << d << "." << m << "." << y << ":\n";
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        idx++;
        if(strcmp(p.name, name) == 0 && isDateAfter(p, d, m, y)) {
            found++;
            std::cout << "#" << idx << ": " << p.name 
                      << ", кол-во: " << p.quantity
                      << ", цена: " << p.pricePerOne
                      << ", дата: " << p.arrivalDate.day << "."
                      << p.arrivalDate.month << "." << p.arrivalDate.year << std::endl;
            if(out){
                fprintf(out, "#%d: %s, кол-во: %d, цена: %.2f, масса: %.3f, дата: %02d.%02d.%04d\n",
                                idx, p.name, p.quantity, p.pricePerOne, p.weightPerOne,
                                p.arrivalDate.day, p.arrivalDate.month, p.arrivalDate.year);
            }
        }
    }
    if(found == 0) {
        std::cout << "Не найдено\n";
        if(out) {
            fclose(out);
            remove(fname);
        }
    } else {
        std::cout << "\nНайдено: " << found << "\n";
        if(out) {
            fclose(out);
            std::cout << "Сохранено\n";
        }
    }
}


void generateReport(FILE* file) {
    if(globalsize == 0){
        std::cout << "Нет записей\n";
        return;
    }
    rewind(file);

    FILE* report = fopen(reportfile, "w");

    fprintf(report, "ОТЧЕТ ПО ТОВАРАМ\n");
    fprintf(report, "Файл: %s\n", currentFilename);
    fprintf(report, "Записей: %d\n\n", globalsize);
    
    fprintf(report, "%-4s %-20s %-8s %-10s %-10s %-12s\n", 
            "N", "Name", "Qty", "Price", "Weight", "Date");
    fprintf(report, "------------------------------------------------------------------\n");
    Product p;
    int idx = 0;
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        idx++;
        fprintf(report, "%-4d %-20s %-8d %-10.2f %-10.3f %02d.%02d.%04d\n",
                idx, p.name, p.quantity, p.pricePerOne, p.weightPerOne,
                p.arrivalDate.day, p.arrivalDate.month, p.arrivalDate.year);

    }
    fclose(report);
    std::cout << "Отчет сохранен\n";
}

void generateRandomData(FILE* file) {
    if(strlen(currentFilename) == 0) {
        std::cout << "Выберите файл\n";
        return;
    }
    rewind(file);
    srand(time(NULL));
    const char* names[] = {"Brick", "Cement", "Sand", "Board", "Nails", "Hammer", "Saw", "Drill", "Paint", "Brush"};
    for(int i = 0; i < 20; i++) {
        Product p;
        strcpy(p.name, names[rand() % 10]);
        p.quantity = rand() % 100 + 1;
        p.pricePerOne = (rand() % 9901 + 100) / 100.0;
        p.weightPerOne = (rand() % 5001 + 100) / 100.0;
        p.arrivalDate.day = rand() % 28 + 1;
        p.arrivalDate.month = rand() % 12 + 1;
        p.arrivalDate.year = 2025;
        fwrite(&p, sizeof(Product), 1, file);
    }
    globalsize = 20;
    std::cout << "Сгенерировано 20 записей\n";
}

int main() {
    FILE* file = nullptr;
    int choice;
    int fc;
    bool flag = true;
    while(flag){
        while(true){
            flag = false;
            std::cout << "1-Создать, 2-Выбрать: ";
            std::cin >> fc;
            if(fc != 1 && fc != 2){
                clearInputStream();
                std::cout << "Ошибка\n";
            }
            if(fc == 1){
                createNewFile(&file);
                break;
            }
            else if(fc == 2) {
                selectFile(&file, flag);
                break;
            }
        }
    }
    do {
        std::cout << "\n--- Учет товаров ---\n";        
        std::cout << "\n0. Работа с файлами (1-создать, 2-выбрать)\n";
        std::cout << "1. Просмотр\n2. Добавить\n3. Удалить\n4. Редактировать\n";
        std::cout << "5. Сортировка\n6. Поиск по имени\n7. Поиск по цене\n";
        std::cout << "8. Задание 1 (кол-во<N и дата=D)\n9. Задание 2 (товары после даты)\n";
        std::cout << "10. Отчет\n11. Случайные данные\n12. Выход\n";
        std::cout << "Выбор: ";
        std::cin >> choice;

        if(std::cin.fail()) {
            clearInputStream();
            std::cout << "Ошибка\n";
            continue;
        }

        switch (choice) {
            case 0: {
                int fc;
                std::cout << "1-Создать, 2-Выбрать: ";
                std::cin >> fc;
                if(fc == 1){
                    createNewFile(&file);
                }
                else if(fc == 2){
                    selectFile(&file, flag);
                }
                break;
            }
            case 1: viewRecords(file); break;
            case 2: addRecord(file); break;
            case 3: deleteRecord(file); break;
            case 4: editRecord(file); break;
            case 5: sortMenu(file); break;
            case 6: linearSearchByName(file); break;
            case 7: binarySearchByPrice(file); break;
            case 8: task1(file); break;
            case 9: task2(file); break;
            case 10: generateReport(file); break;
            case 11: generateRandomData(file); break;
            case 12: std::cout << "До свидания\n"; break;
            default: std::cout << "Неверно\n";
        }
    } while (choice != 12);
    return 0;
}