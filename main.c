#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
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
#define PINO_BUZZER_A 21
#define PINO_BUZZER_B 10

#define PINO_BOTAO_B 6
#define PINO_BOTAO_A 5
#define PINO_BOTAO_JOYSTICK 22

#define PINO_I2C_SDA 14
#define PINO_I2C_SCL 15
#define ENDERECO_I2C 0x3C

#define LARGURA_DISPLAY 128
#define ALTURA_DISPLAY 64

#define ATRASO_DEBOUNCE_MS 300

// Definição das notas musicais (frequências em Hz)
#define NOTE_C4 262
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_A3 220
#define NOTE_B3 247
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_C6 1047

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
void atualizar_display(uint8_t nivel_atual, uint8_t indice_atual, bool game_over, bool vez_jogador);
void exibir_tela_inicial();
void exibir_tela_instrucoes();
void exibir_segunda_tela_instrucoes();
void exibir_mensagem_centralizada(char *mensagem);
void tocar_nota(uint pino_buzzer, uint frequencia, uint duracao_ms);
void tocar_introducao();
void tocar_som_erro();
void tocar_melodia_parabens();
void configurar_pwm(uint pin);
void ajustar_brilho_led(uint pin, uint8_t brilho);

// Função para configurar o PWM em um pino
void configurar_pwm(uint pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM); // Configura o pino para usar PWM
    uint slice_num = pwm_gpio_to_slice_num(pin); // Obtém o slice do PWM
    pwm_set_wrap(slice_num, 255); // Define o valor máximo do PWM (8 bits)
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(pin), 0); // Inicia com duty cycle 0 (LED desligado)
    pwm_set_enabled(slice_num, true); // Habilita o PWM
}

// Função para ajustar o brilho do LED
void ajustar_brilho_led(uint pin, uint8_t brilho) {
    uint slice_num = pwm_gpio_to_slice_num(pin);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(pin), brilho);
}

// Função para gerar tom no buzzer
void tocar_nota(uint pino_buzzer, uint frequencia, uint duracao_ms)
{
    uint delay_us = 1000000 / frequencia / 2;
    uint ciclos = (duracao_ms * 1000) / (delay_us * 2);

    for (uint i = 0; i < ciclos / 2; i++)
    {
        gpio_put(pino_buzzer, 1);
        sleep_us(delay_us);
        gpio_put(pino_buzzer, 0);
        sleep_us(delay_us);
    }
}

// Função para tocar o som de erro
void tocar_som_erro()
{
    // Som descendente para indicar erro
    tocar_nota(PINO_BUZZER_A, NOTE_A4, 200);
    sleep_ms(50);
    tocar_nota(PINO_BUZZER_B, NOTE_A3, 300);
    sleep_ms(50);
    tocar_nota(PINO_BUZZER_A, NOTE_B3, 400);
}

// Função para tocar melodia de "PARABENS"
void tocar_melodia_parabens()
{
    tocar_nota(PINO_BUZZER_A, NOTE_C5, 200); // Dó agudo
    sleep_ms(50);
    tocar_nota(PINO_BUZZER_A, NOTE_E5, 200); // Mi agudo
    sleep_ms(50);
    tocar_nota(PINO_BUZZER_A, NOTE_G5, 300); // Sol agudo
    sleep_ms(50);
    tocar_nota(PINO_BUZZER_A, NOTE_C6, 400); // Dó mais agudo (final alegre)
}

// Função para tocar a introdução musical
void tocar_introducao()
{
    // Configurar os pinos dos buzzers como saída
    gpio_init(PINO_BUZZER_A);
    gpio_init(PINO_BUZZER_B);
    gpio_set_dir(PINO_BUZZER_A, GPIO_OUT);
    gpio_set_dir(PINO_BUZZER_B, GPIO_OUT);

    // Tocar notas alternadas nos buzzers
    tocar_nota(PINO_BUZZER_A, NOTE_C4, 200); // Dó no buzzer A
    sleep_ms(100);                           // Pequena pausa entre as notas

    tocar_nota(PINO_BUZZER_B, NOTE_E4, 200); // Mi no buzzer B
    sleep_ms(100);

    tocar_nota(PINO_BUZZER_A, NOTE_G4, 300); // Sol no buzzer A (um pouco mais longo)
    sleep_ms(200);                           // Pausa final mais longa
}

