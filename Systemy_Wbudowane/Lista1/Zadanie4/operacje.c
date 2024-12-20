#include <stdio.h>
#include <stdint.h>

void perform_operations_int8(int8_t a, int8_t b) {
    int8_t wynik1 = a + b;
    int8_t wynik2 = a * b;
    int8_t wynik3 = a / b;
    printf("int8_t: %d + %d = %d, %d * %d = %d, %d / %d = %d\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_int16(int16_t a, int16_t b) {
    int16_t wynik1 = a + b;
    int16_t wynik2 = a * b;
    int16_t wynik3 = a / b;
    printf("int16_t: %d + %d = %d, %d * %d = %d, %d / %d = %d\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_int32(int32_t a, int32_t b) {
    int32_t wynik1 = a + b;
    int32_t wynik2 = a * b;
    int32_t wynik3 = a / b;
    printf("int32_t: %d + %d = %d, %d * %d = %d, %d / %d = %d\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_int64(int64_t a, int64_t b) {
    int64_t wynik1 = a + b;
    int64_t wynik2 = a * b;
    int64_t wynik3 = a / b;
    printf("int64_t: %d + %d = %d, %d * %d = %d, %d / %d = %d\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

void perform_operations_float(float a, float b) {
    float wynik1 = a + b;
    float wynik2 = a * b;
    float wynik3 = a / b;
    printf("float: %d + %d = %d, %d * %d = %d, %d / %d = %d\n", a, b, wynik1, a, b, wynik2, a, b, wynik3);
}

int main() {
    int8_t a8, b8;
    int16_t a16, b16;
    int32_t a32, b32;
    int64_t a64, b64;
    float af, bf;

    printf("Enter two int8_t numbers: ");
    scanf("%hhd %hhd", &a8, &b8);
    perform_operations_int8(a8, b8);

    printf("Enter two int16_t numbers: ");
    scanf("%hd %hd", &a16, &b16);
    perform_operations_int16(a16, b16);

    printf("Enter two int32_t numbers: ");
    scanf("%d %d", &a32, &b32);
    perform_operations_int32(a32, b32);

    printf("Enter two int64_t numbers: ");
    scanf("%lld %lld", &a64, &b64);
    perform_operations_int64(a64, b64);

    printf("Enter two float numbers: ");
    scanf("%f %f", &af, &bf);
    perform_operations_float(af, bf);

    return 0;
}