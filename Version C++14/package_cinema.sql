-- PACKAGE de procédures stockés pour faire différentes requêtes --

CREATE OR REPLACE PACKAGE CINEMA AS

	-- Procédure pour faire l'insertion dans la table t_acteur

	PROCEDURE insert_acteur ( in_idAct IN t_acteur.idAct%TYPE,
							  in_noma IN t_acteur.noma%TYPE,
							  in_ddna IN t_acteur.ddna%TYPE,
							  in_apays IN t_acteur.pays%TYPE );

	-- Procedure pour faire l'insertion dans la table t_realisateur

	PROCEDURE insert_realisateur ( in_idReal IN t_realisateur.idReal%TYPE,
								   in_nomr IN t_realisateur.nomr%TYPE,
								   in_ddnr IN t_realisateur.ddnr%TYPE,
								   in_rpays IN t_realisateur.pays%TYPE );

	-- Procédure pour faire l'insertion dans la table t_film

	PROCEDURE insert_film ( in_idFilm IN t_film.idFilm%TYPE,
							in_titre IN t_film.titre%TYPE,
							in_annee IN t_film.annee%TYPE,
							in_genre IN t_film.genre%TYPE,
							in_classe IN t_film.classe%TYPE );

	-- Procédure pour faire l'insertion dans la table t_jouer

	PROCEDURE insert_jouer ( in_idAct IN t_jouer.idAct%TYPE,
							 in_idFilm IN t_jouer.idFilm%TYPE );

	-- Procédure pour faire l'insertion dans la table t_realiser

	PROCEDURE insert_realiser ( in_idFilm IN t_realiser.idFilm%TYPE,
								in_idReal IN t_realiser.idReal%TYPE );

	-- Procédure pour faire une affichage de details sur un film

	PROCEDURE infoFilm;

	-- Procédure qui genére une liste des genres de films

	PROCEDURE genres;

	-- Procédure qui genére une liste de films

	PROCEDURE films;

	-- Procédure qui genére une liste des acteurs(rices)

	PROCEDURE acteurs;

	-- Procédure  qui genére une liste des réalisateur(rices)

	PROCEDURE realisateurs;

	-- Procédure qui genére une liste des films d'un genre donné

	PROCEDURE films_genre_donne ( genre_in IN t_film.genre%TYPE );

	-- Procédure qui genére une liste des films d'un(e) acteur(rice) donné(e)

	PROCEDURE films_acteur_donne ( noma_in IN t_acteur.noma%TYPE );

	-- Procédure qui genére une liste des films d'un réalisateur(rice) donné(e)

	PROCEDURE films_realisateur_donne ( nomr_in IN t_realisateur.nomr%TYPE );


	-- Procédure qui genére une liste de films d'une année donnée

	PROCEDURE films_annee_donnee ( annee_in IN t_film.annee%TYPE);
END;
/

show errors

CREATE OR REPLACE PACKAGE BODY cinema AS

