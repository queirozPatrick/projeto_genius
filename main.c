#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include <stdlib.h>
#include <time.h>

// Definições de pinos
#define PINO_LED_VERMELHO 13
#define PINO_LED_VERDE 11
#define PINO_LED_AZUL 12

#define PINO_BOTAO_B 6 // Botão B na GPIO 6
#define PINO_BOTAO_A 5
#define PINO_BOTAO_JOYSTICK 22 // Botão do joystick na GPIO 22

#define PINO_I2C_SDA 14
#define PINO_I2C_SCL 15
#define ENDERECO_I2C 0x3C

#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

#define ATRASO_DEBOUNCE_MS 200

// Variáveis globais
volatile bool led_verde_ligado = false;
volatile absolute_time_t ultimo_tempo_botao_b;
volatile absolute_time_t ultimo_tempo_botao_a;
volatile absolute_time_t ultimo_tempo_botao_joystick;

ssd1306_t display;

// Variáveis do jogo Genius
#define MAX_SEQUENCIA 6 // Sequência máxima de 6 cores
uint8_t sequencia[MAX_SEQUENCIA];
uint8_t nivel = 1;
uint8_t indice_jogador = 0;

// Protótipos de funções
void configurar_gpio();
void configurar_i2c();
void inicializar_display();
void callback_botao(uint gpio, uint32_t eventos);
void desenhar_borda();
void gerar_sequencia();
void mostrar_sequencia();
bool verificar_jogada(uint8_t cor);
void atualizar_display(uint8_t nivel_atual, uint8_t indice_atual, bool game_over);
void exibir_tela_inicial();
void exibir_tela_instrucoes(); // Nova função para exibir as instruções
void exibir_mensagem_centralizada(char *mensagem); // Nova função para mensagens centralizadas

// Função para exibir mensagens centralizadas com borda
void exibir_mensagem_centralizada(char *mensagem) {
    // Limpa o display
    ssd1306_fill(&display, false);

    // Desenha a borda
    desenhar_borda();

    // Desenha a mensagem no centro da tela
    ssd1306_draw_string(&display, mensagem, 25, ALTURA_DISPLAY / 2 - 8);

    // Envia os dados para o display
    ssd1306_send_data(&display);
}


// Função para exibir a tela inicial
void exibir_tela_inicial() {
    // Limpa o display
    ssd1306_fill(&display, false);

    // Desenha a borda
    desenhar_borda();

    // Desenha o nome do jogo no centro da tela
    ssd1306_draw_string(&display, "Genius Game", 25, ALTURA_DISPLAY / 2 - 8);

    // Envia os dados para o display
    ssd1306_send_data(&display);

    // Aguarda 3 segundos
    sleep_ms(3000);
}

// Função para exibir a tela de instruções
void exibir_tela_instrucoes() {
    // Limpa o display
    ssd1306_fill(&display, false);

    // Desenha o título
    ssd1306_draw_string(&display, "Como Jogar:", 25, 0);

    // Desenha as instruções
    ssd1306_draw_string(&display, "A Verde", 10, 16);
    ssd1306_draw_string(&display, "B Azul", 10, 32);
    ssd1306_draw_string(&display, "JoyPress Red", 10, 48);

    // Envia os dados para o display
    ssd1306_send_data(&display);

    // Aguarda 5 segundos
    sleep_ms(8000);
}

// Função para atualizar o display
void atualizar_display(uint8_t nivel_atual, uint8_t indice_atual, bool game_over) {
    char buf[32];

    // Limpa o display
    ssd1306_fill(&display, false);

    // Desenha o título
    ssd1306_draw_string(&display, "Genius Game", 20, 0);

    // Mostra o nível atual
    sprintf(buf, "Nivel: %d", nivel_atual);
    ssd1306_draw_string(&display, buf, 0, 16);

    // Mostra o progresso atual
    sprintf(buf, "Progresso: %d/%d", indice_atual, nivel_atual);
    ssd1306_draw_string(&display, buf, 0, 32);

    // Se game over, mostra mensagem
    if (game_over) {
        // Usando a nova função para exibir a mensagem centralizada
        exibir_mensagem_centralizada("GAME OVER!");
    }

    // Envia os dados para o display
    ssd1306_send_data(&display);
}

