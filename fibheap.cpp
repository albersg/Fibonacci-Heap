#include "fibheap.h"
#include <Windows.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>

double PCFreq = 0.0;
__int64 CounterStart = 0;

// Estas dos funciones y las dos variables de arriba se usan para el temporizador.
void StartCounter()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		std::cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart) / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

FiboHeap::FiboNode* FiboHeap::initialize_node(int new_key) {
	FiboNode* node = new FiboNode;
	
	node->child = nullptr;
	node->degree = 0;
	node->key = new_key;
	node->left = node;
	node->mark = false;
	node->parent = nullptr;
	node->right = node;

	return node;
}


FiboHeap::FiboNode* FiboHeap::fib_heap_insert(int new_key) {
	// Creamos un nuevo nodo con la key que nos pasan
	FiboNode* node = initialize_node(new_key);

	// Concatenamos la root list del montículo con el nuevo nodo
	concatenate_root_lists(node);

	// Actualizamos mínimo
	if (new_key < this->min_node->key)
		this->min_node = node;

	// Incrementamos número de nodos
	this->num_nodes++;

	return node;
}

void FiboHeap::concatenate_root_lists(FiboNode* node) {
	if (this->min_node == nullptr)
		this->min_node = node;
	else if (this->min_node != nullptr && node != nullptr) { // Se concatenan las dos listas en tiempo constante
		FiboNode* aux_right = this->min_node->right;

		this->min_node->right = node->right;
		this->min_node->right->left = this->min_node;

		node->right = aux_right;
		node->right->left = node;
	}
}

void FiboHeap::fib_heap_union(FiboHeap* other_heap) {
	// Concatenamos las dos listas
	concatenate_root_lists(other_heap->min_node);

	// Actualizamos mínimo
	if ((this->min_node == nullptr) || (other_heap->min_node != nullptr && other_heap->min_node < this->min_node))
		this->min_node = other_heap->min_node;

	// Actualizamos número de nodos
	this->num_nodes += other_heap->num_nodes;
}


void FiboHeap::fib_heap_extract_min() {
	FiboNode* z = this->min_node;
	FiboNode* x;
	FiboNode* it;

	if (z != nullptr) {
		x = z->child;

		if (x != nullptr) {
			it = x;
			// Desenparentamos a todos los hijos de z
			do {
				it->parent = nullptr;
				it = it->right;
			} while (it != x);

			// Concatenamos la lista de los hijos de z con la root list
			concatenate_root_lists(x);
		}

		// Quitamos z del montículo
		remove_from_list(z);

		if (z == z->right)
			this->min_node = nullptr;
		else {
			this->min_node = z->right;
			consolidate();
		}

		// Actualizamos el número de nodos
		this->num_nodes--;
	}
}


void FiboHeap::consolidate() {

	// Cota superior del numero de hijos de cualquier montículo H. Por las transparencias sabemos que es gn <= 2log(n)
	int d_nodes = (int)(2 * log2(this->num_nodes));

	FiboNode** A = new FiboNode* [d_nodes];

	for (int i = 0; i < d_nodes; i++) {
		A[i] = nullptr;
	}

	FiboNode* it = this->min_node;
	FiboNode* x;
	FiboNode* y;
	FiboNode* aux; // Puntero auxiliar para luego poder hacer el exchange
	int degree;
	bool fin = false;

	// No ponemos condición de parada ya que para finalizar lo hago con un break dentro del bucle
	while(true){
		x = it;
		it = it->right;
		degree = x->degree;

		while (A[degree] != nullptr) {
			y = A[degree];

			// Si ambos punteros son iguales significa que ya hemos recorrido toda la lista
			if (x == y) {
				fin = true;
				break;
			}
			// Hacemos el exchange
			if (x->key > y->key) {
				aux = x;
				x = y;
				y = aux;
			}

			// Para conservar el puntero it en la root list
			if (it == y)
				it = x;

			fib_heap_link(y, x);

			A[degree] = nullptr;

			degree++;
		}

		if (fin)
			break;

		A[degree] = x;

	}

	this->min_node = nullptr;

	// Vaciamos el vector A y metemos todos sus elementos al montículo
	for (int i = 0; i < d_nodes; i++) {
		if (A[i] != nullptr) {
			A[i]->left = A[i];
			A[i]->right = A[i];

			concatenate_root_lists(A[i]);

			if (this->min_node == nullptr || (A[i]->key < this->min_node->key))
				this->min_node = A[i];
		}
	}

	delete[] A;
}

