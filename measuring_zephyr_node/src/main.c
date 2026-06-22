#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>
#include <stdbool.h>

#define NUM_SAMPLES 5000

/* only one of them should be active*/
#define TEST_PATH_A 0
#define TEST_PATH_B 0
#define TEST_PATH_C 1

#define BUTTON_PORT DT_NODELABEL(gpioc)
#define BUTTON_PIN  13

/* Signal output from Board1 */
#define OUT_PORT DT_NODELABEL(gpioa)
#define OUT_PIN  3

/* PATH A: Board2 FreeRTOS -> Board1 PA0 */
#define IN_PORT_A DT_NODELABEL(gpioa)
#define IN_PIN_A  0

/* PATH B: Board3 Zephyr -> Board1 PA15 */
#define IN_PORT_B DT_NODELABEL(gpioa)
#define IN_PIN_B  15

/* PATH C: Board4 ThreadX -> Board1 PB1 */
#define IN_PORT_C DT_NODELABEL(gpiob)
#define IN_PIN_C  1

static const struct device *button_dev;
static const struct device *out_dev;
static const struct device *in_dev_a;
static const struct device *in_dev_b;
static const struct device *in_dev_c;

static struct gpio_callback button_cb;
static struct gpio_callback in_cb_a;
static struct gpio_callback in_cb_b;
static struct gpio_callback in_cb_c;

static volatile uint32_t t_start = 0;

static volatile uint32_t count_a = 0;
static volatile uint32_t count_b = 0;
static volatile uint32_t count_c = 0;

static volatile bool test_running = false;
static volatile bool waiting_for_response = false;

static uint32_t latency_a[NUM_SAMPLES];
static uint32_t latency_b[NUM_SAMPLES];
static uint32_t latency_c[NUM_SAMPLES];

static void send_signal(void)
{
	t_start = k_cycle_get_32();

	gpio_pin_set(out_dev, OUT_PIN, 1);
	k_busy_wait(100);
	gpio_pin_set(out_dev, OUT_PIN, 0);
}

/* ================= PATH A ISR ================= */
void in_a_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
#if TEST_PATH_A
	if (test_running && waiting_for_response && count_a < NUM_SAMPLES) {
		uint32_t t_end = k_cycle_get_32();
		latency_a[count_a] = t_end - t_start;
		count_a++;
		waiting_for_response = false;
	}
#endif
}

/* ================= PATH B ISR ================= */
void in_b_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
#if TEST_PATH_B
	if (test_running && waiting_for_response && count_b < NUM_SAMPLES) {
		uint32_t t_end = k_cycle_get_32();
		latency_b[count_b] = t_end - t_start;
		count_b++;
		waiting_for_response = false;
	}
#endif
}

/* ================= PATH C ISR ================= */
void in_c_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
#if TEST_PATH_C
	if (test_running && waiting_for_response && count_c < NUM_SAMPLES) {
		uint32_t t_end = k_cycle_get_32();
		latency_c[count_c] = t_end - t_start;
		count_c++;
		waiting_for_response = false;
	}
#endif
}

static void print_final_result(const char *name, uint32_t *data, uint32_t count)
{
	uint32_t min = UINT32_MAX;
	uint32_t max = 0;
	uint64_t sum = 0;

	if (count == 0) {
		printk("\n===== FINAL RESULT %s =====\n", name);
		printk("No samples captured.\n");
		return;
	}

	for (uint32_t i = 0; i < count; i++) {
		if (data[i] < min) {
			min = data[i];
		}
		if (data[i] > max) {
			max = data[i];
		}
		sum += data[i];
	}

	uint32_t avg = sum / count;
	uint32_t range = max - min;

	uint64_t avg_ns = ((uint64_t)avg * 1000000000ULL) / sys_clock_hw_cycles_per_sec();

	printk("\n===== FINAL RESULT %s =====\n", name);
	printk("Samples       : %u\n", count);
	printk("Min Latency   : %u cycles\n", min);
	printk("Max Latency   : %u cycles\n", max);
	printk("Avg Latency   : %u cycles (%llu ns)\n", avg, avg_ns);
	printk("Latency Range : %u cycles\n", range);
}

