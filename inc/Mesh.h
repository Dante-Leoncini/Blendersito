//tipos de objetos
typedef enum { mesh, camera, light, empty, armature, curve } TiposObjetos;
typedef GLshort Edge[2];

class Mesh { //clase Mesh
	public:
		TiposObjetos type;
		TBool visible;
		TBool textura;
		TBool smooth;
		TBool transparencia;
		GLfloat posX; GLfloat posY; GLfloat posZ;
		GLfloat rotX; GLfloat rotY; GLfloat rotZ;
		GLfixed scaleX; GLfixed scaleY; GLfixed scaleZ;
		TInt vertexSize;
		TInt vertexGroupSize;
		TInt normalsSize;
		TInt facesSize;
		TInt edgesSize;
		TInt uvSize;
		TInt interpolacion;		
		GLuint texturaID;
		GLfloat diffuse[4];		
		GLfloat specular[4];	
		GLfloat emission[4];	

		GLshort* vertex;
		GLbyte* normals;
		GLushort* faces;
		GLushort* edges;
		GLbyte* uv;
		GLushort* vertexGroup;
		TInt* vertexGroupIndiceSize;
		GLushort** vertexGroupIndice;
		
		RArray<GLushort> NewFaces;	
		
		void Mesh::RemoveVertex(TInt vertexToRemove){
			vertexGroupSize--;
			if (vertexGroupSize <= 0){
				vertexSize = 0;
				normalsSize = 0;
				facesSize = 0;
				edgesSize = 0;
				uvSize = 0;
				vertexGroup = new GLushort[0];
				vertexGroupIndice = new GLushort*[0];
				vertexGroupIndiceSize = new TInt[0];
				vertex = new GLshort[0];
				normals = new GLbyte[0];
				return;
			}
			GLshort * newVertex = new GLshort[vertexSize];
			GLbyte * newNormals = new GLbyte[vertexSize];
			GLbyte* newUv = new GLbyte[uvSize];
			GLushort* newVertexGroup = new GLushort[vertexGroupSize];
			GLushort** newVertexGroupIndice = new GLushort*[vertexGroupSize];
			TInt* newVertexGroupIndiceSize = new TInt[vertexGroupSize];
			TInt newSize = 0;
			//solo guarda los vertices que no estan en el grupo
			TBool encontrado = false;
			for(TInt i = 0; i < vertexSize/3; i++){
				encontrado = false;
				for(TInt e = 0; e < vertexGroupIndiceSize[vertexToRemove]; e++){
					//si coincide el vertice con el indice del grupo a borrar, no lo copia
					if (vertexGroupIndice[vertexToRemove][e] == i){
						encontrado = true;						
					}
				}	
				if (!encontrado){
					newVertex[newSize*3] = vertex[i*3];
					newVertex[newSize*3+1] = vertex[i*3+1];
					newVertex[newSize*3+2] = vertex[i*3+2];
					newNormals[newSize*3] = normals[i*3];
					newNormals[newSize*3+1] = normals[i*3+1];
					newNormals[newSize*3+2] = normals[i*3+2];
					newUv[newSize*2] = uv[i*2];
					newUv[newSize*2+1] = uv[i*2+1];
					newSize++;								
				}	
			}
			//borra 3 por cada vertice en el grupo
			vertexSize -= vertexGroupIndiceSize[vertexToRemove]*3;
			uvSize -= vertexGroupIndiceSize[vertexToRemove]*2;
			normalsSize = vertexSize;
			newSize = 0;	
			TInt Restar = 0;
			for(TInt i = 0; i < vertexGroupSize+1; i++){
				if (i != vertexToRemove) {
					newVertexGroup[newSize] = vertexGroup[i];
					Restar = 0; //reinicia
					for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
						if (vertexGroupIndice[vertexToRemove][g] <= newVertexGroup[newSize]){Restar++;}					
					}
					newVertexGroup[newSize] -= Restar;
					newVertexGroupIndiceSize[newSize] = vertexGroupIndiceSize[i];
					newVertexGroupIndice[newSize] = new GLushort[vertexGroupIndiceSize[i]];
					for(TInt s = 0; s < vertexGroupIndiceSize[i]; s++){
						newVertexGroupIndice[newSize][s] = vertexGroupIndice[i][s];	
						Restar = 0; //reinicia
						for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
							if (vertexGroupIndice[vertexToRemove][g] <= newVertexGroupIndice[newSize][s]){
								Restar++;						
							}					
						}
						newVertexGroupIndice[newSize][s] -= Restar;
					}
					newSize++;
				}				
			}	
			//esto hay que ahcerlo ANTES de borrar los viejos datos
			//tenemos que cambiar los indices de los Array temporales
			vertexGroup = new GLushort[vertexGroupSize - 1];
			vertexGroupIndice = new GLushort*[vertexGroupSize - 1];
			vertexGroupIndiceSize = new TInt[vertexGroupSize - 1];
			vertex = new GLshort[vertexSize];
			normals = new GLbyte[vertexSize];
			uv = new GLbyte[uvSize];
			for(TInt i = 0; i < vertexSize; i++) {
				vertex[i] = newVertex[i];	
				normals[i] = newNormals[i];	
			}
			for(TInt i = 0; i < uvSize; i++) {
				uv[i] = newUv[i];	
			}
			for(TInt i = 0; i < vertexGroupSize; i++) {
				vertexGroup[i] = newVertexGroup[i];
				vertexGroupIndiceSize[i] = newVertexGroupIndiceSize[i];
				vertexGroupIndice[i] = new GLushort[newVertexGroupIndiceSize[i]];
				for(TInt s = 0; s < newVertexGroupIndiceSize[i]; s++){
					vertexGroupIndice[i][s] = newVertexGroupIndice[i][s];						
				}
			}		
	
			delete[] newVertex;
			delete[] newNormals;
			delete[] newUv;	
			delete[] newVertexGroup;
			delete[] newVertexGroupIndiceSize;
			for (TInt k = 0; k < vertexGroupSize; ++k) {
				delete[] newVertexGroupIndice[k];
			}
			delete[] newVertexGroupIndice;
		}

		void Mesh::RemoveFaces(RArray<GLushort>& facesIndexToRemove, TInt vertexToRemove) {
			GLushort * newFaces = new GLushort[facesSize - facesIndexToRemove.Count()*3];
			TInt newSize = 0;
			for(TInt i = 0; i < facesSize; i += 3) {
				TInt j = 0;
				for (; j < facesIndexToRemove.Count(); j++) {
					if (i/3 == facesIndexToRemove[j]) break;
				}
				if (j == facesIndexToRemove.Count()) {
					newFaces[newSize] = faces[i];
					newFaces[newSize + 1] = faces[i + 1];
					newFaces[newSize + 2] = faces[i + 2];
					newSize += 3;
				}
			}
			facesSize = newSize;
			delete[] faces;
			faces = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				faces[i] = newFaces[i];
				TInt Restar = 0;
				//resta un indice cada vez que un valor es superior al indice del grupo
				for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
					if (vertexGroupIndice[vertexToRemove][g] <= faces[i]){
						Restar++;						
					}					
				}
				faces[i] -= Restar;
			}
			delete[] newFaces;
		}
		void Mesh::RemoveEdges(RArray<GLushort>& edgesIndexToRemove, TInt vertexToRemove) {
		    GLushort * newEdges = new GLushort[edgesSize - edgesIndexToRemove.Count()*2];
		    TInt newSize = 0;
		    for(TInt i = 0; i < edgesSize; i += 2) {
		        TInt j = 0;
		        for (; j < edgesIndexToRemove.Count(); j++) {
		            if (i/2 == edgesIndexToRemove[j]) break;
		        }
		        if (j == edgesIndexToRemove.Count()) {
		            newEdges[newSize] = edges[i];
		            newEdges[newSize + 1] = edges[i + 1];
		            newSize += 2;
		        }
		    }
		    edgesSize = newSize;
		    edges = new GLushort[newSize];
			for(TInt i = 0; i < newSize; i++) {
				edges[i] = newEdges[i];
				TInt Restar = 0;
				//resta un indice cada vez que un valor es superior al indice del grupo
				for(TInt g = 0; g < vertexGroupIndiceSize[vertexToRemove]; g++){
					if (vertexGroupIndice[vertexToRemove][g] <= edges[i]){
						Restar++;						
					}					
				}
				edges[i] -= Restar;
			}
		    delete[] newEdges;
		}
		void Mesh::AgruparVertices() {
			//agrupar vertices
		    GLshort* TempVertexPos = new GLshort[vertexSize];
		    GLushort* TempVertexIndice = new GLushort[vertexSize];
		    TInt* TempVertexIndiceGroupSize = new TInt[vertexSize];
		    GLushort** TempVertexIndiceGroup = new GLushort*[vertexSize];
			for(TInt a=0; a < vertexSize; a++){
				TempVertexPos[a] = 0;//obj.vertex[a];
				//TempVertexIndiceGroup[a] = new GLushort[obj.vertexSize/3];
				TempVertexIndiceGroup[a] = new GLushort[5]; //lo ideal seria el maximo posible. pero problemas de memoria
			}	
		    vertexGroupSize = 0;
		    bool iguales = false;
			for(TInt a=0; a < vertexSize/3; a++){
				iguales = false;
				//busca copias
				for(TInt s=0; s < vertexGroupSize; s++){
					if (TempVertexPos[s*3] == vertex[a*3] &&
						TempVertexPos[s*3+1] == vertex[a*3+1] &&
						TempVertexPos[s*3+2] == vertex[a*3+2]){
						iguales = true;
						//aumenta el tama�o del array y le agrega el indice que coincide
						//GLushort* temp = new GLushort[TempVertexIndiceGroupSize[s]+1];
						//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
						//	temp[t] = TempVertexIndiceGroup[s][t];			
						//}	
						//temp[TempVertexIndiceGroupSize[s]] = a;	
						//TempVertexIndiceGroupSize[s]++;
						//TempVertexIndiceGroup[s] = new GLushort[TempVertexIndiceGroupSize[s]]; //agrega el primer indice
						//for(int t=0; t < TempVertexIndiceGroupSize[s]; t++){
						//	TempVertexIndiceGroup[s][t] = temp[t];			
						//}
						//delete[] temp;
						TempVertexIndiceGroup[s][TempVertexIndiceGroupSize[s]] = a;
						TempVertexIndiceGroupSize[s]++;
						break;
					}			
				};
				//si no se encontro el vertice. lo guarda y aumenta el size
				if (!iguales){
					TempVertexPos[vertexGroupSize*3] = vertex[a*3];
					TempVertexPos[vertexGroupSize*3+1] = vertex[a*3+1];
					TempVertexPos[vertexGroupSize*3+2] = vertex[a*3+2];
					TempVertexIndice[vertexGroupSize] = a;	
					
					//TempVertexIndiceGroup[obj.vertexGroupSize] = new GLushort[1]; //agrega el primer indice
					TempVertexIndiceGroupSize[vertexGroupSize] = 1; //arranca en uno
					TempVertexIndiceGroup[vertexGroupSize][0] = a;
					vertexGroupSize++;
				}
			}
			//copia los indices de los vertices a dibujar
			vertexGroup = new GLushort[vertexGroupSize];
			for(TInt a=0; a < vertexGroupSize; a++){
				vertexGroup[a] = TempVertexIndice[a];
			}	
			vertexGroupIndice = new GLushort*[vertexGroupSize];
			vertexGroupIndiceSize = new TInt[vertexGroupSize];
			for(TInt a=0; a < vertexGroupSize; a++){
				vertexGroupIndiceSize[a] = TempVertexIndiceGroupSize[a];
				vertexGroupIndice[a] = new GLushort[vertexGroupIndiceSize[a]];
				for(TInt i=0; i < vertexGroupIndiceSize[a]; i++){
					vertexGroupIndice[a][i] = TempVertexIndiceGroup[a][i];	
				}
			}	

			// Liberar memoria de TempVertexPos, TempVertexIndice y TempVertexIndiceGroup
			delete[] TempVertexPos;
			delete[] TempVertexIndice;

			// Liberar memoria de TempVertexIndiceGroup y sus elementos
			for (TInt k = 0; k < vertexSize; ++k) {
				delete[] TempVertexIndiceGroup[k];
			}
			delete[] TempVertexIndiceGroup;
		}
		
		TBool Mesh::isDuplicateEdge(Edge* edges, TInt edgesSize, GLushort v1, GLushort v2) {
		    for (TInt i = 0; i < edgesSize; i++) {
		        if ((edges[i][0] == v1 && edges[i][1] == v2) || (edges[i][0] == v2 && edges[i][1] == v1)) {
		            return true;
		        }
		    }
		    return false;
		}

		void Mesh::RecalcularBordes() {
			for (int i = 0; i < facesSize/3; i += 3) {
				edges[i*6+0] = faces[i];   edges[i*6+1] = faces[i+1];
				edges[i*6+2] = faces[i+1]; edges[i*6+3] = faces[i+2];
				edges[i*6+4] = faces[i+2]; edges[i*6+5] = faces[i];
			}
			Edge* TempEdgesVertex = new Edge[edgesSize];
			TInt uniqueEdgeCount = 0;
			/*Edge edge1 = { 0, 0 };
			Edge edge2 = { 0, 0 };
			Edge edge3 = { 0, 0 };
			for (int i = 0; i < facesSize/3; i += 3) {
			    edge1[0] = faces[i];   edge1[1] = faces[i+1];
			    edge2[0] = faces[i+1]; edge2[1] = faces[i+2];
			    edge3[0] = faces[i+2]; edge3[1] = faces[i];
			    if (!isDuplicateEdge(TempEdgesVertex, uniqueEdgeCount, edge1[0], edge1[1])) {
					TempEdgesVertex[uniqueEdgeCount++][0] = edge1[0];
					TempEdgesVertex[uniqueEdgeCount][1] = edge1[1];
				}
			    if (!isDuplicateEdge(TempEdgesVertex, uniqueEdgeCount, edge2[0], edge2[1])) {
					TempEdgesVertex[uniqueEdgeCount++][0] = edge2[0];
					TempEdgesVertex[uniqueEdgeCount][1] = edge2[1];
				}
			    if (!isDuplicateEdge(TempEdgesVertex, uniqueEdgeCount, edge3[0], edge3[1])) {
					TempEdgesVertex[uniqueEdgeCount++][0] = edge3[0];
					TempEdgesVertex[uniqueEdgeCount][1] = edge3[1];
				}
			}
			edgesSize = uniqueEdgeCount*2;
			for (int i = 0; i < edgesSize/2; i += 3) {
				edges[i*2] = TempEdgesVertex[i][0];
				edges[i*2+1] = TempEdgesVertex[i][1];
			}
			delete edge1;
			delete edge2;
			delete edge3;*/
			delete[] TempEdgesVertex;	
		}
};