void FiboHeap::fib_heap_link(FiboNode* y, FiboNode* x) {
	// Lo quitamos de la lista
	remove_from_list(y);

	y->left = y;
	y->right = y;

	// Si no tiene hijos, caso trivial
	if (x->child == nullptr) {
		x->child = y;

		y->right = y;
		y->left = y;
	}
	else { // Si tiene hijos, tenemos que concatenar las dos listas
		FiboNode* aux_right = x->child->right;

		x->child->right = y->right;
		x->child->right->left = x->child;

		y->right = aux_right;
		y->right->left = y;
	}

	// Actualizamos atributos
	y->parent = x;
	x->degree++;
	y->mark = false;
}

void FiboHeap::remove_from_list(FiboNode* node) {
	node->left->right = node->right;
	node->right->left = node->left;
}

void FiboHeap::fib_heap_decrease_key(FiboNode* node, int new_key) {
	if (new_key <= node->key) {
		node->key = new_key;

		FiboNode* y;
		y = node->parent;

		// Hacemos la llamada a las funciones de corte
		if (y != nullptr && node->key < y->key) {
			cut(node, y);
			cascading_cut(y);
		}

		// Actualizamos mínimo
		if (node->key < this->min_node->key)
			this->min_node = node;
	}
	else {
		std::cout << "El valor de la clave introducida es mayor al actual.\n";
	}
}

void FiboHeap::cut(FiboNode* x, FiboNode* y) {
	// Quitamos x de la lista en la que estaba y lo llevamos a la root list
	remove_from_list(x);
	y->degree--;
	concatenate_root_lists(x);

	x->parent = nullptr;
	x->mark = false;
}

void FiboHeap::cascading_cut(FiboNode* y) {
	FiboNode* z = y->parent;

	if (z != nullptr) {
		if (!z->mark) {
			z->mark = true;
		}
		else {
			cut(y, z);
			cascading_cut(z);
		}
	}
}

int FiboHeap::fib_heap_min() {
	if (this->min_node != nullptr)
		return this->min_node->key;
	else {
		std::cout << "El heap esta vacio.\n";
		return -1;
	}
}

void FiboHeap::print_root_list(FiboNode* x) {
	FiboNode* it = x;

	do {
		std::cout << it->key << " -> ";
		it = it->right;
	} while (it != this->min_node);

	std::cout << std::endl;
}


int main() {
	std::ifstream myFile;
	int numOfHeaps = 0;
	int numOfNodes1 = 0;
	int numOfNodes2 = 0;
	int data = 0;
	char op = ' ';

	myFile.open("fichero2.txt");

	if (!myFile.is_open()) {
		std::cout << "No se pudo abrir el fichero.\n";
		return EXIT_FAILURE;
	}

	myFile >> numOfHeaps;

	FiboHeap* H1 = new FiboHeap(); 
	FiboHeap::FiboNode* aux = nullptr;

	// En el primer montículo se insertan números pares, y en el segundo, si existe, los impares
	myFile >> numOfNodes1;
	for (int j = 0; j < numOfNodes1; j++) {
		aux = H1->fib_heap_insert(2*j);
	}

	if (numOfHeaps > 1) {
		FiboHeap* H2 = new FiboHeap();

		myFile >> numOfNodes2;
		for (int j = 0; j < numOfNodes2; j++) {
			H2->fib_heap_insert(2*j +1);
		}
		
		H1->fib_heap_union(H2);
	}

	while (true) {
		myFile >> op;
		if (myFile.eof()) 
			break;

		if (op == 'e') { // Eliminar el mínimo
			H1->fib_heap_extract_min();
		}
		else if (op == 'm') { // Obtener el mínimo
			std::cout << H1->fib_heap_min() << "\n";
		}
		else if (op == 'd') { // Decrementar una clave
			myFile >> data;
			H1->fib_heap_decrease_key(aux, data);
		}
	}
	
	myFile.close();

	return 0;
}