-- Procédure pour faire l'insertion dans la table t_acteur

	PROCEDURE insert_acteur ( in_idAct IN t_acteur.idAct%TYPE,
							  in_noma IN t_acteur.noma%TYPE,
							  in_ddna IN t_acteur.ddna%TYPE,
							  in_apays IN t_acteur.pays%TYPE ) IS
	
	nCount NUMBER;
	requete VARCHAR2(256);

	BEGIN
		SELECT COUNT(*) INTO nCount FROM t_acteur WHERE idAct = in_idAct;
		IF(nCount <= 0) THEN
			requete := 'INSERT INTO t_acteur VALUES ( in_idAct, in_noma, in_ddna, in_apays )';
			execute immediate requete;
		END IF;
		
		EXCEPTION
			WHEN OTHERS THEN
				ROLLBACK WORK;
	END;

	-- Procedure pour faire l'insertion dans la table t_realisateur

	PROCEDURE insert_realisateur ( in_idReal IN t_realisateur.idReal%TYPE,
								   in_nomr IN t_realisateur.nomr%TYPE,
								   in_ddnr IN t_realisateur.ddnr%TYPE,
								   in_rpays IN t_realisateur.pays%TYPE ) IS

	BEGIN
		INSERT INTO t_realisateur
		VALUES ( in_idReal, in_nomr, in_ddnr, in_rpays );

		EXCEPTION
			WHEN OTHERS THEN
				ROLLBACK WORK;
	END;

	-- Procédure pour faire l'insertion dans la table t_film

	PROCEDURE insert_film ( in_idFilm IN t_film.idFilm%TYPE,
							in_titre IN t_film.titre%TYPE,
							in_annee IN t_film.annee%TYPE,
							in_genre IN t_film.genre%TYPE,
							in_classe IN t_film.classe%TYPE ) IS

	BEGIN
		INSERT INTO t_film
		VALUES ( in_idFilm, in_titre, in_annee, in_genre, in_classe );

		EXCEPTION
			WHEN OTHERS THEN
				ROLLBACK WORK;
	END;

	-- Procédure pour faire l'insertion dans la table t_jouer

	PROCEDURE insert_jouer ( in_idAct IN t_jouer.idAct%TYPE,
							 in_idFilm IN t_jouer.idFilm%TYPE ) IS

	BEGIN
		INSERT INTO t_jouer
		VALUES ( in_idAct, in_idFilm );

		EXCEPTION
			WHEN OTHERS THEN
				ROLLBACK WORK;
	END;

	-- Procédure pour faire l'insertion dans la table t_realiser

	PROCEDURE insert_realiser ( in_idFilm IN t_realiser.idFilm%TYPE,
								in_idReal IN t_realiser.idReal%TYPE )  IS

	BEGIN
		INSERT INTO t_realiser
		VALUES ( in_idFilm, in_idReal );

		EXCEPTION
			WHEN OTHERS THEN
				ROLLBACK WORK;
	END;
-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
-- ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	-- Procédure pour faire une affichage de details sur un film

	PROCEDURE infoFilm IS

		CURSOR curseur IS
			SELECT a.titre, a.genre, a.annee, t_realisateur.nomr
			FROM t_realisateur, ( SELECT titre, genre, annee, idReal
								  FROM t_film, t_realiser
								  WHERE t_film.idFilm = t_realiser.idFilm) a
			WHERE t_realisateur.idReal = a.idReal;

		titre_out t_film.titre%TYPE;
		genre_out t_film.genre%TYPE;
		annee_out t_film.annee%TYPE;
		nomr_out t_realisateur.nomr%TYPE;

		BEGIN
			OPEN curseur;

			FETCH curseur INTO titre_out, genre_out, annee_out, nomr_out;

			IF curseur%NOTFOUND THEN
				DBMS_OUTPUT.PUT_LINE( 'Les tables sont vides');
			ELSE
				DBMS_OUTPUT.PUT_LINE ( 'Liste des films dans la BD :'/* || chr(10) */);
				DBMS_OUTPUT.PUT_LINE ( chr(9) || 'Titre du film' || chr(9) || 'Genre du film' || chr(9) || 'Annee sortie' || chr(9) || 'Realisateur' || chr(10));

				WHILE curseur%FOUND LOOP
					DBMS_OUTPUT.PUT_LINE ( chr(9) || titre_out || chr(9) || genre_out || chr(9) || annee_out || chr(9) || nomr_out );
					FETCH curseur INTO titre_out, genre_out, annee_out, nomr_out;
				END LOOP;
			END IF;

			CLOSE curseur;

			--DBMS_OUTPUT.NEW_LINE();
		END;

