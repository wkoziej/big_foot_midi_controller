# Aby big footem zmieniać instrumenty:
1. RC-600 nie wysyła do PC (PC OUT = NO) (aby nie zmieniał presetów w arturia)
2. Arturia wszystkie kanały midi ma włączone (aby leciały z big foota i Sonuus'a)
3. Preset 127 w Arturia jest "cichy" - można wyłączyć w ten sposób midi z instumentów 

## Pamiętaj o  
= Numery programów na Arturia Microfreak o jednen niżej (PC76 -> to instrument 77)
= Kanały Midi przesunięte o jeden niżej (w ustawieniach filtrujemy na kanale 15) a wysyłany 14 i jest OK:
```
    aseqdump -p 32:0 | grep -v Clock
 32:0   Program change         14, program 77
 32:0   Note on                14, note 64, velocity 100
 32:0   Note off               14, note 64
 ```

 