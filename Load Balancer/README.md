###  Pirlea Andrei 312CAa 2022-2023

# Tema 2 - Load Balancer
- fisiere: main.c, hashtable.h, hashtable.c, server.h, server.c,
  load_balancer.h, load_balancer.c, utils.h

## Descriere

Scopul este implementarea unui Load Balancer folosind Consistent Hashing.

Acest lucru se realizeaza prin simularea unui hash ring. Astfel, pentru fiecare
pereche cheie-valoare vom calcula hashul pentru cheie si vom stoca aceste date
pe serverul cu cel mai apropiat hash, mai mare decat hashul actual. Fiecare
server este duplicat de 3 ori pentru a asigura distributia cat mai uniforma a
acestora.

Functia *init_load_balancer* aloca memorie pentru Load Balancer.

Functia *loader_add_server* mareste dimensiunea hash ring-ului pentru noul
server adaugat, iar pentru fiecare copie a acestuia cauta pozitia din hash ring
unde trebuie amplasat. Memoria acestui server este alocata in functia
*init_server_memory*, unde cream un hashtable pentru perechile chei-valoare.
Dupa gasirea pozitiei, mutam cu o pozitie in sensul acelor de ceasornic (la
dreapta) serverele de dupa cel pe care il inseram si ne asiguram ca noul server
preia load din serverul de dupa acesta prin functia *server_balance*. Aceasta
parcurge toate bucket-urile din hashtable si muta in noul server doar perechile
cheie-valoare corespunzatoare.

Functia *loader_remove_server* cauta toate copiile serverului cu ID-ul dat, le
elimina din hash ring, mutand restul serverelor la stanga cu o pozitie si
apeleaza functia *server_remove* care muta toate perechile cheie-valoare in
urmatorul server si elimina serverul dat. Totodata, micsoreaza dimensiunea hash
ring-ului.

Functia *loader_store* cauta serverul unde trebuie adaugata informatia si o
adauga.

Functia *loader_retrieve* cauta serverul unde este stocata informatia, o extrage
din hashtable si returneaza valoarea.

Functia *free_load_balancer* sterge fiecare server din hash ring prin apeluri
succesive ale functiei *free_server_memory*, care elibereaza hashtable-ul. In
final, elibereaza memoria hash ring-ului si a Load Balancer-ului.
