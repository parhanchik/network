#include <stdio.h>
#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>





/*
	В данной функции происходит открытие файла in.txt, чтение из  него
	входных данных (N и IP), а также минимальная проверка их на корректность.
*/
int input_data(uint32_t *ip, short *N){
	FILE *fp;
	char n_str[3], ip_str[15], *octet;
	uint8_t one_byte = 255;
	char c;


	//open in.txt
	if ((fp = fopen("in.txt", "r")) == NULL) { 
		printf("Error: Can\'t open in.txt file.");
		exit(1);
	}

		
	//read in.txt
	if (!fgets(n_str, 3, fp)){
		printf("Error: N input err.");
		exit(1);
	}


	if (fseek(fp,0,SEEK_SET)){
		printf("Error: fseek().");
		exit(1);
	}
	int numb_in_N =0;
	while ((c = fgetc(fp))!='\n'){ numb_in_N++; }
	if (numb_in_N > 2){
		printf("Error: Incorrect N.");
		exit(1);
	}


	if (!fgets(ip_str, 15, fp)){
		printf("Error: IP input err.");
		exit(1);
	}


	fclose(fp);

	int iter =0;
	c = n_str[0];
	while (c != '\n' && c !='\0'){ 
		
		if (c < '1' || c > '9') {
			printf("Error: Incorrect N");
			exit(1);
		}
		iter++;
		c = n_str[iter];
	}


	*N = (short)(atoi(n_str));


	//check max subnets
	if (*N > 24 || *N < 0){
		printf("Error: Incorrect input subnets numb");
		exit(1);
	}





/*						TODO


	Данный участок кода был предназначен для проверки соответствия введенного IP адреса шаблону.
	На чистом С реализовать проверку не удалость, удалось только на С++.


	regex_t regex;
	if (regcomp(&regex, "[1-9]{1}[0-9]{0,1,2}(\.[0-9]{1,2,3}){2}\.[1-9]{1}[0-9]{0,1,2}", 0)){
		printf("Error: N input err.");
		exit(1);
	}
	else printf("OK compile reg\n");
	int reti = regexec(&regex, ip_str, 0, NULL, 0);
	printf("%s\n, %d", ip_str, reti);
	if( !reti ){
               printf("Match");
	}
        else if(reti == REG_NOMATCH ){
               printf("No match");
        }
        else{
               printf("Regex match failed:\n");
               exit(1);
        }
	 regfree(&regex);
*/       


	// проверка IP на корректность (костыль вместо регулярки)
	uint8_t numbs_in_octet = 0, points_count = 0;

	iter = 0;
	c = ip_str[0];
	while (c != '\n' && c!= '\0') {
		
		if ((c < '0' || c > '9') && c != '.') {
			printf("Error: Incorrect IP address1");
			exit(1);
		}
		else if (c != '.')
			numbs_in_octet ++;
		     else if (numbs_in_octet > 3 || numbs_in_octet < 1) {
				printf("Error: Incorrect IP address2");
				exit(1);
		     }
			  else {
				points_count ++;
				numbs_in_octet = 0;
			}
	iter++;
	c = ip_str[iter];
	}

	if (points_count != 3) {
		printf("Error: Incorrect IP address3");
		exit(1);
	}


	//парсинг прочитанной из файла ip строки
	uint32_t first_octet = (uint32_t)(atoi(octet = strtok(ip_str, ".")));
	uint32_t second_octet = (uint32_t)(atoi(octet = strtok(NULL, ".")));
	uint32_t third_octet = (uint32_t)(atoi(octet = strtok(NULL, ".")));
	uint32_t fourth_octet = (uint32_t)(atoi(octet = strtok(NULL, "\n")));


	// проверка корректности ip
	if (first_octet == 0 || fourth_octet == 0 || first_octet > 255 ||
		second_octet > 255 || third_octet > 255 || fourth_octet > 255){
		printf("Error: Incorrect IP address");
		exit(1);
	}


	//укладываем ip по октетам в uint32_t	
	*ip = first_octet << 24 | second_octet << 16 |
			third_octet << 8 | fourth_octet;

//	printf("N = %hd\n", *N);
//	printf("ip = %u\n", (unsigned int)(*ip));
	
	return 0;
}





