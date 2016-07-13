#!/bin/sh
if [ "$#" -ne 6 ]; then
  echo "Veuillez founir au moins 6 arguments."
  echo "Aide:"
  echo "  m   = Nombre de lignes   (>= 5)"
  echo "  n   = Nombre de colonnes (>= 5)"
  echo "  np  = Nombre de lignes   (>= 1)"
  echo "  td  = Nombre de lignes   (>= 0.0001)"
  echo "  h   = Nombre de lignes   (>= 0.0001)"
  echo "  cpu = Nombre de lignes   (>= 3)"
  exit 1
fi

if [ $1 -gt 5 ] && [ $2 -gt 5 ] && [ $3 -gt 1 ] && [ $4 -gt 0.00001 ] && [ $5 -gt 0.00001 ] && [ $6 -gt 3 ]; then
  mpirun -n $6 --hostfile ./hostfile ./chaleur $1 $2 $3 $4 $5
else
  echo "Un des paramètres est invalide. Veuillez réessayer."
  echo "  m   = Nombre de lignes   (> 5)"
  echo "  n   = Nombre de colonnes (> 5)"
  echo "  np  = Nombre de lignes   (> 1)"
  echo "  td  = Nombre de lignes   (> 0.0001)"
  echo "  h   = Nombre de lignes   (> 0.0001)"
  echo "  cpu = Nombre de lignes   (> 3)"
fi
