//  Programa per a compilar i avaluar notació polonesa inversa

//  Autor: Laura Figueras
//  NIU: 1597857

//  Descripció: El programa fa tot el demanat al guió i a més a més té les següents millores:    El codi que indica si és un número, una operació o una funció és un enter, utilitzant     un vector de structs amb unions dins i et llegeix l'entrada a partir de la terminal.

//  Instruccions: Al començar el programa et demanarà si vols saber les funcions i operacions   que accepta la calculadora, si és així hauràs d'escriure un 0, si no és així, escriu      qualsevol altre nombre. Després podràs començar a calcular. Per entrar els valors ben     escrits hauràs de posar-los entre espais i amb les funcions i operacions ben escrites.    Per exemple, si escrius tg, el programa no ho reconeixerà, ja que pel programa la         tangent és tan. Per aquesta raó a l'inici de la calculadora et dirà les funcions i els    seus noms. Un cop hagis calculat, et preguntarà si vols calcular una altra vegada. Si     és així prem el 0 i si no ho és prem qualsevol altre número (com al principi del          programa).

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//Aquest contador serveix per saber la posició en la qual estem quan operem amb la pila.
unsigned int onEstem;

//  No diferenciem entre les constants i els numeros.
typedef enum {numero, operacio, funcio} e_tipus;
typedef enum {suma, resta, multiplicacio, divisio, potencia, error_op} e_operacions;
typedef enum {sinus, cosinus, tangent, arcsinus, arccosinus, arctangent, arrel, expon, logaritme, factorial, error_f} e_funcions;

//  Definim un union perque per cada tipus d'estructura, els valors tenen valors diferents: els números son double, les operacions són e_operacions i les funcions són e_funcions. Hem definit e_operacions i e_funcions just a sobre.
typedef union {
    double d;
    e_operacions op;
    e_funcions f;
} u_valors;

//  Definim una estructura per guardar el tipus i el valor que pren cada element. Per exemple, sin és una funció i val pel sinus, pi és un número i val per 3.1415.
typedef struct {
    e_tipus tipus;
    u_valors valor;
} s_rpnElement;

//  Aquí definim algunes funcions senzilles que utilitzarem per a calcular.
double sum(double a, double b) {return a+b;}
double rest(double a, double b) {return a-b;}
double multi(double a, double b) {return a*b;}
double divi(double a, double b) {return a/b;}
double fact(int n) {
    double factorial = 1;
    for (int i = 1; i <= n; i++)
        factorial *= i;
    return factorial;
}

//  Aquestes funcions ens serviran per passar d'una cadena a un vector format per s_rpnElement.
s_rpnElement * conversio(char *);
int nombreParaules(char *);
s_rpnElement paraulaAElement(char *);
e_operacions esoperacio(char *str);
double esconstant(char *str);
e_funcions esfuncio(char *str);

//  Aquestes funcions ens serviran per calcular els resultats.
double calcular(char *str);
void empilar(double *, double);
double desempilar(double *);
double feroperacio(e_operacions operacio, double op1, double op2);
double ferfuncio(e_funcions funcio, double op1);

//  Aquesta funció ens serveix per tornar a activar la lectura per la terminal pel fgets.
void netejaStdin(void);

int main()
{
    onEstem = 0;
    double inici, repetir;
    char cadena[120];
    
    printf("Per mirar les operacions i les funcions acceptades per la calculadora escriu 0, per començar a calcular escriu qualsevol altre número.\n");
    scanf("%lf", &inici);
    
    if (inici == 0) {
        printf("La calculadora reconeix les seguents funcions:\n\n\t·Sinus (sin)\n\t·Cosinus (cos)\n\t·Tangent (tan)\n\n(Les funcions trigonomètriques retornen el resultat en radians)\n\n\t·Arcsinus (arcsin)\n\t·Arccosinus (arccos)\n\t·Arctangent (arctan)\n\t·Arrel quadrada (sqrt)\n\t·Exponencial (exp)\n\t·Logarítmica (log)\n\t·Factorial (!)\n\nLa calculadora fa les següents operacions:\n\n\t·Sumar (+)\n\t·Restar (-)\n\t·Multiplicar (*)\n\t·Dividir (/)\n\t·Elevar a una potència (^)\n\n\n");
    }
            
    repetir = 0;
    
    while (repetir == 0) {
        onEstem = 0;
                
        printf("Entra el que vulguis calcular:\n");
        netejaStdin();
        fgets(cadena, 120, stdin);
            
        printf("\nRESULTAT: %lf\n\n", calcular(cadena));
        
        printf("Per tornar a calcular escriu 0, per sortir del programa escriu qualsevol altre numero.\n");
        scanf("%lf", &repetir);
        }
    
    return 0;
}

