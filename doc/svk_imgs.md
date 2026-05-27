# 1

šachovnice 5x5 prázdná, zezhora padají zrníčka "tak nějak"

# 2

ta samá šachovnice v konfiguraci:

|   | A | B | C | D | E |
| - | - | - | - | - | - |
| 5 | 1 | 2 | 3 | 0 | 0 | 
| 4 | 0 | 1 | 0 | 1 | 3 |
| 3 | 2 | 1 | 2 | 3 | 2 |
| 2 | 1 | 0 | 3 | 1 | 0 |
| 1 | 3 | 2 | 1 | 3 | 2 |

12345 ^
abcde >

padá 1 zrníčko na D3 - políčko podbarvené

# 3

to samé, na D3 už jsou 4 zrníčka a 4 šipky ukazují na přilehlá políčka

# 4

to samé, zrníčka už se přesunula, tedy stav je:
D3 = 0
D4 = 2
D2 = 2
C3 = 3
E3 = 3

# 5

to samé, padá zrníčko na E4 - to je podbarvené
D3 už není podbarvené

# 6

to samé, na E4 jsou 4 zrníčka a ukazují z něj 4 šipky

# 7

to samé, zrníčka se přesunula, tj.
E4 = 0
E5 = 1
E3 = 4 <-
D4 = 2

a jedno zrníčko padá na zem

protože E3 má teď 4 zrníčka, je zvýrazněné (asi obtažené, ne podbarvené)
podbarvené = tam to dopadlo
obtažené = tam byla lavina

# 8

stejný, ale z E3 ukazují 4 šipky

# 9

stejný, zrníčka se už přesunula, tj.
E3 = 0
E4 = 1
E2 = 1
D3 = 1
jedno padá na zem

# 10

stejný, ale už není nic pobarvené ani obtažené, zrníčko co padalo už také není
vidět

# 11

stejná konfigurace ale už na grafu (mřížce) - podobně jako fig.4 v bpr

# 12

stejné, ale navíc jeden sink vertex

# 13

ER graf, 1 vertex je nepřipojen

na slide ještě: $G_(ER)(n,p)$

# 14

BA graf

na slide ještě: $G_(BA)(G_0,n,m)$

# 15

ten samý graf, chce se připojit nový vertex a u existujících je ppst s jakou se
připojí konkrétně k němu

ještě: $p = degree(v_i) / Sigma degree(v)$

# 16

WS graf

ještě: $G_(WS)(n,k,beta)$

# 17

základní graf pro následující obrázky
obsahuje 3 komponenty:
1. jeden samotný vertex
2. tři vertexy a dvě hrany
3. úplný graf stupně 4

navíc obsahuje uprostřed sink vertex, zatím nijak nepřipojený

# 18

graf a _Fill to 2_

nadpis: Fill to N
někde tam ještě je: N = 2

# 19

graf a _Fill Up_

někde: "Všichni lidé mají stejnou míru trpělivosti(??)."

# 20 

graf a _As Many As Neighbours_

někde: "Čím víc známých člověk má, tím je odolnější vůči negativním zkušenostem."

# 21

graf a _All Once_

někde: to samý, "ale většina negativních zkušeností zůstává ve společnost."

# 22

power-law na log-log plotu

někde napsáno předpis $𝑦 = 𝑎𝑥^(-𝑘)$

# 23

(graf) mřížka v konfiguraci:

|   | A | B | C | D | E |
| - | - | - | - | - | - |
| 5 | 1 | 2 | 3 | 2 | 1 | 
| 4 | 3 | 3 | 2 | 3 | 1 |
| 3 | 3 | 3 | 0 | 2 | 2 |
| 2 | 1 | 3 | 2 | 1 | 2 |
| 1 | 1 | 2 | 3 | 3 | 1 |

různými barvami jsou zakroužkované klastry lavin různých velikostí:
1: c5, d4
2: c1-d1
5: a4-b4-a3-b3-b2

# 24

ten samý grafík, ale místo vrcholů a zrníček jsou smajlíci a černý mráčky

















































