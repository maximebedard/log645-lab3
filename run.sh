#!/bin/sh
if [ "$#" -ne 6 ]; then
  echo "Veuillez founir au moins 6 arguments."
  echo "Aide:"
  echo "  m   = Nombre de lignes                   (>= 5)"
  echo "  n   = Nombre de colonnes                 (>= 5)"
  echo "  np  = Nombre de pas de temps             (>= 1)"
  echo "  td  = Temps discrétisé                   (>= 0.000001)"
  echo "  h   = Taille d'un coté d'une subdivision (>= 0.000001)"
  echo "  cpu = Nombre de CPU                      (>= 3)"
  exit 1
fi


dec=0.000000001
td=$5
h=$6

if [ $1 -gt 5 ] &&
  [ $2 -gt 5 ] &&
  [ $3 -gt 1 ] &&
  [ 1 -eq "$(echo "${td} > ${dec}" | bc)" ] &&
  [ 1 -eq "$(echo "${h} > ${dec}" | bc)" ] &&
  [ $6 -gt 3 ]; then
  mpirun -n $6 --hostfile ./Hostfile ./chaleur $1 $2 $3 $4 $5 $6
else
  echo "Un des paramètres est invalide. Veuillez réessayer."
  echo "Aide:"
  echo "  m   = Nombre de lignes                   (>= 5)"
  echo "  n   = Nombre de colonnes                 (>= 5)"
  echo "  np  = Nombre de pas de temps             (>= 1)"
  echo "  td  = Temps discrétisé                   (>= 0.000001)"
  echo "  h   = Taille d'un coté d'une subdivision (>= 0.000001)"
  echo "  cpu = Nombre de CPU                      (>= 3)"
fi
