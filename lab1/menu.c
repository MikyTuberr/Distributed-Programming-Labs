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
}

void handle_erase() {
    unsigned int index;
    printf("Podaj indeks elementu do usunięcia: ");
    scanf("%u", &index);
    erase(index);
}

void handle_input() {
    int choice = 0;
    system("clear");
    
    while (1) {
        menu();
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                handle_insert();
                system("clear");
                break;
            case 2:
                system("clear");
                traverse();
                break;
            case 3:
                handle_erase();
                system("clear");
                break;
            case 4:
                free_list();
                return;
            default:
                system("clear");
            }
    }

}