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
			GLshort * newVertex = new GLshort[vertexSize];
			GLushort* newVertexGroup = new GLushort[vertexGroupSize];
			GLushort** newVertexGroupIndice = new GLushort*[vertexGroupSize];
			TInt* newVertexGroupIndiceSize = new TInt[vertexGroupSize];
			TInt newSize = 0;
			//solo guarda los vertices que no estan en el grupo
			/*for(TInt i = 0; i < vertexSize/3; i++){
				for(TInt e = 0; e < vertexGroupIndiceSize[vertexToRemove]; e++){
					//si coincide el vertice con el indice del grupo a borrar, no lo copia
					TBool encontrado = false;
					if (vertexGroupIndice[vertexToRemove][e] == i){
						encontrado = true;						
					}
					if (!encontrado){
						newVertex[newSize*3] = vertex[i*3];
						newVertex[newSize*3+1] = vertex[i*3+1];
						newVertex[newSize*3+2] = vertex[i*3+2];
						newSize++;								
					}
				}		
			}*/
			//borra 3 por cada vertice en el grupo
			vertexSize -= vertexGroupIndiceSize[vertexToRemove]*3;
			newSize = 0;	
			for(TInt i = 0; i < vertexGroupSize+1; i++){
				if (i != vertexToRemove) {
					newVertexGroup[newSize] = vertexGroup[i];
					newVertexGroupIndiceSize[newSize] = vertexGroupIndiceSize[i];
					newVertexGroupIndice[newSize] = new GLushort[vertexGroupIndiceSize[i]];
					for(TInt s = 0; s < vertexGroupIndiceSize[i]; s++){
						newVertexGroupIndice[newSize][s] = vertexGroupIndice[i][s];
						//if(newVertexGroupIndice[newSize][s] > vertexToRemove)
						//	newVertexGroupIndice[newSize][s]--;
					}
					newSize++;
				}				
			}	
			vertexGroup = new GLushort[vertexGroupSize - 1];
			vertexGroupIndice = new GLushort*[vertexGroupSize - 1];
			vertexGroupIndiceSize = new TInt[vertexGroupSize - 1];
			//vertex = new GLshort[vertexSize];
			for(TInt i = 0; i < vertexGroupSize; i++) {
				//vertex[i*3] = newVertex[i*3];
				//vertex[i*3+1] = newVertex[i*3+1];
				//vertex[i*3+2] = newVertex[i*3+2];
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
		}
		void Mesh::RemoveFaces(RArray<GLushort>& facesIndexToRemove) {
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
			}
			delete[] newFaces;
		}
		void Mesh::RemoveEdges(RArray<GLushort>& edgesIndexToRemove) {
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
			}
		    delete[] newEdges;
		}
};
