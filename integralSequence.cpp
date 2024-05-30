#include <iostream>
#include <cmath>
#include <ctime>
#include <chrono>

double f(double x) {
	return  x*x*sqrt(x);
}

double sequencialIntegral(double a, double b, int n) {
	double h = (b-a)/n;
	double x = a;
	double integral = (f(a) + f(b))/2.0;

	for (int i = 1; i < n; i++){
	   x = x + h;
	   integral = integral + f(x);
	}
	return integral*h;
}

double parallelIntegral (double a, double b, int n) {
	double h = (b-a)/n;
	double x = a;
	double integral = (f(a) + f(b))/2.0;
	int i;
	#pragma omp parallel for reduction(+:integral) num_threads(2)
	for (i = 1; i < n; i++) {
		integral = integral + f(i*h+a);
	}
	return integral*h;
}

int main(int argc, char **argv) {


	//init
	double integral;

	//parametros estatisticos
	double taxaDeConfianca = 0.95;
	double tableZValue = 1.96;
	int amostraQnt = 20;
	double proporcaoAmostral = 0.5;//int populacaoQnt = 100 000;

	//resultados estatisticos
	int piorIntervalo;
	double desvioPadrao;
	double avg;
	double margemDeErro;
	int amostras[amostraQnt];

	//tests realizados para a = 1 b = 2
	double a = 1;
	double b = 10;

	//get n
	int n;
	std::cout << "Digite a potencia de n" << std::endl;
	std::cin >> n;
	n = (int)pow((double)10,(double)n);

	//calc
	for (int i = 0; i < amostraQnt; i++) {
		std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
		integral = parallelIntegral(a, b, n);
		std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double, std::milli> timeCalc = t2 - t1;
		amostras[i] = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cout << "O tempo de calculo foi em de " << amostras[i] << " ms" << std::endl;
	}

	//get média
	avg = 0;
	for (int i = 0; i < amostraQnt; i++) {
		avg += amostras[i];
	}
	avg = avg/amostraQnt;

	//get pior intervalo
	long less = amostras[0];
	long more = amostras[0];
	for (int i = 0; i < amostraQnt; i++) {
		if (amostras[i] < less) {
			less = amostras[i];
		}
		if (amostras[i] > more) {
			more = amostras[i];
		}
	}
	piorIntervalo = more-less;
	
	//get desvio padrão
	for (int i = 0; i < amostraQnt; i++) {
		desvioPadrao = desvioPadrao + pow(amostras[i] - avg,2); 
	}
	desvioPadrao = sqrt(desvioPadrao/(amostraQnt-1));

	//get margem de erro para população infinita
	margemDeErro = avg * tableZValue * sqrt(proporcaoAmostral*(1-proporcaoAmostral)/amostraQnt);


	std::cout << "Tempo médio foi de " << round(avg) << std::endl;
	std::cout << "Pior Intervalo foi de " << piorIntervalo << std::endl;
	std::cout << "Desvio padrão foi de " << desvioPadrao << std::endl;
	std::cout << "Margem de erro para " << taxaDeConfianca*100  <<"\% de " << margemDeErro <<std::endl;
	std::cout << integral;

	return 0;
}