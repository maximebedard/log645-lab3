#!/bin/sh
if [ -z "$*" ]; then
  echo "Programme (1|2):"
  read choice
  echo "Valeur par defaut (>= 1):"
  read default_value
  echo "Nombre d'iteration (>= 1):"
  read iteration
  if ([ $choice -eq 1 ] || [ $choice -eq 2 ]) && [ $default_value -gt 0 ] && [ $iteration -gt 0 ]; then
    mpirun --hostfile ./hostfile chaleur $choice $default_value $iteration
  else
    echo "Parametre invalide. Veuillez essayer a nouveau."
  fi
else
  mpirun --hostfile ./hostfile chaleur $@
fi