// Função principal
int main() {
    stdio_init_all();

    configurar_gpio();
    configurar_i2c();
    inicializar_display();

    // Exibe a tela inicial
    exibir_tela_inicial();

    // Exibe a tela de instruções
    exibir_tela_instrucoes();

    srand(time(NULL));
    gerar_sequencia();

    bool game_over = false;

    while (true) {
        // Atualiza o display com o estado inicial
        atualizar_display(nivel, indice_jogador, false);

        mostrar_sequencia();

        // Aguarda a jogada do jogador
        indice_jogador = 0;
        while (indice_jogador < nivel) {
            // Atualiza o display com o progresso
            atualizar_display(nivel, indice_jogador, false);

            if (gpio_get(PINO_BOTAO_B) == 0) {
                if (!verificar_jogada(1)) { // 1 para azul
                    game_over = true;
                    atualizar_display(nivel, indice_jogador, true);
                    sleep_ms(2000);
                    nivel = 1;
                    gerar_sequencia();
                    game_over = false;
                    break;
                }
                sleep_ms(ATRASO_DEBOUNCE_MS);
            }
            if (gpio_get(PINO_BOTAO_A) == 0) {
                if (!verificar_jogada(2)) { // 2 para verde
                    game_over = true;
                    atualizar_display(nivel, indice_jogador, true);
                    sleep_ms(2000);
                    nivel = 1;
                    gerar_sequencia();
                    game_over = false;
                    break;
                }
                sleep_ms(ATRASO_DEBOUNCE_MS);
            }
            if (gpio_get(PINO_BOTAO_JOYSTICK) == 0) {
                if (!verificar_jogada(0)) { // 0 para vermelho
                    game_over = true;
                    atualizar_display(nivel, indice_jogador, true);
                    sleep_ms(2000);
                    nivel = 1;
                    gerar_sequencia();
                    game_over = false;
                    break;
                }
                sleep_ms(ATRASO_DEBOUNCE_MS);
            }
        }

        if (indice_jogador == nivel) {
            nivel++;
            if (nivel > MAX_SEQUENCIA) {
                // Exibe a mensagem de vitória usando a função centralizada
                exibir_mensagem_centralizada("PARABENS");
                sleep_ms(3000);

                // Reinicia o jogo
                nivel = 1;
                gerar_sequencia();
            }
        }

        sleep_ms(1000);
    }

    return 0;
}

// Configuração dos GPIOs
void configurar_gpio() {
    gpio_init(PINO_BOTAO_B);
    gpio_set_dir(PINO_BOTAO_B, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_B);
    gpio_set_irq_enabled_with_callback(PINO_BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &callback_botao);

    gpio_init(PINO_BOTAO_A);
    gpio_set_dir(PINO_BOTAO_A, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_A);
    gpio_set_irq_enabled(PINO_BOTAO_A, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(PINO_BOTAO_JOYSTICK);
    gpio_set_dir(PINO_BOTAO_JOYSTICK, GPIO_IN);
    gpio_pull_up(PINO_BOTAO_JOYSTICK);
    gpio_set_irq_enabled(PINO_BOTAO_JOYSTICK, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(PINO_LED_VERMELHO);
    gpio_set_dir(PINO_LED_VERMELHO, GPIO_OUT);
    gpio_init(PINO_LED_VERDE);
    gpio_set_dir(PINO_LED_VERDE, GPIO_OUT);
    gpio_init(PINO_LED_AZUL);
    gpio_set_dir(PINO_LED_AZUL, GPIO_OUT);
}

// Configuração do I2C
void configurar_i2c() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(PINO_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_I2C_SDA);
    gpio_pull_up(PINO_I2C_SCL);
}

// Inicialização do display
void inicializar_display() {
    ssd1306_init(&display, LARGURA_DISPLAY, ALTURA_DISPLAY, false, ENDERECO_I2C, i2c1);
    ssd1306_config(&display);
}

// Callback para os botões
void callback_botao(uint gpio, uint32_t eventos) {
    absolute_time_t agora = get_absolute_time();

    if (gpio == PINO_BOTAO_B) {
        if (absolute_time_diff_us(ultimo_tempo_botao_b, agora) < ATRASO_DEBOUNCE_MS * 1000)
            return;
        ultimo_tempo_botao_b = agora;
    } else if (gpio == PINO_BOTAO_A) {
        if (absolute_time_diff_us(ultimo_tempo_botao_a, agora) < ATRASO_DEBOUNCE_MS * 1000)
            return;
        ultimo_tempo_botao_a = agora;
    } else if (gpio == PINO_BOTAO_JOYSTICK) {
        if (absolute_time_diff_us(ultimo_tempo_botao_joystick, agora) < ATRASO_DEBOUNCE_MS * 1000)
            return;
        ultimo_tempo_botao_joystick = agora;
    }
}

// Desenha a borda no display
void desenhar_borda() {
    ssd1306_rect(&display, 0, 0, LARGURA_DISPLAY, ALTURA_DISPLAY, true, false);
}

// Gera uma nova sequência aleatória
void gerar_sequencia() {
    for (uint8_t i = 0; i < MAX_SEQUENCIA; i++) {
        sequencia[i] = rand() % 3; // 0 para vermelho, 1 para azul, 2 para verde
    }
}

// Mostra a sequência atual
void mostrar_sequencia() {
    for (uint8_t i = 0; i < nivel; i++) {
        if (sequencia[i] == 0) {
            gpio_put(PINO_LED_VERMELHO, 1);
            sleep_ms(500);
            gpio_put(PINO_LED_VERMELHO, 0);
        } else if (sequencia[i] == 1) {
            gpio_put(PINO_LED_AZUL, 1);
            sleep_ms(500);
            gpio_put(PINO_LED_AZUL, 0);
        } else {
            gpio_put(PINO_LED_VERDE, 1);
            sleep_ms(500);
            gpio_put(PINO_LED_VERDE, 0);
        }
        sleep_ms(200);
    }
}

// Verifica a jogada do jogador
bool verificar_jogada(uint8_t cor) {
    if (sequencia[indice_jogador] == cor) {
        indice_jogador++;
        return true;
    }
    return false;
}