#ifndef FIBHEAP
#define FIBHEAP

class FiboHeap {
public:
	// Declaramos el struct FiboNode en la parte pública para luego poder declarar una variable de este tipo en el main, para guardar el puntero para el decrease_key.
	// Suponemos que la clave de cada nodo es un entero.
	struct FiboNode {
		FiboNode* left;
		FiboNode* right;
		FiboNode* parent;
		FiboNode* child;
		int degree;
		int key;
		bool mark;
	};

	// Contador del numero de nodos que tenemos en el monticulo.
	int num_nodes = 0;

	FiboHeap(): min_node(nullptr) {}

	// Función que devuelve cuál es el elemento mínimo.
	int fib_heap_min();

	// Funcion para insertar un nuevo nodo al montículo.
	FiboNode* fib_heap_insert(int new_key);

	// Funcion para hacer la union entre dos montículos. En este caso, hacemos la unión 
	// entre el monticulo propio de la clase con el que le pasamos como parametro.
	void fib_heap_union(FiboHeap* other_heap);

	// Funcion para extraer del monticulo el minimo.
	void fib_heap_extract_min();

	// Funcion para decrecer el valor de un nodo
	void fib_heap_decrease_key(FiboNode* node, int new_key);

	// Funcion sencilla para simplemente mostrar los elementos de una lista circular.
	void print_root_list(FiboNode* node);

private:
	// Nodo que apunta al minimo del monticulo.
	FiboNode* min_node = nullptr;

	// Funciin para crear un nuevo nodo, inicializando sus valores.
	FiboNode* initialize_node(int new_key);

	// Funcion que recorre los arboles del monticulo y funde repetidamente
	// arboles con el mismo grado, hasta que solo haya un arbol a lo sumo por cada grado.
	void consolidate();

	// Funcion para concatenar dos root lists.
	void concatenate_root_lists(FiboNode* other_node);

	// Funcion para eliminar un nodo de la root list.
	void remove_from_list(FiboNode* node);

	// Funcion que emparenta dos nodos, utilizada en el consolidate.
	void fib_heap_link(FiboNode* node1, FiboNode* node2);

	// Funcion que corta un nodo y lo concatena en la root list.
	void cut(FiboNode* node1, FiboNode* node2);

	// Funcion que va realizando los cuts consecutivamente hasta que llega a la root list.
	void cascading_cut(FiboNode* node);

};


#endif