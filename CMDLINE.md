# Wgrywanie programu na Arduino z wiersza poleceń

Ten dokument zawiera instrukcje, jak wgrywać program na Arduino Leonardo bez użycia Arduino IDE, bezpośrednio z wiersza poleceń.

## Instalacja Arduino CLI

1. Zainstaluj Arduino CLI:
```bash
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```

2. Dodaj do PATH (jeśli jeszcze nie jest):
```bash
echo 'export PATH=$PATH:$HOME/bin' >> ~/.bashrc && source ~/.bashrc
```

3. Zainicjalizuj i zaktualizuj indeks:
```bash
arduino-cli config init
arduino-cli core update-index
```

## Sprawdzenie płytki i portów

Sprawdź dostępne płytki i porty:
```bash
arduino-cli board list
```

## Instalacja wymaganego rdzenia

Zainstaluj rdzeń dla Arduino Leonardo:
```bash
arduino-cli core install arduino:avr
```

## Kompilacja i wgrywanie programu

1. Kompilacja programu:
```bash
arduino-cli compile --fqbn arduino:avr:leonardo big_foot_midi_controller.ino
```

2. Wgrywanie programu na płytkę:
```bash
arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:leonardo big_foot_midi_controller.ino
```

Zastąp `/dev/ttyACM0` rzeczywistym portem, do którego podłączone jest twoje Arduino. Możesz go sprawdzić używając komendy `arduino-cli board list`.

## Przydatny skrypt

Możesz utworzyć prosty skrypt `upload.sh` dla wygodniejszego wgrywania programu:

```bash
#!/bin/bash
PORT=$(arduino-cli board list | grep Leonardo | awk '{print $1}')
echo "Kompilacja programu..."
arduino-cli compile --fqbn arduino:avr:leonardo big_foot_midi_controller
echo "Wgrywanie programu na port $PORT..."
arduino-cli upload -p $PORT --fqbn arduino:avr:leonardo big_foot_midi_controller
```

Utwórz plik, nadaj mu uprawnienia do wykonania i uruchamiaj za każdym razem, gdy chcesz wgrać program:
```bash
chmod +x upload.sh
./upload.sh
```

## Monitorowanie portu szeregowego

Jeśli chcesz monitorować dane z portu szeregowego (odpowiednik Serial Monitor z Arduino IDE):
```bash
arduino-cli monitor -p /dev/ttyACM0 -c baudrate=115200
```

Pamiętaj, by dostosować prędkość transmisji (`baudrate`) do tej ustawionej w kodzie (w twoim przypadku 115200). 