import unicodedata

MAX_TABLA = 4000

tabla = []


# ================= NORMALIZAR =================
def normalizar(texto: str) -> str:
    """
    - Convierte a MAYÚSCULAS
    - Elimina tildes
    - Conserva Ñ
    - Mantiene espacios
    """
    texto = texto.upper()

    resultado = []
    for c in texto:
        if c == "Ñ":
            resultado.append("Ñ")
        else:
            # Quitar tildes
            c_norm = unicodedata.normalize("NFD", c)
            c_norm = "".join(
                x for x in c_norm if unicodedata.category(x) != "Mn"
            )
            if c_norm.isalpha() or c_norm == " ":
                resultado.append(c_norm)

    return "".join(resultado)


# ================= INICIAL MALAGA =================
def inicial_malaga(apellido: str) -> str:
    if apellido.startswith("CH"):
        return "CH"
    if apellido.startswith("LL"):
        return "LL"
    if apellido.startswith("Ñ"):
        return "Ñ"
    return apellido[0]


# ================= TABLA AUXILIAR =================
def valor_aux(c: str) -> int:
    c = c.upper()
    if c <= "B":
        return 1
    if c <= "D":
        return 2
    if c <= "G":
        return 3
    if c <= "K":
        return 4
    if c <= "N" or c == "Ñ":
        return 5
    if c <= "P":
        return 6
    if c <= "S":
        return 7
    if c <= "U":
        return 8
    return 9


# ================= CARGAR TABLA =================
def cargar_tabla():
    try:
        with open("tabla_malaga.txt", encoding="utf-8") as f:
            for linea in f:
                prefijo, codigo = linea.split()
                tabla.append(
                    (normalizar(prefijo), int(codigo))
                )
    except FileNotFoundError:
        print("No se pudo abrir tabla_malaga.txt")


# ================= BUSQUEDAS =================
def buscar_exacto(pref: str) -> int:
    for p, c in tabla:
        if p == pref:
            return c
    return -1


def buscar_anterior(pref3: str) -> int:
    mejor = ""
    codigo = -1
    for p, c in tabla:
        if len(p) == 3 and p < pref3 and p > mejor:
            mejor = p
            codigo = c
    return codigo


# ================= RANGOS SEGUNDO APELLIDO =================
def apellido_tiene_rangos(apellido1: str) -> bool:
    try:
        with open("tabla_malaga_rangos.txt", encoding="utf-8") as f:
            for linea in f:
                ap, _, _, _ = linea.split()
                if normalizar(ap) == apellido1:
                    return True
    except FileNotFoundError:
        pass
    return False


def codigo_por_segundo_apellido(apellido1: str, apellido2: str) -> int:
    letra = apellido2[0]
    try:
        with open("tabla_malaga_rangos.txt", encoding="utf-8") as f:
            for linea in f:
                ap, ini, fin, codigo = linea.split()
                ap = normalizar(ap)
                ini = normalizar(ini)
                fin = normalizar(fin)

                if ap == apellido1 and ini <= letra <= fin:
                    return int(codigo)
    except FileNotFoundError:
        pass
    return -1


# ================= PALABRAS IGNORADAS =================
IGNORADAS = {
    "DE", "DEL", "LA", "LAS", "EL", "LOS",
    "Y", "E", "O", "U", "A", "AL",
    "EN", "CON", "SIN", "POR", "PARA"
}


def extraer_iniciales(nombre: str) -> str:
    tokens = normalizar(nombre).split()
    iniciales = []

    for i, t in enumerate(tokens):
        if i == 0:
            continue
        if t.isdigit():
            continue
        if t in IGNORADAS:
            continue
        iniciales.append(t[0])

    return "".join(iniciales)


# ================= GENERAR NOTACION =================
def generar_notacion(apellido: str):
    apellido = normalizar(apellido)

    codigo = buscar_exacto(apellido)
    if codigo != -1:
        if apellido_tiene_rangos(apellido):
            apellido2 = input("Segundo apellido (0 para omitir): ").strip()
            if apellido2 != "0":
                apellido2 = normalizar(apellido2)
                cr = codigo_por_segundo_apellido(apellido, apellido2)
                codigo = cr if cr != -1 else codigo

        print("Se encontró apellido completo")
        print(f"Notacion: {inicial_malaga(apellido)}{codigo}")
        return

    # Prefijos 5 → 2
    for n in range(5, 1, -1):
        if len(apellido) >= n:
            pref = apellido[:n]
            codigo = buscar_exacto(pref)
            if codigo != -1:
                print(f"Prefijo usado: {pref}")
                print(f"Notacion: {inicial_malaga(apellido)}{codigo}")
                return

    # Prefijo 3 + auxiliar
    pref3 = apellido[:3]
    codigo = buscar_anterior(pref3)
    if codigo == -1:
        print("Apellido NO está en la tabla")
        return

    aux = valor_aux(apellido[2])
    print(f"Notacion: {inicial_malaga(apellido)}{codigo}{aux}")


# ================= MAIN =================
def main():
    cargar_tabla()

    while True:
        print("\n1. Autor personal")
        print("2. Usar tabla auxiliar")
        print("0. Salir")
        opcion = input("Opcion: ").strip()

        if opcion == "0":
            break

        if opcion == "1":
            ap = input("Apellido sin tildes: ")
            generar_notacion(ap)

        elif opcion == "2":
            c = input("Ingrese una letra: ").strip()
            if len(c) != 1 or not c.isalpha():
                print("Error: debe ingresar una letra")
            else:
                print("Valor auxiliar:", valor_aux(normalizar(c)))


if __name__ == "__main__":
    main()
