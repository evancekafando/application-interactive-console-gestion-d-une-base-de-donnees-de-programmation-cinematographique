/******************************************************************************************************
*  Copyright (C) 2016 - Evance Kafando                                                                *
*  Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les termes             *
*  de la Licence Publique Générale GNU publiée par la Free Software Foundation (version 2             *
*  ou bien toute autre version ultérieure choisie par vous).                                          *
*																									  *
*  Ce programme est distribué car potentiellement utile, mais SANS AUCUNE GARANTIE, 				  *
*  ni explicite ni implicite, y compris les garanties de commercialisation ou d'adaptation 			  *
*  dans un but spécifique. Reportez-vous à la Licence Publique Générale GNU pour plus de détails.	  *
*																									  *
*  Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même temps que ce programme;* 
*  si ce n'est pas le cas, écrivez à la Free Software Foundation, Inc., 							  *
*  59 Temple Place, Suite 330, Boston, MA 02111-1307, États-Unis									  *
******************************************************************************************************/


/************************
 *  Programme principal *
 ************************/

#include <errno.h>
#include <curses.h>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <cctype>
#include <chrono>
#include <thread>
#include "ocilib.h"
#include "sha2.h"
using namespace std;

#define str_max_size 64
#define sql_req_length 4096

struct acteur
{
	char id_act[str_max_size] ,
	     nom[str_max_size]    ,
	     ddn[str_max_size]    ,
	     pays[str_max_size]   ;

};

struct realisateur
{
	char id_real[str_max_size],
	     nom[str_max_size]    ,
	     ddn[str_max_size]    ,
	     pays[str_max_size]   ;

};

struct film
{
	char id_film[str_max_size],
	     titre[str_max_size]  ,
	     genre[str_max_size]  ,
	     classe[str_max_size] ,
	     annee[str_max_size]  ;
	     
};

struct jouer
{
	char id_acteur[str_max_size],
	     id_film[str_max_size]  ;

};

struct realiser
{
	char id_film[str_max_size] ,
	     id_real[str_max_size] ;

};

OCI_Connection *cn;
OCI_Statement  *st;
OCI_Resultset  *rs;

acteur unActeur;
realisateur unRealisateur;
film unFilm;
jouer Jouer;
realiser Realiser;

char sql_query[sql_req_length];

/*INSERT INTO film
(idfilm, titre, genre, classe, annee)
SELECT '63fba47cf78f367b79468364141f04043b4949a5219f2a9423e738666', `X-Men: Jours d'un avenir passe`, 'Super heros', '', '2014'
FROM dual
WHERE NOT EXISTS (SELECT *
                  FROM film
                  WHERE film.idfilm = '63fba47cf78f367b79468364141f04043b4949a5219f2a9423e738666');*/