-- Procédure qui genére une liste des genres de films

	PROCEDURE genres IS

		CURSOR curseur IS
			SELECT DISTINCT genre
			FROM t_film;

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste de gernes des films dans la BD :'/* || chr(10) */);

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.genre );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;

	-- Procédure qui genére une liste de films

	PROCEDURE films IS

		CURSOR curseur IS
			SELECT titre
			FROM t_film;

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste de films dans la BD :'/* || chr(10)*/ );

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.titre );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;

	-- Procédure qui genére une liste des acteurs(rices)

	PROCEDURE acteurs IS

		CURSOR curseur IS
			SELECT noma
			FROM t_acteur;

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste des acteurs/actrices des films dans la BD :'/* || chr(10)*/ );

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.noma );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;

	-- Procédure  qui genére une liste des réalisateur(rices)

	PROCEDURE realisateurs IS

		CURSOR curseur IS
			SELECT nomr
			FROM t_realisateur;

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste des realisateurs/realisatrinces des films dans la BD :' /*|| chr(10) */);

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.nomr );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;

	-- Procédure qui genére une liste des films d'un genre donné

	PROCEDURE films_genre_donne ( genre_in IN t_film.genre%TYPE ) IS

		CURSOR curseur IS
			SELECT titre
			FROM t_film
			WHERE UPPER(genre) LIKE '%' || UPPER(genre_in) || '%';

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide ou pas de film de genre: ' || genre_in || '.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste de films dans la BD du genre ' || genre_in || ' :'/* || chr(10) */);

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.titre );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;

	-- Procédure qui genére une liste des films d'un(e) acteur(rice) donné(e)

	PROCEDURE films_acteur_donne ( noma_in IN t_acteur.noma%TYPE ) IS

		CURSOR curseur IS
			SELECT titre
			FROM t_film
			WHERE idFilm IN ( SELECT idFilm
							  FROM t_jouer
							  WHERE idAct IN ( SELECT idAct
							  				  FROM t_acteur
							  				  WHERE UPPER(noma) LIKE '%' || UPPER(noma_in) || '%'));

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide ou pas de film pour l''acteur(rice): ' || noma_in || '.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste de films dans la BD de l''acteur/actrice ' || noma_in || ' :'/* || chr(10) */);

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.titre );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;

	-- Procédure qui genére une liste des films d'un réalisateur(rice) donné(e)

	PROCEDURE films_realisateur_donne ( nomr_in IN t_realisateur.nomr%TYPE ) IS

		CURSOR curseur IS
			SELECT titre
			FROM t_film
			WHERE idFilm IN ( SELECT idFilm
							  FROM t_realiser
							  WHERE idReal IN ( SELECT idReal
							  				   FROM t_realisateur
							  				   WHERE UPPER(nomr) LIKE '%' || UPPER(nomr_in) || '%'));

		tuple curseur%ROWTYPE;

	BEGIN 
		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide ou pas de film pour le realisateur: ' || nomr_in || '.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste de films dans la BD realise par ' || nomr_in || ' :'/* || chr(10) */);

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.titre );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;


	-- Procédure qui genére une liste de films d'une année donnée

	PROCEDURE films_annee_donnee ( annee_in IN t_film.annee%TYPE) IS

		CURSOR curseur IS
			SELECT titre
			FROM t_film WHERE UPPER(annee) LIKE '%' || UPPER(annee_in) || '%';

		tuple curseur%ROWTYPE;

	BEGIN 

		OPEN curseur;

		FETCH curseur INTO tuple;

		IF curseur%NOTFOUND THEN
			DBMS_OUTPUT.PUT_LINE( 'La table est vide ou il n''y a pas de film pour l''annee ' || annee_in ||'.');
		ELSE

			DBMS_OUTPUT.PUT_LINE ( 'Liste de films dans la BD sortis dans l''annee ' || annee_in || ' :'/* || chr(10) */);

			WHILE curseur%FOUND LOOP
				DBMS_OUTPUT.PUT_LINE ( chr(9) || tuple.titre );
				FETCH curseur INTO tuple;
			END LOOP;
		END IF;

		CLOSE curseur;

		--DBMS_OUTPUT.NEW_LINE();
	END;
END;
/
show errors
