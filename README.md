# 🎮 BitColoursLab - Jogo de Memória 

## 📝 Apresentação do Projeto
O *BitColoursLab* é um jogo de memória desenvolvido como projeto final do curso *EmbarcaTech, promovido pelo **CEPEDI, **SOFTEX* e *HwIT - Hardware Innovation Technologies. O jogo foi projetado para ser executado em um sistema embarcado baseado no microcontrolador **Raspberry Pi Pico, integrado à placa de desenvolvimento **BitDogLab. O objetivo do jogo é desafiar o jogador a memorizar e reproduzir sequências de cores geradas aleatoriamente, com níveis de dificuldade progressivos. O sistema utiliza **💡 LEDs, **🎮 botões, um **🖥 display OLED* e um *🔊 buzzer* para criar uma experiência interativa e imersiva.

---

## 🎯 Título do Projeto
*BitColoursLab: Jogo de Memória com LEDs e Display OLED*

---

## 🎯 Objetivos do Projeto
1. 🕹 Desenvolver um jogo de memória interativo utilizando hardware embarcado.
2. 🎨 Integrar LEDs, botões, display OLED e buzzer para criar uma experiência de usuário envolvente.
3. 🔀 Implementar funcionalidades como geração de sequências aleatórias, verificação de jogadas e feedback visual/sonoro.
4. 🛠 Demonstrar a aplicação de conceitos de programação embarcada, como *GPIO, **PWM, **I2C* e manipulação de periféricos.

---

## ⚙ Descrição do Funcionamento
O jogo inicia com uma tela de boas-vindas e instruções no display OLED. Em seguida, o sistema gera uma sequência aleatória de cores, que é exibida através dos LEDs. O jogador deve reproduzir a sequência pressionando os botões correspondentes. A cada acerto, a dificuldade aumenta, com sequências mais complexas. Existem *729 sequências únicas de 6 cores* que podem ser geradas utilizando as 3 cores, onde a ordem importa e a repetição é permitida. Em caso de erro, o jogo reinicia. O buzzer emite sons para indicar erros e vitórias.

---

## 📌 Justificativa
O projeto justifica-se pela aplicação prática de conceitos de sistemas embarcados, como controle de *GPIO, **PWM* para ajuste de brilho de LEDs, comunicação *I2C* com display OLED e geração de sons com buzzer. Além disso, o jogo proporciona uma experiência lúdica e educativa, demonstrando a versatilidade do *Raspberry Pi Pico* e da placa *BitDogLab*.

---

## 🚀 Originalidade
Embora existam projetos semelhantes de jogos de memória com LEDs, a integração de um *display OLED* para exibição de informações e a utilização de um *buzzer* para feedback sonoro diferenciam este projeto. A implementação de uma interface gráfica simples e a geração de sequências aleatórias com níveis progressivos também são aspectos inovadores.

---

## 🛠 Componentes Utilizados
- *💡 LEDs RGB*: Controlados via PWM, conectados nas GPIOs 13 (Vermelho), 11 (Verde) e 12 (Azul).
- *🎮 Joystick*: Conectado aos pinos GPIO 26 (Eixo X) e GPIO 27 (Eixo Y).
- *🔘 Botão Joystick*: Conectado à GPIO 22.
- *🔘 Botão A*: Conectado à GPIO 5.
- *🖥 Display OLED SSD1306*: Conectado via I2C (GPIO 14 - SDA, GPIO 15 - SCL).
- *🔊 Buzzer*: Conectado às GPIOs 21 e 10.
- *📟 Placa de Desenvolvimento BitDogLab com RP2040*.

---

## 🔧 Funcionalidades do Projeto
1. *💡 Controle de LEDs RGB via PWM*:
   - LEDs RGB são controlados com base nas sequências geradas, com brilho ajustado via PWM.
2. *🖥 Exibição no Display OLED*:
   - O display OLED exibe mensagens, instruções e o status do jogo.
3. *🔀 Geração de Sequências Aleatórias*:
   - O sistema gera sequências únicas de cores, com níveis de dificuldade progressivos.
4. *🔊 Feedback Sonoro*:
   - O buzzer emite sons para indicar acertos, erros e vitórias.
5. *🛠 Interrupções e Debouncing*:
   - Botões de controle são configurados com interrupções para evitar múltiplos acionamentos.
6. *⚙ Função de Calibração*:
   - O sistema é calibrado para garantir precisão na geração e exibição das sequências.

---

## 📂 Organização do Código
### 📁 Arquivos Incluídos
- *main.c*: Código principal com a implementação de todas as funcionalidades.
- *lib/ssd1306.h*: Biblioteca para controle do display OLED SSD1306.

### 📌 Estrutura do Código
1. *🛠 Inicialização dos Componentes*:
   - Configuração dos pinos GPIO, PWM, I2C e ADC para LEDs, botões, buzzer e display OLED.
2. *🔀 Geração de Sequência*:
   - Gera uma sequência aleatória de cores.
3. *💡 Exibição de Sequência*:
   - Mostra a sequência nos LEDs.
4. *🎮 Verificação de Jogada*:
   - Compara a entrada do jogador com a sequência gerada.
5. *🔊 Feedback*:
   - Emite sons e atualiza o display conforme o progresso do jogo.
6. *⏳ Rotinas de Interrupção*:
   - Monitora os botões com interrupções e implementa debouncing.

---

## 🚀 Como Executar o Projeto
### ✅ Pré-requisitos
- Placa BitDogLab configurada e conectada.
- Ambiente de desenvolvimento configurado com SDK do RP2040.
- VS Code com a extensão Pico SDK instalada.

### 🔧 Passos
1. Clone este repositório:
   bash
   git clone <https://github.com/queirozPatrick/projeto_genius>
   
2. Abra o projeto no VS Code.
3. Compile e carregue o código na Placa BitDogLab.
4. Conecte-se ao Serial Monitor para monitorar as saídas, se necessário.

---

## 🛠 Testes de Validação
1. ✅ Verificação do funcionamento dos LEDs e ajuste de brilho via PWM.
2. ✅ Validação da geração e exibição de sequências de cores.
3. ✅ Teste da detecção de botões e feedback sonoro.
4. ✅ Validação das telas de inicialização, instruções, vitória e derrota.
5. ✅ Teste da lógica do jogo e progressão de níveis.

---

## 📊 Discussão dos Resultados
O projeto foi implementado com sucesso, atendendo a todos os requisitos. O jogo é funcional e proporciona uma experiência interativa e divertida. A integração de hardware e software demonstrou a viabilidade do uso do *Raspberry Pi Pico* e da placa *BitDogLab* para projetos embarcados.

---

## 🚀 Documentação do projeto concluido (PDF)
- 📖 [BitColoursLab](à ser incluido)

## 🎥 Demonstração do Projeto
- Vídeo do projeto: [à ser incluido](à ser incluido)

---
## 📚 Referências
- 📖 [Documentação do RP2040](https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html)
- 📖 [Datasheet do Display OLED SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
- 📖 [Projetos Anteriores](https://github.com/queirozPatrick)

---

## 👤 Autor
*Patrick Queiroz*  
- 🏆 GitHub: [queirozPatrick](https://github.com/queirozPatrick)  
- 💼 LinkedIn: [Patrick Queiroz](https://www.linkedin.com/in/patrick-queiroz/)