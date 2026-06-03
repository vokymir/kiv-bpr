# SVK

Dobré dopoledne,

jmenuju se Jakub Vokoun a rád bych Vám předal část toho,
co jsem se dozvěděl v rámci předmětu KIV/BPR (bakalářský projekt),
což je v našem oboru náhrada za opravdickou bakalářku.

Vybral jsem si téma, které propojuje dohromady mnoho oblastí,
hlavně ale matematiku, dynamiku a informatiku.

Co jsou to systémy se samoorganizovanou kritikalitou?
Důležité je vědět, co jsou to kritické systémy, nebo taky systémy v kritickém 
stavu.

Jednoduše řečeno, jde o systémy kde malá náhodná změna může (ale nemusí)
způsobit velkou odezvu. 

Jeden příklad: lesní požáry. 
Když vznikne požár, může se rozšířit na obrovské území, nebo ale taky shoří
jenom jeden, dva stromy. Záleží hlavně na hustotě stromů.

To, že malý impuls může způsobit velkou změnu se dá vystihnout v distribuční
funkci. My, jako lidé, jsme zvyklí na normální rozdělení - hodně pravděpodobná
průměrná hodnota a její okolí, velice nepravděpodobné extrémní případy. Systémy
v kritickém stavu ale vykazují power-law, mocninné rozdělení. To klesá pomaleji,
než normální rozdělení - tudíž jsou extrémní události možné a stále relativně
pravděpodobné.

Některé systémy mají kritický bod, do kterého když se dostanou, začnou vykazovat
kritikalitu. To znamená, že se volbou správných parametrů systému dá do
kritikality dostat, ale je to křehká rovnováha.

Naproti tomu, mě zajímaly systémy, které se do kritického stavu dostanou samy,
takzvaná samoorganizovaná kritikalita, anglicky self-organized criticality.
Vybral jsem si k tomu originální model na kterém bylo toto ukázáno -
model hromady písku, anglicky sandpile model, s kterým přišli BTW v roce 1987,
a který vzápětí vysvětlím.

---

Máme šachovnici na kterou náhodně dopadají zrníčka písku.

---

Zrníčka na každém políčku padají přímo na sebe a stavějí věže. Pokud je výška
věže 4 nebo víc, musí se rozpadnout v lavině. Na obrázku právě dopadá čtvrté
zrníčko na pole D3.

---

Věž se rozpadne na 4 sousední políčka.

---

Tím lavina končí, měla velikost jedna, protože spadla právě jedna věž.

---

Když dopadne zrníčko na pole E4, to se opět rozpadne v lavině.

---

Nastává nová situace: pole E4 je na hraně šachovnice, proto jedno zrníčko padá
ze stolu na zem.

---

Zároveň, tím lavina nekončí. Na poli E3 je teď dost zrníček a proto lavina
pokračuje.

---

Jedno zrníčko opět padá ven, lavina končí. Způsobila zhroucení dvou věží, proto
její velikost byla 2.

...

Abych nezkoumal abstraktní chování, vymyslel jsem si motivační příběh.
Představil jsem si lidskou společnost jako graf, kde vrcholy jsou lidé a hrany
jsou vztahy typu "znají se" nebo "jsou přátelé". 

Ve světě se náhodně dějí špatné věci, ať už třeba jenom to, že se špatně vyspím.
To reprezentují zrníčka.
Zároveň, každý člověk snese jenom nějaký počet negativních zkušeností, než
vybouchne nebo nějak jinak negativně ovlivní své okolí. To reprezentují laviny.

To ale znamená, že jsem musel přenést sandpile model z šachovnice na nějaký
graf, který lépe připomíná lidskou společnost.

Postupně, nejprve je třeba převést šachovnici na mřížku - to není tak složité.
Místo políček jsou vrcholy, které je třeba správně propojit.

--- 

Tady nastává zajímavý problém: jak zajistit, aby zrníčka mohla spadnout z grafu?
Chytrým slovem disipace je zajištěná pomocí odtokového vrcholu, anglicky sink
vertexu, který pojme libovolný počet zrníček a nezhroutí se.

Vtip je ještě v tom, jakým způsobem připojit sink k vrcholům? Tady je to
jednoduché, prostě okopírujeme šachovnici - ke krajním vrcholům připojíme sink
jednou a k rohovým dvakrát.

Jak ale napojit sink na obecném grafu? 
... (dramatická pauza) ...
Až sem jsem nedělal nic nového, přechod z mřížky na graf už přede mnou vymysleli
jiní. Ale v téhle fázi jsem nenašel žádnou pomoc. 
Přestože sandpile model se už nasadil na jiné grafové topologie než mřížku, byly
to vždy nějaké speciální topologie, které měly napojení na sink podobně zřejmé.
.. Teda, ledaže jsem nenašel správný paper, to je možné.

---

To mě zaujalo, tak jsem vymyslel několik jednoduchých pravidel. Říkám jim
dissipation rules, protože by měly zajistit disipaci grafu, tedy měly by
zajistit odtok zrníček z grafu, abychom se nezastavili v jedné nekončící lavině.

Všechna pravidla mají nějakou slabinu - vyrobil jsem tenhle graf, abych je mohl
jednoduše demonstrovat. Všechna pravidla fungují poměrně dobře na velkých
grafech, kde s velkou pravděpodobností nenastanou tyto hraniční případy.

Je to nespojitý graf se třemi komponentami - jeden úplný graf, jedna čára a
jeden samostatný vrchol. Modré uprostřed je opět sink.

