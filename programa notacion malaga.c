#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_TABLA 4000
#define MAX 100

typedef struct {
	char prefijo[40];
	int codigo;
} Entrada;

Entrada tabla[MAX_TABLA];
int total = 0;

/* ================= NORMALIZAR ================= */
void normalizar(char *s) {
	char out[MAX];
	int j = 0;
	
	for (int i = 0; s[i]; i++) {
		unsigned char c = s[i];
		
		if (c >= 'a' && c <= 'z')
			out[j++] = c - 32;
		else if (c >= 'A' && c <= 'Z')
			out[j++] = c;
	}
	out[j] = '\0';
	strcpy(s, out);
}

/* ================= INICIAL MALAGA ================= */
const char* inicial_malaga(const char *apellido) {
	if (strncmp(apellido, "CH", 2) == 0) return "CH";
	if (strncmp(apellido, "LL", 2) == 0) return "LL";
	
	static char letra[2];
	letra[0] = apellido[0];
	letra[1] = '\0';
	return letra;
}

/* ================= TABLA AUXILIAR ================= */
int valor_aux(char c) {
	c = toupper(c);
	if (c <= 'B') return 1;
	if (c <= 'D') return 2;
	if (c <= 'G') return 3;
	if (c <= 'K') return 4;
	if (c <= 'N') return 5;
	if (c <= 'P') return 6;
	if (c <= 'S') return 7;
	if (c <= 'U') return 8;
	return 9;
}

/* ================= CARGAR TABLA ================= */
void cargar_tabla() {
	FILE *f = fopen("tabla_malaga.txt", "r");
	if (!f) {
		printf("No se pudo abrir tabla_malaga.txt\n");
		return;
	}
	
	while (fscanf(f, "%s %d", tabla[total].prefijo, &tabla[total].codigo) == 2) {
		normalizar(tabla[total].prefijo);
		total++;
	}
	fclose(f);
}

/* ================= BUSCAR EXACTO ================= */
int buscar_exacto(const char *p) {
	for (int i = 0; i < total; i++)
		if (strcmp(tabla[i].prefijo, p) == 0)
			return tabla[i].codigo;
	return -1;
}

/* ================= INMEDIATO ANTERIOR ================= */
int buscar_anterior(const char *p3) {
	char mejor[10] = "";
	int codigo = -1;
	
	for (int i = 0; i < total; i++) {
		if (strlen(tabla[i].prefijo) == 3 &&
			strcmp(tabla[i].prefijo, p3) < 0 &&
			strcmp(tabla[i].prefijo, mejor) > 0) {
			
			strcpy(mejor, tabla[i].prefijo);
			codigo = tabla[i].codigo;
		}
	}
	return codigo;
}

/* ================= GENERAR NOTACION ================= */
void generar_notacion(char *apellido) {
	char pref[10];
	int codigo;
	
	printf("%s\n", apellido);
	normalizar(apellido);
	printf("%s\n", apellido);
	
	/* 1?? Apellido completo */
	codigo = buscar_exacto(apellido);
	if (codigo != -1) {
		printf("Apellido completo encontrado en la tabla\n");
		printf("Notacion: %s%d\n", inicial_malaga(apellido), codigo);
		return;
	}
	
	/* 2?? Reducir 5 ? 4 ? 3 ? 2 */
	for (int n = 5; n >= 2; n--) {
		if ((int)strlen(apellido) >= n) {
			strncpy(pref, apellido, n);
			pref[n] = '\0';
			
			codigo = buscar_exacto(pref);
			if (codigo != -1) {
				printf("Notacion: %s%d\n", inicial_malaga(apellido), codigo);
				return;
			}
		}
	}
	
	/* 3?? Inmediato anterior + auxiliar */
	strncpy(pref, apellido, 3);
	pref[3] = '\0';
	
	codigo = buscar_anterior(pref);
	if (codigo == -1) {
		printf("Apellido NO esta en la tabla\n");
		return;
	}
	
	int aux = valor_aux(apellido[2]);
	printf("Notacion: %s%d%d\n", inicial_malaga(apellido), codigo, aux);
}

/* ================= MAIN ================= */
int main() {
	char apellido[50];
	
	cargar_tabla();
	
	do {
		printf("\nIngrese apellido sin tildes (0 para salir): ");
		fgets(apellido, sizeof(apellido), stdin);
		apellido[strcspn(apellido, "\n")] = 0;
		
		if (strcmp(apellido, "0") == 0) {
			printf("\nSaliendo del programa...\n");
			break;
		}
		
		generar_notacion(apellido);
		
	} while (1);
	return 0;
}
