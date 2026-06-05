import tkinter as tk
from tkinter import messagebox
import subprocess, os

FICHIER = "tache.conf"

if not os.path.exists(FICHIER):
    open(FICHIER, "w").close()

def lire():
    with open(FICHIER, "r") as f:
        return [l.strip().split(";") for l in f if l.strip()]

def sauver(taches):
    with open(FICHIER, "w") as f:
        for t in taches:
            f.write(";".join(t) + "\n")

def cron_ajout(ligne):
    ancien = subprocess.run(
        "crontab -l", shell=True,
        capture_output=True, text=True
    ).stdout
    subprocess.run(f'echo "{ancien}{ligne}\n" | crontab -', shell=True)

def cron_supprimer(cmd):
    r = subprocess.run(
        "crontab -l", shell=True,
        capture_output=True, text=True
    )
    lignes = [l for l in r.stdout.splitlines() if cmd not in l]
    subprocess.run(
        f'echo "{"\\n".join(lignes)}" | crontab -',
        shell=True
    )

def ajouter():
    f = tk.Toplevel(root)
    f.title("Ajouter")
    f.geometry("350x300")

    e_nom = tk.Entry(f)
    e_cmd = tk.Entry(f)
    e_h = tk.Entry(f)

    tk.Label(f, text="Nom").pack()
    e_nom.pack()

    tk.Label(f, text="Commande").pack()
    e_cmd.pack()

    tk.Label(f, text="Heure HH:MM").pack()
    e_h.pack()

    rep = tk.StringVar(value="Jour")
    tk.OptionMenu(
        f, rep,
        "Heure", "Jour", "Semaine"
    ).pack()

    def valider():
        nom = e_nom.get()
        cmd = e_cmd.get()
        heure = e_h.get()

        if not nom or not cmd or ":" not in heure:
            return

        h, m = heure.split(":")

        if rep.get() == "Heure":
            ligne = f"{m} * * * * {cmd}"
            plan = "Toutes les heures"

        elif rep.get() == "Jour":
            ligne = f"{m} {h} * * * {cmd}"
            plan = f"Tous les jours {heure}"

        else:
            ligne = f"{m} {h} * * 1 {cmd}"
            plan = f"Chaque lundi {heure}"

        t = lire()
        t.append([nom, heure, plan, cmd, "0"])
        sauver(t)

        cron_ajout(ligne)

        rafraichir()
        f.destroy()

    tk.Button(
        f,
        text="Ajouter",
        command=valider
    ).pack(pady=10)

def supprimer():
    s = liste.curselection()
    if not s:
        return

    t = lire()
    cmd = t[s[0]][3]

    t.pop(s[0])
    sauver(t)
    cron_supprimer(cmd)
    rafraichir()

def rafraichir():
    liste.delete(0, tk.END)

    for i, t in enumerate(lire()):
        liste.insert(
            tk.END,
            f"[{i+1}] {t[0]} - {t[2]}"
        )

root = tk.Tk()
root.title("Planificateur de tâches")
root.geometry("700x450")

tk.Label(
    root,
    text="Planificateur de tâches",
    font=("Arial",16,"bold")
).pack(pady=10)

liste = tk.Listbox(root, width=90, height=15)
liste.pack()

cadre = tk.Frame(root)
cadre.pack(pady=15)

tk.Button(
    cadre,
    text="Ajouter",
    command=ajouter
).pack(side=tk.LEFT,padx=5)

tk.Button(
    cadre,
    text="Supprimer",
    command=supprimer
).pack(side=tk.LEFT,padx=5)

tk.Button(
    cadre,
    text="Lister",
    command=rafraichir
).pack(side=tk.LEFT,padx=5)

tk.Button(
    cadre,
    text="Quitter",
    command=root.quit
).pack(side=tk.LEFT,padx=5)

rafraichir()
root.mainloop()