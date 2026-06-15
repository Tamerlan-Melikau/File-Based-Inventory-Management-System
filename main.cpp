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
    std::cout << "Enter name for new file: ";
    std::cin >> filename;
    if(strcmp(filename, currentFilename) == 0){
        std::cout << "File already created and ready for use\n";
        return;
    }
    FILE* test = fopen(filename, "rb");
    if(test) {
        fclose(test);
        std::cout << "File with this name already exists, choose another name\n";
        return;
    }
    if(strlen(currentFilename) != 0){
        fclose(*file);
    }
    *file = fopen(filename, "wb+");
    globalsize = getRecordCount(*file);
    strcpy(currentFilename, filename);
    std::cout << "Created file: " << filename <<". This file is now active and ready for use." << "\n";
}

void selectFile(FILE** file, bool& flag) {
    char filename[maxlenght];
    std::cout << "Enter file name: ";
    std::cin >> filename;
    if(strcmp(filename, currentFilename) == 0){
        std::cout << "File already selected";
        return;
    }
    if(*file) {
        fclose(*file);
    }
    *file = fopen(filename, "rb+");
    if(!*file){
        std::cout << "File with this name does not exist" << "\n";
        *file = fopen(currentFilename, "rb+");
        if(strlen(currentFilename) != 0){
            std::cout << "Current file remains " << "'" << currentFilename << "'" << "\n";
            return;
        }
        else{
            std::cout << "Select an existing file or create a new one\n";
            flag = true;
            return;
        }
    }
    globalsize = getRecordCount(*file);
    strcpy(currentFilename, filename);
    std::cout << "Selected file: " << filename << "\n";
}

void addRecord(FILE* file) {
    Product p;
    std::cout << "Name: ";
    std::cin >> p.name;
    while (true) {
        std::cout << "Quantity: ";
        std::cin >> p.quantity;
        if(std::cin.fail() || p.quantity < 1){
            clearInputStream();
            std::cout << "Error!\n";
        } else{
            break;
        }
    }
    while (true) {
        std::cout << "Price: ";
        std::cin >> p.pricePerOne;
        if(std::cin.fail() || p.pricePerOne <= 0){
            clearInputStream();
            std::cout << "Error!\n";
        }else{
            break;
        }
    }
    while (true) {
        std::cout << "Weight (kg): ";
        std::cin >> p.weightPerOne;
        if(std::cin.fail() || p.weightPerOne < 1){
            clearInputStream();
            std::cout << "Error!\n";
        } else{
            break;
        }
    }
    while (true) {
        std::cout << "Date (dd mm yyyy): ";
        std::cin >> p.arrivalDate.day >> p.arrivalDate.month >> p.arrivalDate.year;
        if(p.arrivalDate.month == 02){
            int maxDay = 28;
            if(p.arrivalDate.year % 4 == 0) {
                maxDay = 29;
            }
            if(p.arrivalDate.day > maxDay) {
                clearInputStream();
                std::cout << "Error!\n";
            }else{
                break;
            }
        }else{
            if(std::cin.fail() || p.arrivalDate.day > 31 || p.arrivalDate.month > 12){
                clearInputStream();
                std::cout << "Error!\n"; 
            } else{
                break;
            }
        }
    }
    globalsize++;
    fseek(file, 0, SEEK_END);
    fwrite(&p, sizeof(Product), 1, file);
    std::cout << "Record added\n";
}

void viewRecords(FILE* file) {
    if(globalsize == 0) {
        std::cout << "No records\n";
        return;
    }
    rewind(file);
    Product p;
    std::cout << "\n------------------------------- File Contents --------------------------------\n";
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
        std::cout << "No records\n";
        return;
    }
    int num;
    std::cout << "Record number (1-" << globalsize << "): ";
    std::cin >> num;
    if(std::cin.fail() || num < 1 || num > globalsize) {
        clearInputStream();
        std::cout << "Invalid number\n";
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
    std::cout << "Record " << num << " deleted\n";
}

void editRecord(FILE* file) {
    if(globalsize == 0) {
        std::cout << "No records\n";
        return;
    }
    int num;
    std::cout << "Record number (1-" << globalsize << "): ";
    std::cin >> num;
    if(std::cin.fail() || num < 1 || num > globalsize) {
        clearInputStream();
        std::cout << "Invalid number\n";
        return;
    }

    fseek(file, (num - 1)*sizeof(Product), SEEK_SET);
    Product p;
    fread(&p, sizeof(Product), 1, file);

    while (true){
        std::cout << "\n--- Record Contents ---\n";
        std::cout << "1. Name\n";
        std::cout << "2. Qty\n";
        std::cout << "3. Price\n";
        std::cout << "4. Weight\n";
        std::cout << "5. Date\n";
        std::cout << "0. Save and exit\n";
        std::cout << "Enter record number: ";
        int choice;
        std::cin >> choice;
        if(choice == 0) {
            break;
        }
        switch (choice){
        case 1:
            std::cout << "Enter new name: ";
            std::cin >> p.name;
            break;
        case 2:
            std::cout << "Enter new quantity: ";
            std::cin >> p.quantity;
            break;
        case 3:
            std::cout << "Enter new price: ";
            std::cin >> p.pricePerOne;
            break;
        case 4:
            std::cout << "Enter new weight: ";
            std::cin >> p.weightPerOne;
            break;
        case 5:
            std::cout << "Enter new date: ";
            std::cin >> p.arrivalDate.day >> p.arrivalDate.month >> p.arrivalDate.year;
        default:
            break;
        }
    }
    fseek(file, (num - 1)*sizeof(Product), SEEK_SET);
    fwrite(&p, sizeof(Product), 1, file);
    std::cout << "Record modified\n";
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
    std::cout << "Sorting by quantity completed\n";
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
    std::cout << "Sorting by weight completed\n";
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
    std::cout << "Sorting by price completed\n";
}

