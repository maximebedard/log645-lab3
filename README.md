LOG645-LAB3

#Execution

```
./run.sh m n np td h cpu
# Un des paramètres est invalide. Veuillez réessayer.
#   m   = Nombre de lignes   (> 5)
#   n   = Nombre de colonnes (> 5)
#   np  = Nombre de lignes   (> 1)
#   td  = Nombre de lignes   (> 0.0001)
#   h   = Nombre de lignes   (> 0.0001)
#   cpu = Nombre de lignes   (> 3)
```

# Evaluation
```
Introduction
Objectifs	 	 	 3
Découpage rapport	 	 	 2
Analyse
Problème	 	 	 1
Formules	 	 	 1
Dépendances / Partitionnement	 	 	 6
Conception
Séquentielle
Explication	 	 	 2
Clareté	 	 	 1
Parallèle
Partitionnement + Aglomération + Répartition	 	 	 4
Communication	 	 	 2
Algorithme / Flot d'exécution	 	 	 2
Clareté	 	 	 1
Résultats et dicussion
1) Resultats h=0.1, td=0.0002, np=200	 	 	 1
2) Répétabilité de 1)	 	 	 2
3) Courbe temps en fonction taille problème
Nombre subdivisions (h ou m+n)	 	 	 2
Nombre de pas de temps	 	 	 2
4) Extrapolation du temps d'exécution en fonc de la taille du prob
Nombre subdivisions (h ou m+n)	 	 	 1
Nombre de pas de temps	 	 	 1
5) Accélération de m=200,n=300,np=200 en variant P
Graphique	 	 	 1
Discussion (Tendance, Courbe parfaite, Plateau, Comm.)	 	 	 4
6) Accélération en variant taille problème
Graphique	 	 	 1
Discussion	 	 	 2
7) Conclusions (Tendances pour aug. taille, aug processeurs)	 	 	 3
Conclusion
Objectifs	 	 	 3
Claret
```


# Variation du temps en fonction du nombre de subdivision (h)
`m=150, n=100, td=0.0002 et np=100`

|h|t_seq|t_par|
|-|---|---|
|100|14.521186|4.806479|
|10|14.688538|4.133749|
|1|14.686818|3.110817|
|0.1|14.44224|5.890218|
|0.01|14.746107|4.343440|
|0.001|14.605829|5.352994|
|0.0001|14.671868|6.227925|
|0.00001|14.555640|3.058996|

# Variation du temps en fonction du nombre de pas de temps (td)
`m=150, n=100, td=0.0002 et np=100`

|h|t_seq|t_par|
|-|---|---|
|10|1.307072|0.498884|
|100|14.643814|4.223510|