static void dump_csv(const char *name, uint32_t *data, uint32_t count)
{
	printk("\n===== CSV %s =====\n", name);
	printk("sample,latency_cycles\n");

	for (uint32_t i = 0; i < count; i++) {
		printk("%u,%u\n", i + 1, data[i]);
	}

	printk("===== END CSV %s =====\n", name);
}

void button_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	if (test_running) {
		return;
	}

	count_a = 0;
	count_b = 0;
	count_c = 0;

	waiting_for_response = false;
	test_running = true;

	printk("\nStarting 5000-sample benchmark...\n");
}

static void run_one_sample(void)
{
	waiting_for_response = true;

	send_signal();

	while (waiting_for_response) {
	}

	k_msleep(50);
}

int main(void)
{
	button_dev = DEVICE_DT_GET(BUTTON_PORT);
	out_dev = DEVICE_DT_GET(OUT_PORT);

	in_dev_a = DEVICE_DT_GET(IN_PORT_A);
	in_dev_b = DEVICE_DT_GET(IN_PORT_B);
	in_dev_c = DEVICE_DT_GET(IN_PORT_C);

	gpio_pin_configure(out_dev, OUT_PIN, GPIO_OUTPUT_INACTIVE);

	gpio_pin_configure(button_dev, BUTTON_PIN, GPIO_INPUT);
	gpio_pin_interrupt_configure(button_dev, BUTTON_PIN, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button_cb, button_isr, BIT(BUTTON_PIN));
	gpio_add_callback(button_dev, &button_cb);

#if TEST_PATH_A
	gpio_pin_configure(in_dev_a, IN_PIN_A, GPIO_INPUT);
	gpio_pin_interrupt_configure(in_dev_a, IN_PIN_A, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&in_cb_a, in_a_isr, BIT(IN_PIN_A));
	gpio_add_callback(in_dev_a, &in_cb_a);
#endif

#if TEST_PATH_B
	gpio_pin_configure(in_dev_b, IN_PIN_B, GPIO_INPUT);
	gpio_pin_interrupt_configure(in_dev_b, IN_PIN_B, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&in_cb_b, in_b_isr, BIT(IN_PIN_B));
	gpio_add_callback(in_dev_b, &in_cb_b);
#endif

#if TEST_PATH_C
	gpio_pin_configure(in_dev_c, IN_PIN_C, GPIO_INPUT);
	gpio_pin_interrupt_configure(in_dev_c, IN_PIN_C, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&in_cb_c, in_c_isr, BIT(IN_PIN_C));
	gpio_add_callback(in_dev_c, &in_cb_c);
#endif

	printk("5000-Sample 3-Path Benchmark Ready\n");
	printk("Press blue USER button to start\n");

	while (1) {
		if (test_running) {
			/* Warm-up: discard first 20 samples */
			for (uint32_t i = 0; i < 20; i++) {
				run_one_sample();
			}

#if TEST_PATH_A
			count_a = 0;
#endif
#if TEST_PATH_B
			count_b = 0;
#endif
#if TEST_PATH_C
			count_c = 0;
#endif

			for (uint32_t i = 0; i < NUM_SAMPLES; i++) {
				run_one_sample();
			}

			test_running = false;

#if TEST_PATH_A
			dump_csv("PATH_A_BOARD2_FREERTOS", latency_a, count_a);
			print_final_result("PATH A - Board2 FreeRTOS", latency_a, count_a);
#endif

#if TEST_PATH_B
			dump_csv("PATH_B_BOARD3_ZEPHYR", latency_b, count_b);
			print_final_result("PATH B - Board3 Zephyr", latency_b, count_b);
#endif

#if TEST_PATH_C
			dump_csv("PATH_C_BOARD4_THREADX", latency_c, count_c);
			print_final_result("PATH C - Board4 ThreadX", latency_c, count_c);
#endif

			printk("\nBenchmark finished.\n");
			printk("Press blue USER button to run again.\n");
		}

		k_msleep(100);
	}

	return 0;
}
