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
|250|37.102615|7.896560|
|500|74.699332|15.751970|
|1000|149.377522|36.785975|

# Variation du temps en fonction de m
`n=100, np=100, td=0.00020, h=0.01000`

|m|t_seq|t_par|
|-|-----|-----|
|10|0.767380|0.250927|
|20|1.771845|0.653761|
|30|2.691128|0.595595|
|40|3.671336|0.720013|
|50|4.697511|0.890072|
|60|5.665916|1.472604|
|70|6.733387|3.075170|
|80|7.691274|2.046273|
|90|8.741064|3.263430|
|100|9.658966|1.823477|
|110|10.570698|1.931854|
|120|11.726576|2.805867|
|130|12.852984|4.083904|
|140|13.739536|4.347655|
|150|14.751609|3.001981|
|160|16.058932|2.972488|
|170|16.855060|3.304644|
|180|17.850659|4.185950|
|190|18.666934|3.229653|
|200|19.707036|3.480351|


# Variation du temps en fonction de n
`m=100, np=100, td=0.00020, h=0.01000`

|n|t_seq|t_par|
|-|-----|-----|
|10|0.837378|0.373125|
|20|1.780098|1.059177|
|30|2.768478|0.614737|
|40|3.848486|1.663653|
|50|4.712668|1.004327|
|60|5.616229|1.669430|
|70|6.798671|2.371445|
|80|7.908095|4.321791|
|90|8.707599|1.716656|
|100|9.772382|2.575059|
|110|10.861310|4.360588|
|120|11.982981|4.502602|
|130|12.677449|2.350872|
|140|13.596404|5.503491|
|150|14.727164|3.221938|
|160|15.684838|5.008783|
|170|16.657169|6.913614|
|180|17.666441|3.875479|
|190|18.749575|3.458428|
|200|19.983381|3.934320|


# Variation du temps en fonction du pas de temps (np)

|n|t_seq|t_par|
|-|-----|-----|
|10|0.227736|0.058699|
|20|0.458377|0.082019|
|30|0.672860|0.122547|
|40|0.880808|0.180292|
|50|1.108959|0.204962|
|60|1.379209|0.288009|
|70|1.610765|0.364781|
|80|1.835778|0.381713|
|90|2.049279|0.378387|
|100|2.289939|1.430495|
|110|2.457150|0.473289|
|120|2.684137|0.492999|
|130|3.015064|0.754581|
|140|3.188324|0.596909|
|150|3.479472|0.767558|
|160|3.685683|0.687948|
|170|4.307205|0.879434|
|180|4.262536|0.758842|
|190|4.458303|3.083425|
|200|4.727112|1.671621|
