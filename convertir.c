#include <stdio.h>
#include <string.h>

int main() {
    FILE *in = fopen("tabla_malaga.txt", "r");
    FILE *out = fopen("tabla_malaga_rangos.txt", "w");

    if (!in || !out) {
        printf("Error abriendo archivos\n");
        return 1;
    }

    char prefijo[100];
    int codigo;

    while (fscanf(in, "%99s %d", prefijo, &codigo) == 2) {
        char *p = strchr(prefijo, '_');
        if (p && strlen(p) == 4) {
            /* Formato APELLIDO_A-M */
            char apellido[100];
            strncpy(apellido, prefijo, p - prefijo);
            apellido[p - prefijo] = '\0';

            char ini = p[1];
            char fin = p[3];

            fprintf(out, "%s %c %c %d\n", apellido, ini, fin, codigo);
        }
    }

    fclose(in);
    fclose(out);

    printf("tabla_malaga_rangos.txt generado correctamente\n");
    return 0;
}
