#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

/* ================= INPUT ================= */
#define IN_PORT DT_NODELABEL(gpioa)
#define IN_PIN  4

/* ================= OUTPUTS ================= */
#define OUT_PORT_A DT_NODELABEL(gpioa)
#define OUT_PIN_A  3

#define OUT_PORT_B DT_NODELABEL(gpiob)
#define OUT_PIN_B  1

/* ================= LED LD1 ================= */
#define LED_PORT DT_NODELABEL(gpiob)
#define LED_PIN  0

static const struct device *in_dev;
static const struct device *out_dev_a;
static const struct device *out_dev_b;
static const struct device *led_dev;

static struct gpio_callback in_cb;

/* ==========================================
   ISR
========================================== */
static void input_isr(const struct device *dev,
                      struct gpio_callback *cb,
                      uint32_t pins)
{
    int state = gpio_pin_get(in_dev, IN_PIN);

    if (state > 0)
    {
        /* PA3 HIGH */
        gpio_pin_set(out_dev_a, OUT_PIN_A, 1);

        /* PB1 HIGH */
        gpio_pin_set(out_dev_b, OUT_PIN_B, 1);

        /* LD1 ON */
        gpio_pin_set(led_dev, LED_PIN, 1);
    }
    else
    {
        /* PA3 LOW */
        gpio_pin_set(out_dev_a, OUT_PIN_A, 0);

        /* PB1 LOW */
        gpio_pin_set(out_dev_b, OUT_PIN_B, 0);

        /* LD1 OFF */
        gpio_pin_set(led_dev, LED_PIN, 0);
    }
}

/* ==========================================
   MAIN
========================================== */
int main(void)
{
    in_dev = DEVICE_DT_GET(IN_PORT);
    out_dev_a = DEVICE_DT_GET(OUT_PORT_A);
    out_dev_b = DEVICE_DT_GET(OUT_PORT_B);
    led_dev = DEVICE_DT_GET(LED_PORT);

    if (!device_is_ready(in_dev) ||
        !device_is_ready(out_dev_a) ||
        !device_is_ready(out_dev_b) ||
        !device_is_ready(led_dev))
    {
        printk("GPIO device not ready\n");
        return 0;
    }

    /* Outputs */
    gpio_pin_configure(out_dev_a,
                       OUT_PIN_A,
                       GPIO_OUTPUT_INACTIVE);

    gpio_pin_configure(out_dev_b,
                       OUT_PIN_B,
                       GPIO_OUTPUT_INACTIVE);

    gpio_pin_configure(led_dev,
                       LED_PIN,
                       GPIO_OUTPUT_INACTIVE);

    /* Input */
    gpio_pin_configure(in_dev,
                       IN_PIN,
                       GPIO_INPUT);

    /*
      Both edges:
      - Rising edge  -> outputs ON
      - Falling edge -> outputs OFF
    */
    gpio_pin_interrupt_configure(in_dev,
                                 IN_PIN,
                                 GPIO_INT_EDGE_BOTH);

    gpio_init_callback(&in_cb,
                       input_isr,
                       BIT(IN_PIN));

    gpio_add_callback(in_dev,
                      &in_cb);

    printk("PA4 -> PA3 + PB1 + LD1 Forwarder Ready\n");

    while (1)
    {
        k_sleep(K_FOREVER);
    }

    return 0;
}
