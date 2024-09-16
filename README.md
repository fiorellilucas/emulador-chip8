# Emulador de Chip-8
Um simples emulador da plataforma Chip8 feito em C++, utlizando a biblioteca SFML para gráficos, áudio e entradas.

<div align="center">
  <img src="https://github.com/user-attachments/assets/9a3c2e85-c9e1-40ed-b949-d39f989ad295" alt="video-emulador-chip8" height="500">
</div>

### Menu seletor de jogos
Use **W** ou **S** para selecionar um jogo, e **ENTER** para confirmar.
<div align="center">
  <img src="https://github.com/user-attachments/assets/0fcfe73b-0f80-44ed-a421-32e49222cdc9" alt="menu emulador" height="500">
</div>

## Chip-8
O Chip-8 é uma linguagem de programação simples, criada nos anos 1970, usada principalmente para desenvolver jogos em computadores e consoles antigos. Ele funciona como uma máquina virtual, interpretando um conjunto reduzido de instruções para controlar gráficos, som e interações básicas em jogos. 

Os opcodes no Chip-8 são comandos de 2 bytes que instruem a máquina virtual sobre o que fazer. Cada opcode executa uma operação específica, como desenhar na tela, manipular registros ou realizar saltos condicionais. Existem 34 opcodes no total, organizados em categorias que lidam com gráficos, controle de fluxo, entrada de dados e matemática. Exemplos comuns de opcodes incluem:

- `00E0`: Limpar a tela.
- `1NNN`: Saltar para o endereço NNN.
- `ANNN`: Definir o registrador I para NNN.
- `DXYN`: Desenhar uma sprite de altura N na posição (X, Y).

## Para baixar e jogar
[Use o link do Google Drive](https://drive.google.com/file/d/1E5iOMh-i-HrZcDZVsBVicfzPrftRkRhx/view?usp=sharing). Cada jogo possui controles diferentes:
- *Blitz [David Winter]*: use `5` para lançar uma bomba;
- *Brick (Brix hack, 1990)*: use `4` e `6` para controlar sua barra;
- *Pong*: use `1` e `4` para controlar sua barra.

## Para compilar
1. Baixe SFML para Visual C++ 17 do [site oficial](https://www.sfml-dev.org/download/sfml/2.6.1/), extraia a pasta e coloque-a em `C:\`. O projeto espera que a pasta SFML fique na raiz do disco C, porém se você quiser colocá-la em outro lugar, altere as configurações do projeto seguindo o [tutorial no próprio site do SFML](https://www.sfml-dev.org/tutorials/2.6/start-vc.php);
2. Clone o repositório e abra a solução no Visual Studio. Se quiser compilar em modo Debug (embora não vá ter a performance necessária para jogar) você precisará colocar as DLLs do SFML na raiz do projeto também, já que o projeto é dynamically linked em modo Debug. Em modo Release, o projeto é statically linked.
3. O emulador também espera que haja uma pasta `roms` na raiz do projeto, com todos os arquivos com extensão `.ch8`, que são os jogos. O repositório inclui algumas dessas roms na pasta `test-roms`, basta passá-las para a pasta `roms`.
