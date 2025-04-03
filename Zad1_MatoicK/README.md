# <a name="naslov"></a>Upravljanje višestrukim prekidima i njihovim prioritetima

## Sadržaj

1. [Opis sustava](#opis-sustava)
2. [Hardverske komponente](#hardverske-komponente)
3. [Rukovanje prekidima](#rukovanje-prekidima)
4. [Funkcionalni zahtjevi](#funkcionalni-zahtjevi)
5. [Testiranje](#testiranje)

## <a name="opis-sustava"></a>1. Opis sustava

Sustav implementira obradu višestrukih prekida s različitim prioritetima koristeći:

- 2 tipkala za generiranje prekida
- 6 LED dioda za vizualizaciju različitih prekida
- Fotootpornik kao analogni ulaz koji generira prekid nakon prelaska određene granice
- Potenciometar kao analogni ulaz koji generira prekid nakon prelaska određene granice
- Timer koji generira periodične prekide
- Arduino Uno mikrokontroler

Svaki prekid se obrađuje zasebno. Kada dođe prekid višeg prioriteta, on se obrađuje prvi. U tom trenutku ostali prekidi čekaju obradu i obrađuju se prema njihovom redoslijedu (prioritetu). U određenom trenutku može biti uključena samo jedna LED dioda koja definira prekid koji se obrađuje. Sustav koristi millis() umjesto delay() gdje je to moguće kako bi osigurao nesmetano upravljanje prekidima.

## <a name="hardverske-komponente"></a>2. Hardverske komponente

| Komponenta           | Količina | Pin                 |
| -------------------- | -------- | ------------------- |
| Arduino Uno          | 1        | -                   |
| Pločica za spajanje  | 1        | -                   |
| Tipkalo              | 2        | 2, 3                |
| LED (razne boje)     | 6        | 4, 5, 6, 7, 8, 9    |
| Otpornik 1kΩ         | 6        | -                   |
| Fotootpornik         | 1        | A0                  |
| Potenciometar        | 1        | A1                  |

## <a name="rukovanje-prekidima"></a>3. Rukovanje prekidima

Sustav obrađuje prekide uzrokovane različitim senzorima i korisničkim ulazima. Prioriteti prekida su sljedeći:
 
1. **Tipkala (INT0 > INT1)** - Ako je tipkalo pritisnuto pali se LED dioda u njegovoj boji i u Serial monitor se ispisuje poruka "Prekid - tipkalo 2 pritisnuto!" ako je pritisnuto zeleno tipkalo ili "Prekid - tipkalo pritisnuto (najviši prioritet)!" ako je pritisnuto plavo tipkalo.
2. **Potenciometar** - Pali žutu LED diodu kada je pročitana vrijednost veća od 512. U Serial monitor se ispisuje poruka "Prekid - potenciometar ima veću vrijednost od 512!"
3. **Fotootpornik** - Pali ljubičastu LED diodu kada je pročitana vrijednost razine osvjetljenja manja od 300 luksa. U Serial monitor se ispisuje poruka "Prekid - osvjetljenje je manje od 300 luksa!"
4. **Serijski unos** - Ako se detektira unos podataka putem serijskog monitora pali se narančasta LED dioda i u Serial monitor se ispisuje poruka "Prekid - serijski ulaz!".
5. **Timer prekid** - Pali crvenu LED diodu svakih 1 sekundu i u Serial monitor ispisuje poruku "Prekid - timer!".

## <a name="funkcionalni-zahtjevi"></a>4. Funkcionalni zahtjevi

| ID       | Opis |
|--------|------|
| FR-1   | Sustav mora detektirati pritiske tipkala i aktivirati odgovarajuće LED diode. |
| FR-2   | Prekid tipkala mora imati definirane prioritete: INT0 > INT1. |
| FR-3   | Sustav mora očitavati vrijednost potenciometra. |
| FR-4   | Ako je očitana vrijednost veća od 512 žuta LED dioda mora svijetliti i mora se ispisati poruka "Prekid - potenciometar ima veću vrijednost od 512!" na Serial monitoru. |
| FR-5   | Sustav mora mjeriti razinu osvjetljenja s pomoću fotootpornika. |
| FR-6   | Ako je izmjerena razina osvjetljenja manja od 300 luksa, ljubičasta LED dioda mora svijetliti i mora se ispisati poruka "Prekid - osvjetljenje je manje od 300 luksa!" na Serial monitoru. |
| FR-7   | Sustav mora detektirati unos na Serial monitoru. |
| FR-8   | Ako postoji unos na Serial monitoru pali se narančasta LED dioda i ispisuje se poruka "Prekid - potenciometar ima veću vrijednost od 512!" na Serial monitoru. |
| FR-9   | Timer mora generirati prekid svake 1 sekunde i uključiti crvenu LED diodu na kratko. |
| FR-10  | Ako se istovremeno dogodi više prekida, mora se poštivati sljedeći prioritet: INT0 > INT1 > potenciometar > fotootpornik > serijski unos > Timer. |
| FR-11  | U određenom trenutku može biti upaljena samo jedna LED dioda koja označava prekid koji se obrađuje. |
| FR-12  | Implementacija mora koristiti millis() umjesto delay() gdje je moguće. |

## <a name="testiranje"></a>5. Testiranje

Kada se kod pokrene timer generira prekid svake sekunde što je detektirano paljenjem crvene LED diode, moguće je unijeti neki test u Serial monitor i taj serijski unos će imati prioritet te će se on obraditi (to je simulirano i paljenjem narančaste LED diode). Također, kada se fotootpornik namjesti na vrijednost manju od 300 luksa ljubičasta LED dioda će se upaliti i izvršavat će se taj prekid. Ako se u istom trenutku vrijednost na potenciometru postavi na veću od 512, pali se žuta LED dioda i izvršava se taj prekid. U slučaju da se pritisne neko od tipkala izvodi se njihov prekid i pali odgovarajuća LED dioda budući da tipkala imaju najviši prioritet. Tipkalo na pinu 2 (INT0) ima veći prioritet od tipkala na pinu 3 (INT1) te se njegov prekid izvršava prvi u slučaju da su tipkala pritisnuta istovremeno.
