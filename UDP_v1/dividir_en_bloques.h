#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

int contar_caracteres(char const* filename)
{
	FILE * f;
	int  numero = 0;
	char caracter;
	f = fopen (filename, "r");
	if (f == NULL)
	{
			return -1;
	}
	while (feof (f) == 0)
	{
			fscanf (f, "%c", &caracter);
			numero++;
	}
	fclose (f);
	return numero-2;
}

void imprimir_bloques(vector<vector<char>> &bloques, int columna, int fila)
{
  cout <<endl;
  for (int i = 0; i < columna; ++i)
  {
    for (int j = 0; j < fila; ++j)
    {
      cout << bloques[i][j] << " ";
    }
    cout << "\n";
  }
  cout << "\n";
}

void dividir_archivo_en_bloques(char const* filename)
{
	FILE *archivo = fopen(filename, "r+b");
	int size;
	fseek(archivo,0,SEEK_END);
	size = ftell(archivo);
	fseek(archivo, 0, SEEK_SET);
	int read_size;
	char send_buffer[size];

	read_size = fread(send_buffer, 1, sizeof(send_buffer)-1, archivo);
	send_buffer[size] = '\0';

	for (int j = 0; j < size; ++j)
	{
	cout << send_buffer[j] << " ";
	}

	int numero_bytes = 8;
	int numero_letras = contar_caracteres(filename);
	int numero_bloques = numero_letras / numero_bytes;
	int modulo = numero_letras % numero_bytes;
	if (modulo != 0){
			numero_bloques++;
	}
	cout<<endl;
	cout << "Matriz de: "<< numero_bloques <<" x "<<numero_bytes<<endl;
	vector<vector<char>> bloques(numero_bloques, vector<char>(numero_bytes, ' '));
	int contador = 0;
	for (int i = 0; i < numero_bloques; ++i)
	{
			for (int j = 0; j < numero_bytes; ++j)
			{
					bloques[i][j] = send_buffer[contador];
					contador++;
			}
	}
	imprimir_bloques(bloques, numero_bloques, numero_bytes);

}
