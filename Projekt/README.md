# Parking senzori za automobile

## Opis sustava
Parking senzori za automobile su ugradbeni sustav razvijen na Arduino platformi, dizajniran za autonomno prepoznavanje prepreka i reagiranje putem vizualne, zvučne i mehaničke povratne informacije.  
Sustav koristi tri ultrazvučna senzora za mjerenje udaljenosti, LCD zaslon za prikaz podataka, LED diode za signalizaciju opasnosti, buzzer za zvučna upozorenja te servo motor za mehaničko upravljanje. Također omogućava prelazak u sleep mode za uštedu energije putem slidera. Slider služi za simulaciju prebacivanja ručice mjenjača u poziciju R.

## Slika spoja
*(Shema sustava iz datoteke "Shema.png" priložena uz projekt)*

## Cilj projekta
Cilj projekta je razviti pouzdan i energetski učinkovit senzorski sustav sposoban za:

- Detekciju prepreka s tri smjera (lijevo, sredina, desno)
- Vizualnu signalizaciju udaljenosti prepreka preko LED dioda
- Prikaz podataka o udaljenosti na LCD zaslonu
- Zvučna upozorenja o blizini prepreka pomoću buzzera
- Mehaničku reakciju putem servo motora
- Ulazak u sleep mode za produženje autonomije sustava

## Potencijalna korist projekta
- Sigurnosni sustavi za robote ili autonomna vozila
- Detekcija prepreka u industrijskim aplikacijama
- Edukacijski projekt za učenje rada s ultrazvučnim senzorima, LCD-ima, servo motorima i upravljanjem potrošnjom energije
- Primjena u pametnim sustavima za pomoć osobama s invaliditetom

## Postojeća slična rješenja
Na tržištu postoje komercijalni robotski sustavi i alarmni sustavi koji koriste slične metode detekcije prepreka pomoću ultrazvučnih senzora. Također, mnogi današnji automobili koriste ovaj sustav s različitim verzijama implementacije. 
Ipak, neki od njih ne nude jednostavnu mogućnost prelaska u sleep mode za štednju energije. Mnoga postojeća rješenja ne kombiniraju identičnu signalizaciju višestrukih vrsta izlaza koja je prikazana u ovom projektu (LED, buzzer, servo motor).

## Skup korisnika
- Studenti elektrotehnike, računarstva i mehatronike
- Hobi programeri i graditelji robota
- Edukacijske institucije za demonstraciju principa rada senzorskih sustava
- Inovatori u području autonomnih sustava

## Mogućnost prilagodbe rješenja
U budućnosti se sustav može nadograditi dodavanjem:

- Više ultrazvučnih senzora za potpunije pokrivanje prostora
- Modula za Bluetooth ili Wi-Fi povezivost za udaljeno očitanje podataka
- Algoritama za automatsko izbjegavanje prepreka
- Kamere za naprednu detekciju

## Opseg projektnog zadatka
Projekt uključuje:

- Razvoj softvera za Arduino
- Projektiranje i spajanje elektroničkih komponenti (senzori, buzzer, servo motor, LCD, LED diode)
- Implementaciju sleep modea za uštedu energije
- Testiranje odziva sustava u stvarnim uvjetima
- Dokumentaciju i analizu izvedbe sustava

## Moguće nadogradnje
- Napredniji algoritmi za predikciju kretanja prepreka
- Vizualizacija izmjerenih podataka u mobilnoj aplikaciji
- Integracija s GPS modulom za vanjsku navigaciju
- Inteligentni alarmi bazirani na udaljenosti i brzini približavanja preprekama

## Dijagram aktivnosti (UML)
- Sustav mjeri udaljenost pomoću tri ultrazvučna senzora
- Na temelju izmjerenih vrijednosti:
  - Pali LED diode (žuta, narančasta, crvena) prema udaljenosti
  - Aktivira buzzer ako je prepreka blizu
  - Pomiče servo motor (koji simulira kretanje automobila) ako nema prepreka
  - Prikazuje podatke na LCD zaslonu
- Provjerava stanje slidera:
  - Ako je slider u stanju HIGH → ulazak u sleep mode
  - Ako je slider u stanju LOW → nastavlja s radom

## Plan ispitivanja
- Testiranje mjerenja udaljenosti za svaki pojedinačni senzor
- Provjera ispravnog prikaza udaljenosti na LCD zaslonu
- Testiranje LED signalizacije na različitim udaljenostima
- Provjera zvučnih upozorenja putem buzzera
- Testiranje servo motora za nesmetan rad kad nema prepreka
- Simulacija ulaska i izlaska iz sleep modea

## Shema sustava
Električna shema sustava uključuje povezivanje:

- Tri ultrazvučna senzora (HC-SR04)
- LCD zaslona (20x4) s I2C komunikacijom
- LED dioda (dvije žute, dvije narančaste, dvije crvene)
- Servo motora
- Buzzera
- Slidera za aktivaciju sleep modea na Arduino mikrokontroler

## Dokumentirani kod
Sav kod je detaljno dokumentiran komentarima koji opisuju:

- Funkcije i njihovu svrhu
- Ulazne i izlazne parametre
- Logiku upravljanja komponentama
- Upravljanje potrošnjom energije  

Ova dokumentacija omogućuje laku adaptaciju i buduće nadogradnje sustava.
