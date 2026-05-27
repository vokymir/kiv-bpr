# Slide 1 - úvod

Dobré dopoledne, 
jmenuji se Jakub Vokoun, studuji Počítačové vědy na katedře Informatiky a 
výpočetní techniky. V rámci předmětu _Bakalářský projekt_ jsem zkoumal dynamiku
na náhodných grafech.

Abych si to dokázal lépe představit, vymyslel jsem si motivační příběh: šíření
špatné nálady mezi lidmi. 

# ??? Slide 2 - obr. 24

K modelování jsem si upravil _Sandpile model_, který v původní verzi funguje
takto:

# Slide 3 - obr. 1

Máme šachovnici na kterou náhodně dopadají zrníčka písku.

# Slide 4 - obr. 2

Zrníčka na každém políčku padají přímo na sebe a _stavějí věže_. Pokud je výška
věže 4 nebo víc, musí se rozpadnout v lavině. Na obrázku právě dopadá čtvrté
zrníčko na pole D3.

# Slide 5 - obr. 3

Věž se rozpadne na 4 sousední políčka.

# Slide 6 - obr. 4

Tím lavina končí, měla velikost jedna, protože spadla právě jedna věž.

# Slide 7 - obr. 5

Když dopadne zrníčko na pole E4, to se opět rozpadne v lavině.

# Slide 8 - obr. 6

Nastává nová situace: pole E4 je na hraně šachovnice, proto jedno zrníčko padá
ze stolu na zem.

# Slide 9 - obr. 7

Zároveň, tím lavina nekončí. Na poli E3 je teď dost zrníček a proto lavina
pokračuje.

# Slide 10 - obr. 8

# Slide 11 - obr. 9

Jedno zrníčko opět padá ven, lavina končí. Způsobila zhroucení dvou věží, proto
je její velikost 2.

# Slide 12 - obr. 10

&&&&TODO

Zrníčka představují negativní zkušenosti, které se všem náhodně dějí. Laviny
reprezentují člověka nepříjemného na jeho okolí poté, co "přeteče pohár jeho
trpělivosti".

Co mi do příběhu nesedělo je šachovnice. Lidská společnost většinou funguje
jinak a lépe se modeluje pomocí grafů.

# Slide 13 - obr. 11

Převést šachovnici na mřížku není nijak těžké, nesmíme ale zapomenout na to, že
zrníčka potřebují mít možnost z grafu _spadnout_ - tomu se říká disipace a
zajistíme ji pomocí tzv. _sink vertex_u. Prozatím jej připojíme stejně, jako to
fungovalo na šachovnici.

# Slide 14 - obr. 12

Mřížka není ideální reprezentací mezilidských vztahů. Možných alternativních
topologií je více, soustřeďme se na tři konkrétní, které se všechny vztahují 
k lidské společnosti, zmiňované v motivačním příběhu:

# Slide 15 - obr. 13

Náhodné grafy podle [Erdose a Renýho] jsou konstruovány jednoduše: zadáme počet
vertexů a pravděpodobnost existence hrany. Algoritmus vytvoří všechny vertexy a
pak doplní každou hranu s pravděpodobností $p$.

Problém takto generovaných grafů je, že výsledný graf nemusí být souvislý, jak
vidíme i na obrázku. Pro velké grafy je ovšem pravděpodobnost velmi malá.

# Slide 16 - obr. 14

[Barabási a Albert] přišli s alternativní metodou. Všimli si, že v mnoha
systémech většina vrcholů není propojených přímo, ale skrze tzv. _hub_y, které
mají výrazně vyšší stupeň než ostatní vrcholy.

Algoritmus generování funguje tak, že se začne s malým úplným grafem a každý
další vrchol se připojí k $m$ existujícím vrcholům, ke každému 
s pravděpodobností určenou stupněm vrcholu dělenou součtem stupňů všech vrcholů.

# Slide 17 - obr. 15


# Slide 18 - obr. 16

[Watts a Štrogátz] si z lidské společnosti vzali to, že lidská společnost je
překvapivě velice blízká mřížce, existují přátelské skupiny kde se téměř všichni
znají navzájem. Navíc ale sem tam někdo patří do více skupin a _propojuje_ tedy
tím ty skupiny.