//  El que fa la funció conversio és passar d'una cadena de caràcters a un array amb elements que tenen estructura de s_rpnElement. Per fer això utilitza altres funcions com paraulaAElement, que ens passa "paraules" individuals a un element amb estructura s_rpnElements.
s_rpnElement *conversio(char *cadena)
{
    s_rpnElement *pila;
    
    pila = (s_rpnElement *) malloc(nombreParaules(cadena) * sizeof(s_rpnElement));
    if (pila == NULL) {
        printf("No hi ha prou espai a la memòria.\n");
        exit(1);
    }
    
//  Les següents variables ens diuen en quina paraula estem i quina és la posició de l'inici de la paraula.
    int npar = 0, inicipar = 0;
    
    for (int i = 0; i < strlen(cadena); i++) {
        if (isspace(cadena[i]) && i > 0 && !isspace(cadena[i-1])) {
            char *paraula;
            
            paraula = strndup(cadena + inicipar, i - inicipar);
            pila[npar] = paraulaAElement(paraula);
            free(paraula);
            
            npar++;
        }
        
        if (!isspace(cadena[i]) && i > 0 && isspace(cadena[i-1]))
            inicipar = i;
    }
    
//  Com que normalment la cadena no acabarà amb un espai, ens faltarà una paraula per contar, així que amb aquest if no ens la deixem.
    if (!isspace(cadena[strlen(cadena)-1])) {
        char *paraula;
        
        paraula = strndup(cadena + inicipar, strlen(cadena) - inicipar);
        pila[npar] = paraulaAElement(paraula);
        free(paraula);
        
        npar++;
    }
    
    return pila;
}

//  Com ja hem dit abans, aquesta funció ens passa de paraules a estructures s_rpnElement. Això ho fa amb ajuda d'altres funcions com esconstant, esoperacio o esfuncio, que serveixen per identificar el tipus de paraula que és.
s_rpnElement paraulaAElement(char *paraula)
{
    s_rpnElement element;
    
    if (isdigit(paraula[0]) || (paraula[0] == '-' && isdigit(paraula[1])))
    {
        element.tipus = numero;
        element.valor.d = atof(paraula);
    }
    else if (esconstant(paraula))
    {
        element.tipus = numero;
        element.valor.d = esconstant(paraula);
    }
    else if (esoperacio(paraula) != error_op)
    {
        element.tipus = operacio;
        element.valor.op = esoperacio(paraula);
    }
    else if (esfuncio(paraula) != error_f)
    {
        element.tipus = funcio;
        element.valor.f = esfuncio(paraula);
    }
    else
    {
        printf("Error: paraula <%s> no reconeguda.\n", paraula);
        free(paraula);
        exit(1);
    }
        
    return element;
}

//  Amb aquesta funció podem saber el nombre de paraules que té una cadena.
int nombreParaules(char *cadena)
{
    int nombreparaules = 0;
    
    for (int i = 0; i < strlen(cadena); i++) {
        if (isspace(cadena[i]) && i > 0 && !isspace(cadena[i-1]))
            nombreparaules++;
    }
    
    if (!isspace(cadena[strlen(cadena)-1]))
        nombreparaules++;
    
    return nombreparaules;
}

//  esconstant et retorna el valor numèric de la constant que sigui reconeguda pel programa.
double esconstant(char *paraula)
{
    if (strcmp(paraula, "pi") == 0)
        return M_PI;
    
    else if (strcmp(paraula, "e") == 0)
        return M_E;
    
    else if (strcmp(paraula, "-pi") == 0)
        return -M_PI;
    
    else if (strcmp(paraula, "-e") == 0)
        return -M_E;
    
    else
        return 0;
}

//  esoperacio et torna la operació que identifica en la paraula. Per fer-ho, et torna només la part del valor del struct.
e_operacions esoperacio(char *paraula)
{
    if (strcmp(paraula, "+") == 0)
        return suma;
    
    else if (strcmp(paraula, "-") == 0)
        return resta;
    
    else if (strcmp(paraula, "*") == 0)
        return multiplicacio;
    
    else if (strcmp(paraula, "/") == 0)
        return divisio;
    
    else if (strcmp(paraula, "^") == 0)
        return potencia;
    
    else
        return error_op;
}

