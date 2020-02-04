# Criptare-si-recunoastere-numere
Programul cripteaza si decripteaza o imagine, detecteaza numere scrise de mana dupa un anumit pattern

1) Functia “XORSHIFT32” genereaza “dim” numere random avand ca seed variabila “seed” si le returneaza in vectorul “v” dat ca parametru.
2) Functia “incarcareImagine” returneaza imaginea din fisierul cu calea “bmp” in parametrul “I” de tip imagine (contine lungimea, latimea, header-ul si pixelii imaginii).
3) Functia “salvareImagine” copiaza header-ul si pixelii pe care ii contine “I” in fisierul cu calea “bmp” adaugand si padding-ul.
4) Functia “generarePermutare” returneaza in “sigma” permutarea de dimensiune “dim” ce foloseste primele “dim” numere random ale vectorului “R” (conform algoritmului lui Durstenfeld).
5) Functia “intTo3Chars” primeste ca parametru un intreg (4 octeti) “y” (de forma (x3,x2,x1,x0)) si returneaza 3 variabile de tip char x2,x1,x0.
6) Functia “criptare” incarca imaginea aflata in fisierul cu calea “imagineInitiala” in memoria interna folosind functia “incarcareImagine”, cripteaza imaginea respectiva folosind cheia ce se afla in fisierul cu calea “cheieSecreta”, si apoi salveaza imaginea criptata in fisierul cu calea “imagineCriptata” folosind functia “salvare imagine”.
7) Functia “inversarePermutare” returneaza inversa permutarii “sigma”, cu dimensiunea “dim”, tot in parametrul “sigma”.
8) Functia “decriptare” incarca imaginea aflata in fisierul cu calea “imagineCriptata” in memoria interna folosind functia “incarcareImagine”, decripteaza imaginea respectiva folosind cheia aflata in fisierul cu calea “cheieSecreta”, si apoi salveaza imaginea decriptata in fisierul cu calea “imagineInitiala” folosind functia “salvare imagine”.
9) Functia “chiPatrat” afiseaza valorile testului chi patrat a imaginii din fisierul cu calea “imagineBMP”.
10) Functia “citireCale” citeste de la tastatura caractererele pana la ‘\n’ si returneaza caracterele citite in parametrul “s” (acesta va reprezenta calea unui fisier).
11) Functia “citireCaleDinFisier” citeste, din fisierul catre care pointeaza “f”, caracterele pana la ‘\n’ si returneaza caracterele citite in parametrul “s”.
12) Functia “mainCriptografie” citeste de la tastatura calea unui fisier text, dupa care citeste din acesta caile imaginii initiale, criptate, decriptate si a cheii secrete. Cripteaza imaginea initiala si salveaza imaginea criptata in memoria externa folosind functia “criptare”, dupa care decripteaza imaginea criptata si salveaza imaginea decriptata in memoria externa folosind functia “decriptare”. Totodata afiseaza valorile testului chi patrat pentru imaginea initiala si cea criptata folosind functia “chiPatrat”.
13) Functia “grayscale” primeste ca parametru imaginea “I” si o transforma intr-o imagine grayscale.
14) Functia “corelatie” calculeaza corelatia dintre sablonul “S” si continutul corespunzator al imaginii dat de fereastra “f”.
15) Functia “templateMatching” realizeaza operatia de template matching dintre o imagine “I” si un sablon “S”. Functia returneaza in parametrul D toate ferestrele, corespunzatoare sablonului S, care au corelatia mai mare decat pragul “ps”.
16) Functia “desenare” deseneaza in imaginea “I” conturul ferestrei “f” folosind culoarea “C”.
17) Functia “cmp” este folosita la qsort.
18) Functia “sortare” sorteaza vectorul D in ordinea descrescatoare a corelatiilor detectitiilor, folosind functia qsort.
19) Functia “suprapunere” returneaza valoarea suprapunerii ferestrei “a” cu fereastra “b”.
20) Functia “eliminareNonMaxime” elimina din vectorul D ferestrele ce se suprapun cu alte ferestre ce au corelatii mai mari.
21) Functia “mainRecunoasterePatternuri” citeste de la tastatura calea unui fisier text, dupa care citeste din acesta calea imaginii si a celor 10 sabloane. Ruleaza operatia de template matching dintre imagine si soabloane si reuneste toate detectiile in vectorul “D”. Ruleaza functia de eliminare a non-maximelor pe vectorul “D” si deseneaza detectiile ramase folosind o culoare specifica pentru fiecare sablon.
Programul ruleaza functiile “mainCriptografie” si “mainRecunoasterePatternuri”. Toate imaginile sunt citite din fisiere. Pentru rularea programului e nevoie de 2 fisiere text citite de la tastatura.
Primul fisier text trebuie sa contina in aceasta ordine, despartite prin ‘\n’:
➔ Calea imaginii initiale
➔ Calea imaginii criptate
➔ Calea imaginii decriptate
➔ Calea fisierului text ce contine cheia secreta
Al doilea fisier text trebuie sa contina in aceasta ordine, despartite prin ‘\n’:
➔ Calea imaginii
➔ Caile sabloanelor despartite prin ‘\n’
