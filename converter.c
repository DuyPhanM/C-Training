#include <stdio.h>

int main(int argc, char *argv[]) {
    double inp, outp;
    char unit;
    double final_ratio;
    double final_add;
    char unit_conv;
    int func = -1;
    

    char unit_in[3] = {'F', 'M', 'P'};  // Fahrenheit, Miles, Pounds
    char unit_out[3] = {'C', 'K', 'G'}; // Celsius, Kilometers, Grams
    double ratio[3] = {5.0/9.0, 1.609, 453.592};
    double add[3] = {-32*(5.0/9.0), 0.0, 0.0};
    char *funtion[3] = {"temperature", "distance", "mass"};

    printf("Enter a quantity: ");
    scanf("%lf %c", &inp, &unit);
    
    for(int i=0; i<3; i++){
        if (unit == unit_in[i]){
            final_ratio = ratio[i];
            final_add = add[i];
            unit_conv = unit_out[i];
            func = i;
        } else if (unit == unit_out[i]){
            final_ratio = 1.0/ratio[i];
            final_add = -add[i]/ratio[i];
            unit_conv = unit_in[i];
            func = i;
        }
    }

    if (func == -1){
        printf("Invalid input.");
        return 1;
    }
    
    outp = inp * final_ratio + final_add;

    printf("\nThe %s %.2lf%c converts to %.2lf%c\n", funtion[func], inp, unit, outp, unit_conv);

    return 0;
}
