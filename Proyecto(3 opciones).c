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
		printf("Error en apertura de archivo");							//VALIDAMOS LA APERTURA DEL ARCHIVO//
		return 0;
	}
	else
	{
		fscanf(fp, "%d %d\n",&n,&m);									//LEEMOS EL ARCHIVO//
	}

																		//CON ESTO SE LLENA LA MATRIZ DE ADYACENCIA DE CEROS//
    int arr[n][n],x=0,y=0;												//DECLARAMOS LAS VARIABLES Y EL ARRGLO DE LA MATRIZ//

		for (x=0;x<n;x++){
			for (y=0;y<n;y++){
			arr[y][x]=0;
							}
						}
																		
																		/* DECLARACION DE VARIABLES QUE SE USARAN PARA LA MATRIZ DE ADYACENCIA*/
	int fila, colu, j=0, max=0, cont=0, cont1=0,i;
	char aux;
	
	for (i=0;i<m;i++){
																		/*LEEMOS DEL ARCHIVO LAS FILAS Y LAS COLUMNAS Y USAMOS LA VARIABLE AUXILIAR PORQUE EN EL ARCHIVO ESTA EN FORMATO 
																		PARENTESIS-NUMERO-COMA-NUMERO-PARENTESIS EJ (9,7)*/
		fscanf(fp,"%c %d %c %d %c\n",&aux, &fila, &aux, &colu, &aux);
		arr[fila-1][colu-1]=1;
		arr[colu-1][fila-1]=1;
	}																	/* EN ARR SE LE RESTA 1 A FILAS Y COLUMNAS PORQUE SINO EMPEZARIA EN CERO Y NO HAY NINGUN DELEGADO NUMERO CERO*/
	fclose(fp);
	
	printf("Organización de Protocolo para Recepciones Internacionales\n");
	printf("Ingrese la Función a Ejecutar:\n1. Verificación de Compatibilidad.\n2.Delegado(s) más incompatible.\n3.Listado de Delegados Sin Incompatibilidades\n4.Distribución de Mesas\n5.Salir\n");
	scanf("%d",&o);
	
	switch (o){															//DISEÑAMOS EL MENÚ//
		
		case 1:															//OPCIÓN 1//
			printf("Ingrese el primer delegado\n"); scanf("%d", &fila);
				if ((fila>n)||(fila<0)) {								//VALIDACIÓN DEL PRIMER DELEGADO//
					printf("El numero delegado ingresado es inválido\n");
		return 0;
	}
				printf("Ingrese el segundo delegado\n");
				scanf("%d", &colu);
				if ((colu>n)||(colu<0)) {								//VALIDACIÓN DEL SEGUNDO DELEGADO//
					printf("El número delegado ingresado es inválido\n");
		return 0;
	}
		if (arr[fila-1][colu-1]==1)
		{
			printf("Los delegados son incompatible\n");					//ACCIONES A EJECUTAR SEGÚN LA CONDICIÓN//
		}
		else
		{
			printf("Los delegados son compatibles\n");					
		}
	break;
		
		case 2:															//OPCIÓN 2//
		for (x=0;x<n;x++){
			j=0;
			 for (y=0;y<n;y++){
			 	if (arr[x][y]==1){
			 		j=j+1;
								}
				
							}
			 if (j>max){
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
			 	printf("%d tiene el mayor numero de conflicto\n",x+1);
			 }
			 
		}break;
			
			case 3: 													//OPCIÓN 3//
			for (x=0;x<n;x++){
				cont=0;
			 for (y=0;y<n;y++){
			 	if (arr[x][y]==1)										//RECORREMOS PARA SABER CUALES DELEGADOS POSEEN INCOMPATIBILIDAD Y EL CONTADOR SE INCREMENTA AL ENCONTRAR 1'S//
			 	{
			 		cont++;
				}	
			 }
			 if (cont>0){
				cont1=cont1+1;
			}
		}
		if (cont1==n){
		printf("Todos los delegados poseen al menos una incompatibilidad\n");
	}
		else{
			printf("No poseen incompatibilidades con nadie los siguientes delegados:\n");
		for (x=0;x<n;x++){
			cont=0;
			 for (y=0;y<n;y++)
			 {
			 	if (arr[x][y]==1)
			 	{
			 		cont++;
				}
			 }
			 if (cont==0)
			 {
			 	printf("-%d\n",x+1);
			 }
		}
	}break;
			 
			case 4: printf("El número mínimo de mesas es:\n");			//OPCIÓN 4//
				
			break;
			
			case 5: printf("Ha salido del Programa"); break;			//OPCIÓN 5//
			
			default: printf("Ingresó Una Opción Inválida\n");
		}
return 0;
}
