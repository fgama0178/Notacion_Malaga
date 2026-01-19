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
		else if (c == ' ')
			out[j++] = ' ';
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

/* ================= FUNCIONES SEGUNDO APELLIDO ================= */
int apellido_tiene_rangos(const char *apellido1) {
	FILE *f = fopen("tabla_malaga_rangos.txt", "r");
	if (!f) return 0;
	
	char ap[40];
	char ini, fin;
	int codigo;
	
	while (fscanf(f, "%39s %c %c %d", ap, &ini, &fin, &codigo) == 4) {
		normalizar(ap);
		if (strcmp(ap, apellido1) == 0) {
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
		normalizar(ap);
		
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

/* ================= PALABRAS A IGNORAR ================= */
int es_palabra_ignorada(const char *palabra) {
	const char *ignoradas[] = {
		"DE", "DEL", "LA", "LAS", "EL", "LOS",
		"Y", "E", "O", "U", "A", "AL", "EN",
		"CON", "SIN", "POR", "PARA", NULL
	};
	
	for (int i = 0; ignoradas[i] != NULL; i++) {
		if (strcmp(palabra, ignoradas[i]) == 0)
			return 1;
	}
	return 0;
}

/* ================= EXTRAER INICIALES (INSTITUCIONES) ================= */
void extraer_iniciales(const char *nombre, char *iniciales) {
	char copia[MAX];
	strcpy(copia, nombre);
	
	iniciales[0] = '\0';
	int primera = 1;
	
	char *token = strtok(copia, " ");
	while (token != NULL) {
		/* Verificar que no sea solo números */
		int es_numero = 1;
		for (int i = 0; token[i]; i++) {
			if (!isdigit(token[i])) {
				es_numero = 0;
				break;
			}
		}
		
		/* Saltar primera palabra, números, y palabras ignoradas */
		if (!primera && !es_numero && !es_palabra_ignorada(token)) {
			char letra[2];
			letra[0] = token[0];
			letra[1] = '\0';
			strcat(iniciales, letra);
		}
		
		primera = 0;
		token = strtok(NULL, " ");
	}
}

/* ================= GENERAR NOTACION INSTITUCIONAL ================= */
void generar_notacion_institucional(char *nombre) {
	char primera_palabra[50];
	char iniciales[20];
	char pref[10];
	int codigo;
	
	normalizar(nombre);
	
	/* Extraer primera palabra */
	sscanf(nombre, "%s", primera_palabra);
	
	/* 1) PALABRA COMPLETA */
	codigo = buscar_exacto(primera_palabra);
	if (codigo != -1) {
		printf("Primera palabra encontrada en la tabla\n");
		extraer_iniciales(nombre, iniciales);
		printf("Notacion: %s%d -> %s%d%s\n",
			   inicial_malaga(primera_palabra), codigo,
			   inicial_malaga(primera_palabra), codigo / 10, iniciales);
		return;
	}

	/* 2) REDUCCION 5 → 4 → 3 → 2 */
	for (int n = 5; n >= 2; n--) {
		if ((int)strlen(primera_palabra) >= n) {
			strncpy(pref, primera_palabra, n);
			pref[n] = '\0';

			codigo = buscar_exacto(pref);
			if (codigo != -1) {
				extraer_iniciales(nombre, iniciales);
				printf("Notacion: %s%d -> %s%d%s\n",
					   inicial_malaga(primera_palabra), codigo,
					   inicial_malaga(primera_palabra), codigo / 10, iniciales);
				return;
			}
		}
	}

	/* 3) INMEDIATO ANTERIOR + AUXILIAR */
	strncpy(pref, primera_palabra, 3);
	pref[3] = '\0';

	codigo = buscar_anterior(pref);
	if (codigo == -1) {
		printf("Primera palabra NO esta en la tabla\n");
		return;
	}

	int aux = valor_aux(primera_palabra[2]);
	int codigo_completo = codigo * 10 + aux;
	
	extraer_iniciales(nombre, iniciales);
	printf("Notacion: %s%d -> %s%d%s\n",
		   inicial_malaga(primera_palabra), codigo_completo,
		   inicial_malaga(primera_palabra), codigo_completo / 10, iniciales);
}

/* ================= GENERAR NOTACION (APELLIDOS) ================= */
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

void menu() {
	printf("=====================================\n");
	printf("   Generador de Notacion Malaga V3   \n");
	printf("=====================================\n");
	printf(" Opciones:\n");
	printf(" 1. Modo Apellido\n");
	printf(" 2. Modo Institucional\n");
	printf(" 0. Salir\n");
	printf("=====================================\n");
}

/* ================= MAIN ================= */
int main() {
	char entrada[200];
	int opcion;

	cargar_tabla();

	do {
		menu();
		printf("\nSeleccione opcion: ");
		scanf("%d", &opcion);
		getchar(); // Limpiar buffer

		if (opcion == 0) {
			printf("\nSaliendo del programa...\n");
			break;
		}

		if (opcion == 1) {
			printf("\nIngrese apellido sin tildes: ");
			fgets(entrada, sizeof(entrada), stdin);
			entrada[strcspn(entrada, "\n")] = 0;
			generar_notacion(entrada);
		} 
		else if (opcion == 2) {
			printf("\nIngrese nombre institucional completo sin tildes: ");
			fgets(entrada, sizeof(entrada), stdin);
			entrada[strcspn(entrada, "\n")] = 0;
			generar_notacion_institucional(entrada);
		}
		else {
			printf("\nOpcion invalida\n");
		}

	} while (1);

	return 0;
}