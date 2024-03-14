#include "menu.h"

void menu() {
    printf("1. Wstaw napis\n");
    printf("2. Przejrzyj listę\n");
    printf("3. Usuń element o danym indeksie\n");
    printf("4. Wyjdź\n");
    printf("Wybierz opcję: ");
}

void handle_insert() {
    char* str = NULL;
    size_t len = 0;
    printf("Podaj napis do wstawienia: ");
    getline(&str, &len, stdin);
    str[strlen(str) - 1] = '\0';
    insert(str);
    free(str);
    system("clear");
}

void handle_erase() {
    unsigned int index;
    printf("Podaj indeks elementu do usunięcia: ");
    scanf("%u", &index);
    erase(index);
    system("clear");
}

void handle_traverse() {
    system("clear");
    traverse();
}

void handle_quit() {
    free_list();
    exit(0);
}


void handle_input() {
    system("clear");
    int choice = 0;
    void (*func_arr[])(void) = {handle_insert, handle_traverse, handle_erase, handle_quit};
    
    while (1) {
        menu();
        scanf("%d", &choice);
        getchar();
        if(choice >= 1 && choice <= 4) {
            (func_arr[choice - 1])();
        }
        else {
            system("clear");
        }
    }

}
