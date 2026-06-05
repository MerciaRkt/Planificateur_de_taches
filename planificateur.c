#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FICHIER "tache.conf"

typedef struct 
{
    char nom[50];
    char heure[10];
    char planification[100];
    char commande[200];
    int id_at;
} Tache;

void afficher_taches() 
{
    FILE *f = fopen(FICHIER, "r");
    if (f == NULL) 
    {
        printf("Aucune tache.\n");
        return;
    }
    Tache t;
    int id = 1;
    printf("\n=== Taches planifiees ===\n");
    printf("ID | Nom | Heure | Planification | Commande\n");
    while (fscanf(f,"%49[^;];%9[^;];%99[^;];%199[^;];%d\n",t.nom,t.heure,t.planification,t.commande,&t.id_at) == 5)
    {
        printf("[%d] %s | %s | %s | %s\n",id,t.nom,t.heure,t.planification,t.commande);
        id++ ;
    }
    fclose(f);
}

void ajouter_tache() 
{
    Tache t;
    int choix ;
    char cron [255] ;
    t.id_at = 0;

    printf("Nom : ");
    fgets(t.nom, sizeof(t.nom), stdin);
    t.nom[strcspn(t.nom, "\n")] = '\0';

    printf("Commande : ");
    fgets(t.commande, sizeof(t.commande), stdin);
    t.commande[strcspn(t.commande, "\n")] = '\0';

    printf("Heure (HH:MM) : ");
    fgets(t.heure, sizeof(t.heure), stdin);
    t.heure[strcspn(t.heure, "\n")] = '\0';

    printf("\nType de planification :\n");
    printf("1. Date et heure précises\n");
    printf("2. Chaque heure\n");
    printf("3. Chaque jour\n");
    printf("4. Chaque semaine\n");
    printf("5. Chaque mois\n");
    printf("Choix : ");
    scanf("%d", &choix);
    getchar();
    
    switch (choix) {
        case 1: 
        {
            char sortie_at [255] ;
            printf("Date (AAAA-MM-JJ) : ");
            fgets(t.planification, sizeof(t.planification), stdin);
            t.planification[strcspn(t.planification, "\n")] = '\0';

            char cmd[500];
            sprintf(cmd,"echo \"%s\" | at \"%s %s\"",t.commande,t.heure,t.planification);

            FILE *pf = popen(cmd, "r");
            while (fgets( sortie_at, sizeof(sortie_at), pf)) 
            {
                sscanf(sortie_at, "job %d", &t.id_at);
            }
            pclose(pf);
            printf("Tâche programmée.\n");
            break;
        }
        case 2 :
        {
            int h, m;
            sscanf(t.heure, "%d:%d", &h, &m); 
            strcpy(t.planification, "Chaque heure");
            sprintf (cron , "%d * * * * %s" ,m, t.commande);
            break;
        }
        case 3 :
        {
            int h, m;
            sscanf(t.heure, "%d:%d", &h, &m); 
            strcpy(t.planification, "Chaque jour");
            sprintf (cron , "%d %d * * * %s" ,m, h , t.commande);
            break;
        }
        case 4 :
        {
            int h, m;
            sscanf(t.heure, "%d:%d", &h, &m); 
            strcpy(t.planification, "Chaque semaine");
            sprintf (cron , "%d %d * * 1 %s" ,m, h , t.commande);
            break;
        }
        case 5 :
        {
            int h, m , jmois ;
            sscanf(t.heure, "%d:%d", &h, &m); 

            printf("Entrez le jour du mois (1-31): ");
            scanf ("%d" , &jmois) ;
            getchar();
            strcpy(t.planification, "Chaque mois");
            sprintf (cron , "%d %d %d * * %s" ,m, h ,jmois, t.commande);
            break;
        }
        default:
            printf("Choix invalide.\n");
            return;
    }

    if (choix != 1)
    {
        char cmd[512];
        sprintf (cmd , "(crontab -l ; echo '%s') | crontab -", cron );
        system (cmd);
        printf("Tâche programmée.\n");
    }
    FILE *pf = fopen("tache.conf", "a");
    if (pf != NULL)
    {
        fprintf(pf,"%s;%s;%s;%s;%d\n",t.nom,t.heure,t.planification,t.commande,t.id_at);
        fclose(pf);
    }
}


void supprimer_tache() 
{
    int numero;
    char cmd[512];
    Tache t;
    char ligne[512];
    int id = 1;
    printf("Numero de tache a supprimer : ");
    scanf("%d", &numero);
    getchar();

//effacer dans le fichier
    FILE *f = fopen(FICHIER, "r");
    FILE *tmp = fopen("tmp.conf", "w");

    if (!f || !tmp) {
        printf("Erreur fichier.\n");
        return;
    }

    while (fgets(ligne, sizeof(ligne), f)) 
    {
        if (id != numero)
            fputs(ligne, tmp);
        else
        {
            if (id == numero)
            {
                sscanf(ligne,"%49[^;];%9[^;];%99[^;];%199[^;];%d",t.nom,t.heure,t.planification,t.commande,&t.id_at);
            }
        }
        id++;
    }
    fclose(f);
    fclose(tmp);
    remove(FICHIER);
    rename("tmp.conf", FICHIER);

//effacer dans crontab ou AT
    if (t.id_at == 0)
    {
        sprintf(cmd,"crontab -l | grep -v '%s' | crontab -",t.commande);
        system(cmd);
    }
    else
    {
        sprintf(cmd, "atrm %d", t.id_at);
        system(cmd);
    }
    printf("Tache supprimee.\n");
}


int main() {
    int choix;
    do {
        printf("\n===== PLANIFICATEUR DE TACHES =====\n");
        printf("1. Ajouter une tache\n");
        printf("2. Supprimer une tache\n");
        printf("3. Lister les tâches existantes\n");
        printf("4. Quitter\n");
        printf("Choix : ");

        scanf("%d", &choix);
        getchar();

        switch (choix) {
            case 1:
                ajouter_tache();
                break;
            case 2:
                supprimer_tache();
                break;
            case 3:
                afficher_taches();
                break;
            case 4:
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 4);
    return 0;
}