Pomocí argumentu $beta$ lze upravit vlastnosti grafu. Když je beta nula,
zůstáváme na mřížce. Když se beta zvětšuje, objevují se náhodné hrany napříč
grafem. Kdybychom došli s betou až k jedničce, dostaneme náhodný graf podobný ER
modelu.

Na obrázku je vidět dvě skupiny spojené několika málo hranami.

# Slide 19 - obr. 17

Je třeba zajistit disipaci. U mřížky je to jednoduché, protože má zřejmý okraj.
Jak toho docílit u náhodných grafů?

Přišel jsem s čtyřmi disipačními pravidly. Demonstruji je na tomto grafu, který
byl volen tak, aby ukázal slabiny všech čtyř pravidel.

# Slide 20 - obr. 18

Tak funguje mřížka, kde N je 4 - sink je připojen ke všem vrcholům právě
tolikrát, aby stupeň všech vrcholů byl 4. 

U mřížky je to dobré pravidlo, protože víme, jak přesně bude vypadat. U
náhodných grafů můžeme zvolit špatně N a některé komponenty mohou zůstat úplně
bez disipace.

# Slide 21 - obr. 19

To se snaží vyřešit pravidlo Fill Up - nejdříve najde vrchol s nejvyšším stupňem
a daný stupeň použije jako N.

Problém by mohl nastat, když je vrcholů s nejvyšším stupněm více a jsou v
samostatné komponentě, které by se tudíž nedostávalo žádné disipace.

# Slide 22 - obr. 20

Pravidlo As Many As Neighbours řeší problém úplných komponent, protože všechny 
k sinku připojuje tolikrát, kolik je stupeň vrcholu.

Vzniká ale problém s vrcholy se stupněm nula, které zůstavají nepřipojené.

# Slide 23 - obr. 21

Disipační pravidlo All Once připojí všechny vrcholy k sinku právě jednou, čímž
velice elegantně řeší problémy všech předchozích pravidel.

# Slide 24 - obr. 22

V originálním sandpile modelu se vyskytuje rozdělení power-law, mocninné
rozdělení. Počet lavin v závislost na velikosti lavin vykazují power-law.
Ten se nejlépe pozoruje na log-log plotu, kde tvoří rovnou čáru.

Teda, tvoří rovnou čáru pokud je graf nekonečný. Pro konečný graf drží power-law
jen na podintervalu, za kterým rychle klesá k nule - to je ten ocásek na konci.

Systémy v kritickém stavu vykazují power-law. Sandpile model je speciální,
protože se do kritického stavu sám zorganizuje. Kritikalita je vlastnost
systému, která se vyznačuje tím, že lokální událost, dopadnutí zrníčka, se může
projevit na globální úrovni, třeba tím, že proběhne velká lavina.

# Slide 25 - obr. 23

Power-law klesá pomaleji, než normální rozdělení. Proto jsou extrémnější
události, velké laviny, docela pravděpodobné.

Na obrázku jsou vidět čtyři lavinové klastry - kamkoliv v klastru dopadne
zrníčko, způsobí to lavinu o velikosti 1, 2 a 5. Přestože laviny menších
velikostí jsou pravděpodobnější, i velké laviny jsou možné.

Při experimentech s různými grafovými topologiemi a disipačními pravidly se
ukázalo, že power-law rozdělení velikosti lavin zůstalo. 
S jednou výjimkou, náhodným ER grafem. 
Tam se objevuje power-law jen pro velmi nízké pravděpodobnosti hran,
řádově 10^(-2) - protože pro větší pravděpodobnosti je graf blízko úplnému.

Disipační pravidla fungovala také všechna rozumná, fill up, as many as
neighbours a all once. 

Zpátky k příběhu, když připustíme, že graf vztahů lidské společnosti připomíná
jeden ze zmiňovaných náhodných grafů a že šíření negativních zkušeností se dá
modelovat pomocí sandpile modelu, pak vidíme, že negativní zážitky se mohou mezi
lidmi šířit velice daleko - protože platí power-law. Z toho vyplývá, že bychom
si měli navzájem odpouštět, protože nevíme, co může za špatnou náladu druhého.

# ??? Slide 26 - obr. 24

*konec*

*čas přednesu* ~ 9 minut
(časová dotace je 12+3 minuty)








