void t_acteur()
{
	strcpy(sql_query,"create table acteur ( idacteur varchar2(64) primary key, nom varchar2(64) not null, ddnais date, pays varchar2(64) )");

	if(!OCI_ExecuteStmt(st, sql_query))
	{ 
		printf("Error on table acteur creation.\n");
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
		perror("Error on table realisateur creation: ");
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
		perror("Error on table film creation: ");
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
		perror("Error on table jouer creation: ");
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
  		perror("Erreur dans lors de l'insertion dans la table acteur.\n");
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
  		perror("Erreur lors de l'insertion dans la table realisateur.\n");
}

void insertFilm(char id[65], char titre[65], char genre[65], char classe[65], char annee[65]) // Fonction qui fait l'insertion dans la table t_film
{
	/*strcat(sql_query, "', REPLACE('");
	strcat(sql_query, titre);
	strcat(sql_query, "', ''', ''''), '");*/
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
  		printf("Erreur lors de l'insertion dans la table film.\n");
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
  		printf("Erreur lors de l'insertion dans la table jouer.\n");
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
  		printf("Erreur lors de l'insertion dans la table realiser.\n");
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
  		printf("Error on table dropping.\n");
}

vector<string> field_splitter(string line_of_string)  // Fonction qui coupe une ligne de mot lit dans le fichier
{
	regex reg(R"(\s*[;]\s*)"); // split on ';'
	vector<string> vector_of_string;
	const sregex_token_iterator end;
    for(sregex_token_iterator iter(cbegin(line_of_string), cend(line_of_string), reg, -1);iter != end; ++iter) 
    {
         vector_of_string.push_back(iter->str());
    }

  	return vector_of_string;
}

string idact_or_idreal_generator(string name, string birthday)
{
	regex reg(R"(\s*[\s*]\s*)"); // split on space
	regex _reg(R"(\s*[-]\s*)"); // split on '-'
	stringstream ss;
	string generated_id("");
	vector<string> str;
	transform(name.begin(), name.end(), name.begin(), ::tolower);
    const sregex_token_iterator end;
    const sregex_token_iterator _end;
    for(sregex_token_iterator iter(cbegin(name), cend(name), reg, -1);iter != end; ++iter) 
    {
         str.push_back(iter->str());
    }
    ss << str[0].at(0) << str[1];
    generated_id = ss.str();
    for(sregex_token_iterator _iter(cbegin(birthday), cend(birthday), _reg, -1);_iter != _end; ++_iter) 
    {
         generated_id+= _iter->str();
    }

    return generated_id;
}

string idfilm_generator(string titrefilm)
{
	regex reg(R"(\s*[-',:\.*\s*]\s*)"); // split on espaces . , ' :
	string generated_id("");
	transform(titrefilm.begin(), titrefilm.end(), titrefilm.begin(), ::tolower);
    const sregex_token_iterator end;
    for(sregex_token_iterator iter(cbegin(titrefilm), cend(titrefilm), reg, -1);iter != end; ++iter) 
    {
         generated_id+= iter->str();
    }
    generated_id = sha224(generated_id);
    return generated_id;
}

string replace(string titre)
{
	regex reg("[']+");
	string str("");
	str = regex_replace(titre, reg, "''"); // replace ' by '' 
	return str;
}

void peupler_lestables(string file_name)   // fonction qui lit le fichier de donn?es (cinema.csv)
{
	string idActeur("");
	string nomActeur("");
	string ddnActeur("");
	string paysActeur("");
	             string idfilm("");
	             string titrefilm("");
	             string genrefilm("");
	             string classementfilm("");
	             string anneefilm("");
	                 string idrealisateur("");
	                 string nomrealisateur("");
	                 string ddnrealisateur("");
	                 string paysrealisateur("");

    vector<string> strings_vector;
    ifstream mystream(file_name, ios::in);

	/*createTableActeur();
	createTableRealisateur();
	createTableFilm();
	createTableJouer();
	createTableRealiser();

	this_thread::sleep_for(chrono::seconds(5));*/
	
	if(mystream.is_open())
	{
		string line;

		//printf("Lecture reussi!\n");

		getline(mystream, line);

		while(/*!myFile.eof()*/getline(mystream,line))
		{
			//vector <string> index;
			//string l("");
			//getline(myFile,line);
			strings_vector = field_splitter(line);

			for(int i = 0; i < strings_vector.size(); i++)     // recuperation des champs de strings_vector.
			{
				if(i == 0)
					nomActeur = strings_vector[i];
				else if(i == 1)
					ddnActeur = strings_vector[i];
				else if(i == 2)
					paysActeur = strings_vector[i];
				else if(i == 3)
					titrefilm = strings_vector[i];
				else if(i == 4)
					genrefilm = strings_vector[i];
				else if(i == 5)
					classementfilm = strings_vector[i];
				else if(i == 6)
					anneefilm = strings_vector[i];
				else if(i == 7)
					nomrealisateur = strings_vector[i];
				else if(i == 8)
					ddnrealisateur = strings_vector[i];
				else 
					paysrealisateur = strings_vector[i];
			}

			idActeur = idact_or_idreal_generator(nomActeur, ddnActeur);
			idrealisateur = idact_or_idreal_generator(nomrealisateur, ddnrealisateur);
			idfilm = idfilm_generator(titrefilm);

			titrefilm = replace(titrefilm);

			strcpy(unActeur.id_act, idActeur.c_str());
			strcpy(unActeur.nom, nomActeur.c_str());
			strcpy(unActeur.ddn, ddnActeur.c_str());
			strcpy(unActeur.pays, paysActeur.c_str());

			strcpy(unRealisateur.id_real, idrealisateur.c_str());
			strcpy(unRealisateur.nom, nomrealisateur.c_str());
			strcpy(unRealisateur.ddn, ddnrealisateur.c_str());
			strcpy(unRealisateur.pays, paysrealisateur.c_str());

			strcpy(unFilm.id_film, idfilm.c_str());
			strcpy(unFilm.titre, titrefilm.c_str());
			strcpy(unFilm.genre, genrefilm.c_str());
			strcpy(unFilm.classe, classementfilm.c_str());
			strcpy(unFilm.annee, anneefilm.c_str());

			strcpy(Jouer.id_acteur, idActeur.c_str());
			strcpy(Jouer.id_film, idfilm.c_str());

			strcpy(Realiser.id_film, idfilm.c_str());
			strcpy(Realiser.id_real, idrealisateur.c_str());

			insertActeur(unActeur.id_act, unActeur.nom, unActeur.ddn, unActeur.pays);
			insertRealisateur(unRealisateur.id_real, unRealisateur.nom, unRealisateur.ddn, unRealisateur.pays);
			insertFilm(unFilm.id_film, unFilm.titre, unFilm.genre, unFilm.classe, unFilm.annee);
			insertJouer(Jouer.id_acteur,Jouer.id_film);
			insertRealiser(Realiser.id_film,Realiser.id_real);

			printf("\n\n");

		}

		mystream.close();
	}
	else
		perror("Erreur lors de l'ouverture du fichier\n");
	    //exit(1);
}
/*void insertActeur(char, char, char, char);  
void insertRealisateur(char, char, char, char); 
void insertFilm(char, char, char, char, char);
void insertJouer(char, char);  
void insertRaliser(char, char); 
void dropTable(string);  
vector<string> tockenizer(string, string);  
string genereridAct(string);  
string genereridReal(string); 
string genereridFilm(string);  
void peupler_lestables(string);
void menu();*/ 
int main(int argc, char const *argv[])
{
	/* code */
   /*if(argc<3)
   {
   	perror("Nombre arguments invalide: ");
   	printf("\n");
    printf("Usage: %s projet_cinema.cpp sha2.cpp\n", *argv);
    return EXIT_FAILURE;
   }*/


    unsigned int choice;
    char genre[65],
         acteur[65],
         realisateur[65],
         annee[65];
    static int i = 0;


   if(!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
      return EXIT_FAILURE;

   cn = OCI_ConnectionCreate("XE", "u_name", "u_pass", OCI_SESSION_DEFAULT);
   st = OCI_StatementCreate(cn);
   OCI_ServerEnableOutput(cn, 64000, 10, 255);
   OCI_Date *dt;
   const char *p;
  	/*
     cn : identifiant de connexion
     bufsize : taille maximale tampon c?t? serveur
     arrsize : nombre de lignes ? r?cup?rer en une fois
     lnsize : taille maximale d'une ligne
   */

   t_acteur();
   t_realisateur();
   t_film();
   t_jouer();
   t_realiser();

   this_thread::sleep_for(chrono::seconds(6));
    /*using namespace chrono_literals;
    cout << "Hello waiter" << std::endl;
    auto start = chrono::high_resolution_clock::now();
    this_thread::sleep_for(6s);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> elapsed = end-start;
    cout << "Waited " << elapsed.count() << " ms\n";*/

    peupler_lestables("projet.csv");
    this_thread::sleep_for(chrono::seconds(6));
    system("clear");

    bool use_system = true;
	while (use_system != false)
	{
		printf("\n\n\n********************************************************\n");
		printf("1. Liste des genres.\n");
		printf("2. Liste des films.\n");
		printf("3. Liste des acteurs(rices).\n");
		printf("4. Liste des r?alisateurs(rices).\n");
		printf("5. Liste des films d?un genre donn?.\n");
		printf("6. Liste des films d?un(e) acteur(rice) donn?(e).\n");
		printf("7. Liste des films d?un(e) r?alisateur(rice) donn?(e).\n");
		printf("8. Liste des films d?une ann?e donn?e.\n");
		printf("9. Quitter\n");
		printf("**************************************************************\n\n");
		printf("Enter your choice and press return: ");

		cin >> choice;

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
						    i++;
				            printf("#%d %s\n", i, OCI_GetString(rs,1));
				            //printf("%s\n",OCI_GetString(rs,1));
				         }while(OCI_FetchNext(rs));
				         printf("%d genre(s) au total.\n", OCI_GetRowCount(rs));
				         i = 0;
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
						    i++;
				            printf("#%d %s\n", i, OCI_GetString(rs,1));
				            //printf("%s\n",OCI_GetString(rs,1));
				         }while(OCI_FetchNext(rs));
				         printf("%d film(s) au total.\n", OCI_GetRowCount(rs));
				         i=0;
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
						    i++;
				            printf("#%d %s\n", i, OCI_GetString(rs,1));
				            //printf("%s\n",OCI_GetString(rs,1));
				         }while(OCI_FetchNext(rs));
				         printf("%d acteur(s)(rices) au total.\n", OCI_GetRowCount(rs));
				         i=0;
				    }
				    else
				      printf("Aucun acteur ou actrice\n");
					break;
			case 4:
					printf("Liste des r?alisateurs(rices).\n");
					strcpy(sql_query, "select distinct nom from realisateur order by nom");

				    OCI_ExecuteStmt(st, sql_query);

				    rs = OCI_GetResultset(st);
				    //int i = 0;

				    if(OCI_FetchNext(rs) == TRUE)
				    {
				    	 do
					     {
						    i++;
				            printf("#%d %s\n", i, OCI_GetString(rs,1));
				            //printf("%s\n",OCI_GetString(rs,1));
				         }while(OCI_FetchNext(rs));
				         printf("%d r?alisateur(s)(rices) au total.\n", OCI_GetRowCount(rs));
				         i=0;
				    }
				    else
				      printf("Aucun ou realisateur ou realisatrice\n");
					break;
			case 5:
					printf("Liste des films de grenre: \n");
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
					printf("Liste des films de l'acteur(rice) :\n");
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
					printf("Liste des films du(de la) r?alisateur(rice).\n");
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
					printf("Liste des films de l'ann?e: \n");
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
					use_system = false;
					break;
			default:
					printf( "Not a Valid Choice. \n");
					printf( "Choose again.\n");
					//cin >> choice;
					break;
		}

}

  OCI_DateFree(dt);

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
  this_thread::sleep_for(chrono::seconds(6));
  printf("Au revoir.\n");




	/* cleanup */
  OCI_Cleanup();
  printf("\n");
  return EXIT_SUCCESS;
} // main()