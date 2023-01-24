//tipos de objetos
typedef enum { mesh, camera, light, empty, armature, curve } TiposObjetos;

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
		TInt normalsSize;
		TInt facesSize;
		TInt edgesSize;
		TInt uvSize;
		TInt interpolacion;
		GLshort * vertex;
		GLbyte * normals;
		GLushort * faces;
		RArray<GLushort> NewFaces;
		GLushort * edges;
		GLbyte * uv;
		
		GLuint texturaID;
		GLfloat diffuse[4];		
		GLfloat specular[4];	
		GLfloat emission[4];		

		TInt vertexGroupSize;
		GLushort* vertexGroup;
		TInt* vertexGroupIndiceSize;
		GLushort** vertexGroupIndice;
		
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
					newUv[newSize*3] = uv[i*3];
					newUv[newSize*3+1] = uv[i*3+1];
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
			delete[] newVertexGroup;
			delete[] newVertexGroupIndice;	
			delete[] newVertexGroupIndiceSize;	
			delete[] newVertex;
			delete[] newNormals;
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
};
