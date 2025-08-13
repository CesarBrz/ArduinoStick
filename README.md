### ArduinoStick v2 — Open‑source PixelStick

> PT: Varredura de imagem com duas fitas WS2812 intercaladas, lendo BMPs do SD e exibindo via POV.
>
> EN: POV image painter using two interleaved WS2812 strips, reading 24‑bit BMPs from microSD.

## Português

### Visão geral
ArduinoStick v2 é uma implementação open‑source de um PixelStick. Ele lê imagens no formato BMP de um cartão microSD e as “pinta” no ar usando persistência de visão (POV) enquanto você desloca a haste com os LEDs. O projeto usa duas fitas WS2812 lado a lado, levemente desalinhadas na vertical, para preencher os espaços entre LEDs e aumentar a resolução efetiva.

### Como funciona
- **Leitura de BMP no SD**: o arquivo é aberto pela `SD`/`SPI`, o cabeçalho é ignorado e as cores são lidas em BGR.
- **Duas fitas intercaladas**: a imagem é organizada em colunas; para cada posição vertical, o código lê um pixel para a fita A e outro para a fita B, preenchendo os “vãos” entre LEDs.
- **Troca de imagem**: os arquivos devem ser nomeados `0.bmp`, `1.bmp`, `2.bmp`, ... A cada reset, o próximo índice é gravado na `EEPROM` e carregado no próximo boot.

Arquivo principal: `ArduinoStick_v2.ino` (usa `EEPROM.h`, `FastLED.h`, `SPI.h` e `SD.h`).

### Arranjo físico dos LEDs
Monte as duas fitas lado a lado, com uma delas levemente deslocada para que os LEDs de uma preencham os espaços da outra:

```
1    2
[L] [E]
[E] [L]
...

[L] = LED
[E] = espaço entre LEDs
```

- **Quantidade**: 176 LEDs no total (duas fitas de 88).
- **Ligação em série**: `DOUT` da fita 1 no `DIN` da fita 2. O código endereça `leds[0..87]` (fita 1) e `leds[88..175]` (fita 2).
- **Orientação**: mantenha a mesma direção física das fitas para preservar o mapeamento do código.

### Componentes (BOM)
- 1x Arduino (UNO ou Nano com ATmega328P)
- 2x fitas WS2812 (88 LEDs cada)
- 1x módulo microSD (CS 10, MOSI 11, MISO 12, SCK 13)
- 1x cartão microSD (FAT32)
- 1x pack de bateria 5 V capaz de fornecer corrente suficiente
- Fios, resistor de 330 Ω no sinal de dados, capacitor eletrolítico (≥1000 µF/6,3 V) entre 5 V e GND

### Ligações elétricas
- **LEDs**:
  - `DATA_PIN` do Arduino: pino 7 → resistor 330 Ω → `DIN` da fita 1
  - `DOUT` da fita 1 → `DIN` da fita 2
  - 5 V e GND do pack → 5 V e GND das fitas (injete em ambas as pontas se possível)
  - GND comum entre Arduino, módulo SD e fitas
- **microSD (SPI)**:
  - `CS` → pino 10
  - `MOSI` → pino 11
  - `MISO` → pino 12
  - `SCK` → pino 13
  - 5 V e GND conforme o módulo

### Preparando as imagens (BMP)
- Coloque os arquivos no diretório raiz do cartão com nomes sequenciais: `0.bmp`, `1.bmp`, `2.bmp`, ...
- **Formato**: BMP 24‑bit, sem compressão.
- **Altura**: 176 pixels (um por LED efetivo).
- **Largura**: livre (cada coluna é um “passo” de exibição durante o movimento).
- **Ordem de pixels por coluna**: BGR, intercalando fita A e fita B a cada posição vertical (como o código lê: primeiro pixel para `leds[x]`, depois para `leds[x+88]`).

Observação: ferramentas de conversão para PixelStick facilitam gerar BMPs compatíveis. Mantenha brilho moderado para reduzir consumo.

### Compilação e upload
1. Instale as bibliotecas na IDE do Arduino: `FastLED` e `SD` (a `SPI`/`EEPROM` já vêm com a IDE).
2. Selecione a placa (UNO/Nano) e a porta correta.
3. Faça o upload de `ArduinoStick_v2.ino`.

Parâmetros úteis no código:
- `#define NUM_LEDS 176`
- `#define DATA_PIN 7`
- `int TempoDelay = 0` (caso queira ajustar o ritmo entre colunas)

### Uso
1. Formate o microSD em FAT32 e copie `0.bmp`, `1.bmp`, ...
2. Insira o cartão no módulo SD.
3. Alimente o sistema com o pack de 5 V (corrente adequada).
4. Mova a haste a velocidade constante para “pintar” a imagem no ar.
5. Pressione reset no Arduino para trocar para a próxima imagem.

