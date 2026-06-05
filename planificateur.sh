#!/bin/bash
TACHE="tache.conf"
[[ ! -f "$TACHE" ]] && touch "$TACHE"
 
BLEU='\e[1;36m'
RESET='\033[0m'
GRAS='\033[1m'
 

afficher_taches() 
{
    echo ""
    echo -e "${GRAS}=== Tâches planifiées ===${RESET}"
    echo ""
    echo -e "\n${GRAS}  ID   Nom                  Heure             Planification       Commande ${RESET}"
    echo    "  ────────────────────────────────────────────────────────────────────────────"
 
    numero=1
    while IFS=';' read -r nom heure planification commande ; do
        echo -e "  ${GRAS}[$numero]${RESET}   $nom              $heure            $planification      $commande"
        numero=$(( numero + 1 ))
    done < "$TACHE"
}

ajouter_tache() 
{
    echo ""
    echo -e "${GRAS}=== Ajouter une tâche ===${RESET}"
    echo ""
 
    read -rp "nom de la tache : " nom 
    read -rp "  Commande : " commande
    [[ -z "$commande" ]] && echo "Commande vide" && return
 
    echo ""
    echo "  Repetition : "
    echo "    1) Date et heure precises"
    echo "    2) Toutes les heures"
    echo "    3) Tous les jours"
    echo "    4) Toutes les semaines"
    echo ""
    read -rp "  Choix : " choix
    read -rp "  Heure (HH:MM) : " heure
 
    h="${heure%%:*}"
    m="${heure##*:}"
 
    case "$choix" in
        1)
            read -rp "Date (AAAA-MM-JJ) : " date
            echo "$commande" | at "$heure $date" ;;
        2) ligne="$m * * * * $commande"      ; date="chaque heure" ;;
        3) ligne="$m $h * * * $commande"  ; date="tous les jours à $heure" ;;
        4) ligne="$m $h * * 1 $commande"  ; date="chaque lundi à $heure" ;;
        *) echo -e "Choix invalide." ; return ;;
    esac
 
    echo "$nom;${heure};${date};${commande}" >> "$TACHE"

    if [ "$choix" != "1" ] ;then
        ( crontab -l 2>/dev/null ; echo "$ligne" ) | crontab -
    fi
}
 

supprimer_tache() 
{
    [[ ! -s "$TACHE" ]] && return
    read -rp "Numéro de tache à supprimer : " numero
    commande=$(sed -n "${numero}p" "$TACHE" | cut -d';' -f4)

    if [[ -z "$commande" ]]; then
        echo -e " Numéro invalide."
        return
    fi
    sed -i "${numero}d" "$TACHE"
    crontab -l  | grep -v "$commande" | crontab -
}
 


while true; do

    clear
    echo -e "\n${GRAS}${BLEU}  ╔══════════════════════════════╗"
    echo      "  ║   PLANIFICATEUR DE TACHES    ║"
    echo -e   "  ╚══════════════════════════════╝${RESET}"
 
    echo -e "${GRAS}--- Menu ---${RESET}"
    echo "  1) Ajouter une tâche"
    echo "  2) Supprimer une tâche"
    echo "  3) Lister les tâches existants"
    echo "  4) Quitter"
    echo ""
    read -rp "  Votre choix : " choix_menu
    echo ""
 
    case "$choix_menu" in
        1) ajouter_tache ;;
        2) supprimer_tache ;;
        3) 
            afficher_taches
            read -rp "Appuyez sur Entrée pour continuer...";;      
        4) exit 0 ;;
        *) echo -e "Choix invalide." ;;
    esac
done