/*
	Эта функция не вызывается, использовал ее для отладки (вывода сразу в консоль)
*/
void printSubnets(uint64_t ip){

	uint32_t octet = 255;
	printf("ip= %u.%u.%u.%u/%u\n", (unsigned int)((ip>>24)&octet), (unsigned int)((ip>>16)&octet),
					(unsigned int)((ip>>8)&octet), (unsigned int)(ip&octet),
					(unsigned int)((ip>>32)&octet));

}



/* 
	Функция подсчитывает максимально возможное количество подсетей для конкретного IP адреса.
*/
	
int findSubnetsCount(uint32_t ip){
	int subnets_count = 24;
	uint32_t bit = 1;
	while (ip % 2 == 0) {
		ip >>= 1;
		subnets_count--;
	}

	return subnets_count;
}




/*
	Данная функция осуществляет генерацию всех возможных подсетей для конкретного IP адреса
	и выбирает из них случайным образом N подсетей, которые выводит в файл autogen.txt. Подсеть
	с максимальным префиксом выводится в out.txt. 

*/
void generateSubnets(uint32_t ip, int N, int subnets_count){


	if (N > subnets_count) {
		printf("Error: Input subnets count more then have this IP address.");
		exit(1);
	}



	uint64_t *subnets = (uint64_t *)malloc(subnets_count * sizeof(uint64_t));
	uint64_t *new_subnets_arr = (uint64_t *)malloc(subnets_count * sizeof(uint64_t));
	uint64_t good_ip = 0;
	uint64_t prefix_octet = (uint64_t)255<<32;
	uint8_t octet = 255;
	uint64_t cur_subnet;
	bool flag = false;
	uint32_t one = 1;

	uint32_t mask = 255;
	uint32_t bit = 1;
	uint64_t subnet;
	int hosts_bits = 24;
	
	mask <<= hosts_bits;
	bit <<= 24;


	// Генерируются все возможные подсети для IP адреса
	for (int i = 0; hosts_bits > 0, i < subnets_count; hosts_bits--, i++) {
		mask |= bit;
		bit >>= 1;
		
		if ((ip != (ip&mask)) || flag)
			subnet = ((uint64_t)(ip & mask)) | ((uint64_t)(32-hosts_bits)<<32);

			
		subnets[i] = subnet;
	}


	
	FILE *out, *autogen;
	autogen = fopen("autogen.txt", "w");


	// Случайным образом из всех подсетей выбирается и выводится в файл N штук.
	srand(time(NULL));
	for (int i=0; i <N; i++)
	{
		uint64_t max_prefix;
		int subnet_pos = rand() % subnets_count;
		cur_subnet = subnets[subnet_pos];
		fprintf(autogen, "%u.%u.%u.%u/%u\n", (unsigned int)((cur_subnet>>24)&octet), 
				(unsigned int)((cur_subnet>>16)&octet),(unsigned int)((cur_subnet>>8)&octet),
				(unsigned int)(cur_subnet&octet),(unsigned int)((cur_subnet>>32)&octet));
		subnets_count--;
		new_subnets_arr = (uint64_t *)realloc(new_subnets_arr, subnets_count * sizeof(uint64_t));
		for (int j=0, k=0; j< subnets_count+1; j++)
		{
			if (j != subnet_pos) {
				new_subnets_arr[k] = subnets[j];
				k++;
			} else {
				max_prefix = subnets[j]&prefix_octet;
				if ( max_prefix > good_ip)
					good_ip = subnets[j];

			}
		}

		subnets = (uint64_t *)realloc(subnets, subnets_count * sizeof(uint64_t));
		memcpy(subnets, new_subnets_arr, subnets_count * sizeof(uint64_t));
	}


	// Вывод в файл out.txt
	out = fopen("out.txt", "w");
	fprintf(out, "%u.%u.%u.%u\n", (unsigned int)((ip>>24)&octet), 
		(unsigned int)((ip>>16)&octet),(unsigned int)((ip>>8)&octet),
		(unsigned int)(ip&octet));
	fprintf(out, "%u.%u.%u.%u/%u\n", (unsigned int)((good_ip>>24)&octet), 
		(unsigned int)((good_ip>>16)&octet),(unsigned int)((good_ip>>8)&octet),
		(unsigned int)(good_ip&octet),(unsigned int)((good_ip>>32)&octet));
		



	// закрываем файлы и освобождаем память
	fclose(autogen);
	fclose(out);
	free(subnets);
	free(new_subnets_arr);

}


int main (){
	uint32_t ip;
	short N;
	if ( input_data(&ip, &N))
     		return 1;
	generateSubnets(ip, N, findSubnetsCount(ip));


	return 0;
}