---

První pravidlo je Fill to N. Inspiruje se na mřížce, která tak taky funguje.

Chceme, aby stupně všech vrcholů byly alespoň N. Kde je méně, tam doplníme hranu
do sinku tolikrát, aby měl vrchol stupeň alespoň N.

Toto pravidlo má zjevnou slabinu - dopředu nevíme, jak graf vypadá - takže
můžeme zvolit moc malé N a některé komponenty grafu nemusí být vůbec připojené k
sinku. Disipace nezajištěna.

---

Tohle se snaží řešit pravidlo Fill Up. To určuje N dynamicky tak, že si najde
nejvyšší stupeň vrcholu na grafu a to je jeho N.

To je také nedokonalé, ale už náchylné pouze na úplné grafy. Takže, pokud si
dáme podmínku, že v grafu může být pouze jeden vrchol s ostře nejvyšším N, bude
pravidlo fungovat.

V motivačním příběhu dává pravidlo také smysl: všichni lidé mají stejnou míru
trpělivosti - jsme lidi.

---

Další pravidlo: As Many As Neighbours. Připojí sink tolikrát, kolik už má vrchol
stupeň - tím se zvětší význam stupně, protože je teď pro všechny dvakrát větší,
větší rozdíl.

Problém je jen u vrcholů, které nemají žádné sousedy.

A i do motivace to funguje - čím víc známých člověk má, tím je odolnější vůči
negativním zkušenostem.

---

Poslední pravidlo: All Once. Připojí sink ke každému vrcholu jednou.

Disipace je zajištěna, jen je to trochu *nudné*.

Co se motivace týče, funguje stejně jako u as many as neighbours, akorát v
systému zůstává větší poměr zrníček.

---

Pravidla jsou za námi, na jaký graf je ale chceme použít?
Hledal jsem takové modely grafů, které by mohly připomínat lidskou společnost.

Našel jsem tři zajímavé kandidáty, všechny náhodné grafy:

První je ER model. To je čistě náhodný graf. ER model vygeneruje n vrcholů a
každé dva propojí se stejnou pravděpodobností p. Takže se může stát, že některý
bude sám.

Pro větší grafy je ale pravděpodobnost hodně nízká.

---

BA model je zajímavější pro motivaci, protože autoři si všimli, že v mnoha
systémech, i mezi lidmi, je stupeň většiny vrcholů o dost menší, než u několika
málo tzv. hubů.

Generují graf preferenčně. Začnou s grafem 0, většinou úplný graf. V každém
kroku přidají dalši vrchol a ten propojí s m existujícími vrcholy.

Vypadá to asi takhle:

---

Nový vrchol se chce připojit k jednomu existujícímu, který si má vybrat? Vybírá
si s preferencí, takže největší šanci má vrchol s velkým stupněm, s hodně
hranami.

Pravděpodobnost, že se připojí k tomuhle je dána tím, že má X sousedů a
součet stupňů všech vrcholů je 32.

Pak se všechny pravděpodobnosti seřadí za sebe, součet dá dohromady jedničku,
počítač vygeneruje náhodný číslo a kam se trefí, tam se vrchol připojí.

---

Poslední model, WS. Ti si všimli, že lidé jsou většinou součástí více skupin.
každá skupina je celkem uzavřená, všichni znají všechny (téměř). Jednotlivé
skupiny jsou propojené konkrétními lidmi, kteří jsou členy více skupin.

Na obrázku to je celkem vidět, spodní skupina má tři členy, kteří se znají i s
tou horní. Horní má taky tři.

Důležitý parametr generování je beta, která říká jaká je šance, že je jeden
člověk ve více skupinách (víceméně).

---

Celou dobu jsem si nebyl jistý, jestli to bude fungovat.
Jestli budou modely pořád samy mířit ke kritikalitě, nebo jestli se to rozbije.

Jako míru jsem si vzal power-law. O něm jsem mluvil na začátku, kde je
kritikalita, tam je power-law. Zpátky to neplatí nutně.

Power-law se v klasickém sandpile modelu najde v rozdělení lavin.
Na ose x je velikost laviny, na ose y její četnost, kolikrát se stala.

Protože power-law je mocninné rozdělení, je výhodnější ho ukázat na log-log
plotu - obě osy jsou logaritmické. Tady vypadá jako lineární funkce.

Teda, vypadal by jako lineární funkce pokud by byl systém nekonečně velký. Pro
konečné systémy je na konci tenhle ocásek.

...

Sledoval jsem, jestli se power-law objeví i při použití grafových topologií,
o kterých jsem mluvil. A taky mě zajímalo, jaký vliv budou mít disipační
pravidla.

Dobrá zpráva - možná - na všech topologiích kromě ER grafu power-law distribuce
je pro libovolnou volbu parametrů tvorby grafu. I na ER můžeme dosáhnout
power-law, ale pouze při speciální volbě parametrů.

...

Na závěr - k motivaci. Pokud graf lidské společnosti připomíná jeden z
diskutovaných grafů tak, stejně jako na mřížce, je vzhledem k sandpile modelu
kritický systém.

Takže, jestli se dají negativní zkušenosti a jejich předávání mezi lidmi
modelovat pomocí sandpile modelu, máme sociologický model který říká, že jedna
malá negativní zkušenost se může dostat hodně daleko a ovlivnit mnoho lidí.

---

Z toho vyplývá, že bychom si měli navzájem odpouštět, protože nevíme, co může za
špatnou náladu druhého.

