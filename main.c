#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Function to calculate the maximum of two numbers
double max(double a, double b) {
    return (a > b) ? a : b;
}

double calculate_option_payoff(double stock_price, double strike_price) {
	return max(stock_price - strike_price, 0.0);
}


void calculate_underlying_tree(double price, double up, double down, int steps, double **stockTree) { 
	int i, j;

	for(i = 0; i < steps+1; i++) {
		for(j = 0; j < i+1; j++) {
			stockTree[j][i] = price * pow(up, j) * pow(down, i-j);
		}
	}
}	

void print_tree(double **stockTree, int steps) {
	int i, j;

	for(i = 0; i < steps + 1; i++){
		for(j = 0; j < steps + 1; j++){
			printf("%.2lf\t", stockTree[i][j]);
		}
		printf("\n");
	}
}

double binomial_price_model(double price, double strike, double rate, double time, double volatility, int steps) {
	// Calculate up and down factor
	double time_step = time / steps;
	double u = exp(volatility * sqrt(time_step));
	double d = 1 / u;

	// Calculate probability of an up and down move
	double p = (exp(rate * time_step) - d) / (u-d);
	double q = 1 - p; 

	// Create a 2D array to store stock prices representing the nodes of the tree
	double **stockTree = (double **) malloc((steps + 1) * sizeof(double *));
	int i, j;
	for(i = 0; i < steps+1; i++){
		stockTree[i] = (double *) malloc((steps + 1) * sizeof(double));
	}
	calculate_underlying_tree(price, u, d, steps, stockTree);

	// Create a 2D array to store option prices representing the nodes of the tree
	double **optionTree = (double **) malloc((steps + 1) * sizeof(double *));
	for(i = 0; i < steps+1; i++){
		optionTree[i] = (double *) malloc((steps + 1) * sizeof(double));
	}

	// Calculate option value at termimal nodes
	for(i = 0; i < steps + 1; i++){ 
		optionTree[i][steps] = max(stockTree[i][steps] - strike, 0);
	}

	// Calculate options values of initial position by moving backwards through tree
	for(i = steps - 1; i >= 0; i--) {
		for(j = 0; j <= i; j++) {
			double exercise_value = max(stockTree[j][i] - strike, 0);
			double hold_value = exp(-rate * time_step) * (p * optionTree[j][i+1] + q * optionTree[j+1][j+1]);
			optionTree[j][i] = max(exercise_value, hold_value);
		}
	}
	
	double calculated = optionTree[0][0];

	//print_tree(stockTree, steps);
	//printf("\n");
	//print_tree(optionTree, steps);

	// Deallocate memory for the 2D array stockTree
	for (i = 0; i < steps + 1; i++) {
		free(stockTree[i]);
	}
	free(stockTree);

	// Deallocate memory for the 2D array optionTree
	for (i = 0; i < steps + 1; i++) {
		free(optionTree[i]);
	}
	free(optionTree);
	
	return calculated;
}

int main() {
    // Input parameters
    double S0 = 100;    // Initial stock price
    double K = 105;     // Strike price
    double r = 0.05;      // Risk-free interest rate
    double T = 1;       // Time to expiration (in years)
    int n = 2;            // Number of time steps
    double volatility = 0.2; // Annual volatility value

	double optionPrice = binomial_price_model(S0, K, r, T, volatility, n);

    printf("Option Price: %.2lf\n", optionPrice);

    return 0;
}
