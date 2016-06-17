#!/bin/sh
if [ -z "$*" ]; then
  echo "Nombre de lignes (n >= 1):"
  read nbLines
  echo "Nombre de colonnes (m >= 1):"
  read nbColumns
  echo "Nombre de pas de temps (np >= 1):"
  read timeStep
  echo "Temps discrétisé (td >= 1):"
  read descreteTime
  echo "Taille d'une subdivision (h >= 1):"
  read subdivisionSize
  echo "Nombre de processeurs (cpus >= 1):"
  read nbCpus
  if [ $nbLines -gt 0 ] && [ $nbColumns -gt 0 ] && [ $timeStep -gt 0 ] && [ $descreteTime -gt 0 ] && [ $subdivisionSize -gt 0 ] && [ $nbCpus -gt 0 ]; then
    mpirun --hostfile ./hostfile ./chaleur $nbLines $nbColumns $timeStep $descreteTime $subdivisionSize $nbCpus
  else
    echo "Parametre invalide. Veuillez essayer a nouveau."
  fi
else
  mpirun --hostfile ./hostfile ./chaleur $@
fi