// Função para exibir mensagens centralizadas com borda
void exibir_mensagem_centralizada(char *mensagem)
{
    ssd1306_fill(&display, false);
    desenhar_borda();
    ssd1306_draw_string(&display, mensagem, 25, ALTURA_DISPLAY / 2 - 8);
    ssd1306_send_data(&display);
}

// Função para exibir a tela inicial
void exibir_tela_inicial()
{
    ssd1306_fill(&display, false);
    desenhar_borda();
    ssd1306_draw_string(&display, "BitColoursLab", 15, ALTURA_DISPLAY / 2 - 8);
    ssd1306_send_data(&display);
    tocar_introducao();
    sleep_ms(3000);
}

// Função para exibir a tela de instruções
void exibir_tela_instrucoes()
{
    ssd1306_fill(&display, false);
    ssd1306_draw_string(&display, "Reproduza as", 20, 16);
    ssd1306_draw_string(&display, "cores exibidas", 10, 32);
    ssd1306_draw_string(&display, "no LED", 40, 48);
    ssd1306_send_data(&display);
    sleep_ms(8000);
}

// Função para exibir a segunda tela de instruções
void exibir_segunda_tela_instrucoes()
{
    ssd1306_fill(&display, false);
    ssd1306_draw_string(&display, "Pressione", 25, 0);
    ssd1306_draw_string(&display, "A Green", 10, 16);
    ssd1306_draw_string(&display, "B Blue", 10, 32);
    ssd1306_draw_string(&display, "JoyPress Red", 10, 48);
    ssd1306_send_data(&display);
    sleep_ms(8000);
}

// Função para atualizar o display
void atualizar_display(uint8_t nivel_atual, uint8_t indice_atual, bool game_over, bool vez_jogador)
{
    ssd1306_fill(&display, false);
    ssd1306_draw_string(&display, "BitColoursLab", 15, 0);

    if (vez_jogador)
    {
        ssd1306_draw_string(&display, "SUA VEZ!", 35, ALTURA_DISPLAY / 2 - 8);
    }

    if (game_over)
    {
        exibir_mensagem_centralizada("GAME OVER!");
    }

    ssd1306_send_data(&display);
}

// Gera uma nova sequência aleatória
void gerar_sequencia()
{
    uint8_t cores_base[3] = {0, 1, 2};
    uint8_t indices[3] = {0, 1, 2};
    uint8_t temp;
    int j;

    // Embaralhar as cores iniciais
    for (int i = 2; i > 0; i--)
    {
        j = rand() % (i + 1);
        temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < 3; i++)
    {
        sequencia[i] = cores_base[indices[i]];
    }

    // Gerar o restante da sequência de forma aleatória
    for (uint8_t i = 3; i < MAX_SEQUENCIA; i++)
    {
        sequencia[i] = rand() % 3;
    }
}

// Função principal
int main()
{
    stdio_init_all();

    // Inicializar o ADC para gerar uma semente mais aleatória
    adc_init();
    adc_gpio_init(26);  // Usar o pino GP26 (ADC0) como entrada flutuante
    adc_select_input(0);

    // Ler um valor aleatório do ADC
    uint16_t random_adc_value = adc_read();

    // Combinar o tempo atual com o valor do ADC para a semente
    absolute_time_t now = get_absolute_time();
    srand(to_us_since_boot(now) + random_adc_value);

    configurar_gpio();
    configurar_i2c();
    inicializar_display();

    // Configurar PWM para os LEDs
    configurar_pwm(PINO_LED_VERMELHO);
    configurar_pwm(PINO_LED_VERDE);
    configurar_pwm(PINO_LED_AZUL);

    exibir_tela_inicial();
    exibir_tela_instrucoes();
    exibir_segunda_tela_instrucoes();

    gerar_sequencia();

    bool game_over = false;

    while (true)
    {
        atualizar_display(nivel, indice_jogador, false, false);
        mostrar_sequencia();

        indice_jogador = 0;
        while (indice_jogador < nivel)
        {
            atualizar_display(nivel, indice_jogador, false, true);

            if (gpio_get(PINO_BOTAO_B) == 0)
            {
                if (!verificar_jogada(1))
                {
                    game_over = true;
                    atualizar_display(nivel, indice_jogador, true, false);
                    tocar_som_erro();
                    sleep_ms(2000);
                    nivel = 1;
                    gerar_sequencia();
                    game_over = false;
                    break;
                }
                sleep_ms(ATRASO_DEBOUNCE_MS);
            }
            if (gpio_get(PINO_BOTAO_A) == 0)
            {
                if (!verificar_jogada(2))
                {
                    game_over = true;
                    atualizar_display(nivel, indice_jogador, true, false);
                    tocar_som_erro();
                    sleep_ms(2000);
                    nivel = 1;
                    gerar_sequencia();
                    game_over = false;
                    break;
                }
                sleep_ms(ATRASO_DEBOUNCE_MS);
            }
            if (gpio_get(PINO_BOTAO_JOYSTICK) == 0)
            {
                if (!verificar_jogada(0))
                {
                    game_over = true;
                    atualizar_display(nivel, indice_jogador, true, false);
                    tocar_som_erro();
                    sleep_ms(2000);
                    nivel = 1;
                    gerar_sequencia();
                    game_over = false;
                    break;
                }
                sleep_ms(ATRASO_DEBOUNCE_MS);
            }
        }

        if (indice_jogador == nivel)
        {
            nivel++;
            if (nivel > MAX_SEQUENCIA)
            {
                exibir_mensagem_centralizada("PARABENS");
                tocar_melodia_parabens();
                sleep_ms(3000);
                nivel = 1;
                gerar_sequencia();
            }
        }

        sleep_ms(1000);
    }

    return 0;
}

