# Emulador de Chip-8
Um simples emulador da plataforma Chip8 feito em C++, utlizando a biblioteca SFML para gráficos, áudio e entradas.

<div align="center">
  <img src="https://github.com/user-attachments/assets/9a3c2e85-c9e1-40ed-b949-d39f989ad295" alt="video-emulador-chip8" height="450">
</div>

## Chip-8
O Chip-8 é uma linguagem de programação simples, criada nos anos 1970, usada principalmente para desenvolver jogos em computadores e consoles antigos. Ele funciona como uma máquina virtual, interpretando um conjunto reduzido de instruções para controlar gráficos, som e interações básicas em jogos. 

Os opcodes no Chip-8 são comandos de 2 bytes que instruem a máquina virtual sobre o que fazer. Cada opcode executa uma operação específica, como desenhar na tela, manipular registros ou realizar saltos condicionais. Existem 34 opcodes no total, organizados em categorias que lidam com gráficos, controle de fluxo, entrada de dados e matemática. Exemplos comuns de opcodes incluem:

- `00E0`: Limpar a tela.
- `1NNN`: Saltar para o endereço NNN.
- `ANNN`: Definir o registrador I para NNN.
- `DXYN`: Desenhar uma sprite de altura N na posição (X, Y).
