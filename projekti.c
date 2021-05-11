#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

struct Pisteet{
	int taulukko[6];
    int kok;
};

struct Opiskelija{
	char *etu;
	char *suku;
	int opnro;
    struct Pisteet pisteet;
};

struct Latauspaketti{
	struct Opiskelija *lista;
    int maara;
};

int npituus(long long n)
{
    int i = 0;
    while (n != 0) {
        n = n / 10;
        ++i;
    }
    return i;
}

void tulosta_tietty(struct Opiskelija *lista, int maara, int opnro) {
    for (int i = 0; i < maara; i++) {
        if (lista[i].opnro == opnro) {
            printf("%d ", lista[i].opnro);
            for (int k = 0; k < 6; k++) {
                printf(" %d ", lista[i].pisteet.taulukko[k]);
            }
            printf(" yht. %d %s %s\n", lista[i].pisteet.kok, lista[i].suku, lista[i].etu);
            i = maara;
            break;
        }
    }
}

struct Opiskelija *lisaa(int opnro, char *etu, char *suku, struct Opiskelija *lista, int maara) {

    if (maara > 0) {
        for (int i = 0; i < maara; i++) {
            if (lista[i].opnro == opnro) {
                printf("Et voi lisätä samaa opiskelijaa monesti.\n\n");
                return NULL;
            }
        }
        lista = realloc(lista, (maara + 1) * sizeof(struct Opiskelija));
        assert(lista);
    }

    lista[maara].etu = malloc((strlen(etu) + 1) * sizeof(char));
    lista[maara].suku = malloc((strlen(suku) + 1) * sizeof(char));
    assert(lista[maara].etu);
    assert(lista[maara].suku);
    lista[maara].opnro = opnro;
    strcpy(lista[maara].etu, etu);
    strcpy(lista[maara].suku, suku);
    for (int j = 0; j < 6; j++) {
        lista[maara].pisteet.taulukko[j] = 0;
    }
    lista[maara].pisteet.kok = 0;

    return lista;
}

struct Opiskelija *paivita_pisteet(int opnro, int kierros, int pisteet, struct Opiskelija *lista, int maara) {

    for (int i = 0; i < maara; i++) {
        if (lista[i].opnro == opnro) {
            lista[i].pisteet.taulukko[kierros - 1] = pisteet;
            lista[i].pisteet.kok += pisteet;
            return lista;     
        }
    }
    printf("Opiskelijaa ei löydy listasta.\n\n");
    return NULL;
}

int vertaa(const void *a, const void *b) {
    const struct Opiskelija *eka = a;
	const struct Opiskelija *toka = b;
    
    int tulos;
	if (eka->pisteet.kok > toka->pisteet.kok) {
        tulos = -1;
    } else if (eka->pisteet.kok == toka->pisteet.kok) {
        tulos = 0;
    } else {
        tulos = 1;
    }
    return tulos;
}

void jarjesta(struct Opiskelija *lista, int maara) {
    qsort(lista, maara, sizeof(struct Opiskelija), vertaa);
}

void tulosta(struct Opiskelija *lista, int maara) {
    for(int i = 0; i < maara; i++) {
        printf("%d ", lista[i].opnro);
        for (int k = 0; k < 6; k++) {
            printf(" %d ", lista[i].pisteet.taulukko[k]);
        }
        printf(" yht. %d %s %s\n", lista[i].pisteet.kok, lista[i].suku, lista[i].etu);
    }
    printf("\n");
}

void vapahda(struct Opiskelija *lista, int maara) {
    
    for (int i = 0; i < maara; i++) {
        free(lista[i].suku);
        free(lista[i].etu);
    }
    free(lista);
}

struct Latauspaketti *lataa(struct Opiskelija *lista, int maara, const char *tiedosto) {

    FILE *lahde = fopen(tiedosto, "r");
    if (!lahde) {
        printf("Virhe. Tiedosto käyttäytyy huonosti eikä aukea.\n\n");
        return NULL;
    }

    struct Opiskelija *uusi_lista = malloc(sizeof(struct Opiskelija));
    int uusi_maara = 0;
    char etu[80];
    char suku[80];
    int opnro;
    int pisteet[6];

    while (fscanf(lahde, "%d %s %s %d %d %d %d %d %d\n", &opnro, suku, etu, &pisteet[0], &pisteet[1], &pisteet[2], &pisteet[3], &pisteet[4], &pisteet[5]) == 9) {

        uusi_lista = lisaa(opnro, etu, suku, uusi_lista, uusi_maara);
        uusi_maara++;
        for (int k = 0; k < 6; k++) {
            paivita_pisteet(opnro, k + 1, pisteet[k], uusi_lista, uusi_maara);
        }
    }
    fclose(lahde);
    struct Latauspaketti *paketti = malloc(sizeof(struct Latauspaketti));
    assert(paketti);
    paketti->lista = uusi_lista;
    paketti->maara = uusi_maara;
    vapahda(lista, maara);

