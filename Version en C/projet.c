#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>
#include <openssl/sha.h>
#include "ocilib.h"

#define str_max_size 64
#define buffer_max_length 1024
#define sql_req_length 4096

typedef enum 
{ 
	_false, _true 
}boolean_type;


typedef struct
{
	char actor_name[str_max_size],
	     actor_birthd[str_max_size],
	     actor_country[str_max_size],

	                    film_title[str_max_size],
	                    film_genre[str_max_size],
	                    film_class[str_max_size],
	                    film_year[str_max_size],

	                                 realisator_name[str_max_size],
	                                 realisator_birthd[str_max_size],
	                                 realisator_country[str_max_size];
}retrieve_data;

OCI_Connection *cn;
OCI_Statement  *st;
OCI_Resultset  *rs;
retrieve_data file_data;
char sql_query[sql_req_length];

void show_errors();
void t_acteur();
void t_realisateur();
void t_film();
void t_jouer();
void t_realiser();
void insertActeur(char [], char [], char [], char []);
void insertRealisateur(char [], char [], char [], char []);
void insertFilm(char [], char [], char [], char [], char []);
void insertJouer(char [], char []);
void insertRealiser(char [], char []);
void dropTable(char []);
char *strtok_single(char *, char const *);
retrieve_data field_splitter(char []);
char *to_lower(char []);
char *idact_or_idreal_generator(char [], char []);
char *idfilm_generator(char []);
char *to_sql_good_qoteformat(char []);

