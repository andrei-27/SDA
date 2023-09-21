###  Pirlea Andrei 312CAa 2022-2023

# Tema 1 - Virtual Memory Allocator
- fisiere: main.c, vma.c, vma.h, functions.c, functions.h, list.c, list.h

## Descriere

Programul citeste de la tastatura operatia, iar apoi apeleaza functia respectiva
sau afiseaza mesajul *Invalid command* daca aceasta nu exista.

Functia *alloc_arena* creeaza arena precum si lista de blocuri adiacenta
acesteia. Functia *dealloc_arena* elimina arena prin apeluri succesive a
functiei free_block si incheie programul.

Alocarea unui bloc/minibloc se realizeaza in interiorul functiei *alloc_block*
in functie de adresa si de dimensiunea noului bloc, fiind apelata functia
necesara pentru alocarea acestuia.

Stergerea unui minibloc este similara cu stergerea unui nod dintr-o lista,
diferite fiind cazurile in care stergem tot blocul sau stergem un minibloc din
jumatatea listei, caz ce duce la spargerea in doua blocuri (caz in care apeleaza
functia *split_block*).

Citirea si afisarea datelor aflate in rw_buffer in miniblocuri se realizeaza cu
ajutorul functiilor *read* si *write*. Initial, este verificat daca adresa la
care efectuam operatia este valida si daca avem perimsiunile necesare.
Verificarea permisiunilor se face intr-o functie separata *check_perm* prin
relizarea operatiei & intre permisiunea necesara si permisiunea din fiecare
bloc. Scrierea si citirea se realizeaza in fiecare minibloc in parte, urmand sa
trecem in urmatorul minibloc daca nu am ajuns la dimensiunea data.

Permisiunile sunt date miniblocurilor in cadrul functiei *mprotect*. Afisarea
arenei se face prin intermediul functiei *pmap*, parcurgand fiecare bloc si
lista acestuia si afisand datele solicitate in enunt.