void sortMenu(FILE* file) {
    if(globalsize < 1) {
        std::cout << "Not enough records for sorting\n";
        return;
    }

    int choice;
    std::cout << "\n--- Sorting ---\n";
    std::cout << "1. By quantity (asc)\n";
    std::cout << "2. By quantity (desc)\n";
    std::cout << "3. By weight (asc)\n";
    std::cout << "4. By weight (desc)\n";
    std::cout << "5. By price (asc)\n";
    std::cout << "6. By price (desc)\n";
    std::cout << "Choice: ";
    std::cin >> choice;

    switch (choice) {
        case 1: quickSortQuantity(true, file, globalsize); break;
        case 2: quickSortQuantity(false, file, globalsize); break;
        case 3: selectionSortByWeight(true, file, globalsize); break;
        case 4: selectionSortByWeight(false, file, globalsize); break;
        case 5: insertionSortByPrice(true, file, globalsize); break;
        case 6: insertionSortByPrice(false, file, globalsize); break;
        default: std::cout << "Invalid choice\n";
    }
}

void linearSearchByName(FILE* file) {
    if(globalsize == 0){
        std::cout << "No records\n";
        return;
    }
    rewind(file);
    char query[maxlenght];
    std::cout << "Name to search: ";
    std::cin >> query;
    Product p;
    int found = 0;
    std::cout << "\nSearch results for \"" << query << "\":\n";
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        if(strcmp(p.name, query) == 0) {
            found++;
            std::cout << found << ") " << p.name 
                      << ", qty: " << p.quantity
                      << ", price: " << p.pricePerOne
                      << ", weight: " << p.weightPerOne
                      << ", date: " << p.arrivalDate.day << "." 
                      << p.arrivalDate.month << "." << p.arrivalDate.year << std::endl;
        }
    }
    if(found == 0) std::cout << "Not found\n";
}

void binarySearchByPrice(FILE* file) {
    if(globalsize == 0) {
        std::cout << "No data\n";
        return;
    }
    double target;
    std::cout << "Price to search: ";
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
        std::cout << "\nFound " << (last - first + 1) << " records:\n";
        for(int i = first; i <= last; i++) {
            fseek(file, i*sizeof(Product), SEEK_SET);
            fread(&curr, sizeof(Product), 1, file);
            std::cout << "#" << (i+1) << ": " << curr.name 
                      << ", qty: " << curr.quantity
                      << ", price: " << curr.pricePerOne
                      << ", date: " << curr.arrivalDate.day << "."
                      << curr.arrivalDate.month << "." << curr.arrivalDate.year << std::endl;
        }
    }
    else std::cout << "Not found\n";
}

void task1(FILE* file) {
    if(globalsize == 0){
        std::cout << "No records\n";
        return;
    }
    rewind(file);

    char choice;
    std::cout << "Would you like to save the data? (Yes - y | No - n)\n";
    std::cin >> choice;

    int qty, d, m, y;
    std::cout << "Quantity to compare: ";
    std::cin >> qty;
    std::cout << "Date (dd mm yyyy): ";
    std::cin >> d >> m >> y;
    if(std::cin.fail()) {
        clearInputStream();
        std::cout << "Input error\n";
        return;
    }

    FILE* out = nullptr;
    char fname[maxlenght] = "";
    if(choice == 'y'){
        while(true){
            std::cout << "File name: ";
            std::cin >> fname;
            if(strcmp(fname, currentFilename) == 0){
                std::cout << "This is the main file name, need a different name\n";
                continue;
            }
            FILE* test = fopen(fname, "rb");
            if(test) {
                fclose(test);
                std::cout << "File with this name already exists, choose another name\n";
                continue;
            }
            out = fopen(fname, "w");
            if(out) {
                fprintf(out, "Results: qty < %d, date %02d.%02d.%04d\n\n", qty, d, m, y);
                break;
            }
        }
    }

    Product p;
    int idx = 0, count = 0;
    std::cout << "\nProducts (qty < " << qty << ", date " << d << "." << m << "." << y << "):\n";
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        idx++;
        if(p.quantity < qty && isSameDate(p, d, m, y)){
            count++;
            std::cout << "#" << idx << ": " << p.name 
                      << ", qty: " << p.quantity
                      << ", price: " << p.pricePerOne
                      << ", date: " << p.arrivalDate.day << "."
                      << p.arrivalDate.month << "." << p.arrivalDate.year << std::endl;
            if(out){
                fprintf(out, "%d: %s, qty: %d, price: %.2f, weight: %.3f, date: %02d.%02d.%04d\n",
                                idx, p.name, p.quantity, p.pricePerOne, p.weightPerOne,
                                p.arrivalDate.day, p.arrivalDate.month, p.arrivalDate.year);
            }
        }
    }
    if(count == 0) {
        std::cout << "Not found\n";
        if(out) {
            fclose(out);
            remove(fname);
        }
    } else {
        std::cout << "\nFound: " << count << "\n";
        if(out) {
            fclose(out);
            std::cout << "Saved\n";
        }
    }
}