// Configuração dos GPIOs
void configurar_gpio()
{
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

    // Configura os pinos dos LEDs para PWM
    configurar_pwm(PINO_LED_VERMELHO);
    configurar_pwm(PINO_LED_VERDE);
    configurar_pwm(PINO_LED_AZUL);

    // Inicialização dos buzzers
    gpio_init(PINO_BUZZER_A);
    gpio_init(PINO_BUZZER_B);
    gpio_set_dir(PINO_BUZZER_A, GPIO_OUT);
    gpio_set_dir(PINO_BUZZER_B, GPIO_OUT);
}

// Configuração do I2C
void configurar_i2c()
{
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(PINO_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PINO_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_I2C_SDA);
    gpio_pull_up(PINO_I2C_SCL);
}

// Inicialização do display
void inicializar_display()
{
    ssd1306_init(&display, LARGURA_DISPLAY, ALTURA_DISPLAY, false, ENDERECO_I2C, i2c1);
    ssd1306_config(&display);
}

// Callback para os botões
void callback_botao(uint gpio, uint32_t eventos)
{
    absolute_time_t agora = get_absolute_time();

    if (gpio == PINO_BOTAO_B)
    {
        if (absolute_time_diff_us(ultimo_tempo_botao_b, agora) < ATRASO_DEBOUNCE_MS * 1000)
            return;
        ultimo_tempo_botao_b = agora;
    }
    else if (gpio == PINO_BOTAO_A)
    {
        if (absolute_time_diff_us(ultimo_tempo_botao_a, agora) < ATRASO_DEBOUNCE_MS * 1000)
            return;
        ultimo_tempo_botao_a = agora;
    }
    else if (gpio == PINO_BOTAO_JOYSTICK)
    {
        if (absolute_time_diff_us(ultimo_tempo_botao_joystick, agora) < ATRASO_DEBOUNCE_MS * 1000)
            return;
        ultimo_tempo_botao_joystick = agora;
    }
}

// Desenha a borda no display
void desenhar_borda()
{
    ssd1306_rect(&display, 0, 0, LARGURA_DISPLAY, ALTURA_DISPLAY, true, false);
}

// Mostra a sequência atual
void mostrar_sequencia()
{
    for (uint8_t i = 0; i < nivel; i++)
    {
        if (sequencia[i] == 0)
        {
            ajustar_brilho_led(PINO_LED_VERMELHO, 76); // 30% de brilho
            sleep_ms(500);
            ajustar_brilho_led(PINO_LED_VERMELHO, 0);
        }
        else if (sequencia[i] == 1)
        {
            ajustar_brilho_led(PINO_LED_AZUL, 76); // 30% de brilho
            sleep_ms(500);
            ajustar_brilho_led(PINO_LED_AZUL, 0);
        }
        else
        {
            ajustar_brilho_led(PINO_LED_VERDE, 76); // 30% de brilho
            sleep_ms(500);
            ajustar_brilho_led(PINO_LED_VERDE, 0);
        }
        sleep_ms(200);
    }
}

// Verifica a jogada do jogador
bool verificar_jogada(uint8_t cor)
{
    if (sequencia[indice_jogador] == cor)
    {
        indice_jogador++;
        return true;
    }
    return false;
}