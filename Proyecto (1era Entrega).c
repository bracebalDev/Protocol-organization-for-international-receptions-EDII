#include <stdio.h>
#include <stdlib.h>
int main() {
	int n, m, o;
	char nombre[80];

/*INGRESAR EL NOMBRE DEL ARCHIVO EN ESTE CASO SERIA Convencion.in*/	
	FILE *fp;
	puts("Ingrese el nombre del archivo");
	scanf("%s",nombre);
	fp = fopen ( nombre, "r");
	
	if (fp==NULL){
		return 0;
	}
	else
	{
		fscanf(fp, "%d %d\n", &n,&m);
	}

/*CON ESTO SE LLENA LA MATRIZ DE CEROS*/
        int arr[n][n],x=0,y=0;

	for (x=0;x<n;x++){
		for (y=0;y<n;y++)
		{
			arr[y][x]=0;
		}
	}

	/* DECLARACION DE VARIABLES QUE SE USARAN PARA LA MATRIZ*/
	int fila, colu, i, j=0, max=0;
	char aux;
	
	for (i=0;i<m;i++){
		/*LEEMOS DEL ARCHIVO LAS FILAS Y LAS COLUMNAS Y USAMOS LA VARIABLE AUXILIAR PORQUE EN EL ARCHIVO ESTA EN FORMATO 
	PARENTESIS-NUMERO-COMA-NUMERO-PARENTESIS EJ (9,7)*/
		fscanf(fp,"%c %d %c %d %c\n",&aux, &fila, &aux, &colu, &aux);
		arr[fila-1][colu-1]=1;
		arr[colu-1][fila-1]=1;
	}	/* EN ARR SE LE RESTA 1 A FILAS Y COLUMNAS PORQUE SINO EMPEZARIA EN CERO Y NO HAY NINGUN DELEGADO NUMERO CERO*/

	fclose(fp);
	
	printf("Organización de Protocolo para Recepciones Internacionales\n");
	printf("Ingrese la Función a Ejecutar:\n1. Verificación de Compatibilidad.\n2.Delegado(s) más incompatible.\n3.Salir\n");
	scanf("%d",&o);
	
	switch (o){
		case 1:
			printf("Ingrese el primer delegado\n"); scanf("%d", &fila);
				if ((fila>n)||(fila<0)) {
					printf("El numero delegado ingresado es inválido\n");
		return 0;
	}
				printf("Ingrese el segundo delegado\n");
				scanf("%d", &colu);
				if ((colu>n)||(colu<0)) {
					printf("El número delegado ingresado es inválido\n");
		return 0;
	}
		if (arr[fila-1][colu-1]==1)
		{
			printf("Los delegados son incompatible\n");
		}
		else
		{
			printf("Los delegados son compatibles\n");
		}
	break;
		
		case 2:
		for (x=0;x<n;x++)
		{
			j=0;
			 for (y=0;y<n;y++)
			 {
			 	if (arr[x][y]==1)
			 	{
			 		j=j+1;
				}
				
			 }
			 if (j>max)
			 {
			 max=j;	
			 }
			 
		}
		
		for (x=0;x<n;x++)
		{
			j=0;
			 for (y=0;y<n;y++)
			 {
			 	if (arr[x][y]==1)
			 	{
			 		j=j+1;
				}
				
			 }
			 if (j==max)
			 {
			 	printf("%d tiene el mayor numero de conflicto\n", x+1);
			 }
			 
		}
			break;
			case 3: printf("Ha salido del Programa"); break;
			default: printf("Ingresó Una Opción Inválida\n");
		}
return 0;
}
