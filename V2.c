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
/* SOLO MAYUSCULAS (usuario ingresa sin tildes) */
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

/* ================= CARGAR TABLA BASE ================= */
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

/* ================================================= */
/* ===== FUNCIONES NUEVAS PARA SEGUNDO APELLIDO ==== */
/* ================================================= */

/* ¿Este apellido tiene rangos? */
int apellido_tiene_rangos(const char *apellido1) {
	FILE *f = fopen("tabla_malaga_rangos.txt", "r");
	if (!f) return 0;
	
	char ap[40];
	char ini, fin;
	int codigo;
	
	while (fscanf(f, "%39s %c %c %d", ap, &ini, &fin, &codigo) == 4) {
		normalizar(ap);                  // <- CLAVE
		if (strcmp(ap, apellido1) == 0) { // apellido1 ya viene normalizado
			fclose(f);
			return 1;
		}
	}
	fclose(f);
	return 0;
}

int codigo_por_segundo_apellido(const char *apellido1, const char *apellido2) {
	FILE *f = fopen("tabla_malaga_rangos.txt", "r");
	if (!f) return -1;
	
	char ap[40];
	char ini, fin;
	int codigo;
	
	char letra = toupper((unsigned char)apellido2[0]);
	
	while (fscanf(f, "%39s %c %c %d", ap, &ini, &fin, &codigo) == 4) {
		normalizar(ap); // <- CLAVE
		
		ini = toupper((unsigned char)ini);
		fin = toupper((unsigned char)fin);
		
		if (strcmp(ap, apellido1) == 0) {
			if (letra >= ini && letra <= fin) {
				fclose(f);
				return codigo;
			}
		}
	}
	fclose(f);
	return -1;
}

/* ================= GENERAR NOTACION ================= */
void generar_notacion(char *apellido) {
	char pref[10];
	int codigo;
	
	normalizar(apellido);

	/* 1) APELLIDO COMPLETO */
	codigo = buscar_exacto(apellido);
	if (codigo != -1) {

		/* SI TIENE RANGOS, PEDIR SEGUNDO APELLIDO */
		if (apellido_tiene_rangos(apellido)) {
			char apellido2[50];

			printf("Apellido completo encontrado.\n");
			printf("\033[1;31mSegundo apellido (0 para omitir): \033[0m");
			fgets(apellido2, sizeof(apellido2), stdin);
			apellido2[strcspn(apellido2, "\n")] = 0;

			if (strcmp(apellido2, "0") == 0) {
				printf("Notacion: %s%d\n",
					   inicial_malaga(apellido), codigo);
				return;
			}

			normalizar(apellido2);
			int cod_rango = codigo_por_segundo_apellido(apellido, apellido2);
			
			if (cod_rango == -1) {
				/* Seguridad: si no se encuentra rango, usar codigo base */
				printf("Notacion: %s%d\n", inicial_malaga(apellido), codigo);
			} else {
				printf("Notacion: %s%d\n", inicial_malaga(apellido), cod_rango);
			}
			return;
		}

		/* SI NO TIENE RANGOS */
		printf("Apellido completo encontrado en la tabla\n");
		printf("Notacion: %s%d\n",
			   inicial_malaga(apellido), codigo);
		return;
	}

	/* 2) REDUCCION 5 → 4 → 3 → 2 */
	for (int n = 5; n >= 2; n--) {
		if ((int)strlen(apellido) >= n) {
			strncpy(pref, apellido, n);
			pref[n] = '\0';

			codigo = buscar_exacto(pref);
			if (codigo != -1) {
				printf("Notacion: %s%d\n",
					   inicial_malaga(apellido), codigo);
				return;
			}
		}
	}

	/* 3) INMEDIATO ANTERIOR + AUXILIAR */
	strncpy(pref, apellido, 3);
	pref[3] = '\0';

	codigo = buscar_anterior(pref);
	if (codigo == -1) {
		printf("Apellido NO esta en la tabla\n");
		return;
	}

	int aux = valor_aux(apellido[2]);
	printf("Notacion: %s%d%d\n",
		   inicial_malaga(apellido), codigo, aux);
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