    return paketti;
}

int tallenna(struct Opiskelija *lista, int maara, const char *tiedosto) {

    FILE *kohde = fopen(tiedosto, "w");
    if (!kohde) {
        printf("Virhe. Tiedosto käyttäytyy huonosti eikä aukea.\n\n");
        return 0;
    }

    for (int i = 0; i < maara; i++) {
        fprintf(kohde, "%d %s %s %d %d %d %d %d %d\n", lista[i].opnro, lista[i].suku, lista[i].etu, lista[i].pisteet.taulukko[0], lista[i].pisteet.taulukko[1], lista[i].pisteet.taulukko[2], lista[i].pisteet.taulukko[3], lista[i].pisteet.taulukko[4], lista[i].pisteet.taulukko[5]);
    }

    fclose(kohde);
    return 1;
}

int main(void) {
    
    struct Opiskelija *lista = malloc(sizeof(struct Opiskelija));
    struct Latauspaketti *paketti;
    assert(lista);
    int maara = 0;
    int jarjestys = 0;
    int jatkuu = 1;
    int latauksia = 0;
    int ladattu = 0;

    while (jatkuu) {
        int opnro;
        char etu[80];
        char suku[80];
        int pisteet;
        int kierros;

        struct Opiskelija *temp;
        char tiedosto[80];
        char puskuri[80];
        printf("Käskysi: ");
        char *kasky = fgets(puskuri, 80, stdin);
        if (kasky == NULL) {
            jatkuu = 0;
            break;
        }
        switch (kasky[0]) {
            case 'A':
                if (sscanf(kasky, "A %d %s %s", &opnro, suku, etu) < 3 || npituus(opnro) > 6 || opnro < 0) {
                    printf("Lisäys ei onnistunut. Anna käsky seuraavasti: A [opiskelijanumero (max 6 numeroa)] [sukunimi] [etunimi]\n\n");
                    break;
                } else {
                    temp = lisaa(opnro, etu, suku, lista, maara);
                    if (temp != NULL) {
                        lista = temp;
                        maara++;
                        tulosta_tietty(lista, maara, opnro);
                        printf("Lisätty!\n\n");
                        jarjestys = 0;
                        ladattu = 0;
                    }
                }
                break;
            case 'U':
                if (sscanf(kasky, "U %d %d %d", &opnro, &kierros, &pisteet) < 3 || kierros > 6 || npituus(opnro) > 6 || opnro < 0) {
                    printf("Päivitys ei onnistunut. Anna käsky seuraavasti: U [opiskelijanumero(max 6 numeroa)] [kierros(max 6)] [pisteet]\n\n");
                    break;
                } else {
                    temp = paivita_pisteet(opnro, kierros, pisteet, lista, maara);
                    if (temp != NULL) {
                        lista = temp;
                        ladattu = 0;
                        printf("Päivitetty!\n\n");
                        jarjestys = 0;
                    }
                }
                break;
            case 'L':
                if (!jarjestys && maara > 1) {
                    jarjesta(lista, maara);
                    jarjestys = 1;
                }
                if (maara == 0) {
                    printf("Ei tulostettavaa\n\n");
                }
                tulosta(lista, maara);
                break;
            case 'Q':
                jatkuu = 0;
                break;
            case 'W':
                
                if (sscanf(kasky, "W %s", tiedosto) != 1) {
                    printf("Virhe. Anna käsky seuraavasti: W [tiedosto]\n\n");
                    break;
                }
                if (tallenna(lista, maara, tiedosto)) {
                        printf("Tiedot tallennettu onnistuneesti tiedostoon %s.\n\n", tiedosto);
                }
                break;
            case 'O':
                if (sscanf(kasky, "O %s", tiedosto) != 1) {
                    printf("Virhe. Anna käsky seuraavasti: O [tiedosto]\n\n");
                    break;
                } else if (ladattu) {
                    printf("Samat tiedot on jo ladattu.\n\n");
                    break;
                } else {
                    paketti = lataa(lista, maara, tiedosto);
                    if (paketti != NULL) {
                        lista = paketti->lista;
                        maara = paketti->maara;
                        printf("Tiedot ladattiin onnistuneesti.\n\n");
                        latauksia++;
                        ladattu = 1;
                    }
                    break;
                }
            default:
                printf("Virhe. Anna seuraavanlainen käsky:\nA [opiskelijanumero] [sukunimi] [etunimi]\nU [opiskelijanumero] [kierros] [pisteet]\nL\nW [tiedosto]\nO [tiedosto]\nQ\n\n");
                break;
        }
    }
    if (latauksia) {
        free(paketti);
    }
    vapahda(lista, maara);
    printf("Suljetaan.\n\n");
    return 1;
}