void task2(FILE* file){
    if(globalsize == 0){
        std::cout << "No records\n";
        return;
    }
    rewind(file);

    char choice;
    std::cout << "Would you like to save the data? (Yes - y | No - n)\n";
    std::cin >> choice;

    char name[maxlenght];
    int d, m, y;
    std::cout << "Name: ";
    std::cin >> name;
    std::cout << "Date (dd mm yyyy): ";
    std::cin >> d >> m >> y;
    if(std::cin.fail()) {
        clearInputStream();
        std::cout << "Input error\n";
        return;
    }

    FILE* out = nullptr;
    char fname[30] = "";
    if(choice == 'y'){
        while(true){
            std::cout << "File name: ";
            std::cin >> fname;
            if(strcmp(fname, currentFilename) == 0){
                std::cout << "This is the main file name, need a different name\n";
                continue;
            }
            FILE* test = fopen(fname, "rb");
            if(test) {
                fclose(test);
                std::cout << "File with this name already exists, choose another name\n";
                continue;
            }
            out = fopen(fname, "w");
            if(out) {
                fprintf(out, "Results: product \"%s\", after %02d.%02d.%04d\n\n", name, d, m, y);
                break;
            }
        }
    }

    Product p;
    int idx = 0, found = 0;
    std::cout << "\nProducts \"" << name << "\" after " << d << "." << m << "." << y << ":\n";
    while (fread(&p, sizeof(Product), 1, file) == 1) {
        idx++;
        if(strcmp(p.name, name) == 0 && isDateAfter(p, d, m, y)) {
            found++;
            std::cout << "#" << idx << ": " << p.name 
                      << ", qty: " << p.quantity
                      << ", price: " << p.pricePerOne
                      << ", date: " << p.arrivalDate.day << "."
                      << p.arrivalDate.month << "." << p.arrivalDate.year << std::endl;
            if(out){
                fprintf(out, "#%d: %s, qty: %d, price: %.2f, weight: %.3f, date: %02d.%02d.%04d\n",
                                idx, p.name, p.quantity, p.pricePerOne, p.weightPerOne,
                                p.arrivalDate.day, p.arrivalDate.month, p.arrivalDate.year);
            }
        }
    }
    if(found == 0) {
        std::cout << "Not found\n";
        if(out) {
            fclose(out);
            remove(fname);
        }
    } else {
        std::cout << "\nFound: " << found << "\n";
        if(out) {
            fclose(out);
            std::cout << "Saved\n";
        }
    }
}


void generateReport(FILE* file) {
    if(globalsize == 0){
        std::cout << "No records\n";
        return;
    }
    rewind(file);

    FILE* report = fopen(reportfile, "w");

    fprintf(report, "PRODUCT REPORT\n");
    fprintf(report, "File: %s\n", currentFilename);
    fprintf(report, "Records: %d\n\n", globalsize);
    
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
    std::cout << "Report saved\n";
}

void generateRandomData(FILE* file) {
    if(strlen(currentFilename) == 0) {
        std::cout << "Select a file\n";
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
    std::cout << "Generated 20 records\n";
}

int main() {
    FILE* file = nullptr;
    int choice;
    int fc;
    bool flag = true;
    while(flag){
        while(true){
            flag = false;
            std::cout << "1-Create, 2-Select: ";
            std::cin >> fc;
            if(fc != 1 && fc != 2){
                clearInputStream();
                std::cout << "Error\n";
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
        std::cout << "\n--- Product Management ---\n";        
        std::cout << "\n0. File operations (1-create, 2-select)\n";
        std::cout << "1. View\n2. Add\n3. Delete\n4. Edit\n";
        std::cout << "5. Sort\n6. Search by name\n7. Search by price\n";
        std::cout << "8. Task 1 (qty<N and date=D)\n9. Task 2 (products after date)\n";
        std::cout << "10. Report\n11. Random data\n12. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;

        if(std::cin.fail()) {
            clearInputStream();
            std::cout << "Error\n";
            continue;
        }

        switch (choice) {
            case 0: {
                int fc;
                std::cout << "1-Create, 2-Select: ";
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
            case 12: std::cout << "Goodbye\n"; break;
            default: std::cout << "Invalid\n";
        }
    } while (choice != 12);
    return 0;
}