int main(int argc, char const *argv[])
{
	/* code */
    if(!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
      return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("XE", "u_name", "u_pass", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    OCI_ServerEnableOutput(cn, 64000, 10, 255);
    const char *p;

    t_acteur();
    t_realisateur();
    t_film();
    t_jouer();
    t_realiser();

    printf("\n\n");

    char id_act[str_max_size] ,
         id_real[str_max_size],
         id_film[str_max_size],
         titre[str_max_size]  ;

	FILE *file = NULL;
    char line[buffer_max_length];
    int tempChar;
    unsigned int tempCharIdx = 0U;
    int i = 0, c;
    retrieve_data data;




    if (argc == 2)
         file = fopen(argv[1], "r");
    else 
    {
         fprintf(stderr, "error: wrong number of arguments\n"
                         "usage: %s textfile or csvfile\n", argv[0]);
         return EXIT_FAILURE;
    }

    if (!file) 
    {
         fprintf(stderr, "error: could not open textfile: %s\n", argv[1]);
         return EXIT_FAILURE;
    }
    
    //Skip the firt line
    do
    {
    	i++;
    	c = fgetc(file);  
    }while(c != '\n');

    fseek(file, i, SEEK_SET);

    while((tempChar = fgetc(file)))
    {
        /* avoid buffer overflow error */
        if(tempCharIdx == buffer_max_length) 
        {
            fprintf(stderr, "error: line is too long. increase buffer_max_length.\n");
            return EXIT_FAILURE;
        }

        /* test character value */
        if(tempChar == EOF) 
        {
            line[tempCharIdx] = '\0';
             //fprintf(stdout, "%s\n", line);
            data = field_splitter(line);
            //printf("%s  %s  %s  %s  %s  %s  %s  %s  %s  %s\n", data.actor_name, data.actor_birthd, data.actor_country, data.film_title, data.film_genre, data.film_class, data.film_year, data.realisator_name, data.realisator_birthd, data.realisator_country);
            char *idActeur = (char*) malloc(64), *idRealisateur = (char*) malloc(64), *idFilm = (char*) malloc(64), *titreFilmBonFormat = (char*) malloc(64);

            idActeur = idact_or_idreal_generator(data.actor_name,data.actor_birthd);
            idRealisateur = idact_or_idreal_generator(data.realisator_name,data.realisator_birthd);
            idFilm = idfilm_generator(data.film_title);
            titreFilmBonFormat = to_sql_good_qoteformat(data.film_title);


            if(!(idActeur && idRealisateur && idFilm && titreFilmBonFormat))
            {
                perror("Error, could not allocate memory for ids \n");
                printf("Compile and Execute the program again\n");
            }

            strcpy(id_act,idActeur);
            strcpy(id_real,idRealisateur);
            strcpy(id_film,idFilm);
            strcpy(titre,titreFilmBonFormat);

            //printf("%s %s  %s  %s  %s  %s  %s  %s %s %s  %s  %s  %s\n", idActeur, data.actor_name, data.actor_birthd, data.actor_country, idFilm, data.film_title, data.film_genre, data.film_class, data.film_year, idRealisateur, data.realisator_name, data.realisator_birthd, data.realisator_country);
            insertActeur(id_act, data.actor_name, data.actor_birthd, data.actor_country);
            insertRealisateur(id_real, data.realisator_name, data.realisator_birthd, data.realisator_country);
            insertFilm(id_film, titre, data.film_genre, data.film_class, data.film_year);
            insertJouer(id_act,id_film);
            insertRealiser(id_film,id_real);

            break;
        }
        else if(tempChar == '\n') 
        {
            line[tempCharIdx] = '\0';
            tempCharIdx = 0U;
            //fprintf(stdout, "%s\n", line);
            data = field_splitter(line);
            char *idActeur = (char*) malloc(64), *idRealisateur = (char*) malloc(64), *idFilm = (char*) malloc(64), *titreFilmBonFormat = (char*) malloc(64);

            idActeur = idact_or_idreal_generator(data.actor_name,data.actor_birthd);
            idRealisateur = idact_or_idreal_generator(data.realisator_name,data.realisator_birthd);
            idFilm = idfilm_generator(data.film_title);
            titreFilmBonFormat = to_sql_good_qoteformat(data.film_title);


            if(!(idActeur && idRealisateur && idFilm && titreFilmBonFormat))
            {
            	perror("Error, could not allocate memory for ids \n");
            	printf("Execute the program again\n");
            }

            strcpy(id_act,idActeur);
            strcpy(id_real,idRealisateur);
            strcpy(id_film,idFilm);
            strcpy(titre,titreFilmBonFormat);

       
            //printf("%s %s  %s  %s  %s  %s  %s  %s %s %s  %s  %s  %s\n", idActeur, data.actor_name, data.actor_birthd, data.actor_country, idFilm, data.film_title, data.film_genre, data.film_class, data.film_year, idRealisateur, data.realisator_name, data.realisator_birthd, data.realisator_country);
            insertActeur(id_act, data.actor_name, data.actor_birthd, data.actor_country);
            insertRealisateur(id_real, data.realisator_name, data.realisator_birthd, data.realisator_country);
            insertFilm(id_film, titre, data.film_genre, data.film_class, data.film_year);
            insertJouer(id_act,id_film);
            insertRealiser(id_film,id_real);

           

            printf("\n"); 

            continue;
        }
        else
            line[tempCharIdx++] = (char)tempChar;

    }

    unsigned int choice;
    char genre[65],
         acteur[65],
         realisateur[65],
         annee[65];
    static int j = 0;

  
    boolean_type use_system = _true;
    while (use_system != _false)
    {
        printf("\n\n\n********************************************************\n");
        printf("1. Liste des genres.\n");
        printf("2. Liste des films.\n");
        printf("3. Liste des acteurs(rices).\n");
        printf("4. Liste des réalisateurs(rices).\n");
        printf("5. Liste des films d’un genre donné.\n");
        printf("6. Liste des films d’un(e) acteur(rice) donné(e).\n");
        printf("7. Liste des films d’un(e) réalisateur(rice) donné(e).\n");
        printf("8. Liste des films d’une année donnée.\n");
        printf("9. Quitter\n");
        printf("**************************************************************\n\n");
        printf("Enter your choice and press return: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                    printf("Liste des genres disponibles.\n");
                    strcpy(sql_query, "select distinct genre from film order by genre");

                    OCI_ExecuteStmt(st, sql_query);

                    rs = OCI_GetResultset(st);
                   
                    //int i = OCI_GetRowCount(rs);
                    //printf("%d\n", i);

                    if(OCI_FetchNext(rs) == TRUE)
                    {
                         do
                         {
                            j++;
                            printf("#%d %s\n", j, OCI_GetString(rs,1));
                            //printf("%s\n",OCI_GetString(rs,1));
                         }while(OCI_FetchNext(rs));
                         printf("%d genre(s) au total.\n", OCI_GetRowCount(rs));
                         j = 0;
                    }
                    else
                      printf("Aucun Genre\n");
                    break;
            case 2:
                    printf("Liste des films.\n");
                    strcpy(sql_query, "select distinct titre from film order by titre");

                    OCI_ExecuteStmt(st, sql_query);

                    rs = OCI_GetResultset(st);
                    
                   // int i = 0;

                    if(OCI_FetchNext(rs) == TRUE)
                    {
                         do
                         {
                            j++;
                            printf("#%d %s\n", j, OCI_GetString(rs,1));
                            //printf("%s\n",OCI_GetString(rs,1));
                         }while(OCI_FetchNext(rs));
                         printf("%d film(s) au total.\n", OCI_GetRowCount(rs));
                         j=0;
                    }
                    else
                      printf("Aucun Film\n");
                    break;
            case 3:
                    printf("Liste des acteurs(rices).\n");
                    strcpy(sql_query, "select distinct nom from acteur order by nom");

                    OCI_ExecuteStmt(st, sql_query);

                    rs = OCI_GetResultset(st);
                    //int i = 0;

                    if(OCI_FetchNext(rs) == TRUE)
                    {
                         do
                         {
                            j++;
                            printf("#%d %s\n", j, OCI_GetString(rs,1));
                            //printf("%s\n",OCI_GetString(rs,1));
                         }while(OCI_FetchNext(rs));
                         printf("%d acteur(s)(rices) au total.\n", OCI_GetRowCount(rs));
                         j=0;
                    }
                    else
                      printf("Aucun acteur ou actrice\n");
                    break;
            case 4:
                    printf("Liste des réalisateurs(rices).\n");
                    strcpy(sql_query, "select distinct nom from realisateur order by nom");

                    OCI_ExecuteStmt(st, sql_query);

                    rs = OCI_GetResultset(st);
                    //int i = 0;

                    if(OCI_FetchNext(rs) == TRUE)
                    {
                         do
                         {
                            j++;
                            printf("#%d %s\n", j, OCI_GetString(rs,1));
                            //printf("%s\n",OCI_GetString(rs,1));
                         }while(OCI_FetchNext(rs));
                         printf("%d réalisateur(s)(rices) au total.\n", OCI_GetRowCount(rs));
                         j=0;
                    }
                    else
                      printf("Aucun ou realisateur ou realisatrice\n");
                    break;
            case 5:
                    //printf("Liste des films de grenre: \n");
                    printf("Entrer genre :\n"); // Action, ou comedie noire ou science fiction...
                    scanf(" %[^\n]s",genre);
                    OCI_Prepare(st, "BEGIN films_genre(:genre); END;");
                    OCI_BindString(st, ":genre",  (char *)genre,  0);

                    /* execute */
                    OCI_Execute(st);

                 
                    while ((p = OCI_ServerGetOutput(cn)) != NULL) 
                    {
                        printf("%s", p);
                        printf("\n");
                    }
                    
                    break;
            case 6:
                    //printf("Liste des films de l'acteur(rice) :\n");
                    printf("Entrer acteur/actrice :\n"); 
                    scanf(" %[^\n]s",acteur);
                    OCI_Prepare(st, "BEGIN films_acteur(:acteur); END;");
                    OCI_BindString(st, ":acteur",  (char *)acteur,  0);

                    /* execute */
                    OCI_Execute(st);

                 
                    while ((p = OCI_ServerGetOutput(cn)) != NULL) 
                    {
                        printf("%s", p);
                        printf("\n");
                    }
                    break;
            case 7:
                    //printf("Liste des films du(de la) réalisateur(rice).\n");
                    printf("Entrer realisateur :\n"); 
                    scanf(" %[^\n]s",realisateur);
                    OCI_Prepare(st, "BEGIN films_realisateur(:realisateur); END;");
                    OCI_BindString(st, ":realisateur",  (char *)realisateur,  0);

                    /* execute */
                    OCI_Execute(st);

                 
                    while ((p = OCI_ServerGetOutput(cn)) != NULL) 
                    {
                        printf("%s", p);
                        printf("\n");
                    }
                    break;
            case 8:
                    //printf("Liste des films de l'année: \n");
                    printf("Entrer annee :\n"); // Action, ou comedie noire ou science fiction...
                    scanf(" %[^\n]s",annee);
                    OCI_Prepare(st, "BEGIN films_annee(:annee); END;");
                    OCI_BindString(st, ":annee",  (char *)annee,  0);
                    //OCI_BindDate(st, ":annee", dt);
                    

                    /* execute */
                    OCI_Execute(st);

                 
                    while ((p = OCI_ServerGetOutput(cn)) != NULL) 
                    {
                        printf("%s", p);
                        printf("\n");
                    }
                    break;
            case 9:
                    printf( "Merci d'avoir utiliser ce programme.\n");
                    use_system = _false;
                    break;
            default:
                    printf( "Not a Valid Choice. \n");
                    printf( "Choose again.\n");
                    //cin >> choice;
                    break;
        }

}

  //OCI_DateFree(dt);

  char t_acteur[] = "acteur";
  char t_realisateur[] = "realisateur";
  char t_film[] = "film";
  char t_jouer[] = "jouer";
  char t_realiser[] = "realiser";
  dropTable(t_acteur);
  dropTable(t_realisateur);
  dropTable(t_film);
  dropTable(t_jouer);
  dropTable(t_realiser);
  //this_thread::sleep_for(chrono::seconds(6));
  printf("Au revoir.\n");

    /* cleanup */
    OCI_Cleanup();
    printf("\n");

	return EXIT_SUCCESS;
}

void show_errors()
{
    OCI_Error *err = OCI_GetLastError();
    printf("Error:");
    printf("\n");
    printf(
                "code  : ORA-%05i\n"
                "msg   : %s\n"
                "sql   : %s\n",
                OCI_ErrorGetOCICode(err), 
                OCI_ErrorGetString(err),
                OCI_GetSql(OCI_ErrorGetStatement(err))
           );
    printf("\n");
}

void t_acteur()
{
    strcpy(sql_query,"create table acteur ( idacteur varchar2(64) primary key, nom varchar2(64) not null, ddnais date, pays varchar2(64) )");

    if(!OCI_ExecuteStmt(st, sql_query))
    { 
        printf("Error on table acteur creation.\n");
        show_errors();
        printf("\n");
        exit(1); 
    }
    else
        printf("Table acteur created!\n");
}

void t_realisateur()
{
    strcpy(sql_query,"create table realisateur ( idrealisateur varchar2(64) primary key, nom varchar2(64) not null, ddnais date, pays varchar2(64) )");

    if(!OCI_ExecuteStmt(st, sql_query))
    { 
        printf("Error on table realisateur creation. \n");
        show_errors();
        printf("\n");
        exit(1); 
    }
    else
        printf("Table realisateur created!\n");
}

void t_film()
{
    strcpy(sql_query,"create table film ( idfilm varchar2(64) primary key, titre varchar2(64) not null, genre varchar2(64), classe varchar2(64), annee varchar2(64) )");

    if(!OCI_ExecuteStmt(st, sql_query))
    { 
        printf("Error on table film creation. \n");
        show_errors();
        printf("\n");
        exit(1); 
    }
    else
        printf("Table film created!\n");
}  

void t_jouer()
{
    strcpy(sql_query,"create table jouer ( idacteur varchar2(64), idfilm varchar2(64), foreign key(idacteur) references acteur(idacteur), foreign key(idfilm) references film(idfilm))");

    if(!OCI_ExecuteStmt(st, sql_query))
    { 
        printf("Error on table jouer creation. \n");
        show_errors();
        printf("\n");
        exit(1); 
    }
    else
        printf("Table jouer created!\n");
}

void t_realiser()
{
    strcpy(sql_query,"create table realiser ( idfilm varchar2(64), idrealisateur varchar2(64), foreign key(idfilm) references film(idfilm), foreign key(idrealisateur) references realisateur(idrealisateur))");

    if(!OCI_ExecuteStmt(st, sql_query))
    { 
        perror("Error on table realiser creation: ");
        show_errors();
        printf("\n");
        exit(1); 
    }
    else
        printf("Table realiser created!\n");
}

void insertActeur(char id[65], char nom[65], char ddn[65], char pays[65])   // Fonction qui fait l'insertion dans la table t_acteur
{
    strcpy(sql_query, "INSERT INTO acteur(idacteur, nom, ddnais, pays)");
    strcat(sql_query, " SELECT '");
    strcat(sql_query, id);
    strcat(sql_query, "', '");
    strcat(sql_query, nom);
    strcat(sql_query, "', TO_DATE('");
    strcat(sql_query, ddn);
    strcat(sql_query, "'), '");
    strcat(sql_query, pays);
    strcat(sql_query, "' FROM DUAL");
    strcat(sql_query, " WHERE NOT EXISTS (SELECT *");
    strcat(sql_query, " FROM acteur");
    strcat(sql_query, " WHERE acteur.idacteur = '");
    strcat(sql_query, id);
    strcat(sql_query, "')");

    if(OCI_ExecuteStmt(st, sql_query))
    {
        OCI_ExecuteStmt(st, "commit");
        printf("Insertion dans la table acteur reussie.\n");
    }
    else
    {
        printf("Erreur dans lors de l'insertion dans la table acteur.\n");
        show_errors();
    }
}

void insertRealisateur(char id[65], char nom[65], char ddn[65], char pays[65])  // Fonction qui fait l'insertion dans la table t_realisateur
{
    strcpy(sql_query, "INSERT INTO realisateur(idrealisateur, nom, ddnais, pays)");
    strcat(sql_query, " SELECT '");
    strcat(sql_query, id);
    strcat(sql_query, "', '");
    strcat(sql_query, nom);
    strcat(sql_query, "', TO_DATE('");
    strcat(sql_query, ddn);
    strcat(sql_query, "'), '");
    strcat(sql_query, pays);
    strcat(sql_query, "' FROM DUAL");
    strcat(sql_query, " WHERE NOT EXISTS (SELECT *");
    strcat(sql_query, " FROM realisateur");
    strcat(sql_query, " WHERE realisateur.idrealisateur = '");
    strcat(sql_query, id);
    strcat(sql_query, "')");

    if(OCI_ExecuteStmt(st, sql_query))
    {
        OCI_ExecuteStmt(st, "commit");
        printf("Insertion dans la table realisateur reussi.\n");
    }
    else
    {
        printf("Erreur lors de l'insertion dans la table realisateur.\n");
        show_errors();
    }
}

void insertFilm(char id[65], char titre[65], char genre[65], char classe[65], char annee[65]) // Fonction qui fait l'insertion dans la table t_film
{
    
    strcpy(sql_query, "INSERT INTO film(idfilm, titre, genre, classe, annee)");
    strcat(sql_query, " SELECT '");
    strcat(sql_query, id);
    strcat(sql_query, "', '");
    strcat(sql_query, titre);
    strcat(sql_query, "', '");
    strcat(sql_query, genre);
    strcat(sql_query, "', '");
    strcat(sql_query, classe);
    strcat(sql_query, "', '");
    strcat(sql_query, annee);
    strcat(sql_query, "' FROM DUAL");
    strcat(sql_query, " WHERE NOT EXISTS (SELECT *");
    strcat(sql_query, " FROM film");
    strcat(sql_query, " WHERE film.idfilm = '");
    strcat(sql_query, id);
    strcat(sql_query, "')");

    if(OCI_ExecuteStmt(st, sql_query))
    {
        OCI_ExecuteStmt(st, "commit");
        printf("Insertion dans la table film reussi.\n");
    }
    else
    {
        printf("Erreur lors de l'insertion dans la table film.\n");
        show_errors();
    }
}

void insertJouer(char id_acteur[65], char id_film[65])  // Fonction qui fait l'insertion dans la table t_jouer
{
    strcpy(sql_query, "INSERT INTO jouer(idacteur, idfilm)");
    strcat(sql_query, " SELECT '");
    strcat(sql_query, id_acteur);
    strcat(sql_query, "', '");
    strcat(sql_query, id_film);
    strcat(sql_query, "' FROM DUAL");
    strcat(sql_query, " WHERE NOT EXISTS (SELECT *");
    strcat(sql_query, " FROM jouer");
    strcat(sql_query, " WHERE jouer.idacteur = '");
    strcat(sql_query, id_acteur);
    strcat(sql_query, "' AND jouer.idfilm = '");
    strcat(sql_query, id_film);
    strcat(sql_query, "')");

    if(OCI_ExecuteStmt(st, sql_query))
    {
        OCI_ExecuteStmt(st, "commit");
        printf("Insertion dans la table jouer reussi.\n");
    }
    else
    {
        printf("Erreur lors de l'insertion dans la table jouer.\n");
        show_errors();
    }
}

void insertRealiser(char id_film[65], char id_realisateur[65])  // Fonction qui fait l'insertion dans la table t_realiser
{
    strcpy(sql_query, "INSERT INTO realiser(idfilm, idrealisateur)");
    strcat(sql_query, " SELECT '");
    strcat(sql_query, id_film);
    strcat(sql_query, "', '");
    strcat(sql_query, id_realisateur);
    strcat(sql_query, "' FROM DUAL");
    strcat(sql_query, " WHERE NOT EXISTS (SELECT *");
    strcat(sql_query, " FROM realiser");
    strcat(sql_query, " WHERE realiser.idfilm = '");
    strcat(sql_query, id_film);
    strcat(sql_query, "' AND realiser.idrealisateur = '");
    strcat(sql_query, id_realisateur);
    strcat(sql_query, "')");

    if(OCI_ExecuteStmt(st,sql_query))
    {
        OCI_ExecuteStmt(st, "commit");
        printf("Insertion dans la table realiser reussie.\n");
    }
    else
    {
        printf("Erreur lors de l'insertion dans la table realiser.\n");
        show_errors();
    }
}

void dropTable(char table[65])
{
    strcpy(sql_query, "DROP TABLE ");
    strcat(sql_query, table);
    strcat(sql_query, " CASCADE CONSTRAINTS");

    if(OCI_ExecuteStmt(st, sql_query))
    {
        printf("Table %s succefully dropped.\n", table);
    }
    else
    {
        printf("Error on table dropping.\n");
        show_errors();
    }
}

char *strtok_single(char *str, char const *delimiter)
{
    static char *src = NULL;
    char  *p,  *ret = 0;

    if (str != NULL)
      src = str;

    if (src == NULL)
      return NULL;

    if((p = strpbrk (src, delimiter)) != NULL) 
    {
      *p  = 0;
      ret = src;
      src = ++p;
    }
    else if(*src)
    {
      ret = src;
      src = NULL;
    }
    return ret;
}

retrieve_data field_splitter(char file_line[buffer_max_length])
{
    char delimiter[] = ";";
    char *ptr = strtok_single(file_line, delimiter);
    int i = 0;
    

    while(ptr) 
    {
        if(i==0)
        {
            strcpy(file_data.actor_name,ptr);
        }
        else if(i==1)
        {
            strcpy(file_data.actor_birthd,ptr);
        }
        else if(i==2)
        {
            strcpy(file_data.actor_country,ptr);
        }
        else if(i==3)
        {
            strcpy(file_data.film_title,ptr);
        }
        else if(i==4)
        {
            strcpy(file_data.film_genre,ptr);
        }
        else if(i==5)
        {
            strcpy(file_data.film_class,ptr);
        }
        else if(i==6)
        {
            strcpy(file_data.film_year,ptr);
        }
        else if(i==7)
        {
            strcpy(file_data.realisator_name,ptr);
        }
        else if(i==8)
        {
            strcpy(file_data.realisator_birthd,ptr);
        }
        else
        {
            strcpy(file_data.realisator_country,ptr);
        }

        ptr = strtok_single (NULL, delimiter);
        i++;
    }

    return file_data;
}

char *to_lower(char str[])
{
    char *s = (char*) malloc(strlen(str)+1);
    if(!s)
        perror("error on allocating memory \n");

    char temp_str[strlen(str)+1];
    strcpy(temp_str,str);
    

    int i=0;
    char c;
    while(temp_str[i])
    {
       c=temp_str[i];
       c = tolower(c);
       temp_str[i] = c;
       i++;
    }
    
    strcpy(s,temp_str);
    return s;
}

char *idact_or_idreal_generator(char name[], char birthday[])
{
    char *generated_id = (char*) malloc(64);
    if(!generated_id)
        perror("error on allocating memory \n");

    char delimiter[] = " ", delimiter2[] = "-";
    //char c;
    int i = 0;

    
    char *ptr_name = to_lower(name);
    char *ptr_birthday = to_lower(birthday);

    char temp_name[strlen(ptr_name)+1];
    char temp_birthday[strlen(ptr_birthday)+1];

    strcpy(temp_name,ptr_name);
    strcpy(temp_birthday,ptr_birthday);


    i = 0;
    char *ptr = NULL;
    ptr = strtok_single(temp_name, delimiter);

    while(ptr) 
    {
        if(i==0)
        {
            char c_char = *ptr;
            char *pChar = (char*) malloc(sizeof(char));
            if(pChar)
            {       
                *pChar = c_char;
                strcpy(generated_id,pChar);
                free(pChar);
            }
        }
        else if(i==1)
        {
            strcat(generated_id,ptr);
        }
       
        ptr = strtok_single (NULL, delimiter);
        i++;
    }

    ptr = strtok_single(temp_birthday, delimiter2);

    while(ptr) 
    {
        strcat(generated_id,ptr);
        ptr = strtok_single (NULL, delimiter2);
        
    }

    return generated_id;
}

char *idfilm_generator(char titrefilm[])
{
    char *_titrefilm = to_lower(titrefilm);
    int i = 0;
    unsigned char temp[SHA_DIGEST_LENGTH];
    char generated_id[SHA_DIGEST_LENGTH*2];

    memset(generated_id, 0x0, SHA_DIGEST_LENGTH*2);
    memset(temp, 0x0, SHA_DIGEST_LENGTH);

    SHA1((unsigned char *)_titrefilm, strlen(_titrefilm), temp);

    for (i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(generated_id[i*2]), "%02x", temp[i]);
    }

    char *final_idfilm = (char*) malloc(strlen(generated_id)+1);
    if(!final_idfilm)
        return NULL;

    strcpy(final_idfilm,generated_id);

    return final_idfilm;
}

char *to_sql_good_qoteformat(char titrefilm[])
{
    //titrefilm = to_lower(titrefilm);

    const char *titre = titrefilm;
    int nbrQuote = 0;

    const char *p;
    for(p = strchr(titre, '\''); p; p = strchr(p + 1, '\''))
        nbrQuote++;

    char bonformat_titre[strlen(titre) + nbrQuote + 1];
    memset(bonformat_titre, 0, sizeof(bonformat_titre));

    const char *s = titre;
    for(p = strchr(s, '\''); p; p = strchr(s, '\'')) 
    {
        strncat(bonformat_titre, s, p - s);
        strcat(bonformat_titre, "''");
        s = p + 1;
    }

    const char *end = titre + strlen(titre);
    strncat(bonformat_titre, s, end - s);

    char *title_good_format = (char*) malloc(strlen(bonformat_titre)+1);
    if(!title_good_format)
        perror("can't allocate memory in to_sql_good_qoteformat \n");

    strcpy(title_good_format,bonformat_titre);

    return title_good_format;
}