//  esfuncio és molt similar a esoperacio però et retorna la funció en comptes de l'operació.
e_funcions esfuncio(char *paraula)
{
    if (strcmp(paraula, "sin") == 0)
        return sinus;
    
    else if (strcmp(paraula, "cos") == 0)
        return cosinus;
    
    else if ((strcmp(paraula, "tan")) == 0)
        return tangent;
    
    else if (strcmp(paraula, "arcsin") == 0)
        return arcsinus;
    
    else if (strcmp(paraula, "arccos") == 0)
        return arccosinus;
    
    else if (strcmp(paraula, "arctan") == 0)
        return arctangent;
    
    else if (strcmp(paraula, "sqrt") == 0)
        return arrel;
    
    else if (strcmp(paraula, "exp") == 0)
        return expon;
        
    else if (strcmp(paraula, "log") == 0)
        return logaritme;
    
    else if (strcmp(paraula, "!") == 0)
        return factorial;
    else
        return error_f;
}

//  Passem ara a les funcions per calcular:

//  Aquesta és la funció principal, que passa d'una cadena de caràcters a el resultat final. El codi és similar al pseudocodi escrit en el guió del treball.
double calcular(char *cadena)
{
    double *pilaResultant, resultat;
    pilaResultant = (double *) malloc(nombreParaules(cadena) * sizeof(double));
    if (pilaResultant == NULL) {
        printf("No hi ha prou espai a la memòria.\n");
        exit(1);
    }
    
    s_rpnElement *pilaInici = conversio(cadena);
    double op1, op2;
    
    for (int i = 0; i < nombreParaules(cadena); i++)
    {
        if (pilaInici[i].tipus == numero){
            empilar(pilaResultant, pilaInici[i].valor.d);
        }
//  Aquí comencem a utilitzar el onEstem. El fem servir per saber si la pila és buida i per saber en quina posició està l'últim valor que hem afegit. Aquest contador funciona de manera que ens indica la posició de l'últim valor de la pilaResultant. Com que hem començat a posar els valors a partir de la posició 1, quan onEstem valgui 0 voldrà dir que la pila és buida.
        else if (pilaInici[i].tipus == operacio) {
            if (onEstem == 0) {
                printf("Error: Valors entrats incorrectament.\n");
                exit(1);
            }
            else
                op2 = desempilar(pilaResultant);
            if (onEstem == 0) {
                printf("Error: Valors entrats incorrectament.\n");
                exit(1);
            }
            else
                op1 = desempilar(pilaResultant);
            empilar(pilaResultant,  feroperacio(pilaInici[i].valor.op, op1, op2));
        }
        else if (pilaInici[i].tipus == funcio) {
            if (onEstem == 0) {
                printf("Error: Valors entrats incorrectament.\n");
                exit(1);
            }
            else
                op1 = desempilar(pilaResultant);
            empilar(pilaResultant,  ferfuncio(pilaInici[i].valor.f, op1));
        }
    }
    
//  Aquí mirem que quan acabi de calcular només quedi un valor a la pilaResultant.
    if (onEstem != 1) {
        printf("Error: Valors entrats incorrectament.\n");
        exit(1);
    }
    else {
        resultat = pilaResultant[onEstem];
        free(pilaResultant);
        return resultat;
    }
}

//  Les funcions empilar i desempilar fan bàsicament el que diu el seu nom.
void empilar(double *pila, double valor)
{
    onEstem++;
    pila[onEstem] = valor;
}

double desempilar(double *pila)
{
    double retorna;
    
    retorna = pila[onEstem];
    pila[onEstem] = 0;
    onEstem--;
    
    return retorna;
}

//  Aquesta funció identifica el tipus d'operació i et calcula el resultat.
double feroperacio(e_operacions operacio, double op1, double op2)
{
    if (operacio == suma)
        return sum(op1, op2);
    
    else if (operacio == resta)
        return rest(op1, op2);
    
    else if (operacio == multiplicacio)
        return multi(op1, op2);
    
    else if (operacio == divisio)
        return divi(op1, op2);
    
    else if (operacio == potencia)
        return pow(op1, op2);
    
    else
        return 1;
}

//  Aquesta funció és similar a l'anterior, però amb funcions.
double ferfuncio(e_funcions funcio, double op1)
{
    if (funcio == sinus)
        return sin(op1);
    
    else if (funcio == cosinus)
        return cos(op1);
    
    else if (funcio == tangent)
        return tan(op1);
    
    else if (funcio == arcsinus)
        return asin(op1);
    
    else if (funcio == arccosinus)
        return acos(op1);
    
    else if (funcio == arctangent)
        return atan(op1);
    
    else if (funcio == arrel)
        return sqrt(op1);
    
    else if (funcio == expon)
        return exp(op1);
    
    else if (funcio == logaritme)
        return log(op1);
    
    else if (funcio == factorial)
        return fact(op1);
    
    else
        return 1;
}

//  Amb aquesta funció podem utilitzar el fgets tantes vegades com vulguem, ja que d'alguna manera "neteja" l'entrada per la consola.
void netejaStdin(void)
{
    int c;
    do {
        c = fgetc(stdin);
    } while (c != '\n' && c != EOF);
}
