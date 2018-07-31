## **Application Console: Gestion d'une Base de Données de Programmation Cinématographique - C/C++, OCILIB & PL/SQL
**Bases de données relationnelles et réparties : C/C++, OCI & PL/SQL**

**I.  Description générale**

L’application consiste à gérer une **base de données « Cinéma »**.

Les **étapes à suivre** pour le projet sont :

 - i. Création des tables (section II.).
 - ii. Insertion des données à partir d’un fichier texte au format CSV
   (section III.).
 - iii. Développement d’un menu (console) utilisant les langages **C/C++**
   **OCI** et **PL/SQL** (sec-tion IV.).

**II. Tables**
Les clés primaires (resp. clés étrangères) ont été soulignées (resp. surlignées) 
 - Acteur(IDAct, Nom, Ddn, Pays)
 - Realisateur(IDReal, Nom, Ddn, Pays)
 - Film(IDFilm, Titre, Annee, Genre, Classe)
 - Jouer(IDAct, IDFilm)
 - Realiser(IDFilm, IDReal)

**Créer les tables** avec leurs clés et les spécifications supplémentaires suivantes :
 - Tous les champs sont de type “chaîne de caractères” sauf Ddn (date de
   naissance) de type date – Longueur maximale de 64 caractères ;
 - Les noms et titres de films sont obligatoires ;
 - La date de naissance, le pays, le genre et le classement peuvent être
   des champs vides.

**III. Données**
Les données à insérer doivent être extraites d’un seul fichier CSV qui vous est fourni au format suivant :
   

     `Acteur/rice;Naissance;Pays;Film;Genre;Classement;Annee;Realisateur/rice;Naissance;Pays`

On peut générer les clés primaires automatiquement en utilisant par exemple certaines lettres des noms d’acteurs, de réalisateurs ou des titres de films.
Par exemple : **bpitt** pour l’acteur Brad Pitt, **le_bon** pour le film “Le bon, la brute et le truand”,etc.
Ou bien vous pouvez aussi générer des **numéros d’une séquence Oracle**.

**IV. Application**
Écrire une application interactive en **C/C++, OCI** proposant le menu suivant :

 1. Liste des genres
 2. Liste des films
 3. Liste des acteurs(rices)
 4. Liste des réalisateurs(rices)
 5. Liste des films d’un genre donné
 6. Liste des films d’un(e) acteur(rice) donné(e)
 7. Liste des films d’un(e) réalisateur(rice) donné(e)
 8. Liste des films d’une année donnée
 9. Quitter


- Les **actions** correspondant à chaque choix doivent être implémentées par des **fonctions ou procédures stockées PL/SQL**.
- Les **paramètres** nécessaires à certaines fonctions/procédures stockées (ex. : choix 5, 6, 7, 8) doivent être entrés en ligne de commande.
- On peut utiliser les **exceptions PL/SQL** pour gérer les cas d’erreurs (aucun film d’un genre donné, d’une année donnée, etc.).