Na inicialização, alguns LEDs indicam o índice atual e um pequeno “contador” azul confirma o início.

### Alimentação e segurança
- Corrente máxima pode ser alta em branco total; use fonte/bateria 5 V com folga.
- Adicione capacitor entre 5 V e GND próximo às fitas e resistor de 330 Ω em série no sinal de dados.
- Mantenha fios curtos e garanta GND comum estável.

---

## English

### Overview
ArduinoStick v2 is an open‑source PixelStick. It reads BMP images from a microSD card and “paints” them in mid‑air using persistence of vision (POV) while you move the LED bar. Two WS2812 strips are mounted side by side with a slight vertical offset so one strip fills the gaps of the other, increasing effective vertical resolution.

### How it works
- **BMP from SD**: file is opened via `SD`/`SPI`, header skipped, colors read in BGR.
- **Two interleaved strips**: the image is column‑oriented; for each vertical position, the code reads one pixel for strip A and one for strip B, filling the gaps.
- **Image cycling**: files must be named `0.bmp`, `1.bmp`, `2.bmp`, ... Each reset advances the index stored in `EEPROM` and loads the next image.

Main file: `ArduinoStick_v2.ino` (uses `EEPROM.h`, `FastLED.h`, `SPI.h`, `SD.h`).

### Physical LED arrangement
Mount the two strips side by side with a slight vertical offset so LEDs from one fill the gaps of the other:

```
1    2
[L] [E]
[E] [L]
...

[L] = LED
[E] = gap between LEDs
```

- **Count**: 176 LEDs total (two strips of 88).
- **Daisy‑chain**: strip 1 `DOUT` → strip 2 `DIN`. The code addresses `leds[0..87]` (strip 1) and `leds[88..175]` (strip 2).
- **Orientation**: keep both strips in the same physical direction so mapping matches the code.

### Bill of materials (BOM)
- 1x Arduino (UNO or Nano, ATmega328P)
- 2x WS2812 LED strips (88 LEDs each)
- 1x microSD module (CS 10, MOSI 11, MISO 12, SCK 13)
- 1x microSD card (FAT32)
- 1x 5 V battery pack with sufficient current capability
- Wires, 330 Ω data resistor, electrolytic capacitor (≥1000 µF/6.3 V) across 5 V and GND

### Wiring
- **LEDs**:
  - Arduino `DATA_PIN` 7 → 330 Ω → strip 1 `DIN`
  - strip 1 `DOUT` → strip 2 `DIN`
  - 5 V and GND from the battery → both strips (inject at both ends if possible)
  - Common GND between Arduino, SD module, and strips
- **microSD (SPI)**:
  - `CS` → pin 10
  - `MOSI` → pin 11
  - `MISO` → pin 12
  - `SCK` → pin 13
  - 5 V and GND per module specs

### Preparing images (BMP)
- Place files in the SD root named sequentially: `0.bmp`, `1.bmp`, `2.bmp`, ...
- **Format**: 24‑bit BMP, uncompressed.
- **Height**: 176 pixels (one per effective LED).
- **Width**: arbitrary (each column is one display step as you move).
- **Per‑column pixel order**: BGR, interleaving strip A then strip B at each vertical position (as read by the code: first `leds[x]`, then `leds[x+88]`).

Tip: PixelStick‑style image converters can generate compatible BMPs. Keep brightness moderate to reduce power draw.

### Build and upload
1. Install Arduino libraries: `FastLED` and `SD` (IDE already provides `SPI`/`EEPROM`).
2. Select your board (UNO/Nano) and correct port.
3. Upload `ArduinoStick_v2.ino`.

Useful parameters in code:
- `#define NUM_LEDS 176`
- `#define DATA_PIN 7`
- `int TempoDelay = 0` (can be used to adjust inter‑column timing)

### Usage
1. Format microSD as FAT32 and copy `0.bmp`, `1.bmp`, ...
2. Insert the card into the SD module.
3. Power the system with a 5 V pack (adequate current).
4. Sweep the bar at a steady speed to “paint” the image in the air.
5. Press the Arduino reset button to advance to the next image.

On startup, LEDs briefly indicate the current index and a short blue “counter” confirms the image will start.

### Power and safety
- Peak current can be high at full white; use a robust 5 V source.
- Add a bulk capacitor near the strips and a 330 Ω series resistor on the data line.
- Keep wires short and ensure a solid common ground.

---

Made with `FastLED`, `SD`, `SPI`, and `EEPROM`. Hardware pinout assumes Arduino UNO/Nano SPI: MOSI 11, MISO 12, SCK 13, SD CS 10, LED data 7.


