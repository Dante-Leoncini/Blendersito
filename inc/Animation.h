typedef enum { Constant, Linear, Bezier };
class keyFrame { 
	public:
		TInt frame;
		//TInt value;
		GLfloat valueX;
		GLfloat valueY;
		GLfloat valueZ;
		//TBool boolean;
		TInt Interpolation;
};

// Función de intercambio
void Swap(keyFrame& a, keyFrame& b) {
    keyFrame temp = a;
    a = b;
    b = temp;
}

// Partición para quicksort
TInt Partition(RArray<keyFrame>& arr, TInt low, TInt high) {
    TInt pivot = arr[high].frame;
    TInt i = low - 1;

    for (TInt j = low; j < high; j++) {
        if (arr[j].frame < pivot) {
            i++;
            Swap(arr[i], arr[j]);
        }
    }
    Swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Implementación de quicksort
void QuickSort(RArray<keyFrame>& arr, TInt low, TInt high) {
    if (low < high) {
        TInt pi = Partition(arr, low, high);
        QuickSort(arr, low, pi - 1);
        QuickSort(arr, pi + 1, high);
    }
}

TBool compareKeyFrames(const keyFrame& a, const keyFrame& b) {
    return a.frame < b.frame;
}

typedef enum { AnimPosition, AnimRotation, AnimScale };
class AnimProperty { 
	public:
		TInt Property;
		RArray<keyFrame> keyframes;

        void SortKeyFrames() {
            QuickSort(keyframes, 0, keyframes.Count() - 1);
        }
};

class Animation { 
	public:
		TInt Id;
		RArray<AnimProperty> Propertys;
};